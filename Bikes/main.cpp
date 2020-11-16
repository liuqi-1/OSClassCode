#include<cstdio>
#include<cstdlib>
#include<pthread.h>
#include<unistd.h>
#include<random>
#include<semaphore.h>
#include<algorithm>
#include<time.h>


//文件部分，缓存部分
//互斥锁部分
pthread_mutex_t lock;
sem_t dormNorth;//雁北园宿舍停放点，编号1
sem_t dormSouth;//雁南宿舍停放点，编号2
sem_t classRooms;//教学楼停放点，编号3
sem_t delivery;//菜鸟裹裹停放点，编号4

//用车函数，取车，用车，还车
//目的地和出发点随机
void* usingCars(void* args) {
	//随机数获得目的地
	srand(time(NULL));
	int from = rand() % 4 + 1;
	int to;
	while ((to = rand() % 4 + 1) == from);
	const char* from_name;
	const char* to_name;
	switch (from) {
	case 1:
		from_name = "雁北园";
		break;
	case 2:
		from_name = "雁南园";
		break;
	case 3:
		from_name = "教学楼";
		break;
	case 4:
		from_name = "菜鸟裹裹";
	}
	switch (to)
	{
	case 1:
		to_name = "雁北园";
		break;
	case 2:
		to_name = "雁南园";
		break;
	case 3:
		to_name = "教学楼";
		break;
	case 4:
		to_name = "菜鸟裹裹";
	}

	int leftBikes;//起点还剩的车辆

	//获取互斥锁
	pthread_mutex_lock(&lock);

	//获取车辆信号锁
	switch (from) {
	case 1:
		if (sem_trywait(&dormNorth)) {
			printf("我是:%ld，%s没有车，选择步行……\n", pthread_self(), from_name);
			pthread_exit(NULL);
		}
		sem_getvalue(&dormNorth, &leftBikes);
		break;
	case 2:
		if (sem_trywait(&dormSouth)) {
			printf("我是:%ld，%s没有车，选择步行……\n", pthread_self(), from_name);
			pthread_exit(NULL);
		}
		sem_getvalue(&dormSouth, &leftBikes);
		break;
	case 3:
		if (sem_trywait(&classRooms)) {
			printf("我是:%ld，%s没有车，选择步行……\n", pthread_self(), from_name);
			pthread_exit(NULL);
		}
		sem_getvalue(&classRooms, &leftBikes);
		break;
	case 4:
		if (sem_trywait(&delivery)) {
			printf("我是:%ld，%s没有车，选择步行……\n", pthread_self(), from_name);
			pthread_exit(NULL);
		}
		sem_getvalue(&delivery, &leftBikes);
	}

	//释放互斥锁
	pthread_mutex_unlock(&lock);

	//骑车过程，生成随机数时间
	int usingTime = 0;
	usingTime = rand() % 6 + 20;
	printf("我是%ld，从%s骑走一辆车，还剩下%d辆车，我将在%2f秒后到达%s……\n",
		pthread_self(), from_name, leftBikes, (float)usingTime / 10, to_name);
	usleep(usingTime * 100);

	//获取互斥锁
	pthread_mutex_lock(&lock);

	//还车
	switch (to)
	{
	case 1:
		sem_post(&dormNorth);
		sem_getvalue(&dormNorth, &leftBikes);
		break;
	case 2:
		sem_post(&dormSouth);
		sem_getvalue(&dormSouth, &leftBikes);
		break;
	case 3:
		sem_post(&classRooms);
		sem_getvalue(&classRooms, &leftBikes);
		break;
	case 4:
		sem_post(&delivery);
		sem_getvalue(&delivery, &leftBikes);
	}
	//释放互斥锁
	pthread_mutex_unlock(&lock);

	printf("我是%ld，到达%s，还车后该地还剩下%d辆车……\n", pthread_self(), to_name, leftBikes);
	pthread_exit(NULL);
}

//管理员线程函数，每隔10s将各站点车辆重置为开始状态
void* manageCars(void* args) {
	int flag = 1;//标志那个站点获得余数车辆
	while (true) {
		//等待10s
		sleep(10);

		//获取各车位点的互斥锁
		pthread_mutex_lock(&lock);

		//获取各站点的车辆数量
		int num_1 = 0;
		int num_2 = 0;
		int num_3 = 0;
		int num_4 = 0;
		sem_getvalue(&classRooms, &num_1);
		sem_getvalue(&dormNorth, &num_2);
		sem_getvalue(&dormSouth, &num_3);
		sem_getvalue(&delivery, &num_4);

		//计算平均值和余数
		int average = num_1 + num_2 + num_3 + num_4;
		int remainder = average % 4;
		average /= 4;

		//重置信号量
		sem_init(&dormNorth, 0, average);
		sem_init(&dormSouth, 0, average);
		sem_init(&classRooms, 0, average);
		sem_init(&delivery, 0, average);

		switch (flag) {
		case 1:
			sem_init(&dormSouth, 0, average + remainder);
			printf("我是管理员，现在车辆调整：\n");
			printf("雁南园：%d辆，", average + remainder);
			printf("雁北园，%d辆，", average);
			printf("教学楼：%d辆，", average);
			printf("菜鸟裹裹，%d辆\n", average);
			break;
		case 2:
			sem_init(&dormNorth, 0, average + remainder);
			printf("我是管理员，现在车辆调整：\n");
			printf("雁南园：%d辆,", average);
			printf("雁北园，%d辆,", average + remainder);
			printf("教学楼：%d辆,", average);
			printf("菜鸟裹裹，%d辆\n", average);
			break;
		case 3:
			sem_init(&classRooms, 0, average + remainder);
			printf("我是管理员，现在车辆调整：\n");
			printf("雁南园：%d辆，", average);
			printf("雁北园，%d辆，", average);
			printf("教学楼：%d辆，", average + remainder);
			printf("菜鸟裹裹，%d辆\n", average);
			break;
		case 4:
			sem_init(&delivery, 0, average + remainder);
			printf("我是管理员，现在车辆调整：\n");
			printf("雁南园：%d辆，", average);
			printf("雁北园，%d辆，", average);
			printf("教学楼：%d辆，", average);
			printf("菜鸟裹裹，%d辆\n", average + remainder);
			break;
		}

		//更新flag，换下一个站点获取余数车辆
		flag = (flag + 1) % 4 + 1;

		//释放互斥锁
		pthread_mutex_unlock(&lock);
	}
	return NULL;
}

int main() {
	//初始化信号量
	sem_init(&dormNorth, 0, 5);
	sem_init(&dormSouth, 0, 5);
	sem_init(&classRooms, 0, 5);
	sem_init(&delivery, 0, 5);

	//初始化互斥锁
	pthread_mutex_init(&lock, NULL);

	//创建管理员线程
	pthread_t admin;
	pthread_create(&admin, NULL, manageCars, NULL);

	//开始模拟
	srand(time(NULL));
	while (true) {
		pthread_t user;
		pthread_create(&user, NULL, usingCars, NULL);
		int waitTime = rand() % 1 + 3;
		sleep(waitTime);
	}
	return 0;
}