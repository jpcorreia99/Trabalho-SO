#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>

// le uma linha para o line.
ssize_t readln2(int fd, char *line, size_t size){
  int i = 0; ssize_t res; int stop = 1; int j = 0; // int keepReading = 1; i
  while(i < size-1 && stop && (res = read(fd,&(line[i]),200)) ) {
     if (res){
      for(j = 0; j < res && i+j < size && stop; j++){
        if (line [i+j] == '\0') stop = 0; 
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
	if ((output_fd = open("log.idx", O_RDWR , 0666)) < 0){
		output_fd = open("log.idx", O_RDWR | O_CREAT, 0666);
		//write(output_fd,"0,",2);
		//lseek(output_fd,0,SEEK_SET);
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
	    linha_length2 = sprintf(linha,"%d,",output_length + size);
	   // lseek(output_fd,-1,SEEK_CUR);
	    write(output_fd,linha,linha_length2);
	}


void update_output_index2(int size, int index){	
	int output_fd;
	printf("INDEX : %d\n",index);
	output_fd = open("log.idx", O_RDWR | O_CREAT, 0666);
	char linha [1024];
	int index_line_size = readln2(output_fd,linha,1024);
	char *token = strtok(linha,",");
	char *penultimate_token = token; 
	token = strtok(NULL,",");
	int sum = 0;
	char buf[24];
	int aux_size;
	int current_size;
	while (token != NULL){
		if (index == 1) sum = size;
		current_size = strtol(penultimate_token,NULL,10) + sum;
		aux_size = sprintf(buf,"%d,",current_size);
		write(output_fd,buf,aux_size);
		token = strtok(NULL,",");
		index--;	
	}
	sum = size;
	current_size = strtol(penultimate_token,NULL,10) + sum;
	aux_size = sprintf(buf,"%d,",current_size);
	write(output_fd,buf,aux_size);
	close(output_fd);
}

void main(){	
	int output_fd = open("log.idx", O_RDWR , 0666);
	//write(output_fd,"0,",2);
	//close(output_fd);
	update_output_index(13);
//	update_output_index(13);
}
