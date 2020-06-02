#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int* pids;
int pids_count;
int time_limit_execute = 2;

typedef struct record {
    char* name;
    int status;
    int pid;  // toKill
} * Record;

char*** separate_commands(char* argv[], int* number_commands,
                          int** size_commands_array) {
    char* str = argv[2];
    char str2[] = "grep -v ^# /etc/passwd | cut -f7 -d: | uniq | wc";
    char str3[] = "pstree | wc";

    char*** command_matrix = NULL;
    char* tokenized_string = strtok(str, " ");
    int n_args = 0;      // contará em cada comando o número de argumentos
    int n_commands = 0;  // nº de comandos

    int* size_commands;  // array que guarda o número de parametros de cada
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
    size_commands = realloc(size_commands, sizeof(int) * (n_commands + 1));
    size_commands[n_commands++] = n_args;

    *number_commands = n_commands;
    *size_commands_array = size_commands;
    return command_matrix;
}

int execute_pipe(char*** commands, int command_count,
                 int* size_commands_array) {
    alarm(time_limit_execute);
    if (command_count < 1) {
        return -1;
    }
    int pid;
    pids_count = command_count;
    pids = malloc(sizeof(int) * pids_count);
    if (command_count == 1) {
        if ((pid = fork()) == 0) {
            execvp(commands[0][0], commands[0]);
            _exit(-1);
        }
        pids[0] = pid;  // nota: o fork devolve o pid do filho para o pai
        return 0;
    }

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
    pids[0] = pid;
    close(fildes[0][1]);

    int i;
    for (i = 1; i < command_count - 1; i++) {
        if (pipe(fildes[i]) == -1) {
            return -1;
        }
        if ((pid = fork()) == 0) {
            close(fildes[i][0]);
            dup2(fildes[i - 1][0], 0);
            close(fildes[i - 1][0]);
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
        dup2(fildes[i - 1][0], 0);
        close(fildes[i - 1][0]);
        execvp(commands[i][0], commands[i]);
        _exit(1);
    }
    pids[i] = pid;  // i == command_count-1
    close(fildes[i][0]);
    int j;
    for (j = 0; j < i; j++) {
        wait(NULL);
    }

    return 0;
}

void timeout_handler(int signum) {
    for (int i = 0; i < pids_count; i++) {
        printf("Killing process %d due to timeout\n", pids[i]);
        if (pids[i] > 0) {  // evitar kill -1;23
            kill(pids[i], SIGKILL);
        }
    }
}

ssize_t readln(int fd, char* line, size_t size) {
        int i;
        for (i = 0; read(fd, line + i, 1) && line[i] != '\n' && i < size; i++)
            ;
        return i;
    }

int main(int argc, char* argv[]) {
    /*if(argc<3){
        printf("Not enough arguments\n");
        return -1;
    }*/

    Record records_array[1024];
    int noRecords;

    if (signal(SIGALRM, timeout_handler) == SIG_ERR) {
        perror("timeouthandler error\n");
    }

    if (argc == 1) {
        int in = 1;
        int _argc = 0;
        char* buf;
        //char buf[1024];
        //char* aux;
        char** _argv;
        noRecords = 0;
        while (in) {
            _argc = 0;
            buf = realloc(buf,1024*sizeof(char));
            write(0, "argus$ ", strlen("argus$ "));
            ssize_t size = readln(0, buf, 1024);
            buf[size] = '\0';
            printf("buf: %s\n", buf);
            //_argv = (char**)realloc(_argv, 2 * sizeof(char*));
            //_argv = (char**)realloc(_argv, count * sizeof(char*));

            /*int count = 0;
            aux = malloc(sizeof(buf));
            strcpy(aux, buf);
            
            while((aux = strchr(aux, ' ')) != NULL){
                count++;
                aux++;
            }
            printf("count: %i\n",count);*/
            _argv = (char**)realloc(_argv, 2 * sizeof(char*));
            _argv[0] = strtok(buf, " ");
            _argv[1] = strtok(NULL, "\0");
            _argc = 2;

            for (int ai = 0; ai < _argc; ai++) {
                printf("_argv[%i] = %s\n", ai, _argv[ai]);
            }

            if (strcmp(_argv[0], "tempo-inactividade") == 0) {
                int new_limit = atoi(_argv[1]);
                if (new_limit > 0) {
                    time_limit_execute = new_limit;
                } else {
                    printf("Invalid limit\n");
                }
                printf("Time limit: %d\n", time_limit_execute);
            } else if (strcmp(_argv[0], "executar") == 0) {
                // Record init
                Record record = malloc(sizeof(Record));
                /*char* name;
                for (int index3 = 1; index3 < _argc; index3++) {
                    name = realloc(name, sizeof(_argv[index3]));
                    name = strcat(name, _argv[index3]);
                }
                record->name = realloc(record->name, sizeof(name));
                strcpy(record->name, name);*/
                record->name = realloc(record->name, sizeof(_argv[1]));
                strcpy(record->name, _argv[1]);
                record->status = 0;
                record->pid = getpid();
                records_array[noRecords++] = record;
                //
                int number_commands;
                int* size_commands_array;
                char*** command_matrix = separate_commands(
                    _argv, &number_commands, &size_commands_array);

                for (int i = 0; i < number_commands; i++) {
                    printf("%d\n", size_commands_array[i]);
                }

                for (int i = 0; i < number_commands; i++) {
                    for (int j = 0; j < size_commands_array[i]; j++) {
                        printf("%d %d %s//\n", i, j, command_matrix[i][j]);
                    }
                }

                if (fork() == 0) {
                    int status;
                    pid_t pid = execute_pipe(command_matrix, number_commands,
                                             size_commands_array);
                    printf("nova tarefa #%i\n", noRecords);
                    if (waitpid(pid, &status, 0) != -1) {
                        record->status = 1;
                    }
                    //TODO
                    //record->status = 1;
                    printf("pid count: %d\n", pids_count);
                    for (int i = 0; i < pids_count; i++)
                        printf("Pid: %d\n", pids[i]);
                    _exit(1);    
                }
                wait(NULL);
            } else if (strcmp(_argv[0], "historico") == 0) {
                printf("No Records: %d\n",noRecords);
                for (int index = 0; index < noRecords; index++) {
                    switch (records_array[index]->status) {
                        case 1: {
                            printf("#%i, concluida: %s\n", index + 1,
                                   records_array[index]->name);
                            break;
                        }
                        case 2: {
                            printf("#%i, max inactividade: %s\n", index + 1,
                                   records_array[index]->name);
                            break;
                        }
                        default:{
                            printf("DEFAULT\n"); 
                            break;  
                        } 
                    }
                }
            } else if (strcmp(_argv[0], "listar") == 0) {
                for (int index2 = 0; index2 < noRecords; index2++) {
                    if (!records_array[index2]->status)
                        printf("#%i: %s", index2 + 1,
                               records_array[index2]->name);
                }
            } else if (strcmp(_argv[0], "terminar") == 0) {
                // TODO
                kill(records_array[atoi(argv[2]) - 1]->pid, 0);
            } else if (strcmp(_argv[0], "ajuda") == 0) {
                write(1, "  tempo-inactividade segs\n",
                      strlen("  tempo-inactividade segs\n"));
                write(1, "  tempo-execucao segs\n",
                      strlen("  tempo-execucao segs\n"));
                write(1, "  executar p1 | p2 ... | pn",
                      strlen("  executar p1 | p2 ... | pn"));
                write(1, "  listar\n", strlen("  listar\n"));
                write(1, "  terminar n\n", strlen("  terminar n\n"));
                write(1, "  historico\n", strlen("  historico\n"));
                write(1, "  ajuda\n", strlen("  ajuda\n"));
            } else if (strcmp(_argv[0], "quit") == 0 ||
                       strcmp(_argv[0], "q") == 0)
                in = 0;
        }
        free(buf);
        //free(aux);
        free(_argv);
    } else {
        // int logsfd = open("../documents/logs.txt",O_RDWR,0666);
        if (argv[1][0] == '-') {
            switch (argv[1][1]) {
                case 'm': {
                    int new_limit = atoi(argv[2]);
                    if (new_limit > 0) {
                        time_limit_execute = new_limit;
                    } else {
                        printf("Invalid limit\n");
                    }
                    printf("Time limit: %d\n", time_limit_execute);
                    break;
                    ;
                }
                case 'e': {
                    // Record init
                    Record record = malloc(sizeof(Record));
                    char* name;
                    for (int index3 = 1; index3 < argc; index3++) {
                        name = realloc(name, sizeof(argv[index3]));
                        name = strcat(name, argv[index3]);
                    }
                    record->name = realloc(record->name, sizeof(name));
                    strcpy(record->name, name);
                    record->status = 0;
                    record->pid = getpid();
                    records_array[noRecords++] = record;
                    //
                    int number_commands;
                    int* size_commands_array;
                    char*** command_matrix = separate_commands(
                        argv, &number_commands, &size_commands_array);

                    for (int i = 0; i < number_commands; i++) {
                        printf("%d\n", size_commands_array[i]);
                    }

                    for (int i = 0; i < number_commands; i++) {
                        for (int j = 0; j < size_commands_array[i]; j++) {
                            printf("%d %d %s//\n", i, j, command_matrix[i][j]);
                        }
                    }

                    if (fork() == 0) {
                        int status;
                        pid_t pid =
                            execute_pipe(command_matrix, number_commands,
                                         size_commands_array);
                        printf("nova tarefa #%i\n", noRecords);
                        if (waitpid(pid, &status, 0) != -1) {
                            record->status = 1;
                        }
                        printf("pid count: %d\n", pids_count);
                        for (int i = 0; i < pids_count; i++)
                            printf("Pid: %d\n", pids[i]);
                    }
                    break;
                }
                case 'r': {
                    for (int index = 0; index < noRecords; index++) {
                        switch (records_array[index]->status)
                        case 1: {
                            printf("#%i, concluida: %s", index + 1,
                                   records_array[index]->name);
                            break;
                        }
                        case 2: {
                            printf("#%i, max inactividade: %s", index + 1,
                                   records_array[index]->name);
                            break;
                        }
                    }
                    break;
                }
                case 'l': {
                    for (int index2 = 0; index2 < noRecords; index2++) {
                        if (!records_array[index2]->status)
                            printf("#%i: %s", index2 + 1,
                                   records_array[index2]->name);
                    }
                    break;
                }
                case 'k': {
                    // TODO
                    kill(records_array[atoi(argv[2]) - 1]->pid, 0);
                }
                case 'h': {
                    write(1, "  tempo-inactividade segs\n",
                      strlen("  tempo-inactividade segs\n"));
                    write(1, "  tempo-execucao segs\n",
                      strlen("  tempo-execucao segs\n"));
                    write(1, "  executar p1 | p2 ... | pn",
                      strlen("  executar p1 | p2 ... | pn"));
                    write(1, "  listar\n", strlen("  listar\n"));
                    write(1, "  terminar n\n", strlen("  terminar n\n"));
                    write(1, "  historico\n", strlen("  historico\n"));
                    write(1, "  ajuda\n", strlen("  ajuda\n"));
                    break;
                }
            }
        }
    }
    wait(NULL);
    return 0;
}