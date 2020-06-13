#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>


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



void teste(int size){
	int output_fd = open("output.txt", O_RDWR, 0666);
	char linha [1024];
	int linha_length;
	int linha_length2 = readln2(output_fd,linha,1024);
	printf("TAMANHO DA PRIMEIRA LINHA: %d\n",linha_length2);
	    //output_fd = lseek(output_fd,linha_length2,SEEK_SET);
	   // lseek(output_fd, linha_length, SEEK_CUR);
	    
	    for(linha_length = linha_length2 - 3 ; linha_length >= 0 && linha[linha_length] != ','; linha_length--){
		;
	    }
	    linha_length++; 
	    int output_length = strtol(&(linha[linha_length]),NULL,10);
	    printf("LINHA LENGTH: %s\n",(&(linha[linha_length])));
	    printf("OUTPUT LENGTH: %d\n",output_length);
	    linha_length2 = sprintf(linha,"%d,\n",output_length + size);
	    lseek(output_fd,-1,SEEK_CUR);
	    write(output_fd,linha,linha_length2);
	    printf("%s\n",linha);
	}

void main(){
	teste(3);
}
