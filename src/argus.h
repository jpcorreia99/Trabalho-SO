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

typedef struct index_lines{
	int index;
	int start;
	int end;
	} IndexRecord;

#define FIFO_SERVER_TO_CLIENT "fifo_server_to_client"
#define FIFO_CLIENT_TO_SERVER "fifo_client_to_server"
#define LOG_INDEX_FILE "log2.idx"
#define LOG_FILE "log.txt"

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

#endif
