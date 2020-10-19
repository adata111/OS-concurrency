
#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<stdlib.h>
#include<string.h>
#include <wait.h>
#include<unistd.h>
#include<stdbool.h>

typedef struct pe{
    int tim;
    char name[100];
    char instr;
    int stage;
	struct timespec ts;
} pe;

typedef struct st{
	char name[100];
	char sname[100];
	char instr;
	char type;
	int noPerf;
	pthread_mutex_t singMut;
	pthread_mutex_t stMut;
}st;

pe* perf[500];	//performer struct array
st* stages[500];	//stages struct array

int t1,t2,e,a,t;	//inputs

sem_t astage;		//acoustic stages
sem_t estage;		//electric stages
sem_t stage;		//total stages
sem_t sstage;		//total stages for singers
sem_t coord;		//coordinators

void *srujana(void *inp){
	pe* inputs = (pe*) inp;
	if(inputs->tim!=0) sleep(inputs->tim); //sleep for until arrival time
	printf("\033[0;32m%s %c has arrived\n\033[0m", inputs->name, inputs->instr);
	clock_gettime(CLOCK_REALTIME, &inputs->ts);
	inputs->ts.tv_sec += t;
	if(inputs->instr == 'v'){	//acoustic
		int ret=sem_timedwait(&astage, &(inputs->ts));//check for semaphore till the impatience limit
		if(ret==-1){
			printf("\033[0;37m%s %c left because of impatience\n\033[0m", inputs->name, inputs->instr);
		}
		else{

			sem_wait(&stage);
			for(int i=0;i<a;i++)//looking for a stage
			{
				pthread_mutex_lock(&stages[i]->stMut);//lock the stage to prevent any other thread from making changes in the no. of performers variable
				if((stages[i]->noPerf)==0 && stages[i]->type=='a')
				{
					stages[i]->noPerf=1;
					strcpy(stages[i]->name, inputs->name);
					stages[i]->instr= inputs->instr;
					inputs->stage = i;
					pthread_mutex_unlock(&stages[i]->stMut);
					break;
				}
				pthread_mutex_unlock(&stages[i]->stMut);
			}
			srand(time(NULL)); //seeding
			int perfTime = t1 + rand()%(t2-t1+1);
			printf("\033[0;34m%s performing %c at acoustic stage no.%d for %d secs\n\033[0m",inputs->name,inputs->instr, 1+inputs->stage,perfTime);
			sleep(perfTime);//performer performing
			pthread_mutex_lock(&stages[inputs->stage]->stMut);
			if(stages[inputs->stage]->noPerf == 2){
				sleep(2);
			}
			pthread_mutex_unlock(&stages[inputs->stage]->stMut);
			printf("\033[0;36m%s performance at acoustic stage no.%d has ended\n\033[0m", inputs->name, 1+inputs->stage);
			sem_post(&stage);
			sem_post(&astage);
			pthread_mutex_lock(&stages[inputs->stage]->stMut);
			if(stages[inputs->stage]->noPerf == 2){	//check if singer joined, then let singer collect tshirt
				sem_wait(&coord);
				printf("\033[0;35m%s is collecting t-shirt\n\033[0m", stages[inputs->stage]->sname);
				sleep(2);		//time to give tshirt
				sem_post(&coord);
			}
			stages[inputs->stage]->noPerf = 0;
			pthread_mutex_unlock(&stages[inputs->stage]->stMut);
			sem_wait(&coord);
			printf("\033[0;35m%s is collecting t-shirt\n\033[0m", inputs->name);
			sleep(2);
			sem_post(&coord);
		}
		

	}
	else if(inputs->instr == 'b'){	//electric
	
		int ret=sem_timedwait(&estage, &(inputs->ts));
		if(ret==-1){
			printf("\033[0;37m%s %c left because of impatience\n\033[0m", inputs->name, inputs->instr);
		}
		else{

			sem_wait(&stage);
			for(int i=a;i<a+e;i++)
			{
			//	printf("%d\n", i);
				pthread_mutex_lock(&stages[i]->stMut);
				if((stages[i]->noPerf)==0 && stages[i]->type=='e')
				{
					stages[i]->noPerf=1;
					strcpy(stages[i]->name, inputs->name);
					stages[i]->instr= inputs->instr;
					inputs->stage = i;
				//	printf("%d\n", inputs->stage);
					pthread_mutex_unlock(&stages[i]->stMut);
					break;
				}
				pthread_mutex_unlock(&stages[i]->stMut);
			}
			srand(time(NULL));
			int perfTime = t1 + rand()%(t2-t1+1);
			printf("\033[0;34m%s performing %c at electric stage no.%d for %d secs\n\033[0m",inputs->name,inputs->instr, 1+inputs->stage,perfTime);
			sleep(perfTime);//performer performing
			pthread_mutex_lock(&stages[inputs->stage]->stMut);
			if(stages[inputs->stage]->noPerf == 2){
				sleep(2);
			}
			pthread_mutex_unlock(&stages[inputs->stage]->stMut);
			printf("\033[0;36m%s performance at electric stage no.%d has ended\n\033[0m", inputs->name, inputs->stage +1);
			sem_post(&stage);
			sem_post(&estage);
			pthread_mutex_lock(&stages[inputs->stage]->stMut);
			if(stages[inputs->stage]->noPerf == 2){
				sem_wait(&coord);
				printf("\033[0;35m%s is collecting t-shirt\n\033[0m", stages[inputs->stage]->sname);
				sleep(2);
				sem_post(&coord);
			}
			stages[inputs->stage]->noPerf = 0;
			pthread_mutex_unlock(&stages[inputs->stage]->stMut);
			sem_wait(&coord);
			printf("\033[0;35m%s is collecting t-shirt\n\033[0m", inputs->name);
			sleep(2);
			sem_post(&coord);
		}

	}
	else if(inputs->instr == 's'){	//singer
	//	singImpl();
		int ret=sem_timedwait(&sstage, &(inputs->ts));
		if(ret==-1){
			printf("\033[0;37m%s %c left because of impatience\n\033[0m", inputs->name, inputs->instr);
		}
		else{

			for(int i=0;i<a+e;i++)
			{
				//printf("%d %s\n", i, inputs->name);
				pthread_mutex_lock(&stages[i]->stMut);
				if(stages[i]->noPerf==0)	//now singer will be sole performer on this stage 
				{
				//	printf("hi1\n");
					sem_wait(&stage);
				//	printf("hi2\n");
					if(stages[i]->type=='a')
						sem_wait(&astage);
					else
						sem_wait(&estage);
					stages[i]->noPerf=1;
					strcpy(stages[i]->name, inputs->name);
					stages[i]->instr= inputs->instr;
					pthread_mutex_unlock(&stages[i]->stMut);
					inputs->stage = i;
					srand(time(NULL));
					int perfTime = t1 + rand()%(t2-t1+1);
					if(stages[i]->type=='a')
						printf("\033[0;34m%s is singing at acoustic stage no.%d for %d seconds\n\033[0m",inputs->name, 1+inputs->stage, perfTime);
					else
						printf("\033[0;34m%s is singing at electric stage no.%d for %d seconds\n\033[0m",inputs->name, 1+inputs->stage, perfTime);
					sleep(perfTime);
					if(stages[i]->type=='a')
						printf("\033[0;36m%s performance at acoustic stage no.%d has ended\n\033[0m", inputs->name, inputs->stage +1);
					else
						printf("\033[0;36m%s performance at electric stage no.%d has ended\n\033[0m", inputs->name, inputs->stage +1);
					
				//	pthread_mutex_unlock(&stages[i]->singMut);
					if(stages[i]->type=='a')
						sem_post(&astage);
					else
						sem_post(&estage);
					sem_post(&stage);
					pthread_mutex_lock(&stages[i]->stMut);
					stages[i]->noPerf=0;
					pthread_mutex_unlock(&stages[i]->stMut);
					sem_wait(&coord);
					printf("\033[0;35m%s is collecting t-shirt\n\033[0m", inputs->name);
					sleep(2);
					sem_post(&coord);
					
					stages[inputs->stage]->noPerf = 0;
					break;
				}
				else if(stages[i]->noPerf==1 && stages[i]->instr!='s'){	//now singer joins another performer
			//		printf("hi\n");
					pthread_mutex_lock(&stages[i]->singMut);
					stages[i]->noPerf=2;
					pthread_mutex_unlock(&stages[i]->stMut);
					strcpy(stages[i]->sname, inputs->name);
					inputs->stage = i;

					printf("\033[0;33m%s joined %s's peformance, extended by 2 secs\n\033[0m", inputs->name, stages[i]->name);
					
					pthread_mutex_unlock(&stages[i]->singMut);
					
					break;
				}
				pthread_mutex_unlock(&stages[i]->stMut);
			}
			sem_post(&sstage);
		}

	}
	else{	//electric and acous
		int ret=sem_timedwait(&stage, &(inputs->ts));
		if(ret==-1){
			printf("\033[0;37m%s %c left because of impatience\n\033[0m", inputs->name, inputs->instr);
		}
		else{
			int as, es, i;
			sem_getvalue(&astage, &as);
			sem_getvalue(&estage, &es);
			if(as>0 && es>0){	//if both stages are available, randomly choose between them 
				srand(time(NULL));
				if(rand()%2){	//acoustic chosen
					sem_wait(&astage);
					for(i=0;i<a;i++)
					{
						pthread_mutex_lock(&stages[i]->stMut);
						if((stages[i]->noPerf)==0 && stages[i]->type=='a')
						{
							stages[i]->noPerf=1;
							strcpy(stages[i]->name, inputs->name);
							stages[i]->instr= inputs->instr;
							inputs->stage = i;
							pthread_mutex_unlock(&stages[i]->stMut);
							break;
						}
						pthread_mutex_unlock(&stages[i]->stMut);
					}
					srand(time(NULL));
					int perfTime = t1 + rand()%(t2-t1+1);
					printf("\033[0;34m%s performing %c at acoustic stage no.%d for %d secs\n\033[0m",inputs->name,inputs->instr, 1+inputs->stage,perfTime);
					sleep(perfTime);//performer performing
					pthread_mutex_lock(&stages[inputs->stage]->stMut);
					if(stages[inputs->stage]->noPerf == 2){
						sleep(2);
					}
					pthread_mutex_unlock(&stages[inputs->stage]->stMut);
					printf("\033[0;36m%s performance at acoustic stage no.%d has ended\n\033[0m", inputs->name, 1+inputs->stage);
					sem_post(&astage);
					sem_post(&stage);
				}
				else{	//electric chosen
					sem_wait(&estage);
					for(int i=a;i<a+e;i++)
					{
						pthread_mutex_lock(&stages[i]->stMut);
						if((stages[i]->noPerf)==0 && stages[i]->type=='e')
						{
							stages[i]->noPerf=1;
							strcpy(stages[i]->name, inputs->name);
							stages[i]->instr= inputs->instr;
							inputs->stage = i;
							pthread_mutex_unlock(&stages[i]->stMut);
							break;
						}
						pthread_mutex_unlock(&stages[i]->stMut);
					}
					srand(time(NULL));
					int perfTime = t1 + rand()%(t2-t1+1);
					printf("\033[0;34m%s performing %c at electric stage no.%d for %d secs\n\033[0m",inputs->name,inputs->instr, 1+inputs->stage,perfTime);
					sleep(perfTime);//performer performing
					pthread_mutex_lock(&stages[inputs->stage]->stMut);
					if(stages[inputs->stage]->noPerf == 2){
						sleep(2);
					}
					pthread_mutex_unlock(&stages[inputs->stage]->stMut);
					printf("\033[0;36m%s performance at electric stage no.%d has ended\n\033[0m", inputs->name, 1+inputs->stage);
					sem_post(&estage);
					sem_post(&stage);
				}
			}
			else if(as>0){	//only acoustic is available
				sem_wait(&astage);
				for(i=0;i<a;i++)
				{
					pthread_mutex_lock(&stages[i]->stMut);
					if((stages[i]->noPerf)==0 && stages[i]->type=='a')
					{
						stages[i]->noPerf=1;
						strcpy(stages[i]->name, inputs->name);
						stages[i]->instr= inputs->instr;
						inputs->stage = i;
						pthread_mutex_unlock(&stages[i]->stMut);
						break;
					}
					pthread_mutex_unlock(&stages[i]->stMut);
				}
				srand(time(NULL));
				int perfTime = t1 + rand()%(t2-t1+1);
				printf("\033[0;34m%s performing %c at acoustic stage no. %d for %d secs\n\033[0m",inputs->name,inputs->instr, 1+inputs->stage,perfTime);
				sleep(perfTime);//performer performing
				pthread_mutex_lock(&stages[inputs->stage]->stMut);
				if(stages[inputs->stage]->noPerf == 2){
					sleep(2);
				}
				pthread_mutex_unlock(&stages[inputs->stage]->stMut);
				printf("\033[0;36m%s performance at acoustic stage no. %d has ended\n\033[0m", inputs->name, inputs->stage +1);
				sem_post(&astage);
				sem_post(&stage);
			}
			else{	//only electric is available
				sem_wait(&estage);
				for(int i=a;i<a+e;i++)
				{
					pthread_mutex_lock(&stages[i]->stMut);
					if((stages[i]->noPerf)==0 && stages[i]->type=='e')
					{
						stages[i]->noPerf=1;
						strcpy(stages[i]->name, inputs->name);
						stages[i]->instr= inputs->instr;
						inputs->stage = i;
						pthread_mutex_unlock(&stages[i]->stMut);
						break;
					}
					pthread_mutex_unlock(&stages[i]->stMut);
				}
				srand(time(NULL));
				int perfTime = t1 + rand()%(t2-t1+1);
				printf("\033[0;34m%s performing %c at electric stage no. %d for %d secs\n\033[0m",inputs->name,inputs->instr, 1+inputs->stage,perfTime);
				sleep(perfTime);//performer performing
				pthread_mutex_lock(&stages[inputs->stage]->stMut);
				if(stages[inputs->stage]->noPerf == 2){
					sleep(2);
				}
				pthread_mutex_unlock(&stages[inputs->stage]->stMut);
				printf("\033[0;36m%s performance at electric stage no.%d has ended\n\033[0m", inputs->name, 1+inputs->stage);
				sem_post(&estage);
				sem_post(&stage);
			}
			pthread_mutex_lock(&stages[inputs->stage]->stMut);
			pthread_mutex_unlock(&stages[inputs->stage]->stMut);
			pthread_mutex_lock(&stages[inputs->stage]->stMut);
			if(stages[inputs->stage]->noPerf == 2){
				sem_wait(&coord);
				printf("\033[0;35m%s is collecting t-shirt\n\033[0m", stages[inputs->stage]->sname);
				sleep(2);
				sem_post(&coord);
			}
			stages[inputs->stage]->noPerf = 0;
			pthread_mutex_unlock(&stages[inputs->stage]->stMut);
			sem_wait(&coord);
			printf("\033[0;35m%s is collecting t-shirt\n\033[0m", inputs->name);
			sleep(2);
			sem_post(&coord);
		}
	}
}

int main(){
	int k,c,i;
	scanf("%d %d %d %d %d %d %d",&k,&a,&e,&c,&t1,&t2,&t);
	pthread_t tid[k+5];
	
	sem_init(&estage, 0, e);	
	sem_init(&astage, 0, a);
	sem_init(&stage, 0, a+e);
	sem_init(&sstage, 0, a+e);
	sem_init(&coord, 0, c);
	for(i=0;i<k;i++)
		perf[i]=(pe*)malloc(sizeof(pe));
	for(i=0;i<a+e;i++)
		stages[i]=(st*)malloc(sizeof(st));
	for(i=0;i<k;i++){
		scanf("%s %c %d",perf[i]->name,&perf[i]->instr,&perf[i]->tim);
	//	scanf("%c",&perf[i]->instr);
	//	scanf("%d",&perf[i]->tim);
	}
	for(i=0;i<(a+e);i++)
	{
		stages[i]->noPerf=0;
		pthread_mutex_init(&stages[i]->stMut,NULL);
		pthread_mutex_init(&stages[i]->singMut,NULL);
		if(i<a)
			stages[i]->type='a';
		else
			stages[i]->type='e';
	}
	for(i=0;i<k;i++){
		
		pthread_create(&tid[i],NULL,srujana,(void *)(perf[i]));
		
	}
	for(i=0;i<k;i++){
		pthread_join(tid[i], NULL);
	}
	for(i=0;i<(a+e);i++){
		pthread_mutex_destroy(&stages[i]->stMut);
		pthread_mutex_destroy(&stages[i]->singMut);
	}
	sem_destroy(&estage);
	sem_destroy(&sstage);
	sem_destroy(&astage);
	sem_destroy(&stage);
	sem_destroy(&coord);
	printf("\033[0;31mFinished\n\033[0m");

}