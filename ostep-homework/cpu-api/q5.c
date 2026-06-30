/*
 * OSTEP Process API - Question 5
 * ================================
 * Q: What does wait() return in the hilcd? What happens if you call
 *    wait() in the child process?
 *
 * A: wait() returns -1 in the child because the child has no children
 *    of its own to wait for. errno is set to ECHILD ("No child processes").
 *    In the parent, wait() returns the PID of the terminated child.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    printf("parent (pid:%d) about to fork\n", (int)getpid());

    pid_t rc = fork();

    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        /* ---- CHILD PROCESS ---- */
        printf("child  (pid:%d) calling wait()...\n", (int)getpid());

        /*
         * The child has no children of its own, so wait() has nothing
         * to wait for. It returns -1 immediately and sets errno to ECHILD.
         */
        int wc = wait(NULL);

        printf("child  (pid:%d) wait() returned: %d\n",
               (int)getpid(), wc);
        printf("child  (pid:%d) errno = %d (%s)\n",
               (int)getpid(), errno, strerror(errno));

        if (errno == ECHILD) {
            printf("child  (pid:%d) confirmed: ECHILD - "
                   "no children to wait for\n", (int)getpid());
        }
    } else {
        /* ---- PARENT PROCESS ---- */
        /*
         * The parent calls wait() which blocks until the child terminates.
         * It returns the PID of the terminated child.
         */
        int wc = wait(NULL);

        printf("parent (pid:%d) wait() returned: %d "
               "(child pid was %d)\n",
               (int)getpid(), wc, (int)rc);

        if (wc == rc) {
            printf("parent (pid:%d) confirmed: wait() returned "
                   "the child's PID\n", (int)getpid());
        }
    }

    return 0;
}
