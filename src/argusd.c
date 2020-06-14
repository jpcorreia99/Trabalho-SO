#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
//./a.out -e "ls | wc"
//./a.out -e "grep -v ˆ# /etc/passwd | cut -f7 -d: | uniq | wc -l"

//mudar leitura do pipe client -> server para não ser só read



int* pids;
int pids_count;
int time_limit_execute = 500;
int time_limit_communication = 100;
int forced_termination = 0;
int timeout_termination = 0;
int timeout_communication = 0;

int fifo_server_to_client_fd=-1;

typedef struct record {
    char* name;
    int status;
    int pid;
} * Record;

Record records_array[1024];
int number_records=0;




// le uma linha para o line.
ssize_t readln2(int fd, char *line, size_t size){
  int i = 0; ssize_t res; int stop = 1; int j = 0; // int keepReading = 1; i
  while(i < size-1 && stop && (res = read(fd,&(line[i]),200)) ) {
     stop = stop;	
     if (res){
      for(j = 0; j < res && i+j < size && stop; j++){
        if (line [i+j] == '\n') stop = 0; 
        }
      }
      if (stop) i += res; 
     // printf("Linha nº %d: %s\n",counter, line);
     }
  line[i+j] = '\0';
  //off_t lseek(int fd, off_t offset, int whence);
  //printf("Linha nº %d: %s\n",counter, line);
  if (!stop) lseek(fd, -res+j, SEEK_CUR);
  return i + j;
 }

void update_output_index(int size){
	int output_fd;
	if ((output_fd = open("logs.idx", O_RDWR , 0666)) < 0){
		output_fd = open("logs.idx", O_RDWR | O_CREAT, 0666);
		write(output_fd,"0,\n",3);
		lseek(output_fd,0,SEEK_SET);
	}
	char linha [1024];
	int linha_length;
	int linha_length2 = readln2(output_fd,linha,1024);
	    //output_fd = lseek(output_fd,linha_length2,SEEK_SET);
	   // lseek(output_fd, linha_length, SEEK_CUR);
	    
	    for(linha_length = linha_length2 - 3 ; linha_length >= 0 && linha[linha_length] != ','; linha_length--){
		;
	    }
	    linha_length++; 
	    int output_length = strtol(&(linha[linha_length]),NULL,10);
	    linha_length2 = sprintf(linha,"%d,\n",output_length + size);
	    lseek(output_fd,-1,SEEK_CUR);
	    write(output_fd,linha,linha_length2);
	    printf("%s\n",linha);
	}



char* read_fifo(int fifo_fd,int* bytes_read){
    *bytes_read=0;
    int total_bytes_read=0;
    char* res = malloc(sizeof(char)*1024);
    int n_bytes_read;
    while((n_bytes_read=read(fifo_fd,res+total_bytes_read,1024))>0){
        total_bytes_read +=n_bytes_read;
        if(n_bytes_read==1024){
            res = realloc(res,sizeof(char) * (total_bytes_read * 2));
        }
    }
    res[total_bytes_read]='\0';
    *bytes_read = total_bytes_read;
    return res;
}


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

/* utilizado quando é ultrapassado o limite de 
tempo de falta de comunicação entre pipes*/
void communication_limit_handler(int signum) { 
    int parent_pid = getppid();
    kill(parent_pid,SIGUSR2);
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
    char buf [1024];
    int buf_line_size = 0;
    int buf_total_size = 0;
    int output_fd;
    if((pid = fork())==0){
        if (signal(SIGCHLD, sigchld_handler_child) == SIG_ERR){
            perror("sigchild son error\n");
        }
        close(fifo_server_to_client_fd);
        alarm(time_limit_execute);
        int pipe_command_output[2];
	pipe(pipe_command_output);
        int pid;
        pids_count = command_count;
        pids = malloc(sizeof(int) * pids_count);
        memset(pids,-1,pids_count);
        if (command_count == 1) {
            printf("2\n");
            if ((pid = fork()) == 0) {
	        dup2(pipe_command_output[1],1);
		close(pipe_command_output[1]);
                execvp(commands[0][0], commands[0]);
                _exit(-1);
           }
 	   output_fd = open("output.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
	   close(pipe_command_output[1]);
	   while((buf_line_size = read(pipe_command_output[0],buf,1024)) > 0){
			buf_total_size += write(output_fd,buf,buf_line_size);
			write(1,buf,buf_line_size);
	   update_output_index(buf_total_size);
	    }
	    close(pipe_command_output[1]);
	    close(pipe_command_output[0]);
            pids[0] = pid;  // nota: o fork devolve o pid do filho para o pai
	    printf("TESTE PRINTS\n");
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

		dup2(pipe_command_output[1],1);
		close(pipe_command_output[1]);
                execvp(commands[i][0], commands[i]);
                _exit(1);
            }
/*	        dup2(pipe_command_output[1],1);
		close(pipe_command_output[1]);
                execvp(commands[0][0], commands[0]);
                _exit(-1);
           }*/
 	   output_fd = open("output.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
	   close(pipe_command_output[1]);
	   while((buf_line_size = read(pipe_command_output[0],buf,1024)) > 0){
			buf_total_size += write(output_fd,buf,buf_line_size);
			write(1,buf,buf_line_size);
	    }
	    update_output_index(buf_total_size);
	    close(pipe_command_output[1]);
	    close(pipe_command_output[0]);
	    close(output_fd);
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
    Record record = malloc(sizeof(Record));
    record->name = strdup(task);
    record->status = 0;
    char buf[50];
    int bytes_written = snprintf(buf,50,"Nova tarefa #%d\n",number_records+1);
    write(fifo_server_to_client_fd,buf,bytes_written);

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
    task_number--;// para o numero da tarefa corresponder ser o indice
    if(task_number<number_records){
        if(records_array[task_number]->status==0){
            kill(records_array[task_number]->pid, SIGUSR1);
            char buf[80];
            int bytes_written = snprintf(buf,64,"Servidor: tarefa %d terminada manualmente\n",task_number+1);
            write(fifo_server_to_client_fd,buf,bytes_written);
        }else{
            write(fifo_server_to_client_fd,"Servidor: Tarefa já tinha terminado\n",strlen("Servidor: Tarefa já tinha terminado\n"));
        }
    }else{
         write(fifo_server_to_client_fd,"Servidor: Tarefa inexistente\n",strlen("Servidor: Tarefa inexistente\n"));
    }
}

void change_time_limit_execute(char* limit){
    long value = strtol(limit,NULL,10); // using base 10
    time_limit_execute = value;
    char buf[100];
    int bytes_writen = snprintf(buf,100,"Servidor: Novo limite de tempo de execução: %d\n",time_limit_execute);
    write(fifo_server_to_client_fd,buf,bytes_writen);
}

void change_ime_limit_communication(char* limit){
    long value = strtol(limit,NULL,10); // using base 10
    time_limit_communication = value;
    char buf[100];
    int bytes_writen = snprintf(buf,100,"Servidor: Novo limite de tempo de inatividade: %d\n",time_limit_communication);
    write(fifo_server_to_client_fd,buf,bytes_writen);
}

void show_current_tasks(){
    write(fifo_server_to_client_fd,"Servidor: Tarefas em execução: \n",strlen("Servidor: Tarefas em execução: \n"));
    for(int i=0;i<number_records;i++){
        if(records_array[i]->status==0){
            int string_size = 15+strlen(records_array[i]->name);
            char buf[string_size];
            int bytes_written = snprintf(buf,string_size,"\t#%d: %s\n",i+1,records_array[i]->name);
            write(fifo_server_to_client_fd,buf,bytes_written);
        }
    }
}

void show_history(){
    write(fifo_server_to_client_fd,"Servidor: Histórico: \n",strlen("Servidor: Histórico: \n"));
    for(int i=0;i<number_records;i++){
        if(records_array[i]->status>0){
            char *status;
            
            switch(records_array[i]->status){
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
                    status = "Erro: status indefinido";
            }
            int string_size = 30+strlen(records_array[i]->name) + strlen(status);
            char buf[string_size];
            int bytes_written = snprintf(buf,string_size,"\t#%d: %s, status: %s\n",i+1,records_array[i]->name,status);
            write(fifo_server_to_client_fd,buf,bytes_written);
        }
    }
}

int process_instruction(char* instruction, int instruction_size){

    char* rest;
    char* token = strtok_r(instruction," ",&rest);
    if(token!=NULL && strlen(token)>1){ // nunca deverá ser preciso verificar, mas adicionou-se por uma questão de segurança
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

    //inicializar ficheiro de indices
     int output_fd = open("logs.idx", O_RDWR | O_TRUNC, 0666);
     write(output_fd,"0,\n",3);
     close(output_fd);
    //inicializar ficheiro de outputs
     output_fd = open("output.txt", O_RDWR | O_TRUNC, 0666);
     close(output_fd);
     
    int fifo_client_to_server_fd;
    //int fifo_server_to_client_fd;
    while(fifo_client_to_server_fd = open("fifo_client_to_server",O_RDONLY)){ // para ir lendo continuamente
        printf("\n\n\nNovo Ciclo:\n");
        printf("fifo is open\n");

        printf("A abrir o fifo de escrita\n");
        if((fifo_server_to_client_fd = open("fifo_server_to_client",O_WRONLY))<0){
            perror("open");
            return 1;
        }
        printf("Fifo de escrita aberto\n");
       

        
        //char buf[1024];
        //ssize_t bytes_read = readln(fifo_fd, buf, 1024);
        //ssize_t bytes_read=readln(fifo_fd,buf,1024);
        //int bytes_read =read(fifo_client_to_server_fd,buf,1024);
        //buf[bytes_read] = '\0';
        int bytes_read;
        char* line = read_fifo(fifo_client_to_server_fd,&bytes_read);

        printf("Linha lida %s\n, comprimento_ %d\n",line,bytes_read);
        close(fifo_client_to_server_fd);
        printf("acabou a leitura\n");

        process_instruction(line,bytes_read);
        free(line);

        close(fifo_server_to_client_fd);
        printf("fifo fechado\n");
    }


    return 0;
}


