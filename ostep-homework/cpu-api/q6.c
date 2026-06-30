/*
 * OSTEP Process API - Question 6
 * ================================
 * Q: How does waitpid() differ from wait()?
 *
 * A: waitpid() provides more control than wait():
 *    - Wait for a SPECIFIC child by PID (not just any child)
 *    - WNOHANG flag: return immediately if child hasn't exited (non-blocking)
 *    - WUNTRACED flag: also report stopped (not just terminated) children
 *    - Useful when a parent has multiple children and needs to wait
 *      for a particular one, or needs to poll without blocking
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    printf("parent (pid:%d) about to fork\n", (int)getpid());

    pid_t child_pid = fork();

    if (child_pid < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (child_pid == 0) {
        /* ---- CHILD PROCESS ---- */
        printf("child  (pid:%d) starting work...\n", (int)getpid());

        /* Simulate some work */
        for (int i = 1; i <= 3; i++) {
            printf("child  (pid:%d) working... step %d/3\n",
                   (int)getpid(), i);
            usleep(100000); /* 100ms */
        }

        printf("child  (pid:%d) done, exiting with status 42\n",
               (int)getpid());
        exit(42); /* Exit with a distinctive status for demonstration */
    } else {
        /* ---- PARENT PROCESS ---- */
        int status;

        /*
         * waitpid(pid, &status, options):
         *   pid > 0  : wait for the specific child with that PID
         *   pid = -1 : wait for any child (like wait())
         *   pid = 0  : wait for any child in the same process group
         *   options  : 0        = block until child exits
         *              WNOHANG  = return immediately if no child has exited
         *              WUNTRACED = also report stopped children
         *
         * Here we wait for the specific child we just forked.
         */
        printf("parent (pid:%d) waiting for child %d with waitpid()...\n",
               (int)getpid(), (int)child_pid);

        pid_t rc = waitpid(child_pid, &status, 0);

        printf("parent (pid:%d) waitpid() returned: %d\n",
               (int)getpid(), (int)rc);

        /* Use status macros to inspect how the child exited */
        if (WIFEXITED(status)) {
            printf("parent (pid:%d) child exited normally with status: %d\n",
                   (int)getpid(), WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("parent (pid:%d) child killed by signal: %d\n",
                   (int)getpid(), WTERMSIG(status));
        }

        /*
         * Key advantage over wait():
         * If we had forked multiple children, we could waitpid() for
         * a specific one by its PID, whereas wait() just returns
         * whichever child finishes first.
         */
        printf("\n--- Summary ---\n");
        printf("waitpid() is more flexible than wait() because:\n");
        printf("  1. Can wait for a SPECIFIC child by PID\n");
        printf("  2. WNOHANG: non-blocking check (poll)\n");
        printf("  3. WUNTRACED: catch stopped children too\n");
        printf("  4. Essential when managing multiple children\n");
    }

    return 0;
}
