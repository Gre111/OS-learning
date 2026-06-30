/*
 * OSTEP Process API - Question 4
 *
 * Q: Write a program that calls fork() and then calls some form of
 *    exec() to run the program /bin/ls. Try a few variants of exec(),
 *    including execl(), execle(), execlp(), execv(), execvp(), and
 *    execvpe(). Why do you think there are so many variants?
 *
 * =========================================================================
 * THE exec() FAMILY — A SYSTEMATIC BREAKDOWN
 * =========================================================================
 *
 * All exec functions replace the current process image with a new program.
 * The naming convention encodes the interface:
 *
 *   exec  +  l/v  +  p (optional)  +  e (optional)
 *          |         |                 |
 *          |         |                 +-- 'e': pass custom environment (envp[])
 *          |         +-- 'p': search PATH for the program
 *          +-- 'l': args as a va_list  (execl, execlp, execle)
 *              'v': args as a vector   (execv, execvp, execvpe)
 *
 * -------------------------------------------------------------------------
 * Variant        | Args   | PATH?  | Env?   | Signature
 * -------------------------------------------------------------------------
 * execl(...)     | list   | NO     | inherit| execl(path, arg0, ..., NULL)
 * execlp(...)    | list   | YES    | inherit| execlp(file, arg0, ..., NULL)
 * execle(...)    | list   | NO     | custom | execle(path, arg0,...,NULL,envp)
 * execv(...)     | vector | NO     | inherit| execv(path, argv[])
 * execvp(...)    | vector | YES    | inherit| execvp(file, argv[])
 * execvpe(...)   | vector | YES    | custom | execvpe(file, argv[], envp[])
 *                |        |        |        | (Linux/glibc only, not POSIX)
 * -------------------------------------------------------------------------
 *
 * WHY SO MANY VARIANTS?
 *
 * Convenience! Different situations call for different interfaces:
 *
 *   - "l" (list) variants are handy when you know the exact arguments at
 *     compile time:  execl("/bin/ls", "ls", "-la", NULL)
 *
 *   - "v" (vector) variants are better when arguments are built at runtime
 *     (e.g., from user input or config):  execvp("ls", argv)
 *
 *   - "p" variants search the PATH environment variable, so you can just
 *     say "ls" instead of "/bin/ls". Useful for portability.
 *
 *   - "e" variants let you pass a custom environment, which is important
 *     for sandboxing, setting locale, or passing secrets.
 *
 * They all do the same thing: replace the process image. The variety is
 * purely about the CALLING CONVENTION.
 *
 * Compile: clang -o q4 q4.c -Wall
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        /* ---------- CHILD PROCESS ---------- */

        /* Using execvp(): vector args + PATH search.
         * This is the most commonly used variant because:
         *   1. argv[] is flexible (easy to build dynamically)
         *   2. PATH search means we don't hardcode /bin/ls
         */
        char *argv[] = {"ls", "-l", NULL};
        printf("(child) About to execvp(\"ls\", {\"ls\", \"-l\", NULL})\n");
        printf("------- ls output follows -------\n");
        execvp("ls", argv);

        /* If we reach here, exec failed */
        perror("execvp");
        exit(EXIT_FAILURE);

        /* ---------------------------------------------------------------
         * Other ways to call the same command (uncomment one to try):
         *
         * execl:   execl("/bin/ls", "ls", "-l", (char *)NULL);
         * execlp:  execlp("ls", "ls", "-l", (char *)NULL);
         * execv:   char *argv[] = {"ls", "-l", NULL};
         *          execv("/bin/ls", argv);
         * execle:  char *envp[] = {"PATH=/bin", NULL};
         *          execle("/bin/ls", "ls", "-l", (char *)NULL, envp);
         *
         * execvpe is Linux-only (glibc extension), not available on macOS.
         * --------------------------------------------------------------- */

    } else {
        /* ---------- PARENT PROCESS ---------- */
        int status;
        if (waitpid(pid, &status, 0) < 0) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status)) {
            printf("------- end of ls output -------\n");
            printf("(parent) Child exited with status %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}
