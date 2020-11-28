#include<cstdio>
#include<cstdlib>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include<algorithm>
#include<time.h>
#include<string.h>
#include<sys/file.h>

/*
config:
	管理员线程：每十秒工作一次
	维修员线程：每十秒工作一次
	实时记录线程：每一秒工作一次
*/

//互斥锁
pthread_mutex_t lock;

//信号量
//雁北园编号0
//雁南园编号1
//教学楼编号2
//菜鸟裹裹编号3
sem_t sems[4];

//站点名称
const char* stations[4] = { "雁北园","雁南园","教学楼","菜鸟裹裹" };
const char* stationsEn[4] = { "dormNorth ","dormSouth ","classRooms","delivery  " };

//当前各站点剩余车辆信息
int leftBikes[4] = { 5,5,5,5 };

//用车函数，取车，用车，还车
//目的地和出发点随机
void* usingCars(void* args) {
	//随机生成出发点和目的地
	int from = rand() % 4;
	int to;
	while ((to = rand() % 4) == from);
	const char* from_name = stations[from];
	const char* to_name = stations[to];

	//获取车辆
	pthread_mutex_lock(&lock);
	if (sem_trywait(&sems[from])) {
		printf("我是:%ld，%s没有车，选择步行……\n", pthread_self(), from_name);
		pthread_mutex_unlock(&lock);
		pthread_exit(NULL);
	}
	leftBikes[from]--;
	pthread_mutex_unlock(&lock);

	//计算车程
	int usingTime;
	usingTime = rand() % 6 + 50;
	printf("我是%ld，从%s骑走一辆车，还剩下%d辆车，我将在%.2f秒后到达%s……\n",
		pthread_self(), from_name, leftBikes[from], (float)usingTime / 10, to_name);

	//文件部分,记录出发信息
	FILE* fp1;
	fp1 = fopen("./start.txt", "a");
	//用户id，出发点，目的地，车程
	fprintf(fp1, "%ld\t%s\t%s\t%.2f\n", pthread_self(), from_name, to_name, (float)usingTime / 10);
	fclose(fp1);

	//模拟车是否出故障，出故障的概率保持在10%
	int r = rand() % 10 + 1;
	if (r == 1) {
		//模拟计算出故障的时间
		int dangerTime = 0;
		dangerTime = rand() % 6 + 50;
		usleep(dangerTime * 100000);
		printf("我是%ld，在从%s到%s过程中，距离%s%.2f车程的地方，车出现故障……\n",
			pthread_self(), from_name, to_name, from_name, (float)dangerTime / 10);

		//打开错误信息表
		int fp2;
		fp2 = open("./wrong.txt", O_WRONLY | O_APPEND);

		//给文件上锁
		flock(fp2, LOCK_EX);

		//将目的地写进错误信息表
		const char* endEn = stationsEn[to];
		write(fp2, endEn, 10);

		//解锁文件并且关闭
		flock(fp2, LOCK_UN);
		close(fp2);
		pthread_exit(NULL);
	}

	//骑车过程
	usleep(usingTime * 100000);


	//还车
	pthread_mutex_lock(&lock);
	sem_post(&sems[to]);
	leftBikes[to] ++;
	pthread_mutex_unlock(&lock);

	printf("我是%ld，到达%s，还车后该地还剩下%d辆车……\n", pthread_self(), to_name, leftBikes[to]);


	//文件部分,记录到达信息
	FILE* fp3;
	fp3 = fopen("./end.txt", "a");
	//用户id，出发点，目的地，车程
	flock(fileno(fp3), LOCK_EX);
	fprintf(fp3, "%ld \t%s\t%s\t%.2f\n", pthread_self(), from_name, to_name, (float)usingTime / 10);
	flock(fileno(fp3), LOCK_UN);
	fclose(fp3);
	pthread_exit(NULL);
}

//管理员线程函数，每隔10s重新分配各站点车辆数量
//将各站点现有车辆平均分配，并且轮流获取余数
void* manageCars(void* args) {
	int flag = 0;
	while (true) {
		//等待10s
		sleep(10);
		//锁定信号量
		pthread_mutex_lock(&lock);
		//计算平均值和余数
		int sum = 0;
		for (int i = 0; i < 4; i++)
			sum += leftBikes[i];
		int average = sum / 4;//平均值
		int rema = sum % 4;//余数

		//重置信号量
		sem_init(&sems[0], 0, average);
		sem_init(&sems[1], 0, average);
		sem_init(&sems[2], 0, average);
		sem_init(&sems[3], 0, average);
		leftBikes[0] = average;
		leftBikes[1] = average;
		leftBikes[2] = average;
		leftBikes[3] = average;

		//分配余数
		for (int i = 0; i < rema; i++) {
			sem_post(&sems[flag]);
			leftBikes[flag]++;
			flag = (flag + 1) % 4;
		}

		printf("我是管理员，现在车辆调整：\n");
		printf("雁北园：%d辆，", leftBikes[0]);
		printf("雁南园，%d辆，", leftBikes[1]);
		printf("教学楼：%d辆，", leftBikes[2]);
		printf("菜鸟裹裹，%d辆\n", leftBikes[3]);

		//更新flag
		flag = (flag + 1) % 4;

		//释放互斥锁
		pthread_mutex_unlock(&lock);
	}
	return NULL;
}

//实时记录各站点车辆数量，写进文件
void* realTime(void* args) {
	while (true) {
		sleep(1);
		//打开文件
		FILE* fp = fopen("./res.txt", "w");
		if (fp == NULL) {
			printf("打开res文件失败，即将退出程序……");
			exit(-1);
		}
		//写文件
		fprintf(fp, "雁北园现有车辆为：%d \n", leftBikes[0]);
		fprintf(fp, "雁南园现有车辆为：%d \n", leftBikes[1]);
		fprintf(fp, "教学楼现有车辆为：%d \n", leftBikes[2]);
		fprintf(fp, "菜鸟裹裹现有车辆为：%d\n", leftBikes[3]);

		//关闭文件
		fclose(fp);
	}
	return NULL;
}

//维修员线程
void* repairCars(void* args) {
	while (true) {
		//等待10s
		sleep(10);

		//获取信号量的互斥锁
		pthread_mutex_lock(&lock);

		//打开文件
		int fp = open("./wrong.txt", O_RDWR);
		if (fp == -1) //判断文件是否存在及可读
		{
			printf("error!");
		}

		//给文件上锁
		flock(fp, LOCK_EX);

		//读文件并且修复坏掉的车辆
		char str[1024]; //每行最大读取的字符数
		while (read(fp, str, 10))
		{
			if (strcmp(str, "dormNorth ") == 0) {
				sem_post(&sems[0]);
				leftBikes[0] += 1;
			}
			else if (strcmp(str, "dormSouth ") == 0) {
				sem_post(&sems[1]);
				leftBikes[1] += 1;
			}
			else if (strcmp(str, "classRooms") == 0) {
				sem_post(&sems[2]);
				leftBikes[2] += 1;
			}
			else if (strcmp(str, "delivery  ") == 0) {
				sem_post(&sems[3]);
				leftBikes[3] += 1;
			}
		}

		//打印输出信息
		printf("我是修理员，本次车辆调整：\n");
		printf("雁北园：%d辆，", leftBikes[0]);
		printf("雁南园，%d辆，", leftBikes[1]);
		printf("教学楼：%d辆，", leftBikes[2]);
		printf("菜鸟裹裹，%d辆\n", leftBikes[3]);

		//清空错误信息表
		ftruncate(fp, 0);

		//解除文件锁定状态
		flock(fp, LOCK_UN);

		//关闭错误信息表
		close(fp); //关闭文件

		//释放互斥锁
		pthread_mutex_unlock(&lock);
	}
}

int main() {
	//初始化各站点车辆数据(从文件读取)
	FILE* fp = fopen("./init.txt", "r");
	int data[4];
	if (fp == NULL) {
		printf("文件打开错误，即将退出程序……\n");
		exit(-1);
	}
	for (int i = 0; i < 4; i++) {
		fscanf(fp, "%d", &data[i]);
	}
	fclose(fp);

	//初始化信号量
	sem_init(&sems[0], 0, data[0]);
	sem_init(&sems[1], 0, data[1]);
	sem_init(&sems[2], 0, data[2]);
	sem_init(&sems[3], 0, data[3]);

	//初始化互斥锁
	pthread_mutex_init(&lock, NULL);

	//初始化管理员线程
	pthread_t admin;
	pthread_create(&admin, NULL, manageCars, NULL);

	//初始化实时更新线程
	pthread_t realtime;
	pthread_create(&realtime, NULL, realTime, NULL);

	//修理
	pthread_t repair;
	pthread_create(&repair, NULL, repairCars, NULL);

	//开始模拟
	srand(time(NULL));
	while (true) {
		pthread_t user;
		pthread_create(&user, NULL, usingCars, NULL);
		int waitTime = rand() % 5 + 1;//1 - 5秒随机时间产生一个用户线程
		sleep(waitTime);
	}
	return 0;
}
