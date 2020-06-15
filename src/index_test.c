#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>


char* read_line(int fd,int* bytes_read){
    *bytes_read=0;
    int total_bytes_read=0;
    int max_size = 900;
    char* res = malloc(sizeof(char)*max_size);
    int n_bytes_read=0;
    
    while((n_bytes_read=read(fd,res+total_bytes_read,1))>0){
        total_bytes_read++;
        if(res[total_bytes_read-1]=='\n'){
            res[total_bytes_read-1] = '\0';
            break;
        }
        if(total_bytes_read==max_size){
            max_size*=2; 
            res = realloc(res,sizeof(char) * (max_size));
        }
    }
    *bytes_read = total_bytes_read-1;
    return res;
}

void update_output_index2(int size, int index){	
	int output_fd;
	output_fd = open("log.idx", O_RDWR | O_CREAT, 0666);
	int index_line_size ;
	char *linha = read_line(output_fd,&index_line_size);
	char *token = strtok(linha,",");
	char *penultimate_token = token; 
	//token = strtok(NULL,",");
	int sum = 0;
	char buf[24];
	int aux_size;
	int current_size;
	lseek(output_fd,0,SEEK_SET);
	do {
		current_size = strtol(penultimate_token,NULL,10) + sum;
		aux_size = sprintf(buf,"%d,",current_size);
		write(output_fd,buf,aux_size);
		token = strtok(NULL,",");
		index--;	
		if (index == 1) sum = size;
	} while (token != NULL);
	sum = size;
	current_size = strtol(penultimate_token,NULL,10) + sum;
	aux_size = sprintf(buf,"%d,",current_size);
	write(output_fd,buf,aux_size);
	close(output_fd);
}

void main(){	
	int output_fd = open("log.idx", O_RDWR | O_TRUNC | O_CREAT , 0666);
	write(output_fd,"0,\n",3);
	close(output_fd);
	update_output_index2(13,1);
	//update_output_index2(24,2);
}
