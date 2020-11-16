#include<cstdlib>
#include<cstdio>
#include<unistd.h>
#include<sys/wait.h>

int main() {
	pid_t p = fork();
	if (p == 0) {
		//child process
		printf("this is the child process, now this process will excute another program.\n");
		int status = execl("/home/liuqi/projects/Exercise3/exercise2",NULL);
		if (status == -1) {
			printf("error\n");
			sleep(3);
			exit(0);
		}
	}
	sleep(8);
	printf("\nthe main continue to excute....\n");
	sleep(1);
	return 0;
}