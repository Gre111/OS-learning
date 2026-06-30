/*
 * OSTEP Process API - Question 8
 * ================================
 * Q: How can you use pipe() to communicate between two child processes?
 *
 * A: This program creates a pipe and two child processes, implementing
 *    the shell pipeline:   ls | wc -l
 *
 *    Child 1 (writer): redirects stdout → pipe write end, exec's "ls"
 *    Child 2 (reader): redirects stdin  → pipe read end,  exec's "wc -l"
 *    Parent:           closes both pipe ends, waits for both children
 *
 *    This is exactly how the shell implements the "|" operator.
 *
 *    Pipe mechanics:
 *    - pipe() creates two file descriptors: pipefd[0] (read) and pipefd[1] (write)
 *    - Data written to pipefd[1] can be read from pipefd[0]
 *    - When all write ends are closed, the reader gets EOF
 *    - dup2() is used to redirect stdin/stdout to the pipe ends
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    int pipefd[2]; /* pipefd[0] = read end, pipefd[1] = write end */

    /*
     * Create the pipe BEFORE forking, so both children inherit
     * the same pipe file descriptors.
     */
    if (pipe(pipefd) < 0) {
        fprintf(stderr, "pipe failed\n");
        exit(1);
    }

    printf("parent (pid:%d) created pipe, implementing: ls | wc -l\n",
           (int)getpid());

    /* ---- Fork Child 1: the "writer" (runs ls) ---- */
    pid_t child1 = fork();

    if (child1 < 0) {
        fprintf(stderr, "fork (child 1) failed\n");
        exit(1);
    }

    if (child1 == 0) {
        /*
         * CHILD 1: "ls"
         * Redirect stdout (fd 1) to the pipe's write end.
         * Close the read end — this child only writes.
         */
        close(pipefd[0]);                    /* Close unused read end */
        dup2(pipefd[1], STDOUT_FILENO);      /* stdout → pipe write end */
        close(pipefd[1]);                    /* Close original write fd (dup'd) */

        /* exec ls — its output goes into the pipe */
        char *args[] = { "ls", NULL };
        execvp(args[0], args);

        /* Only reached if exec fails */
        fprintf(stderr, "exec ls failed\n");
        exit(1);
    }

    /* ---- Fork Child 2: the "reader" (runs wc -l) ---- */
    pid_t child2 = fork();

    if (child2 < 0) {
        fprintf(stderr, "fork (child 2) failed\n");
        exit(1);
    }

    if (child2 == 0) {
        /*
         * CHILD 2: "wc -l"
         * Redirect stdin (fd 0) to the pipe's read end.
         * Close the write end — this child only reads.
         */
        close(pipefd[1]);                    /* Close unused write end */
        dup2(pipefd[0], STDIN_FILENO);       /* stdin → pipe read end */
        close(pipefd[0]);                    /* Close original read fd (dup'd) */

        /* exec wc -l — it reads from the pipe (which has ls's output) */
        char *args[] = { "wc", "-l", NULL };
        execvp(args[0], args);

        /* Only reached if exec fails */
        fprintf(stderr, "exec wc failed\n");
        exit(1);
    }

    /* ---- PARENT PROCESS ---- */

    /*
     * CRITICAL: Parent must close BOTH pipe ends.
     * If the parent doesn't close the write end, child 2 (wc) will
     * never see EOF on its stdin and will hang forever waiting for
     * more input.
     */
    close(pipefd[0]);
    close(pipefd[1]);

    /* Wait for both children to finish */
    int status1, status2;
    waitpid(child1, &status1, 0);
    waitpid(child2, &status2, 0);

    printf("parent (pid:%d) both children done\n", (int)getpid());
    printf("parent (pid:%d) child 1 (ls,  pid:%d) exit status: %d\n",
           (int)getpid(), (int)child1,
           WIFEXITED(status1) ? WEXITSTATUS(status1) : -1);
    printf("parent (pid:%d) child 2 (wc,  pid:%d) exit status: %d\n",
           (int)getpid(), (int)child2,
           WIFEXITED(status2) ? WEXITSTATUS(status2) : -1);

    return 0;
}
