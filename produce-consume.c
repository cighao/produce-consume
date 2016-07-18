/*用线程的同步和互斥来实现"生产者-消费者"问题.*/
/*
* 多生产者多消费者多缓冲区 生产者和消费者不可同时进行
* 编译：gcc -o produce-consume produce-consume.c -lpthread
* 运行：./produce-consume
*/

#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#define M 10 // 缓冲数目
int in = 0;   // 生产者放置产品的位置
int out = 0; // 消费者取产品的位置
int buff[M] = {0}; // 缓冲初始化为0，开始时没有产品
int buffer=0; //单缓冲
sem_t empty_sem; // 同步信号量， 当满了时阻止生产者放产品
sem_t full_sem;   // 同步信号量， 当没产品时阻止消费者消费
pthread_mutex_t mutex; // 互斥信号量， 一次只有一个线程访问缓冲

int product_id = 0;   //生产者id
int consumer_id = 0; //消费者id
int product_sum=0;
int consumer_sum=0;
struct timeval start; //记录时间
struct timeval end;
unsigned long timer;
/* 打印缓冲情况 */
void print()
{
	int i;
	for(i = 0; i < M; i++)
   		printf("%d ", buff[i]);
}
/* 生产者方法 (多缓冲)*/ 
void *product(void * arg)
{
	int id = ++product_id;
	int t=*(int *)arg;   //t生产者生产时间
	while(1)
	{
  		 // 用sleep的数量可以调节生产和消费的速度，便于观察
  	 	sleep(t);
  	 	//sleep(1);		

 	  	sem_wait(&empty_sem);
   		pthread_mutex_lock(&mutex);

   		gettimeofday(&end,NULL);   //生产开始时间
		in = in % M;
		product_sum++;
		timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;		
   		printf("time:%ldus product_%d in %d. buffer: ", timer,id, in+1);
		//printf("timer = %ld us\n",timer);
  		buff[in] = 1;  
   		print();
		printf("\t%d\n",product_sum);  
   		++in;

   		pthread_mutex_unlock(&mutex);
   		sem_post(&full_sem);  
	}
}
/* 生产者方法 (单缓冲)*/ 
void *product1(void * arg)
{
	int id = ++product_id;
	int t=*(int *)arg;
	while(1)
	{

  	 	sleep(t);		
 	  	sem_wait(&empty_sem);
   		pthread_mutex_lock(&mutex);

   		gettimeofday(&end,NULL);
		product_sum++;
		timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;		
   		printf("time:%ld product_%d is producing.SUM=%d\n",timer,id,product_sum);
  		buffer=1;  
		
   		pthread_mutex_unlock(&mutex);
   		sem_post(&full_sem);  
	}
}
/* 生产者方法 交换wait(metux) wait(empty)*/
void *product2(void * arg)
{
	int id = ++product_id;
	int t=*(int *)arg;  
	while(1)
	{
  	 	sleep(t);
  	 	//sleep(1);		

		pthread_mutex_lock(&mutex); 	  	
		sem_wait(&empty_sem);
   		

   		gettimeofday(&end,NULL);   
		in = in % M;
		product_sum++;
		timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;		
   		printf("time:%ldus product_%d in %d. buffer: ", timer,id, in+1);
		//printf("timer = %ld us\n",timer);
  		buff[in] = 1;  
   		print();
		printf("\t%d\n",product_sum);  
   		++in;

   		pthread_mutex_unlock(&mutex);
   		sem_post(&full_sem);  
	}
}
/* 生产者方法 交换signal(metux) signal(empty)*/ 
void *product3(void * arg)
{
	int id = ++product_id;
	int t=*(int *)arg;   
	while(1)
	{
  	 	sleep(t);
  	 	//sleep(1);		

 	  	sem_wait(&empty_sem);
   		pthread_mutex_lock(&mutex);

   		gettimeofday(&end,NULL);   
		in = in % M;
		product_sum++;
		timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;		
   		printf("time:%ldus product_%d in %d. buffer: ", timer,id, in+1);
		//printf("timer = %ld us\n",timer);
  		buff[in] = 1;  
   		print();
		printf("\t%d\n",product_sum);  
   		++in;

   		sem_post(&full_sem);  
		pthread_mutex_unlock(&mutex);
	}
}
/* 生产者方法 生产者消费者同时访问缓冲区*/ 
void *product4(void * arg)
{
	int id = ++product_id;
	int t=*(int *)arg;   
	while(1)
	{
  	 	sleep(t);
  	 	//sleep(1);		

 	  	sem_wait(&empty_sem);
   		pthread_mutex_lock(&mutex);

   		gettimeofday(&end,NULL);   
		in = in % M;
		product_sum++;
		timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;		
   		printf("time:%ldus product_%d in %d. buffer: ", timer,id, in+1);
		//printf("timer = %ld us\n",timer);
  		buff[in] = 1;  
   		print();
		printf("\t%d\n",product_sum);  
   		++in;

   		sem_post(&full_sem);  
		pthread_mutex_unlock(&mutex);
	}
}
/* 消费者方法(多缓冲) */
void *consumer(void * arg)
{
	int id = ++consumer_id;
	int t=*(int *)arg;
	while(1)
	{
  	 	// 用sleep的数量可以调节生产和消费的速度
 	 	sleep(t); 
   		sem_wait(&full_sem);
  		pthread_mutex_lock(&mutex);

   		gettimeofday(&end,NULL); //消费生产时间
		consumer_sum++;
		out = out % M;    
		timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
   		printf("time:%ldus consumer_%d in %d. buffer: ", timer,id, out+1); 
   		buff[out] = 0;
   		print();
		printf("\t%d\n",consumer_sum);
  	 	++out;	

  		pthread_mutex_unlock(&mutex);
  		sem_post(&empty_sem);
	}
}
/* 消费者方法(单缓冲) */
void *consumer1(void *arg)
{
	int id = ++consumer_id;
	int t=*(int *)arg;
	while(1)
	{
 	 	sleep(t); 
   		sem_wait(&full_sem);
  		pthread_mutex_lock(&mutex);

		gettimeofday(&end,NULL);
		consumer_sum++;
		timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
   		printf("time:%ld consumer_%d is consumeing.SUM=%d\n",timer,id, consumer_sum); 
   		buffer=0;
   		

  		pthread_mutex_unlock(&mutex);
  		sem_post(&empty_sem);
	}
}
/* 消费者方法 wait(metux) wait(empty)互换 */
void *consumer2(void * arg)
{
	int id = ++consumer_id;
	int t=*(int *)arg;
	while(1)
	{
 	 	sleep(t);
		pthread_mutex_lock(&mutex); 
   		sem_wait(&full_sem);
  		
   		gettimeofday(&end,NULL); 
		consumer_sum++;
		out = out % M;    
		timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
   		printf("time:%ldus consumer_%d in %d. buffer: ", timer,id, out+1); 
   		buff[out] = 0;
   		print();
		printf("\t%d\n",consumer_sum);
  	 	++out;	

  		pthread_mutex_unlock(&mutex);
  		sem_post(&empty_sem);
	}
}
/* 消费者方法 signal(metux) signal(empty)互换*/
void *consumer3(void * arg)
{
	int id = ++consumer_id;
	int t=*(int *)arg;
	while(1)
	{
 	 	sleep(t); 
   		sem_wait(&full_sem);
  		pthread_mutex_lock(&mutex);

   		gettimeofday(&end,NULL); 
		consumer_sum++;
		out = out % M;    
		timer = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
   		printf("time:%ldus consumer_%d in %d. buffer: ", timer,id, out+1); 
   		buff[out] = 0;
   		print();
		printf("\t%d\n",consumer_sum);
  	 	++out;	

  		sem_post(&empty_sem);
		pthread_mutex_unlock(&mutex);
	}
}
/*配置生产者消费者参数，生产者和消费的数量以及时间*/
void config(int *N1,int *N2,int *T1,int *T2)
{
	*N1 = 10;
	*N2 = 10;
	*T1 = 1;
	*T2 = 1;
	// 也可以用文件来配置
	// FILE *fp;
	// if (!(fp=fopen("config.txt","r")))
	// {
	// 	printf("Error in open file!\n");
	// 	exit(1);
	// }
	// fscanf(fp,"%d %d %d %d",N1,N2,T1,T2);
	// fclose(fp);
}
/*选择菜单*/
void menu()
{
	printf("------------------------------------------------------------\n");
	printf("------1-多生产者多消费者多缓冲\n");
	printf("------2-多生产者多消费者单缓冲\n");       
	printf("------3-单生产者单消费者单缓冲\n");
	printf("------4-单生产者单消费者多缓冲\n");
	printf("------5-单生产者多消费者多缓冲\n");
	printf("------6-单生产者多消费者单缓冲\n");  
	printf("------7-多生产者单消费者多缓冲\n");
	printf("------8-多生产者单消费者单缓冲\n");
	printf("------9-生产者 wait(metux) wait(empty)互换\n");
	printf("------10-生产者 signal(metux) signal(empty)互换\n");                            
	printf("------11-消费者 wait(metux) wait(empty)互换\n");                      
	printf("------12-消费者 signal(metux) signal(empty)互换\n");
	printf("------0-退出\n");
	printf("-------------------------------------------------------------\n");
	printf("\n");
}
/*多缓冲*/
void f1(int N1,int N2,int T1,int T2,void *product,void *consumer)                                     
{
			//N1,N2分别是生产者消费者数目
			//T1，T2分别是生产消费时间
	pthread_t id1[N1];
	pthread_t id2[N2];
	int i;
	int ret1[N1];
	int ret2[N2];

	// 初始化同步信号量
	int ini1 = sem_init(&empty_sem, 0, M); //信号量将被进程内的线程共享
	int ini2 = sem_init(&full_sem, 0, 0);  
	if(ini1 && ini2 != 0)
	{
 		printf("sem init failed \n");
		exit(1);
	} 
	//初始化互斥信号量 
	int ini3 = pthread_mutex_init(&mutex, NULL);
	if(ini3 != 0)
	{
   		printf("mutex init failed \n");
   		exit(1);
	} 
	// 创建N1个生产者线程
	for(i = 0; i < N1; i++)
	{
		ret1[i] = pthread_create(&id1[i], NULL, product,&T1);
		if(ret1[i] != 0)
   		{
    			printf("product%d creation failed \n", i);
    			exit(1);
   		}
	}
	//创建N2个消费者线程
	for(i = 0; i < N2; i++)
	{
		ret2[i] = pthread_create(&id2[i], NULL, consumer, &T2);
  		if(ret2[i] != 0)
   		{
    			printf("consumer%d creation failed \n", i);
    			exit(1);
   		}
	}
	//销毁线程
	for(i = 0; i < N1; i++)
	{
		pthread_join(id1[i],NULL);
	}
	for(i = 0; i < N2; i++)
	{
   		pthread_join(id2[i],NULL);
	}
	
	exit(0); 
}
/*单缓冲*/
void f2(int N1,int N2,int T1,int T2,void *product,void *consumer)                                     
{			     
	pthread_t id1[N1];
	pthread_t id2[N2];
	int i;
	int ret1[N1];
	int ret2[N2];
	int ini1 = sem_init(&empty_sem, 0, 1); 
	int ini2 = sem_init(&full_sem, 0, 0);  
	if(ini1 && ini2 != 0)
	{
 		printf("sem init failed \n");
		exit(1);
	} 	 
	int ini3 = pthread_mutex_init(&mutex, NULL);
	if(ini3 != 0)
	{
   		printf("mutex init failed \n");
   		exit(1);
	} 	
	for(i = 0; i < N1; i++)
	{
		ret1[i] = pthread_create(&id1[i], NULL, product,&T1);
		if(ret1[i] != 0)
   		{
    			printf("product%d creation failed \n", i);
    			exit(1);
   		}
	}	
	for(i = 0; i < N2; i++)
	{
		ret2[i] = pthread_create(&id2[i], NULL, consumer,&T2);
  		if(ret2[i] != 0)
   		{
    			printf("consumer%d creation failed \n", i);
    			exit(1);
   		}
	}	
	for(i = 0; i < N1; i++)
	{
		pthread_join(id1[i],NULL);
	}
	for(i = 0; i < N2; i++)
	{
   		pthread_join(id2[i],NULL);
	}
	exit(0); 

}
int main(int argc,char* argv)
{
	
	
	menu();
	int c;	    //选择功能
	scanf("%d",&c);
	int N1,N2,T1,T2;
	config(&N1,&N2,&T1,&T2);
	switch(c)
	{
		case 1:			
			gettimeofday(&start,NULL);       //程序开始时间
			f1(N1,N2,T1,T2,product,consumer);
			break;
			
		case 2:
			gettimeofday(&start,NULL);
			f2(N1,N2,T1,T2,product1,consumer1);
			break;
		case 3:
			gettimeofday(&start,NULL);			
			f2(1,1,T1,T2,product1,consumer1);
			break;
		case 4:
			gettimeofday(&start,NULL);			
			f1(1,1,T1,T2,product,consumer);
			break;
		case 5:
			gettimeofday(&start,NULL);			
			f1(1,N2,T1,T2,product,consumer);
			break;
		case 6:
			gettimeofday(&start,NULL);			
			f2(1,N2,T1,T2,product1,consumer1);
			break;
		case 7:
			gettimeofday(&start,NULL);			
			f1(N1,1,T1,T2,product,consumer);
			break;
		case 8:
			gettimeofday(&start,NULL);			
			f2(N1,1,T1,T2,product1,consumer1);
			break;
		case 9:
			gettimeofday(&start,NULL);
			f1(N1,N2,T1,T2,product2,consumer);
			break;
		case 10:
			gettimeofday(&start,NULL);
			f1(N1,N2,T1,T2,product3,consumer);
			break;
		case 11:
			gettimeofday(&start,NULL);
			f1(N1,N2,T1,T2,product,consumer2);
			break;
		case 12:
			gettimeofday(&start,NULL);
			f1(N1,N2,T1,T2,product,consumer3);
			break;
		case 0:
			break;
	
	}
	return 0;
}
 
