#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include<time.h>
#include<sys/time.h>


static struct timeval tm1;

static inline void start()
{
    gettimeofday(&tm1, NULL);
}

static inline void stop()
{
    struct timeval tm2;
    gettimeofday(&tm2, NULL);

    unsigned long long t = 1000 * (tm2.tv_sec - tm1.tv_sec) + (tm2.tv_usec - tm1.tv_usec) / 1000;
    printf("\n%llu ms\n", t);
}



int pids_count;
int *pids;
int limit_communication_pipe=2;


void timeout_handler(int signum) { printf("big kek\n"); }


int execute_pipe(char*** commands, int command_count,
                 int* size_commands_array) {
    if (command_count < 1) {
        return -1;
    }
    int pid;
    pids_count = command_count;
    pids = malloc(sizeof(int) * pids_count);
    if (command_count == 1) {
        if ((pid = fork()) == 0) {
            execvp(commands[0][0], commands[0]);
            _exit(-1);
        }
        pids[0] = pid;  // nota: o fork devolve o pid do filho para o pai
        return 0;
    }

    int fildes[command_count - 1][2];

    if (pipe(fildes[0]) == -1) {
        return -1;
    }
    if ((pid = fork()) == 0) {
        close(fildes[0][0]);
        dup2(fildes[0][1], 1);
        close(fildes[0][1]);
        /**
         * ler do extremo de escrita e enviar o que li para o exec
         * */
        execvp(commands[0][0], commands[0]);
        _exit(1);
    }
    pids[0] = pid;
    close(fildes[0][1]);

    int i;
    for (i = 1; i < command_count - 1; i++) {
        if (pipe(fildes[i]) == -1) {
            return -1;
        }
        if ((pid = fork()) == 0) {
            close(fildes[i][0]);
            dup2(fildes[i - 1][0], 0);
            close(fildes[i - 1][0]);
            dup2(fildes[i][1], 1);
            close(fildes[i][1]);
            execvp(commands[i][0], commands[i]);
            _exit(1);
        }
        pids[i] = pid;
        close(fildes[i - 1][0]);
        close(fildes[i][1]);
    }
    //alarm(3); // simulação do alarm de duração do pipe
    if ((pid = fork()) == 0) {
        //dup2(fildes[i - 1][0], 0);
        //close(fildes[i - 1][0]);
        char buf[5];
        int n_bytes;
        int fildes_aux[2];
        if(pipe(fildes_aux)==-1){
            return -1;
        }
        alarm(limit_communication_pipe);
        while((n_bytes = read(fildes[i - 1][0],buf,5))>0){
            alarm(0);
            alarm(limit_communication_pipe); //activa o alarme novamente
            write(fildes_aux[1],buf,n_bytes);
            write(1,buf,n_bytes);
        }
        alarm(0); // desliga o alarme no final de toda a leitura
        close(fildes_aux[1]);
        dup2(fildes_aux[0],0);
        close(fildes_aux[0]);
        execvp(commands[i][0], commands[i]);
        _exit(1);
    }

    //sleep(4);
    pids[i] = pid;  // i == command_count-1
    close(fildes[i][0]);
    for (int j = 0; j < i; j++) {
        printf("Waiting\n");
        wait(NULL);
    }

    return 0;
}


int main(int argc, char *argv[]) {
    char ** commands[4];
    char *grep[5] = { "grep", "-v", "^#", "/etc/passwd", NULL };
    char *cut[4]  = { "cut", "-f7", "-d:", NULL };
    char *unic[2] = { "uniq", NULL };
    char *wc[3]   = { "wc", "-l", NULL };
    commands[0] = grep;
    commands[1] = cut;
    commands[2] = unic;
    commands[3] = wc;
    if (signal(SIGALRM, timeout_handler) == SIG_ERR) {
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

    /*alarm(2);
    if (fork() == 0) {
        // alarm(1);
        sleep(3);
        printf("fork 1\n");
        execvp(sleep_[0], sleep_);
        printf("fork 1 terminado\n");
        _exit(1);
    }*/

    execute_pipe(commands,4,NULL);
    
    printf("fim\n");
    return 0;
}



