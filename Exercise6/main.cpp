#include<cstdio>
#include<cstdlib>
#include<pthread.h>
#include<unistd.h>
#include<random>

int bikes = 5;
pthread_mutex_t lock;//互斥锁

//用户骑车进来，bikes数量加1
void* come(void* args) {
	srand(time(NULL));
	for (int i = 0; i < 20; i++) {
		//尝试获得互斥锁
		while (pthread_mutex_lock(&lock));
		//骑入自行车
		bikes++;
		printf("我是线程：%ld，我骑入了一辆自行车，还剩下：%d辆自行车......\n", pthread_self(), bikes);
		//释放互斥锁
		if (pthread_mutex_unlock(&lock) != 0) {
			printf("程序错误......\n");
			sleep(1);
			exit(0);
		}
		//等待随机时间，模拟另一个用户骑入
		int waitTime = rand() % 1000 + 1;
		usleep(waitTime * 1000);
	}
	return NULL;
}

//用户将车骑走，bikes减一
void* go(void* args) {
	for (int i = 0; i < 20; i++) {
		//尝试获得互斥锁
		while (pthread_mutex_lock(&lock));
		//骑走自行车
		if (bikes <= 0) {
			printf("我是线程：%ld，没有自行车了，我走路！！！\n", pthread_self());
		}
		else {
			bikes--;
			printf("我是线程：%ld，我骑走了一辆自行车，还剩下：%d辆自行车......\n", pthread_self(), bikes);
		}
		//释放锁
		if (pthread_mutex_unlock(&lock) != 0) {
			printf("程序错误.......\n");
			sleep(1);
			exit(0);
		}
		//等待随机时间，模拟另一个用户骑出
		int waitTime = rand() % 1000 + 1;
		usleep(waitTime * 1000);
	}
	return NULL;
}


int main() {
	//初始化同步锁
	pthread_mutex_init(&lock, NULL);

	//创建进程
	pthread_t comeUser;
	pthread_create(&comeUser, NULL, come, NULL);
	pthread_t goUser;
	pthread_create(&goUser, NULL, go, NULL);

	//等待进程结束
	pthread_join(comeUser, NULL);
	pthread_join(goUser, NULL);

	//输出bikes变量
	printf("最后还剩下：%d辆自行车\n", bikes);

	//销毁互斥锁
	pthread_mutex_destroy(&lock);
	return 0;
}