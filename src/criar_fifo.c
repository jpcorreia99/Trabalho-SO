
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

int main(){
    if(mkfifo("fifo",0666)<0){
        write(2,"mkfifo",strlen("mkfifo"));
        return -1;
    }
    write(2,"ff",2);
    return 0;
}