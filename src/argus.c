#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>

#include <readline/readline.h> // a apagar quando readLn estiver implementado
#include <readline/readline.h>
#include <readline/history.h>

//gcc -lreadline argus.c 

ssize_t readln(int fd, char* line, size_t size) {
        int i;
        for (i = 0; read(fd, line + i, 1) && line[i] != '\n' && i < size; i++)
            ;
        return i;
    }


/**
 * 
 * Checks if a given string contains a number and only a number. The number must be >= 0;
*/
bool is_limit_valid(char* s){
    /*int new_limit = atoi(s);
    printf("%d\n",new_limit);
    return (new_limit >0);*/
    char *temp;
    long value = strtol(s,&temp,10); // using base 10
    if (temp != s && *temp == '\0' && value >=0)
    {
       return true;
    }
    else
    {
       return false;
    }
}


bool is_task_number_valid(char* s){
    /*int new_limit = atoi(s);
    printf("%d\n",new_limit);
    return (new_limit >0);*/
    char *temp;
    long value = strtol(s,&temp,10); // using base 10
    if (temp != s && *temp == '\0' && value >=1)
    {
       return true;
    }
    else
    {
       return false;
    }
}


char* concatena_comando(char** comando,int numero_componentes){
    if(numero_componentes==2){
        int size_string_concated = strlen(comando[0]) + 1 + strlen(comando[1]);
        char *jointCommand = malloc(sizeof(char)* size_string_concated);
        strcpy(jointCommand,comando[0]);
        strcat(jointCommand, " ");
        strcat(jointCommand, comando[1]);
        return jointCommand;
    }else{
        return strdup(comando[0]);
    }
}



bool comando_valido(char** comando, int numero_componentes){
    if(numero_componentes>= 1 && numero_componentes <=2){
            if(strcmp(comando[0], "-m")==0 || strcmp(comando[0],"-i")==0){ // adicionar aqui tempo-inactividade && tempo-execucao
                if(numero_componentes==2){
                    if(!is_limit_valid(comando[1])){
                        write(1,"Limite inválido\n",strlen("Limite inválido\n"));
                        return false;
                    }
                    return true;
                }else{
                    write(1,"Falta de argumentos\n",strlen("Falta de argumentos\n"));
                    return false;
                }
                return true;
            }else if(strcmp(comando[0],"-e")==0){
                if(numero_componentes==1){
                    write(1,"Falta de argumentos\n",strlen("Falta de argumentos\n"));
                    return false;
                }
                return true;
            }else if(strcmp(comando[0],"-l")==0){
                if(numero_componentes>1){
                    write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
                    return false;
                }
                return true;
            }else if(strcmp(comando[0],"-r")==0){
                if(numero_componentes>1){
                    write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
                    return false;
                }
                return true;
            }else if(strcmp(comando[0],"-t")==0){
                if(numero_componentes==2){
                    if(!is_task_number_valid(comando[1])){
                        write(1,"Número inválido\n",strlen("Número inválido\n"));
                        return false;
                    }
                    return true;
                }else{
                    write(1,"Falta de argumentos\n",strlen("Falta de argumentos\n"));
                    return false;
                }
            }else if(strcmp(comando[0],"-h")==0){
                if(numero_componentes>1){
                    write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
                    return false;
                }else{
                    return true;
                } 
            }else{
                write(1,"Comando inexistente\n",strlen("Comando inexistente\n"));
                return false;
            }
    }else{
        write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
        return false;
    }
}


bool valid_comand_prompt(char** command, int number_of_components){
    if(number_of_components>= 1 && number_of_components <=2){
            if(strcmp(command[0], "-m")==0 || strcmp(command[0],"-i")==0){ // adicionar aqui tempo-inactividade && tempo-execucao
                if(number_of_components==2){
                    if(!is_limit_valid(command[1])){
                        write(1,"Limite inválido\n",strlen("Limite inválido\n"));
                        return false;
                    }
                    return true;
                }else{
                    write(1,"Falta de argumentos\n",strlen("Falta de argumentos\n"));
                    return false;
                }
                return true;
            }else if(strcmp(command[0],"-e")==0){
                if(number_of_components==1){
                    write(1,"Falta de argumentos\n",strlen("Falta de argumentos\n"));
                    return false;
                }
                return true;
            }else if(strcmp(command[0],"-l")==0){
                if(number_of_components>1){
                    write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
                    return false;
                }
                return true;
            }else if(strcmp(command[0],"-r")==0){
                if(number_of_components>1){
                    write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
                    return false;
                }
                return true;
            }else if(strcmp(command[0],"-t")==0){
                if(number_of_components==2){
                    if(!is_task_number_valid(command[1])){
                        write(1,"Número inválido\n",strlen("Número inválido\n"));
                        return false;
                    }
                    return true;
                }else{
                    write(1,"Falta de argumentos\n",strlen("Falta de argumentos\n"));
                    return false;
                }
            }else if(strcmp(command[0],"-h")==0){
                if(number_of_components>1){
                    write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
                    return false;
                }else{
                    return true;
                } 
            }else if(strcmp(command[0],"sair")==0){
                if(number_of_components>1){
                    write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
                    return false;
                }else{
                    return true;
                }
            }else{
                write(1,"Comando inexistente\n",strlen("Comando inexistente\n"));
                return false;
            }
    }else{
        write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
        return false;
    }
}

void show_help(){
    write(1,"Adjuda:\n",strlen("Adjuda:\n"));
    char* string = "\t-i <secs>: define o tempo de inactividade de comunicação num pipe anónimo\n";
    write(1,string,strlen(string));

    string = "\t-m <secs>: define o tempo máximo de execução de uma tarefa\n";
    write(1,string,strlen(string));

    string = "\t-e <tarefa>: executa uma tarefa\n";
    write(1,string,strlen(string));

    string = "\t-l: lista as tarefas em execução\n";
    write(1,string,strlen(string));

    string = "\t-t <número tarefa>: termina a execução de uma tarefa\n";
    write(1,string,strlen(string));

    string = "\t-r: Lista as tarefas terminadas assim como o estado em que terminou\n";
    write(1,string,strlen(string));
}


// separates the line which was read by it's spaces

//NOTA, MUDAR O MALLOC
char** separate_line(char* line_to_separate, int* number_of_sublines){
    char* rest;
    char* token = strtok_r(line_to_separate," ",&rest);
    char** res = malloc(sizeof(char*)*50);
    *number_of_sublines=0;
    if(token){
        //special case of execute
        if((strcmp(token,"executar")==0) || strcmp(token,"-e")==0 ){
            res[0] = "-e";
            if(rest){
                res[1] = strdup(rest);
                *number_of_sublines=2;
            }else{
                *number_of_sublines=1;
            }
            return res;
        }else if(strcmp(token,"tempo-inactividade")==0){
            res[0] = "-i";
        }else if(strcmp(token,"tempo-execucao")==0){
            res[0] = "-m";
        }else if(strcmp(token,"listar")==0){
            res[0] = "-l";
        }else if(strcmp(token,"terminar")==0){
            res[0] = "-t";
        }else if(strcmp(token,"historico")==0){
            res[0] = "-r";
        }else if(strcmp(token,"ajuda")==0){
            res[0] = "-h";
        }else{
            res[0] = strdup(token);
        }
        *number_of_sublines=1;
        while(token = strtok_r(NULL, " ",&rest)){
            res[(*number_of_sublines)] = strdup(token);
            (*number_of_sublines)++;
        }
    }
    return res;
}

int main(int argc, char* argv[]){
    if(argc>3){
        write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
        return 1;
    }

    if(argc>1){
        char** comando = argv+1;
        int numero_componentes = argc-1;
        if(comando_valido(comando, numero_componentes)){
            if(strcmp(comando[0],"-h")!=0){
                char* comando_concatenado =  concatena_comando(comando, numero_componentes);
                printf("%s\n",comando_concatenado);

                int fd;
                //write(1,"Opening fifo\n",strlen("Opening fifo\n"));
                if((fd = open("fifo",O_WRONLY))<0){
                    perror("open");
                    return 1;
                }
                //write(1,"Open is done\n",strlen("Open is done\n"));
                if(write(fd,comando_concatenado,strlen(comando_concatenado))<0){
                    perror("Write");
                    return 1;
                }
                close(fd);
            }else{
                show_help();
            }
        }
    }else{
        while(1){
            write(1,"Argus$: ",strlen("Argus$: "));
            char* line = readline("");
            write(1,line, strlen(line));
            printf("\n");
            int number_of_sublines=0;
            char** separated_line = separate_line(line,&number_of_sublines);
            for(int i =0;i<number_of_sublines;i++){
                printf("Separated lines[%d]: %s\n",i,separated_line[i]);
            }
            printf("Number of sublines: %d\n",number_of_sublines);
            if(valid_comand_prompt(separated_line,number_of_sublines)){
                if(strcmp(separated_line[0],"sair")==0){
                    break;
                }else if(strcmp(separated_line[0],"-h")==0){
                    show_help();
                }else{
                    char* comando_concatenado =  concatena_comando(separated_line,number_of_sublines);
                    printf("Comando concatenado: %s\n",comando_concatenado);

                    int fd;
                    if((fd = open("fifo",O_WRONLY))<0){
                        perror("open");
                        return 1;
                    }
                    write(1,"Open is done\n",strlen("Open is done\n"));
                    if(write(fd,comando_concatenado,strlen(comando_concatenado))<0){
                        perror("Write");
                        return 1;
                    }
                    close(fd);
                }
            }else{
                printf("Falhou a validação\n");
            }
        }
    }
    return 0;
}