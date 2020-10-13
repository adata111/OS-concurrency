#define _POSIX_C_SOURCE 199309L //required for clock
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <math.h>

int * shareMem(size_t size){
     key_t mem_key = IPC_PRIVATE;
     int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
     return (int*)shmat(shm_id, NULL, 0);
}

struct arg{
	int *arr;
    long long int l;
    long long int r;
};


void selectionSort(int *arr, long long n){
	int i, j, mini, temp;
	for(i=0;i<n-1;i++){
		mini = i;
		for(j=i+1; j<n; j++){
			if(arr[j]<arr[mini])
				mini=j;
		}
		temp = arr[mini];
		arr[mini]=arr[i];
		arr[i] = temp;
	}
}

void merge(int arr[], long long low, long long int mid, long long int high){
	long long int i,j,k;
	long long int n1 = mid - low+1;
	long long int n2 = high - mid;
	int l[n1+1];int r[n2+1];
	for(i=0;i<n1;i++){
		l[i] = arr[low+i];
	}
	for(j=0;j<n2;j++){
		r[j] = arr[mid+1+j];
	}
	i=0;j=0;k=low;
	while(i<n1 && j<n2){
		if(l[i] <= r[j]){
			arr[k]=l[i];
			i++;
		}
		else{
			arr[k] = r[j];
			j++;
		}
		k++;
	}

	while(i < n1){
		arr[k] = l[i];
		k++;i++;
	}

	while(j<n2){
		arr[k] = r[j];
		k++;j++;
	}
}

void normal_mergesort(int arr[], long long low, long long high){
	
	if(low<high){
          if(high-low < 5){
               selectionSort(arr+low,high-low+1);
          }
          else{
     		long long int mid =  low + (high-low)/2;
     		normal_mergesort(arr, low,mid);
     		normal_mergesort(arr, mid+1,high);

     		merge(arr, low, mid, high);
          }
	}
}

void concurrent_mergesort(int arr[], long long int low, long long int high){
	long long int mid =  low + (high-low)/2;
	if(low<high){
		if(high-low < 5){
			selectionSort(arr+low,high-low+1);
		}
		else{
			int pid1=fork();
	        int pid2;
	        if(pid1==0){
	            concurrent_mergesort(arr, low, mid);
	            _exit(1);
	        }
	        else{
	            pid2=fork();
	            if(pid2==0){
	                concurrent_mergesort(arr, mid + 1, high);
	                _exit(1);
	            }
	            else{
	                int status;
	                waitpid(pid1, &status, 0);
	                waitpid(pid2, &status, 0);
	            }
	        }

	        merge(arr, low , mid, high);
    	     }
	}
}

void *threaded_mergesort(void* a){
     //note that we are passing a struct to the threads for simplicity.
     struct arg *args = (struct arg*) a;

     long long int l = args->l;
     long long int r = args->r;
     int *arr = args->arr;
     if(l>r) return NULL;

     if(r-l < 5){
			selectionSort(arr+l, r-l+1);
	 }
	 else{
     long long mid = l + (r-l)/2;
     //sort left half array
     struct arg a1;
     a1.l = l;
     a1.r = mid;
     a1.arr = arr;
     pthread_t tid1;
     pthread_create(&tid1, NULL, threaded_mergesort, &a1);

     //sort right half array
     struct arg a2;
     a2.l = mid+1;
     a2.r = r;
     a2.arr = arr;
     pthread_t tid2;
     pthread_create(&tid2, NULL, threaded_mergesort, &a2);

     //wait for the two halves to get sorted
     pthread_join(tid1, NULL);
     pthread_join(tid2, NULL);

     merge(arr,l,mid,r);
 	}
}

void runSorts(long long int n){

     struct timespec ts;

     //getting shared memory
     int *arr = shareMem(sizeof(int)*(n+1));
     for(int i=0;i<n;i++) scanf("%d", arr+i);

     int brr[n+1], crr[n+1];
     for(int i=0;i<n;i++){ 
          brr[i] = arr[i];
          crr[i] = arr[i];
     }

     printf("Running concurrent_mergesort for n = %lld\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     long double st = ts.tv_nsec/(1e9)+ts.tv_sec;

     //multiprocess mergesort
     concurrent_mergesort(arr, 0, n-1);
     for(int i=0; i<n; i++){
          printf("%d ",arr[i]);
     }
     printf("\n");
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t1 = en-st;

     pthread_t tid;
     struct arg a;
     a.l = 0;
     a.r = n-1;
     a.arr = crr;
     printf("Running threaded_concurrent_mergesort for n = %lld\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     st = ts.tv_nsec/(1e9)+ts.tv_sec;

     //multithreaded mergesort
     pthread_create(&tid, NULL, threaded_mergesort, &a);
     pthread_join(tid, NULL);
     for(int i=0; i<n; i++){
          printf("%d ",a.arr[i]);
     }
     printf("\n");

     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t2 = en-st;
     
     printf("Running normal_mergesort for n = %lld\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     st = ts.tv_nsec/(1e9)+ts.tv_sec;

     // normal mergesort
     normal_mergesort(brr, 0, n-1);
     for(int i=0; i<n; i++){
          printf("%d ",brr[i]);
     }
     printf("\n");
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t3 = en - st;

     printf("normal_mergesort ran:\n\t[ %Lf ] times faster than concurrent_mergesort\n\t[ %Lf ] times faster than threaded_concurrent_mergesort\n\n", t1/t3, t2/t3);
     shmdt(arr);
     return;
}

int main(){
	long long int n;
    scanf("%lld", &n);
    runSorts(n);
    return 0;
}