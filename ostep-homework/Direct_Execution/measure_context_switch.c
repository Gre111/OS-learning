#ifdef __linux__
#define _GNU_SOURCE   // 暴露 cpu_set_t / CPU_SET / sched_setaffinity
#include <sched.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define N 10000

static long long now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

// 把当前进程绑到 0 号 CPU，确保两进程在同一个核上、测的是纯切换开销。
// Linux 用 sched_setaffinity；macOS（尤其 M 芯片）不支持绑核，留空。
static void pin_to_cpu0(void) {
#ifdef __linux__
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    if (sched_setaffinity(0, sizeof(set), &set) != 0) {
        perror("sched_setaffinity");
    }
#else
    // macOS 无法把进程钉到指定核：建议把 N 调大、多跑几次取最小值。
#endif
}

int main(){
    int pipe1fd[2], pipe2fd[2];
    pipe(pipe1fd);
    pipe(pipe2fd);
    char c = 'x';
    char buff;

    int pid = fork();
    if(pid < 0){
        exit(1);
    }    
    else if(pid == 0){
        pin_to_cpu0();
        for(int i = 0; i < N; i++){
            read(pipe1fd[0], &buff, 1);
            write(pipe2fd[1], &c, 1);
        }
    }
    else{
        pin_to_cpu0();
        long long begin = now_ns();
        for(int i = 0; i < N; i++){
            write(pipe1fd[1], &c, 1);
            read(pipe2fd[0], &buff, 1);
        }
        long long end = now_ns();
        long long total_time = end - begin;
        printf("%.2fns\n", (float)total_time / (N * 2));
    }
}