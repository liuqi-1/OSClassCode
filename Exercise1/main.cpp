#include <cstdio>
#include<cstdlib>
#include<unistd.h>
int main()
{
    pid_t p = fork();
    if (p == 0) {
        printf("this is the second process,it's ID is: %d, the ID of it's father process is: %d\n", getpid(), getppid());
        pid_t pp = fork();
        if (pp == 0) {
            printf("this is the third process,ID is: %d, the ID of it's father process is: %d\n", getpid(), getppid());
            sleep(2);
            printf("the second process is dead, now, the ID of the third process's father process is: %d\n",getppid());
        }
        else {
            sleep(1);
            exit(0);
        }
    }
    else {
        printf("this is the main process,it's ID is: %d\n", getpid());
    }
    sleep(4);
    return 0;
}