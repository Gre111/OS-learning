/*
 * OSTEP Process API - Question 2
 *
 * Q: Write a program that opens a file (with the open() system call)
 *    and then calls fork(). Can both the child and parent access the
 *    file descriptor returned by open()?  What happens when they write
 *    to the file concurrently?
 *
 * A: Yes! When fork() is called, the child inherits the parent's file
 *    descriptor table. Both parent and child share the same underlying
 *    open file description (including the file offset). They can both
 *    write to the file successfully. Because they share the offset,
 *    writes don't overwrite each other — they append sequentially.
 *    However, without synchronization the ORDER of writes is
 *    non-deterministic (here we use wait() to serialize them).
 *
 * Compile: clang -o q2 q2.c -Wall
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(void)
{
    /* Open (or create) the output file BEFORE forking.
     * O_WRONLY  - write only
     * O_CREAT   - create if it doesn't exist
     * O_TRUNC   - truncate to zero length if it exists
     * 0644      - rw-r--r-- permissions
     */
    int fd = open("q2_output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        /* ---------- CHILD PROCESS ---------- */
        /* The child inherited fd from the parent. Both share the same
         * underlying kernel open-file-description, including the offset. */
        const char *line1 = "child writes first line\n";
        const char *line2 = "child writes second line\n";
        write(fd, line1, strlen(line1));
        write(fd, line2, strlen(line2));
        printf("(child) Wrote 2 lines to q2_output.txt using fd=%d\n", fd);
    } else {
        /* ---------- PARENT PROCESS ---------- */
        /* Wait for child so writes are serialized (child first). */
        if (waitpid(pid, NULL, 0) < 0) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        const char *line1 = "parent writes first line\n";
        const char *line2 = "parent writes second line\n";
        write(fd, line1, strlen(line1));
        write(fd, line2, strlen(line2));
        printf("(parent) Wrote 2 lines to q2_output.txt using fd=%d\n", fd);

        printf("\nBoth child and parent wrote to the SAME file descriptor (fd=%d).\n", fd);
        printf("The file descriptor is shared across fork(), so both processes\n");
        printf("can write to the file. Check q2_output.txt to see the result.\n");
    }

    close(fd);
    return 0;
}
