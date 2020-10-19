#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>

int var,left, *vacc;  int n,m,o;
pthread_mutex_t compMut[300];
pthread_mutex_t zoneMut[300];
pthread_mutex_t leftMut;
  
int *x;

typedef struct s{
    int id;
    int w;
    int r;
    int p;
    float x;
} s;

typedef struct co{
    int id;
    int w;
    int r;
    int p;
    float x;
    int batchUnused;
    int batchProduced;
} co;

typedef struct zo{
    int id;
    int vacc;
    int comp;
    int slots;

} zo;

typedef struct st{
    int id;
    int round;
    int zone;
    float x;
    
} st;


    co* comp_input[300];
    zo* zone_input[300];
    st* stud_input[300];
    
void* compImpl(void* inp){
    co* inputs = (co*) inp;
//    sleep(1);

//    pthread_mutex_lock(&mutex);
    int r = inputs->r;
    while(1){
        pthread_mutex_lock(&leftMut);
        if(left<=0){
            pthread_mutex_unlock(&leftMut);
            break;
        }
        pthread_mutex_unlock(&leftMut);
     //   printf("\nleft %d  comp %d\n\n", left, inputs->id);
        pthread_mutex_lock(&compMut[inputs->id]);
            if(inputs->batchUnused){//check if all it's batches have been used
            }    
            else{ 
                inputs->w = 2+(rand()%4);
                inputs->r = 1+(rand()%5);
                inputs->p = 10+(rand()%11);
                pthread_mutex_lock(&leftMut);
                if(left<=0){ 
                    pthread_mutex_unlock(&leftMut);
                    pthread_mutex_unlock(&compMut[inputs->id]);
                    break;
                }
                pthread_mutex_unlock(&leftMut);
                printf("Pharmaceutical Company %d is preparing %d batches of vaccines which have success probability %f\n",1+inputs->id,inputs->r,inputs->x);
                sleep(inputs->w);

                pthread_mutex_lock(&leftMut);
                if(left<=0){ 
                    pthread_mutex_unlock(&leftMut);
                    pthread_mutex_unlock(&compMut[inputs->id]);
                    break;
                }
                pthread_mutex_unlock(&leftMut);
                
                inputs->batchProduced=inputs->r;//batches produced and unassigned
                //when batches are alloted to some zone this counter is reduced
                inputs->batchUnused = inputs->r;//batches assigned and unused
                //when all batches have been used batchUnused becomes 0
                //it is reduced when a batch is finished in some zone
                printf("Pharmaceutical Company %d has prepared %d batches of vaccines which have success probability %f.Waiting for all the vaccines to be used to resume production\n", 1+inputs->id,inputs->r,inputs->x);
            }
        pthread_mutex_unlock(&compMut[inputs->id]);
            
            
        
    }
    return NULL;
}

void* zoneImpl(void* inp){
    zo* inputs = (zo*) inp;
  //  sleep(1);
    //check if there's vaccines left
    int j=0,mini=8;
    while(1){
        pthread_mutex_lock(&leftMut);
        if(left<=0){
            pthread_mutex_unlock(&leftMut);
            break;
        }
        pthread_mutex_unlock(&leftMut);
    //    printf("\nleft %d  zone %d\n\n", left, inputs->id);
    if(inputs->vacc == 0 && inputs->slots==0){

        //decrease unused batch count of company
        if(inputs->comp!=-1){
            pthread_mutex_lock(&compMut[inputs->comp]);
            comp_input[inputs->comp]->batchUnused--;
            pthread_mutex_unlock(&compMut[inputs->comp]);
        }
        pthread_mutex_lock(&leftMut);
        if(left<=0){
            pthread_mutex_unlock(&leftMut);
            break;
        }
        pthread_mutex_unlock(&leftMut);
        printf("Vaccination Zone %d has run out of vaccines\n", 1+inputs->id);
        //check companies for vaccines
        for(j=0;j<=n;j++){
            j=j%n;
            pthread_mutex_lock(&leftMut);
            if(left<=0){
                pthread_mutex_unlock(&leftMut);
                break;
            }
            pthread_mutex_unlock(&leftMut);
            pthread_mutex_lock(&compMut[j]);
            if(comp_input[j]->batchProduced){// if the company has unassigned batches
                printf("Pharmaceutical company %d is delivering a vaccine batch to Vaccination Zone %d which has success probability %f\n", j+1,1+inputs->id,comp_input[j]->x);
                comp_input[j]->batchProduced--; //assigned the batch
                inputs->vacc = comp_input[j]->p; //updated no. of vaccines in zone
                inputs->comp = j;//company from which zone got vaccine
 //sleep               sleep(1); //time to get vaccine from company
                printf("Pharmaceutical company %d has delivered %d vaccines to Vaccination Zone %d, resuming vaccinations now\n",j+1,inputs->vacc,1+inputs->id);

            }
            pthread_mutex_unlock(&compMut[j]);
            if(inputs->vacc)
                break;
        }
        pthread_mutex_lock(&leftMut);
        if(left<=0){
            pthread_mutex_unlock(&leftMut);
            break;
        }
        pthread_mutex_unlock(&leftMut);


    }
    /*decide no. of slots*/
        pthread_mutex_lock(&zoneMut[inputs->id]);
        if(inputs->slots==0 && inputs->vacc!=0){   
            if(mini>inputs->vacc)
                mini=inputs->vacc;
            
            pthread_mutex_lock(&leftMut);
            if(mini>left)
                mini = left;
            pthread_mutex_unlock(&leftMut);

            inputs->slots = 1 + (rand()%mini);
            inputs->vacc -= inputs->slots;
            pthread_mutex_lock(&leftMut);
            if(left<=0){
                pthread_mutex_unlock(&leftMut);
                pthread_mutex_unlock(&zoneMut[inputs->id]);
                break;
            }
            pthread_mutex_unlock(&leftMut);
            printf("Vaccination Zone %d is ready to vaccinate with %d slots\n", 1+inputs->id, inputs->slots);
        }
        pthread_mutex_unlock(&zoneMut[inputs->id]);
    }
    //lock comp
    //number of vacc update
    //decide no slots   
}

void* studImpl(void* inp){
    st* inputs = (st*) inp;
    int k;bool check=false; int success;
    while(inputs->round<3){
        pthread_mutex_lock(&leftMut);
        if(left<=0){
            pthread_mutex_unlock(&leftMut);
            break;
        }
        pthread_mutex_unlock(&leftMut);
     //   printf("\nleft %d  stud %d\n\n", left, inputs->id);
        printf("Student %d has arrived for his %d round of Vaccination\n", 1+inputs->id, inputs->round+1);
        printf("Student %d is waiting to be allocated a slot on a Vaccination Zone\n", 1+inputs->id);
        //check zone for slots
        
        check= false;//if true means slot is assigned
        for(k=0;k<=m;k++){
            k=k%m;
            //lock zone
            pthread_mutex_lock(&zoneMut[k]);
            if(zone_input[k]->slots){//if a zone has non zero slots
                zone_input[k]->slots--;
                printf("Student %d assigned a slot on the Vaccination Zone %d and waiting to be vaccinated\n", inputs->id+1, k+1);
                check=true;
                inputs->x=comp_input[zone_input[k]->comp]->x;
                inputs->zone = k;
            }
            pthread_mutex_unlock(&zoneMut[k]);
            if(check)
                break;

        }
        sleep(1); // 1sec to give vaccine
        inputs->round++;
        printf("Student %d on Vaccination Zone %d has been vaccinated which has success probability %f\n", inputs->id+1, 1+inputs->zone, inputs->x);
        success = (rand()%100);
        if(success<100*(inputs->x)){
            success=1;
            printf("Student %d has tested positive for antibodies.\n",inputs->id+1);
            
            break;        
        }
        else{
            success=0;
            printf("Student %d has tested negative for antibodies.\n", inputs->id+1);
        }

    }
    pthread_mutex_lock(&leftMut);
    --left;
    pthread_mutex_unlock(&leftMut);
}

//check zone
//lock zone
//increase fill(slot) of the zone
//decrease zone vacc count
//increase vacc count for stud
//unlock zone
//antibody test rand num<prob=>success
//if positive done decrease left value
//else loop

int main(){
    int i,j,k;
	scanf("%d %d %d",&n,&m,&o);
    left=o;
    vacc = (int*)malloc(m*sizeof(int));
    for(j=0;j<m;j++){
        vacc[j]=0;
    }
    pthread_t company[n+5];
    pthread_t zone[m+5];
    pthread_t stud[o+5];

    for(i=0;i<n;i++){
        comp_input[i] = (co*)malloc(sizeof(co));
    	comp_input[i]->id = i;
        scanf("%f", &(comp_input[i]->x));   
        comp_input[i]->batchUnused = 0;
        comp_input[i]->batchProduced = 0;
    }
    for(j=0;j<=m+3;j++){
        zone_input[j] = (zo*)malloc(sizeof(zo));
        zone_input[j]->id = j;
        zone_input[j]->slots = 0;
        zone_input[j]->comp = -1;
    }
    for(k=0;k<=o+3;k++){
        stud_input[k] = (st*)malloc(sizeof(st));
        stud_input[k]->id = k;
        stud_input[k]->round = 0;
    }

    printf("Simulation started\n");
    pthread_mutex_init(&leftMut, NULL);
    for(i=0;i<n;i++){
        pthread_mutex_init(&compMut[i], NULL);
    }
    for(j=0;j<m;j++){
        pthread_mutex_init(&zoneMut[j], NULL);
    }
    for(i=0;i<n;i++){
        pthread_create(&company[i], NULL, compImpl, (void*)(comp_input[i]));
    }
    for (j = 0; j < m; j++)
    {
        pthread_create(&zone[j], NULL, zoneImpl, (void*)(zone_input[j]));
    }
    for (k = 0; k < o; k++)
    {
        pthread_create(&stud[k], NULL, studImpl, (void*)(stud_input[k]));
    } 

    for(i=0;i<n;i++){
        pthread_join(company[i], NULL);
    }
    for(j=0;j<m;j++){
        pthread_join(zone[j], NULL);
    }
    for(k=0;k<o;k++){
        pthread_join(stud[k], NULL);
    }
    pthread_mutex_destroy(&leftMut);
    for(i=0;i<n;i++){
        pthread_mutex_destroy(&compMut[i]);
    }
    for(j=0;j<m;j++){
        pthread_mutex_destroy(&zoneMut[j]);
    }
    printf("Simulation over\n");
	return 0;
}