#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
    int p[2];
    pipe(p);

    if(fork() == 0){
        close(p[1]);
        int input = p[0];
        int prime;
        while(read(input, &prime, sizeof(int)) > 0){
            printf("prime %d\n", prime);
            int next[2];
            pipe(next);
            int pid = fork();
            if(pid < 0){
                fprintf(2, "fork\n");
                exit(1);
            }
            else if(pid == 0){
                // child process
                close(input);
                close(next[1]);
                input = next[0];
            }
            else{
                // parent process
                close(next[0]);
                int buf;
                while(read(input, &buf, sizeof(int)) > 0){
                    if(buf % prime != 0){
                        write(next[1], &buf, sizeof(int));
                    }
                }
                close(input);
                close(next[1]);
                wait(0);
                exit(0);
            }
        }
    }

    close(p[0]);
    for(int i = 2; i <= 35; i++){
        write(p[1], &i, sizeof(int));
    }
    close(p[1]);
    wait(0);
    exit(0);
}