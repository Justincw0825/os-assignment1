#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <pthread.h>
#include <math.h>

#define SZ 20
#define consumer_N 3//消费者数量

struct sharing
{
	int producing;
	int consuming;
	int Buffer[SZ];//记录写入和读取的文本
	int number;
	sem_t empty;//同步信号量empty
	sem_t full;//同步信号量full
};

int consumer_id=0;//消费者线程ID
int item;
int ret1[consumer_N];
double x;
pthread_mutex_t mutex;
pthread_t m_consumer[consumer_N];

void *consumer(struct sharing *shared)//消费者
{
	consumer_id++;
	int id = consumer_id;

	//生成负指数参数
	double time = 0.0;
	time = (double)(rand()%100)/100;
	while(time == 0)
	{
		time = (double)(rand() % 100)/100;
	}
	time = (-1/x)*log(1.0-time);
	printf("消费者 %d号 消费数据的间隔时间为 %f\n",id,time);
	while (1)
	{
		if (shared->producing==1)
		{
			sleep(2);
		}
		else if(shared->consuming==0)
		{
			sleep(time);//调节生产者和消费者生产消费的速度
			pthread_t pthread_self(void);
			sem_wait(&(shared->full));//p(full)操作
			shared->consuming=1;
			//完成消费者的操作
			printf("消费者 %d号 从仓库消费产品, 线程id为：%lu，进程的id为:%lu\n",id,pthread_self(),getpid());//输出线程id和进程id
			sem_post(&shared->empty);//v(empty)操作
			shared->consuming=0;
		}
		else
		{
			printf("%d",shared->producing);
			sleep(2);
		}
	}
	pthread_exit(0);
}

int main()
{
	printf("请输入x的值，来决定消费者消费数据时间间隔：");
	scanf("%lf",&x);
	void *shm = NULL;//分配的共享内存的原始首地址
	int shmid;//共享内存标识符
    struct sharing *shared;
	//创建共享内存
	shmid = shmget((key_t)3333, sizeof(struct sharing), 0666|IPC_CREAT);//创造共享内存
	shm = shmat(shmid, 0, 0);
	shared = (struct sharing*) shm;
	shared->consuming=0;
	shared->producing=0;
	int i=0;
	int ret2[3];
	for(i = 0; i < consumer_N; i++)
	{
		ret1[i]= pthread_create(&m_consumer[i], NULL, consumer, shared);
	}
	printf("创建线程\n\n");
	for(i = 0; i < consumer_N; i++){
		ret2[i]=pthread_join(m_consumer[i], NULL);
	}

	printf("删除线程\n\n");
	if(shmdt(shm) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	sleep(2);
	exit(EXIT_SUCCESS);
	return 0;
}