/*
 * OSTEP Process API - Question 1
 *
 * Q: Write a program that calls fork(). Before calling fork(), have the
 *    main process access a variable (e.g., x) and set its value to
 *    something (e.g., 100). What value is the variable in the child
 *    process? What happens when both the child and parent change the
 *    value of x?
 *
 * A: fork() duplicates the entire address space of the parent process.
 *    The child gets its own COPY of x, initially equal to the parent's
 *    value (100). When child changes x to 200, only the child's copy
 *    changes. When parent changes x to 300, only the parent's copy
 *    changes. They are completely independent after fork().
 *
 * Compile: clang -o q1 q1.c -Wall
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int x = 100;
    printf("(main) Initial value: x = %d\n", x);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        /* ---------- CHILD PROCESS ---------- */
        printf("(child) After fork, x = %d  (same as parent's value at fork time)\n", x);
        x = 200;
        printf("(child) Changed x to %d  (only child's copy is affected)\n", x);
    } else {
        /* ---------- PARENT PROCESS ---------- */
        /* Wait for child to finish so output is easy to read */
        if (waitpid(pid, NULL, 0) < 0) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        printf("(parent) After child exits, x = %d  (parent's copy is unchanged)\n", x);
        x = 300;
        printf("(parent) Changed x to %d  (only parent's copy is affected)\n", x);
    }

    return 0;
}
