#include<cstdio>
#include<cstdlib>
#include<semaphore.h>
#include<unistd.h>
#include<random>

sem_t mutex1;//ˮ�����ź���
sem_t mutex2;//�׵Ļ������
sem_t empty;//����װ������
sem_t full;//�Ѿ���ˮ������
sem_t count;//ˮͰ������


void* func(void* args) {
	while (1) {
		sleep(rand() % 3 + 1);
		sem_wait(&empty);
		//��ȡͰ�ź���
		sem_wait(&count);
		//��ȡˮ���ź���
		sem_wait(&mutex1);
		//��ȡ���ź���
		sem_wait(&mutex2);
		//�����ˮ��Ϣ
		int val = 0;
		sem_getvalue(&full, &val);
		printf("I am: %ld , fetch water, water left :%d\n",pthread_self(), val + 1);
		//�ͷ��ź���
		sem_post(&full);
		sem_post(&count);
		sem_post(&mutex1);
		sem_post(&mutex2);

		srand(time(NULL));
		sleep(rand() % 3 + 1);

		//��ȡ����ռ���ź���
		sem_wait(&full);
		//��ȡͰ
		sem_wait(&count);
		//��ȡˮ��
		sem_wait(&mutex1);
		//��ȡ��
		sem_wait(&mutex2);
		//�����ˮ��Ϣ
		sem_getvalue(&full, &val);
		printf("I am: %ld, drink water, water left :%d\n",pthread_self(), val);
		sem_post(&empty);
		sem_post(&count);
		sem_post(&mutex1);
		sem_post(&mutex2);
	}
}


int main() {
	//��ʼ���ź���
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, 10);
	sem_init(&count, 0, 3);
	sem_init(&mutex1, 0, 1);
	sem_init(&mutex2, 0, 1);
	//�����߳�
	pthread_t man;
	for (int i = 0; i < 5; i++) {
		pthread_create(&man, NULL, func, NULL);
	}
	pthread_join(man, NULL);
	return 0;
}