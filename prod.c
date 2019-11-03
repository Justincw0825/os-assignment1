#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>

#define SZ 20
#define producer_N 3//生产者数量
#define Buffer_N 20//缓冲区大小

struct sharing
{
	int producing;
	int consuming;
	int Buffer[SZ];//记录写入和读取的文本
	int number;
	sem_t empty;//同步信号量empty
	sem_t full;//同步信号量full
};


int producer_id=0;//生产者线程ID
int Buffer[Buffer_N];//缓冲区
int item;
double x;

pthread_mutex_t mutex;//互斥锁
pthread_t m_producer[producer_N];


void *producer();//生产者
int ret1[producer_N];


void *producer(struct sharing *shared)//生产者的定义
{	
	producer_id++;
	int id = producer_id;

	//生成负指数参数
	double time = 0.0;
	time = (double)(rand()%100)/100;
	while(time == 0)
	{
		time = (double)(rand() % 100)/100;
	}
	time = (-1/x)*log(1.0-time);
	printf("生产者 %d号 生产数据的间隔时间为 %f\n",id,time);
	while (1)
	{	
		if (shared->consuming==1)
		{
			printf("消费者正在消费\n\n");
			sleep(2);
		}
		else if(shared->producing==0)
		{
			sleep(time);//使用负指数调节生产者和消费者生产消费的速度
			sem_wait(&(shared->empty));//p（empty）操作
			shared->producing = 1;
			item = rand() % 10;
			pthread_t pthread_self(void);
			//完成生产者的操作 
			printf("生产者 %d号 生产产品 %d，并放入仓库,线程id为：%lu，进程的id为:%lu\n",id,item,pthread_self(),getpid());//输出线程id和进程id
			shared->Buffer[shared->number]=item;
			shared->number ++;
			if(shared->number==20){
				printf("仓库已满，请等待...\n");
			}
			sem_post(&shared->full);//v(full)操作
			shared->producing = 0;
		}
		else
		{
			sleep(3);
		}
	}
	pthread_exit(0);
}

int main()
{	
	printf("请输入x的值，来决定生产者生产数据时间间隔：");
	scanf("%lf",&x);
	void *shm = NULL;
	int shmid;
	struct sharing *shared;
	shmid = shmget((key_t)3333, sizeof(struct sharing), 0666|IPC_CREAT);//创造共享内存
	shm = shmat(shmid, (void*)0, 0);
	shared = (struct sharing*) shm;
	shared->number=0;
	shared->consuming=0;
	shared->producing=0;

	//初始化信号量，在进程间共享
	int ru1=sem_init(&(shared->empty),1,Buffer_N);//初始化有界缓冲区大小
	int ru2=sem_init(&(shared->full),1,0);//初始化满缓冲区大小

	if(ru1||ru2)
	{
		printf("error init signal！");
		exit(0);
	}

	int i=0;
	for(i = 0; i < producer_N; i++){
	ret1[i]= pthread_create(&m_producer[i], NULL, producer, shared);
	}

	for(i = 0; i < producer_N; i++){
		pthread_join(m_producer[i], NULL);
	}

	if(shmdt(shm) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}

	sleep(2);
	exit(EXIT_SUCCESS);
	return 0;
}