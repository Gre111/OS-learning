#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

void
run(char **arguments)
{
    // arguments[index] = arg;
    // arguments[index + 1] = 0;
    if(fork() == 0){
        exec(arguments[0], arguments);
        fprintf(2, "xargs: exec %s failed\n", arguments[0]);
        exit(1);
    }
    else
        wait(0);
}

int
main(int argc, char *argv[])
{
    if(argc < 2){
        fprintf(2, "Usage: input | xargs [command]\n");
        exit(1);
    }

    char *arguments[MAXARG];
    int index = 0;
    for(int i = 1; i < argc; i++, index++){
        arguments[index] = argv[i];
    }
    
    char buf[512];
    int i = 0;
    int start = index;
    int arg_start = 0;

    while(read(0, &buf[i], 1) == 1){
        if(i >= sizeof(buf) - 1){ 
            fprintf(2, "xargs: argument too long\n");
            exit(1);
        }
        if(buf[i] == ' ' || buf[i] == '\n'){
            char c = buf[i];
            buf[i] = 0;
            arguments[index++] = &buf[arg_start];
            if(c == '\n'){
                arguments[index] = 0;
                run(arguments);
                index = start;
                i = 0;
                arg_start = 0;
                continue;
            }
            arg_start = i + 1;
        }
        i++;
    }

    if(i > 0){
        buf[i] = 0;
        arguments[index++] = &buf[arg_start];
        arguments[index] = 0;
        run(arguments);
    }
    exit(0);
}