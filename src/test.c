#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void timeout_handler(int signum){
    printf("big kek\n");
}


int main(int argc, char* argv[]){

    char *sleep_[3] = { "sleep", "4",NULL };
    char *grep[5] = { "grep", "-v", "^#", "/etc/passwd", NULL };
    char *ls[3] = {"ls","-l",NULL};

    if(signal(SIGALRM,timeout_handler)==SIG_ERR){
        perror("timeouthandler error\n");
    
    }

   /* alarm(6);
    if(fork()==0){
        alarm(1);
        printf("fork 1\n");
        execvp(sleep_[0],sleep_);
        printf("fork 1 terminado\n");
        _exit(1);
    }
    printf("test\n");
*/

alarm(2);
    if(fork()==0){
        //alarm(1);
        sleep(3);
        printf("fork 1\n");
        execvp(sleep_[0],sleep_);
        printf("fork 1 terminado\n");
        _exit(1);
    }
    printf("test\n");


    sleep(8);
    return 0;
}