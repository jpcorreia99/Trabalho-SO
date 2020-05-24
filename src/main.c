#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>



char*** separate_commands(char* argv[], int* number_commands, int** size_commands_array){
    char *str = argv[2];
    char str2[]= "grep -v ^# /etc/passwd | cut -f7 -d: | uniq | wc";
    char str3[] = "pstree | wc";

    char *** command_matrix = NULL;
    char * tokenized_string = strtok(str, " ");
    int n_args=0;  // contará em cada comando o número de argumentos
    int n_commands = 0; // nº de comandos

    int* size_commands; // array que guarda o número de parametros de cada comando
    command_matrix = malloc(sizeof(char**));
    while(tokenized_string){
        if(strcmp(tokenized_string,"|")!=0){
        command_matrix[n_commands] = realloc(command_matrix[n_commands], sizeof(char*) * ++n_args); // aloca espaço para mais um comando

        command_matrix[n_commands][n_args-1] = tokenized_string;
        }else{
            command_matrix[n_commands] = realloc(command_matrix[n_commands], sizeof(char*) * ++n_args); // aloca espaço para mais um comando
            command_matrix[n_commands][n_args-1] = NULL; // teermina a lista de strings de um comandos em null
            size_commands = realloc(size_commands, sizeof(int) * (n_commands+1));
            size_commands[n_commands++] = n_args;

            command_matrix = realloc(command_matrix, sizeof(char**)*(n_commands+1)); // aloca espaço para mais uma linha de comandos
            n_args=0;
        }
        tokenized_string = strtok(NULL, " ");
    }
    command_matrix[n_commands] = realloc(command_matrix[n_commands], sizeof(char*) * ++n_args); // aloca espaço para mais um comando
    command_matrix[n_commands][n_args-1] = NULL; // teermina a lista de strings de um comandos em null
    size_commands = realloc(size_commands, sizeof(int) * (n_commands+1));
    size_commands[n_commands++] = n_args;

    *number_commands = n_commands;
    *size_commands_array = size_commands;
    //size_commands_array = malloc(sizeof(int) * (n_commands-1));
    //memcpy(size_commands_array,size_commands , (n_commands-1) * sizeof(int));
    printf("OLHA: %d,%d\n",*number_commands,*size_commands_array[0]);
    return command_matrix;

}

int execute_pipe(char*** commands, int command_count, int* size_commands_array ){
    if(command_count < 1) {return -1;}
    if(command_count == 1){
        if(fork()==0){
            execvp(commands[0][0],commands[0]);
            _exit(-1);
        }
        return 0;
    }

    int fildes[command_count-1][2];

    if(pipe(fildes[0])==-1){
        return -1;
    }
    if(fork()==0){
        close(fildes[0][0]);
        dup2(fildes[0][1],1);
        close(fildes[0][1]);
        execvp(commands[0][0],commands[0]);
        _exit(1);
    }
    close(fildes[0][1]);

    int i;
    for(i=1;i<command_count-1;i++){

        if(pipe(fildes[i])==-1){
            return -1;
        }
        if(fork()==0){
            close(fildes[i][0]);
            dup2(fildes[i-1][0],0);
            close(fildes[i-1][0]);
            dup2(fildes[i][1],1);
            close(fildes[i][1]);
            execvp(commands[i][0],commands[i]);
            _exit(1);
        }
        close(fildes[i-1][0]);
        close(fildes[i][1]);
    }

    if(fork()==0){
        dup2(fildes[i-1][0],0);
        close(fildes[i-1][0]);
        execvp(commands[i][0],commands[i]);
        _exit(1);
    }
    close(fildes[i][0]);
    int j;
    for(j=0;j<i;j++){
        wait(NULL);
    }

    return 0;
    
}

int main(int argc, char* argv[]){
    if(argc<3){
        printf("Not enough arguments\n");
        return -1;
    }

    if(strcmp("-e",argv[1])==0){
        /*char *str = argv[2];
        char str2[]= "grep -v ^# /etc/passwd | cut -f7 -d: | uniq | wc";
        char str3[] = "pstree | wc";

        char *** command_matrix = NULL;
        char * tokenized_string = strtok(str, " ");
        int n_args=0;  // contará em cada comando o número de argumentos
        int n_commands = 0; // nº de comandos

        int* size_commands; // array que guarda o número de parametros de cada comando
        command_matrix = malloc(sizeof(char**));
        while(tokenized_string){
            if(strcmp(tokenized_string,"|")!=0){
            command_matrix[n_commands] = realloc(command_matrix[n_commands], sizeof(char*) * ++n_args); // aloca espaço para mais um comando

            command_matrix[n_commands][n_args-1] = tokenized_string;
            }else{
                command_matrix[n_commands] = realloc(command_matrix[n_commands], sizeof(char*) * ++n_args); // aloca espaço para mais um comando
                command_matrix[n_commands][n_args-1] = NULL; // teermina a lista de strings de um comandos em null
                size_commands = realloc(size_commands, sizeof(int) * (n_commands+1));
                size_commands[n_commands++] = n_args;

                command_matrix = realloc(command_matrix, sizeof(char**)*(n_commands+1)); // aloca espaço para mais uma linha de comandos
                n_args=0;
            }
            tokenized_string = strtok(NULL, " ");
        }
        command_matrix[n_commands] = realloc(command_matrix[n_commands], sizeof(char*) * ++n_args); // aloca espaço para mais um comando
        command_matrix[n_commands][n_args-1] = NULL; // teermina a lista de strings de um comandos em null
        size_commands = realloc(size_commands, sizeof(int) * (n_commands+1));
        size_commands[n_commands++] = n_args;*/

        int number_commands;
        int* size_commands_array;
        char*** command_matrix = separate_commands(argv,&number_commands,&size_commands_array);


        printf("big kek");
        for(int i=0;i<number_commands;i++){
            printf("%d\n",size_commands_array[i]);
        }

        for(int i=0; i<number_commands;i++){
            for(int j=0; j<size_commands_array[i] ; j++){
                printf("%d %d %s//\n",i,j,command_matrix[i][j]);
            }
        }
        //execute_pipe(command_matrix,number_commands,size_commands_array);

    }
    wait(NULL);
    return 0;

}