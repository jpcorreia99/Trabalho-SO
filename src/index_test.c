#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>

typedef struct index_lines{
	int index;
	int start;
	int end;
	} IndexRecord;


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
	if (token != NULL)
		token = strtok(NULL,",");
	int sum = 0;
	char buf[24];
	int aux_size;
	int current_size;
	lseek(output_fd,0,SEEK_SET);
	while (penultimate_token != NULL) {
		current_size = strtol(penultimate_token,NULL,10) + sum;
		aux_size = sprintf(buf,"%d,",current_size);
		write(output_fd,buf,aux_size);
		penultimate_token = token;
		if (token != NULL) {
			token = strtok(NULL,",");
		}
		if (index == 1){
			sum = size;
			current_size = sprintf(buf,"%d,",current_size + size);		
			write(output_fd,buf,current_size);
		}
		index--;	
	} 
	if (index >= 1){
		sum = size;
		current_size = sprintf(buf,"%d,",current_size + size);		
		write(output_fd,buf,current_size);
	}	
	write(output_fd,"\n",1);
	close(output_fd);
	free(linha);
}

int update_output_index3(int size, int index){	
	IndexRecord ip;
	ip.index = index;
	ip.start = 0;
	ip.end = size;
	int output_fd;
	if ((output_fd = open("log2.idx", O_RDWR | O_CREAT, 0666)) < 0)
		return -1;
	int file_size = lseek(output_fd,0,SEEK_END);
	if (abs(file_size) >= sizeof(IndexRecord)){
		file_size = lseek(output_fd,file_size - sizeof(IndexRecord),SEEK_SET);
		IndexRecord lastRecord;
		if ((read(output_fd,&lastRecord,sizeof(IndexRecord))) > 0){
			ip.start += lastRecord.end;
			ip.end += lastRecord.end;
		}
		
	}
	write(output_fd,&ip,sizeof(IndexRecord));
	close(output_fd);
	return 0;
}

int printRecordFile(){
	int output_fd;
	if ((output_fd = open("log2.idx", O_RDWR | O_CREAT, 0666)) < 0)
		return -1;
	int buf_size;
	IndexRecord t;
	while ((read(output_fd,&t,sizeof(IndexRecord))) > 0)
		printf("Index :%d, start: %d, end: %d\n",t.index,t.start,t.end);
	return 0;
	close(output_fd);
}

int get_output_from_index(int index, char **linha){
    int output_fd;
    if ((output_fd = open("log2.idx", O_RDWR | O_CREAT, 0666)) < 0)
    	return -1;
    int bytes_t = lseek(output_fd,(index - 1) * sizeof(IndexRecord),SEEK_SET);
    if (bytes_t != (index - 1) * sizeof(IndexRecord)) return -3;
    IndexRecord p1;
    if (read(output_fd,&p1,sizeof(IndexRecord)) != sizeof(IndexRecord) ){
	return -1;
    }
    int start = p1.start;
    int end = p1.end;
    close(fd);
    if (start == end) {
        return -2;
        free(linha);
    }
    if ((fd = open(LOG_FILE, O_RDONLY)) < 0) {
        return -1;
        free(linha);
    }
    *line = malloc((end - start) * sizeof(char));
    lseek(fd, start, SEEK_SET);
    int res = read(fd, *line, (end - start));
    close(fd);
    free(linha);
    return res;
}


void main(){	
	int output_fd = open("log2.idx", O_RDWR | O_TRUNC | O_CREAT , 0666);
	update_output_index3(10,1);
	printRecordFile();
	update_output_index3(20,3);
	printRecordFile();
	update_output_index3(100,10);
	printRecordFile();
	update_output_index2(9,2);
	printRecordFile();
	close(output_fd);
}

