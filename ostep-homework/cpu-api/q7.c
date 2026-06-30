/*
 * OSTEP Process API - Question 7
 * ================================
 * Q: What happens if the child closes STDOUT_FILENO before calling printf()?
 *
 * A: After closing stdout (file descriptor 1), printf() has nowhere to
 *    write its output. The output is simply lost — it does not appear
 *    on the terminal or anywhere else. printf() will return -1 (error)
 *    because the underlying write to fd 1 fails.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

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

        /*
         * Close standard output (file descriptor 1).
         * After this, any writes to stdout will fail because the
         * file descriptor is no longer valid.
         */
        close(STDOUT_FILENO);

        /*
         * Attempt to printf — this goes to fd 1, which is now closed.
         * The output will NOT appear anywhere. It is lost.
         */
        int ret = printf("this should not appear\n");

        /*
         * We must use stderr (fd 2) to report what happened,
         * since stdout is closed.
         */
        fprintf(stderr, "child  (pid:%d) printf() returned: %d\n",
                (int)getpid(), ret);

        if (ret < 0) {
            fprintf(stderr, "child  (pid:%d) printf() failed as expected — "
                    "stdout is closed!\n", (int)getpid());
        } else {
            /*
             * printf() may buffer internally and report success (returning
             * the number of characters). The actual failure happens when
             * the buffer is flushed. Let's force a flush to see:
             */
            int flush_ret = fflush(stdout);
            fprintf(stderr, "child  (pid:%d) printf() returned %d chars "
                    "(buffered), fflush() returned: %d\n",
                    (int)getpid(), ret, flush_ret);
            if (flush_ret != 0) {
                fprintf(stderr, "child  (pid:%d) fflush() failed — "
                        "write to closed fd detected on flush\n",
                        (int)getpid());
            }
        }
    } else {
        /* ---- PARENT PROCESS ---- */
        int status;
        waitpid(rc, &status, 0);

        /* Parent's stdout is still open and works fine */
        printf("parent (pid:%d) child %d has exited\n",
               (int)getpid(), (int)rc);
        printf("parent (pid:%d) note: closing stdout in the child "
               "did NOT affect the parent\n", (int)getpid());
    }

    return 0;
}
