#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define ITERS 10000000   // 迭代次数，足够大以压过计时误差

// 返回当前时间（纳秒）。CLOCK_MONOTONIC 单调递增，Mac/Linux 通用
static long long now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

int main(void) {
    char buffer[3];
    int fd = open("./text.txt", O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    volatile ssize_t sink;     // 接住返回值，避免被优化掉、消除告警
    long long begin = now_ns();
    for (volatile long i = 0; i < ITERS; ++i) {
        sink = read(fd, buffer, 0);   // 0 字节读，测纯 syscall 陷入/返回开销
    }
    long long end = now_ns();
    (void)sink;

    long long total_ns = end - begin;
    printf("total: %lld ns over %d iters\n", total_ns, ITERS);
    printf("per syscall: %.2f ns\n", (double)total_ns / ITERS);

    close(fd);
    return 0;
}
