#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>

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
    if (temp != s && *temp == '\0' && value >=1)
    {
       return true;
    }
    else
    {
       return false;
    }
}


char* concatena_comando(int numero_componentes, char** comando){
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



bool comando_valido(int numero_componentes, char** comando){
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
                    if(!is_limit_valid(comando[1])){
                        write(1,"Número inválido\n",strlen("Número inválido\n"));
                        return false;
                    }
                    return true;
                }else{
                    write(1,"Falta de argumentos\n",strlen("Falta de argumentos\n"));
                    return false;
                }
            }else{
                write(1,"Comando inválido\n",strlen("Comando inválido\n"));
                return false;
            }
    }else{
        write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
        return false;
    }
}



int main(int argc, char* argv[]){
    if(argc>3){
        write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
        return 1;
    }

    if(argc>1){
    char** comando = argv+1;
    int numero_componentes = argc-1;
    printf("CHEGOU AQUI1\n");
    if(comando_valido(numero_componentes,comando)){

        char* comando_concatenado =  concatena_comando(numero_componentes,comando);
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
    }else return 1;
    /*

        int fd;
        if((fd = open("my_fifo",O_RDONLY))<0){
            perror("open");
            exit(1);
        }
        printf("Open is done\n");

        char buf[100];
        int bytes_read=0;
        while(bytes_read = read(fd,buf,100)){ 
            if(write(1,buf,bytes_read)<0){
                perror("Write");
                exit(1);
            }
        }*/
    }else{
        printf("Aqui colocar o prompt\n");
    }
    return 0;
}