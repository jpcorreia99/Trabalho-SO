#ifndef ARGUS
#define ARGUS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>

typedef struct record {
    char* name;
    int status;
    int pid;
} * Record;


#define FIFO_SERVER_TO_CLIENT "fifo_server_to_client"
#define FIFO_CLIENT_TO_SERVER "fifo_client_to_server"


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

// le uma linha para o line.
ssize_t readln2(int fd, char *line, size_t size){
  int i = 0; ssize_t res; int stop = 1; int j = 0; // int keepReading = 1; i
  while(i < size-1 && stop && (res = read(fd,&(line[i]),200)) ) {
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

#endif
