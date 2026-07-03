
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int fork1(void);

int
main(void)
{
    int p1[2], p2[2];
    pipe(p1);
    pipe(p2);

    int pid = fork();
    if(pid < 0){
        fprintf(2, "fork\n");
        exit(1);
    }
    else if(pid == 0){
        char c;
        read(p1[0], &c, 1);
        printf("%d: received ping\n", getpid());
        write(p2[1], &c, 1);
        exit(0);
    }
    char c = 'x';
    write(p1[1], &c, 1);
    read(p2[0], &c, 1);
    wait(0);
    printf("%d: received pong\n", getpid());
    exit(0);   
}