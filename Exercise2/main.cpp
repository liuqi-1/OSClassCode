#include<cstdio>
#include<unistd.h>
#include<cstdlib>
#include<sys/wait.h>
int main() {
	//�����½���
	pid_t p = fork();
	if (p == 0) {
		//�ӽ���
		printf("the son process is excuting the new assigment !\n");
		sleep(5);
		printf("the son process is dead!");
		exit(0);
	}
	else {
		//������
		wait(NULL);
		printf("the father process continue to excute!");
	}
	return 0;
}