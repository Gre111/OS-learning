#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


int
main (int argc, char *argv[])
{
    printf("(parent) hello world (pid:%d)\n", (int) getpid());
    int pipefd[2];
    pipe(pipefd);
    int pid1 = fork();
    if (pid1 < 0){
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (pid1 == 0) {
        printf("(child1) hello world (pid:%d)\n", (int) getpid());
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        printf("Hello, subprocess 2!\n");
        close(pipefd[1]);
        return(0);
    }

    int pid2 = fork();
    if (pid2 < 0){
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (pid2 == 0) {
        printf("(child2) hello world (pid:%d)\n", (int) getpid());
        close(pipefd[1]);
        dup2(STDIN_FILENO, pipefd[0]);
        close(pipefd[0]);
        printf("(child2) recieve: %c\n", buf);
        return(0);
    } 
    return(0);
}