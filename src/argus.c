#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>

/*#include <readline/readline.h> // a apagar quando readLn estiver implementado
#include <readline/readline.h>
#include <readline/history.h>*/

//gcc -o smecep -lreadline argus.c

//mudar o handler do sigchl pequeno para não ser o ignora mas simplesmente dar reset ao comportamento

// acabar de criar a ajuda para o prompt e verificar que um comando não existe
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
        int size_string_concated = strlen(comando[0]) + 2 + strlen(comando[1]);
        char *jointCommand = malloc(sizeof(char)* size_string_concated);
        snprintf(jointCommand, size_string_concated, "%s %s", comando[0], comando[1]);
        return jointCommand;
    }else{
        return strdup(comando[0]);
    }
}



bool comando_valido(char** comando, int numero_componentes){
    if(numero_componentes>= 1){
            if(strcmp(comando[0], "-m")==0 || strcmp(comando[0],"-i")==0){ // adicionar aqui tempo-inactividade && tempo-execucao
                if(numero_componentes==2){
                    if(!is_limit_valid(comando[1])){
                        write(1,"Limite inválido\n",strlen("Limite inválido\n"));
                        return false;
                    }
                    return true;
                }else if (numero_componentes == 1){
                    write(1,"Falta de argumentos\n",strlen("Falta de argumentos\n"));
                    return false;
                }else{
                    write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
                    return false;
                }
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
                }else if(numero_componentes<2){
                    write(1,"Falta de argumentos\n",strlen("Falta de argumentos\n"));
                    return false;
                }else if(numero_componentes>2){
                    write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
                    return false;
                }
            }else if(strcmp(comando[0],"-h")==0){
                if(numero_componentes>1){
                    write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
                    return false;
                }else{
                    return true;
                } 
            }else if(strcmp(comando[0],"-o")==0){
                if(numero_componentes==2){
                    if(!is_task_number_valid(comando[1])){
                        write(1,"Número inválido\n",strlen("Número inválido\n"));
                        return false;
                    }
                    return true;
                }else if(numero_componentes<2){
                    write(1,"Falta de argumentos\n",strlen("Falta de argumentos\n"));
                    return false;
                }else if(numero_componentes>2){
                    write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
                    return false;
                }
            }else{
                write(1,"Comando inexistente\n",strlen("Comando inexistente\n"));
                return false;
            }
    }else{
        write(1,"Falta de argumentos\n",strlen("Falta de argumentos\n"));
        return false;
    }
    return false;
}


bool valid_comand_prompt(char** command, int number_of_components){
    if(number_of_components>= 1){
            if(strcmp(command[0], "-m")==0 || strcmp(command[0],"-i")==0){
                if(number_of_components==2){
                    if(!is_limit_valid(command[1])){
                        write(1,"Limite inválido\n",strlen("Limite inválido\n"));
                        return false;
                    }
                    return true;
                }else if (number_of_components == 1){
                    write(1,"Falta de argumentos\n",strlen("Falta de argumentos\n"));
                    return false;
                }else{
                    write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
                    return false;
                }
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
            }else if(strcmp(command[0],"-o")==0){
                if(number_of_components==2){
                    if(!is_task_number_valid(command[1])){
                        write(1,"Número inválido\n",strlen("Número inválido\n"));
                        return false;
                    }
                    return true;
                }else if(number_of_components<2){
                    write(1,"Falta de argumentos\n",strlen("Falta de argumentos\n"));
                    return false;
                }else if(number_of_components>2){
                    write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
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
        write(1,"Não foi introduzido um comando\n",strlen("Não foi introduzido um comando\n"));
        return false;
    }
}

void show_help(){
    write(1,"Adjuda:\n",strlen("Adjuda:\n"));
    char* string = "\t-i <secs>: Define o tempo de inactividade de comunicação num pipe anónimo\n";
    write(1,string,strlen(string));

    string = "\t-m <secs>: define o tempo máximo de execução de uma tarefa\n";
    write(1,string,strlen(string));

    string = "\t-e <tarefa>: Executa uma tarefa\n";
    write(1,string,strlen(string));

    string = "\t-l: Lista as tarefas em execução\n";
    write(1,string,strlen(string));

    string = "\t-t <número tarefa>: Termina a execução de uma tarefa\n";
    write(1,string,strlen(string));

    string = "\t-r: Lista as tarefas terminadas assim como o estado em que terminou\n";
    write(1,string,strlen(string));

    string = "\t-h: Apresenta o manual de utilização\n";
    write(1,string,strlen(string));
}


void show_help_prompt(){

    write(1,"Adjuda:\n",strlen("Adjuda:\n"));
    char* string = "\ttempo-inactividade <secs>: Define o tempo de inactividade de comunicação num pipe anónimo\n";
    write(1,string,strlen(string));

    string = "\ttempo-execucao <secs>: Define o tempo máximo de execução de uma tarefa\n";
    write(1,string,strlen(string));

    string = "\texecutar <tarefa>: Executa uma tarefa\n";
    write(1,string,strlen(string));

    string = "\tlistar: Lista as tarefas em execução\n";
    write(1,string,strlen(string));

    string = "\tterminar <número tarefa>: Termina a execução de uma tarefa\n";
    write(1,string,strlen(string));

    string = "\thistorico: Lista as tarefas terminadas assim como o estado em que terminou\n";
    write(1,string,strlen(string));

    string = "\tsair: Fecha o cliente\n";
    write(1,string,strlen(string));
    
    string = "\tajuda: Apresenta o manual de utilização\n";
    write(1,string,strlen(string));

    string = "\tNOTA: Os comandos quando se usa a aplicação através da shell também se aplicam à utilização com prompt. Tanto -e ls | wc como argus executa ls | wc são válidos\n";
    write(1,string,strlen(string));
}


// separates the line which was read by it's spaces

//NOTA, MUDAR O MALLOC
char** separate_line(char* line_to_separate, int* number_of_sublines){
    char* rest;
    char* token = strtok_r(line_to_separate," ",&rest);
    char** res = malloc(sizeof(char*)*1);
    *number_of_sublines=0;
    if(token){
        //special case of execute
        if((strcmp(token,"executar")==0) || strcmp(token,"-e")==0 ){
            res[0] = strdup("-e");
            if(rest && strlen(rest)>0){
                res = realloc(res,sizeof(char*)*2);
                res[1] = strdup(rest);
                *number_of_sublines=2;
            }else{
                *number_of_sublines=1;
            }
            return res;
        }else if(strcmp(token,"tempo-inactividade")==0){
            res[0] = strdup("-i");
        }else if(strcmp(token,"tempo-execucao")==0){
            res[0] = strdup("-m");
        }else if(strcmp(token,"listar")==0){
            res[0] = strdup("-l");
        }else if(strcmp(token,"terminar")==0){
            res[0] = strdup("-t");
        }else if(strcmp(token,"historico")==0){
            res[0] = strdup("-r");
        }else if(strcmp(token,"ajuda")==0){
            res[0] = strdup("-h");
        }else if(strcmp(token,"output")==0){
            res[0] = strdup("-o");
        }else{
            res[0] = strdup(token);
        }
        *number_of_sublines=1;
        while(token = strtok_r(NULL, " ",&rest)){
            res = realloc(res,sizeof(char*)*((*number_of_sublines)+1));
            res[(*number_of_sublines)] = strdup(token);
            (*number_of_sublines)++;
        }
    }
    return res;
}

// abre um fifo que irá ler a resposta dada pelo servidor
int read_answer(){
    sleep(1);
    int fifo_server_to_client_fd;
    printf("A abrir o fifo de escrita no lado do cliente\n");
    if((fifo_server_to_client_fd = open("aa",O_RDONLY))<0){
        perror("open");
        return 1;
    }
    printf("Open\n");
    int n_bytes =0;
    char buf[1024];
    while((n_bytes = read(fifo_server_to_client_fd,buf,1024))>0){
        write(1,buf,n_bytes);
    }
    printf("Pós leitura\n");
    close(fifo_server_to_client_fd);
    return 0;
}

int main(int argc, char* argv[]){
    if(argc>3){
        write(1,"Excesso de argumentos\n",strlen("Excesso de argumentos\n"));
        return 1;
    }

    if(argc>1){
        printf("1!\n");
        char** comando = argv+1;
        int numero_componentes = argc-1;
        if(comando_valido(comando, numero_componentes)){
            printf("Comando válido\n");
            if(strcmp(comando[0],"-h")!=0 && strcmp(comando[0],"-o")!=0){
                printf("1\n");
                char* comando_concatenado =  concatena_comando(comando, numero_componentes);

                int fd;
                if((fd = open("fifo_client_to_server",O_WRONLY))<0){
                    perror("open");
                    return 1;
                }
                write(1,"Open is done\n",strlen("Open is done\n"));
                //write(1,"Open is done\n",strlen("Open is done\n"));
                if(write(fd,comando_concatenado,strlen(comando_concatenado))<0){
                    perror("Write");
                    return 1;
                }
                close(fd);
                read_answer();
                
                free(comando_concatenado);
            }else if(strcmp(comando[0],"-h")==0) {
                show_help();
            }else {
                printf("COLOCAR O -O AQUI\n");
            }
            
        }
    }else{
        printf("2!\n");
        while(1){
            write(1,"Argus$: ",strlen("Argus$: "));
        // char* line = readline("");
            char line[1024];
            int n_bytes = readln(1,line,1024);
            line[n_bytes] = '\0';

            write(1,line, strlen(line));
            printf("\n");
            int number_of_sublines=0;
            char** separated_line = separate_line(line,&number_of_sublines);
            /*for(int i =0;i<number_of_sublines;i++){
                printf("Separated lines[%d]: %s\n",i,separated_line[i]);
            }
            printf("Number of sublines: %d\n",number_of_sublines);*/
            if(valid_comand_prompt(separated_line,number_of_sublines)){
                if(strcmp(separated_line[0],"sair")==0){
                    free(separated_line[0]);
                    free(separated_line);
                    break;
                }else if(strcmp(separated_line[0],"-h")==0){
                    printf("A entrar na ajuda\n");
                    show_help_prompt();
                }else if(strcmp(separated_line[0],"-o")==0){
                    printf("OPÇÃO -O AQUI\n");
                }else{
                    char* comando_concatenado =  concatena_comando(separated_line,number_of_sublines);
                    int fd;
                    if((fd = open("fifo_client_to_server",O_WRONLY))<0){
                        perror("open");
                        return 1;
                    }
                    write(1,"Open is done\n",strlen("Open is done\n"));


                    if(write(fd,comando_concatenado,strlen(comando_concatenado))<0){
                        perror("Write");
                        return 1;
                    }
                    close(fd);
                    read_answer();
                    free(comando_concatenado);
                }
            }else{
                printf("Falhou a validação\n");
            }
            printf("%d\n",number_of_sublines);
            for(int i =0;i<number_of_sublines;i++){
                free(separated_line[i]);
            }
            free(separated_line);
        }
    }
    return 0;
}


