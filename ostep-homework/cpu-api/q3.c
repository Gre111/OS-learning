/*
 * OSTEP Process API - Question 3
 *
 * Q: Write a program that uses fork(). The child process should print
 *    "hello"; the parent process should print "goodbye". Try to ensure
 *    that the child prints first WITHOUT calling wait().
 *
 * A: We use pipe() for synchronization. The child prints "hello", then
 *    writes a byte to the pipe. The parent reads from the pipe (which
 *    blocks until data is available), then prints "goodbye". This
 *    guarantees ordering without wait().
 *
 * Alternative approaches (mentioned in comments):
 *   - Signals: parent blocks on sigwait(SIGUSR1), child sends kill(ppid, SIGUSR1)
 *   - vfork(): suspends parent until child calls exec() or _exit(),
 *     but vfork() is deprecated and tricky to use correctly
 *   - Shared memory + busy-wait: wasteful and inelegant
 *
 * Compile: clang -o q3 q3.c -Wall
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int pipefd[2];  /* pipefd[0] = read end, pipefd[1] = write end */

    /* Create the pipe BEFORE fork so both processes inherit both ends */
    if (pipe(pipefd) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        /* ---------- CHILD PROCESS ---------- */
        close(pipefd[0]);   /* Child doesn't need the read end */

        printf("hello\n");

        /* Signal the parent by writing a byte to the pipe.
         * The parent is blocked on read(), so this unblocks it. */
        char done = 'x';
        write(pipefd[1], &done, 1);
        close(pipefd[1]);
    } else {
        /* ---------- PARENT PROCESS ---------- */
        close(pipefd[1]);   /* Parent doesn't need the write end */

        /* Block here until the child writes something to the pipe.
         * This guarantees "hello" is printed before "goodbye". */
        char buf;
        read(pipefd[0], &buf, 1);
        close(pipefd[0]);

        printf("goodbye\n");
    }

    return 0;
}
