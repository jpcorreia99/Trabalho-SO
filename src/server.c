#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
//./a.out -e "ls | wc"
//./a.out -e "grep -v ˆ# /etc/passwd | cut -f7 -d: | uniq | wc -l"

// ver porque é que o handler do sig2 não está a dar print
// talvez meter o records como variável global, testar se os limites estão a funcionar
int* pids;
int pids_count;
int time_limit_execute = 100;
int time_limit_communication = 1;
int forced_termination = 0;
int timeout_termination = 0;
int timeout_communication = 0;


typedef struct record {
    char* name;
    int status;
    int pid;
} * Record;

Record records_array[1024];
int number_records=0;


// para a opção -m
void timeout_handler(int signum) { 
    printf("Timeout handler");
    for (int i = 0; i < pids_count; i++) {
        if (pids[i] > 0) {  // evitar kill -1;
            kill(pids[i], SIGKILL);
        }
    }
    timeout_termination = 1;
}

void communication_limit_handler(int signum) { 
    int parent_pid = getppid();
    kill(parent_pid,SIGUSR2);
    printf("\n\n***Timeout do -i*****, pid do pai: %d\n\n\n",parent_pid);
}

void sigchld_handler_parent(int signum){
    printf("O Handler do pai\n");
    int status,s=0;
    pid_t pid = wait(&status);
    if (WIFEXITED(status)){ 
        int s=WEXITSTATUS(status);
        printf("O Pid apanhado no / do pai é %d\n",pid);
        int found=0;
        int i;
        for(i=0;i<number_records && !found;i++){
            if(records_array[i]->pid == pid){
                found=1;
            }
        }
        if(found){
            records_array[i-1]->status=s;
        }else{
            printf("Problemas a completar a tarefa\n");
        }
    }
}

void sigchld_handler_child(int signum){
    printf("SIGCHLD ignorado\n");
}

// será usado na opção -t
void sigusr1_handler(int signum){
    printf("sigusr1 handler, terminar o processo\n");
    for(int i=0; i<pids_count;i++){
        kill(pids[i],SIGKILL);
    }
    forced_termination = 1;
}

// será usado na opção -i para o pai dos processos matar os filhos todos
void sigusr2_handler(int signum){
    printf("\n\n**sigusr2 handler\n\n\n");
    for(int i=0; i<pids_count;i++){
        printf("A terminar o processo com U pid %d\n",pids[i]);
        kill(pids[i],SIGKILL);
    }
    timeout_communication = 1;
}


ssize_t readln(int fd, char* line, size_t size) {
    int i;
    printf(".");
    for (i = 0; read(fd, line + i, 1) && printf(".") && line[i] != '\n' && i < size; i++);
    return i;
}

char*** separate_commands(char* str, int* number_commands,
                          int** size_commands_array) {
    char str2[] = "grep -v ^# /etc/passwd | cut -f7 -d: | uniq | wc";
    char str3[] = "pstree | wc";

    char*** command_matrix = NULL;
    char* tokenized_string = strtok(str, " ");
    int n_args = 0;      // contará em cada comando o número de argumentos
    int n_commands = 0;  // nº de comandos

    int* size_commands = malloc(sizeof(int));  // array que guarda o número de parametros de cada
                         // comando
    command_matrix = malloc(sizeof(char**));
    while (tokenized_string) {
        if (strcmp(tokenized_string, "|") != 0) {
            command_matrix[n_commands] = realloc(
                command_matrix[n_commands],
                sizeof(char*) * ++n_args);  // aloca espaço para mais um comando
            command_matrix[n_commands][n_args - 1] = tokenized_string;
        } else {
            command_matrix[n_commands] = realloc(
                command_matrix[n_commands],
                sizeof(char*) * ++n_args);  // aloca espaço para mais um comando
            command_matrix[n_commands][n_args - 1] =
                NULL;  // teermina a lista de strings de um comandos em null
            size_commands =
                realloc(size_commands, sizeof(int) * (n_commands + 1));
            size_commands[n_commands++] = n_args;

            command_matrix = realloc(
                command_matrix,
                sizeof(char**) *
                    (n_commands +
                     1));  // aloca espaço para mais uma linha de comandos
            n_args = 0;
        }
        tokenized_string = strtok(NULL, " ");
    }
    command_matrix[n_commands] =
        realloc(command_matrix[n_commands],
                sizeof(char*) * ++n_args);  // aloca espaço para mais um comando
    command_matrix[n_commands][n_args - 1] =
        NULL;  // teermina a lista de strings de um comandos em null
    printf("%d\n",size_commands[0]);
    printf("%d\n",n_commands);
    size_commands = realloc(size_commands, sizeof(int) * (n_commands + 1));
    size_commands[n_commands++] = n_args;

    *number_commands = n_commands;
    *size_commands_array = size_commands;
    return command_matrix;
}

int execute_pipe(char*** commands, int command_count,
                 int* size_commands_array) {
    pid_t pid;
    if((pid = fork())==0){
        if (signal(SIGCHLD, sigchld_handler_child) == SIG_ERR){
            perror("sigchild son error\n");
        }

       alarm(time_limit_execute);
        int pid;
        pids_count = command_count;
        pids = malloc(sizeof(int) * pids_count);
        memset(pids,-1,pids_count);
        if (command_count == 1) {
            printf("2\n");
            if ((pid = fork()) == 0) {
                execvp(commands[0][0], commands[0]);
                _exit(-1);
            }
            pids[0] = pid;  // nota: o fork devolve o pid do filho para o pai
            wait(NULL);
            alarm(0);
        }else{
            int fildes[command_count - 1][2];

            if (pipe(fildes[0]) == -1) {
                return -1;
            }
            if ((pid = fork()) == 0) {
                close(fildes[0][0]);
                dup2(fildes[0][1], 1);
                close(fildes[0][1]);
                execvp(commands[0][0], commands[0]);
                _exit(1);
            }
            close(fildes[0][1]);
            pids[0] = pid;
            int i=1;
            for (i = 1; i < command_count - 1; i++) {
                if (pipe(fildes[i]) == -1) {
                    return -1;
                }
                if ((pid = fork()) == 0) {
                    if (signal(SIGALRM, communication_limit_handler) == SIG_ERR){
                        perror("sigchild son error\n");
                    }
                    close(fildes[i][0]);
                    dup2(fildes[i - 1][0], 0);
                    close(fildes[i - 1][0]);

                    char buf[5];
                    int n_bytes;
                    int fildes_aux[2];
                    if(pipe(fildes_aux)==-1){
                        return -1;
                    }

                    alarm(time_limit_communication);
                    while((n_bytes = read(0,buf,5))>0){
                        alarm(time_limit_communication); //activa o alarme novamente
                        write(fildes_aux[1],buf,n_bytes);
                    // write(1,buf,n_bytes);
                    }
                    //alarm(0); // desliga o alarme no final de toda a leitura
                    
                    close(fildes_aux[1]);
                    dup2(fildes_aux[0],0);
                    close(fildes_aux[0]);
                    dup2(fildes[i][1], 1);
                    close(fildes[i][1]);

                    execvp(commands[i][0], commands[i]);
                    _exit(1);
                }
                pids[i] = pid;
                close(fildes[i - 1][0]);
                close(fildes[i][1]);
            }
            if ((pid = fork()) == 0) {
                if (signal(SIGALRM, communication_limit_handler) == SIG_ERR){
                    perror("sigchild son error\n");
                }
                dup2(fildes[i - 1][0], 0);
                close(fildes[i - 1][0]);

                char buf[5];
                int n_bytes;
                int fildes_aux[2];
                if(pipe(fildes_aux)==-1){
                    return -1;
                }

                alarm(time_limit_communication);

                while((n_bytes = read(0,buf,5))>0){
                    alarm(time_limit_communication); //activa o alarme novamente
                    write(fildes_aux[1],buf,n_bytes);
                //  write(1,buf,n_bytes);
                }
                //alarm(0); // desliga o alarme no final de toda a leitura
                
                close(fildes_aux[1]);
                dup2(fildes_aux[0],0);
                close(fildes_aux[0]);

                execvp(commands[i][0], commands[i]);
                _exit(1);
            }
            pids[i] = pid;  // i == command_count-1
            close(fildes[i-1][0]);
            for (int j = 0; j < command_count; j++) {
                wait(NULL);
            }
            alarm(0);
        }
        if(forced_termination){
            _exit(2);
        }else if(timeout_termination){
            _exit(3);
        }else if(timeout_communication){
            _exit(4);
        }else{
            _exit(1); // exit code quando corre tudo bem
        }
    }
    return pid;
}


int execute_task(char* task){
    printf("Resto: %s\n",task);
    Record record = malloc(sizeof(Record));
    record->name = strdup(task);
    record->status = 0;
    int number_commands;
    int* size_commands_array;
    char*** command_matrix = separate_commands(
        task, &number_commands, &size_commands_array);
    for (int i = 0; i < number_commands; i++) {
        printf("%d, ", size_commands_array[i]);
    }
    printf("\n");
    for (int i = 0; i < number_commands; i++) {
        for (int j = 0; j < size_commands_array[i]; j++) {
            printf("%d %d %s//\n", i, j, command_matrix[i][j]);
        }
    }

    int status;
    pid_t pid =
        execute_pipe(command_matrix, number_commands,
                        size_commands_array);
    record->pid = pid;
    records_array[number_records++] = record;
    return 0;
}

void terminate_task(char* index){
    long task_number = strtol(index,NULL,10); // using base 
    task_number--;// para fazer o numero da tarefa ser o indice
    printf("Task to terminate: %d\n",task_number);
    if(task_number<number_records){
        if(records_array[task_number]->status==0){
            kill(records_array[task_number]->pid, SIGUSR1);
            printf("Terminar a tarefa\n");
        }else{
            printf("Tarefa já terminada\n");
        }
    }else{
        printf("Tarefa inexistente\n");
    }

}

int change_time_limit_execute(char* limit){
    long value = strtol(limit,NULL,10); // using base 10
    time_limit_execute = value;
    printf("New time limit execute: %d\n",time_limit_execute);
    return 0;
}

int change_ime_limit_communication(char* limit){
    long value = strtol(limit,NULL,10); // using base 10
    time_limit_communication = value;
    printf("New time limit communication: %d\n",time_limit_communication);
    return 0;
}

void show_current_tasks(){
        printf("Tarefas em execução:\n");
    for(int i=0;i<number_records;i++){
        if(records_array[i]->status==0){
            printf("Tarefa #%d: %s\n",i+1,records_array[i]->name);
        }
    }
}

void show_history(){
    printf("Histórico:\n");
    for(int i=0;i<number_records;i++){
        char *status;
        
        switch(records_array[i]->status){
            case 0:
                status = "Em execução";
                break;
            case 1:
                status = "Terminado normalmente";
                break;
            case 2:
                status = "Terminada pelo utilizador";
                break;
            case 3:
                status = "Terminada por máximo de tempo de execução";
                break;
            case 4:
                status = "Terminado por máximo de inatividade de comunicação";
                break;
            default:
                status = "Status ainda não definido";
        }

        printf("Tarefa: %d, %s, status: %s\n",i+1,records_array[i]->name,status);
    }
}

int process_instruction(char* instruction, int instruction_size){
    printf("A entrar na process_instructiion\n");
    printf("Comprimento da instrução: %d\n",strlen(instruction));
    printf("Intrução: %s\n",instruction);
    int i=0;

    char* rest;
    char* token = strtok_r(instruction," ",&rest);
    if(token!=NULL && strlen(token)>1){ // nunca deverá ser preciso verificar, mas adicionou-se por uma questão de segurança
        printf("O primeiro tokenize: %s\n",token);
        switch (token[1])
        {
        case 'e':
            execute_task(rest);
            break;
        case 'm':
            change_time_limit_execute(rest);
            break;
        case 'l':
            show_current_tasks();
            break;
        case 'i':
            change_ime_limit_communication(rest); // mudar, falta o t
            break;
        case 'r':
            show_history();    
            break;
        case 't':
            terminate_task(rest);
            break;
        default:
            break;
        }
    }
    return 0;
}



int main(){
    if (signal(SIGALRM, timeout_handler) == SIG_ERR) {
        perror("SIGARLM error\n");
    }

    if (signal(SIGCHLD, sigchld_handler_parent) == SIG_ERR){
        perror("SIGCHLD parent error\n");
    }
    
    if (signal(SIGUSR1, sigusr1_handler) == SIG_ERR){
        perror("SIGUSR1 error\n");
    }

    if (signal(SIGUSR2, sigusr2_handler) == SIG_ERR){
        perror("SIGUSR2 error\n");
    }




    int fifo_fd;
    while(fifo_fd = open("fifo",O_RDONLY)){ // para ir lendo continuamente
        printf("\n\nNova instrução\n");
        printf("fifo is open\n");
        char buf[1024];
        //ssize_t bytes_read = readln(fifo_fd, buf, 1024);
        //ssize_t bytes_read=readln(fifo_fd,buf,1024);
        int bytes_read =read(fifo_fd,buf,1024);
        buf[bytes_read] = '\0';
        printf("acabou a leitura\n");

        process_instruction(buf,bytes_read);
        close(fifo_fd);
    }
    return 0;
}


