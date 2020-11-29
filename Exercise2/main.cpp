#include<cstdio>
#include<unistd.h>
#include<cstdlib>
#include<sys/wait.h>
int main() {
	//创建新进程
	pid_t p = fork();
	if (p == 0) {
		//子进程
		printf("the son process is excuting the new assigment !\n");
		sleep(5);
		printf("the son process is dead!");
		exit(0);
	}
	else {
		//主进程
		wait(NULL);
		printf("the father process continue to excute!");
	}
	return 0;
}