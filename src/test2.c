#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include<time.h>
#include<sys/time.h>
/*int main(){
    char* words[1000] = {NULL};
    int pid;
    if((pid = fork())==0){
        if(fork()==0){
            sleep(2);
            execlp("ls","ls",NULL);
        }
        sleep(2);
        printf("primeiro fork\n");
        if(fork()==0){
            sleep(4);
            printf("segundo fork\n");
            execlp("ls","ls",NULL);
        }
        execlp("ls","ls",NULL);
    }
    sleep(1);
    kill(pid,SIGSTOP);
    kill(-pid,SIGKILL);
    kill(pid,SIGKILL);



    printf("Nani!!!");
    return 0;
}*/

int timeout=0;

void timeout_handler(int signum) {
    timeout = 1;
    printf("timeout\n");
}


int main() {
    int pid;
    if((pid = fork())==0){
        if (signal(SIGALRM, timeout_handler) == SIG_ERR) {
            perror("timeouthandler error\n");
        }
        alarm(1);
        //sleep(2);
        printf("kek\n");
        if(timeout){
            _exit(1);
        }
        _exit(0);
    }   
    //kill(pid,SIGKILL);

    printf("%d\n",pid);
    int status;
    int waited_pid =wait(&status);
    if (WIFEXITED(status)){ 
        int s=WEXITSTATUS(status); //wifeexisted- devolve algo !0 se o status tiver vindo de um child process
        printf("status: %d\n",s);
    }
    printf("%d\n",waited_pid);
    return 0;

}