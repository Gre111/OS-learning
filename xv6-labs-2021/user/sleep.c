#include "kernel/types.h"
#include "user/user.h"


int main(int argc, char *argv[])
{
    if(argc != 2){
        fprintf(2, "Usage: sleep time(second)\n");
        exit(1);
    }
    // char *index;
    // int index;
    int time = atoi(argv[1]);
    // for(index = 0; argv[1][index]; index++){
    //     time = time * 10 + argv[1][index]
    // }
    printf("%d\n", time);
    sleep(time);
    exit(0);
}