#include<cstdio>
#include<cstdlib>
#include<pthread.h>
#include<unistd.h>

static void* work(void* args) {
	printf("start work from main thread....\n");
	sleep(2);
	printf("the work is finished.....\n");
}

int main() {
	//创建新线程
	pthread_t son;
	if (pthread_create(&son, NULL, work, NULL)) {
		printf("create error!!!\n");
	}
	//让子进程执行
	pthread_join(son, NULL);
	//主线程继续执行
	printf("main thread continue to work....\n");
	sleep(5);
	return 0;
}