#include<cstdio>
#include<cstdlib>
#include<semaphore.h>
#include<unistd.h>
#include<random>

sem_t mutex1;//水井的信号量
sem_t mutex2;//缸的互斥变量
sem_t empty;//还能装的数量
sem_t full;//已经有水的数量
sem_t count;//水桶的数量


void* func(void* args) {
	while (1) {
		sleep(rand() % 3 + 1);
		sem_wait(&empty);
		//获取桶信号量
		sem_wait(&count);
		//获取水井信号量
		sem_wait(&mutex1);
		//获取缸信号量
		sem_wait(&mutex2);
		//输出打水信息
		int val = 0;
		sem_getvalue(&full, &val);
		printf("I am: %ld , fetch water, water left :%d\n",pthread_self(), val + 1);
		//释放信号量
		sem_post(&full);
		sem_post(&count);
		sem_post(&mutex1);
		sem_post(&mutex2);

		srand(time(NULL));
		sleep(rand() % 3 + 1);

		//获取空余空间的信号量
		sem_wait(&full);
		//获取桶
		sem_wait(&count);
		//获取水井
		sem_wait(&mutex1);
		//获取缸
		sem_wait(&mutex2);
		//输出打水信息
		sem_getvalue(&full, &val);
		printf("I am: %ld, drink water, water left :%d\n",pthread_self(), val);
		sem_post(&empty);
		sem_post(&count);
		sem_post(&mutex1);
		sem_post(&mutex2);
	}
}


int main() {
	//初始化信号量
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, 10);
	sem_init(&count, 0, 3);
	sem_init(&mutex1, 0, 1);
	sem_init(&mutex2, 0, 1);
	//创建线程
	pthread_t man;
	for (int i = 0; i < 5; i++) {
		pthread_create(&man, NULL, func, NULL);
	}
	pthread_join(man, NULL);
	return 0;
}