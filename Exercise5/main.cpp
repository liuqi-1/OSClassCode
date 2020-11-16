#include<cstdio>
#include<cstdlib>
#include<pthread.h>
#include<unistd.h>

static void* work(void* args) {
	printf("son thread starts executing new code......\n");
	sleep(3);
	printf("son thread finish......\n");
}

int main() {
	//创建新线程
	pthread_t son;
	if (pthread_create(&son, NULL, work, NULL)) {
		printf("creat error!!!\n");
	}
	//让子进程执行
	pthread_detach(son);
	//主线程继续执行
	printf("main thread continue to work, and do not wait....\n");
	sleep(5);
	return 0;
}