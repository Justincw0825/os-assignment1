#include<linux/kernel.h>
#include<linux/sched.h>
#include<sys/syscall.h>
#include<stdio.h>
#include<unistd.h>
int main()
{
	syscall(326);
	printf("正在唤醒\n");
	return 0;
}f