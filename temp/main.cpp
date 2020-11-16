#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int main() {
    pid_t pid1;
    pid1 = fork();
    pid_t pid2;
    if (pid1 == 0) {
        printf("the id of son:%d\n", getpid());
        printf("the id of son's father:%d\n", getppid());
        pid2 = fork();
        if (pid2 == 0) {
            printf("the id of grandson:%d\n", getpid());
            printf("the id of grandson's father:%d\n", getppid());
            sleep(5);
            printf("son process is over\n");
            printf("the id of grandson:%d\n", getpid());
            printf("the id of grandson's father:%d\n", getppid());
        }
        else {
            sleep(2);
            exit(0);
        }
    }
    else {
        printf("the id of father:%d\n", getpid());
    }
    sleep(10);
    return 0;
}
