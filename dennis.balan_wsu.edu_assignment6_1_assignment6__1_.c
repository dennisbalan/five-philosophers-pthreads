#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <string.h>
int randomGaussian(int,int);
static void *philsopher(int);
//mtuxe_t mtx is the mutex thread used in the program
pthread_mutex_t mtx;
//chopsticks is the pointer to an array of 0s and 1s that denote if a chopstick is picked up or not. It is binary. 0 means available, 1 means picked up
int *chopsticks;
//food is the pointer to an array of ints that track the amount of food each philospher eats
int *food;
//size is the number of philosophers
int size;
//philospher is the function that handles the philospher and his actions eating and thinkinig until a philospher has eaten up to a 100. A philospher waits if he can not access both chopsticks necessary for eating. If he can, he picks up the chopsticks eats(thread sleeps for a random of seconds),drops the chopsticks and thinks(sleeps for a random amount of time). It returns a null 
static void *philosopher(void * arg){
	//seed the rand
	srand(time(NULL));
	//cast void * to int i
	//i is the thread index
        int i = *(int *) arg;
	//necessary print statement for synchronization, prints the index of the philospher
	printf("Philospher %d has awoken\n",i);
	//do operations on the philospher until he has eaten up to 100
        while(food[i] <= 1){
		//create second chopstick variable j to pick up a chopstick to the left of the philospher. It is usually i+1. If i is the max size - 1, then j is 0
		int j;
		if(i == size){
			j = 0;
		}
		else{
			j = i+1;
		}
		//if both chopsticks i and j are available to pick up, eat
        	if((chopsticks[i] == 0) && (chopsticks[j] == 0)){ 
			//lock the mutex
	 		int lock = pthread_mutex_lock(&mtx);
			//error check the mutex. print error code and message if 1 exists
			if(lock == -1){
				int error = errno;
				fprintf(stderr,"Error code %d = %s\n",error,strerror(error));
			}
			//k is random amount of food eaten by a philospher		
			int k = randomGaussian(9,3);
			//print the information on the philospher: index number, amount eaten, chopsticks numbers
                        printf("%d eating %d using chopsticks %d and %d \n",i,k,i,j);
			//pick up chopsticks i and j, set both to 1 so that they will be inaccessible for other processes for the duration of the wait
			chopsticks[i] = 1;
			chopsticks[j] = 1;
			//unlock the mutex
			int unlock = pthread_mutex_unlock(&mtx);
			//erroc check
			if(unlock == -1){
				int error = errno;
				fprintf(stderr,"Error code %d = %s\n",error,strerror(error));
			}
			//sleep for k seconds to simulate the philosopher eating
			sleep(k);
			//drop the chopsticks i and j and set them both to 0
                        chopsticks[j] = 0;
                       	chopsticks[i] = 0;
			//add k to the amount of food eaten 
                        food[i] = food[i] + k;
			//l is the random amount of time a philospher eats
			int l = randomGaussian(11,7);
			//print how much the philisopher thinks
			printf("%d sleeping for %d seconds\n",i,l);
			//sleep for l seconds to simulate sleeping
			sleep(l);
		}
	}				
        return NULL;
}

int main(int argc, char **argv){
	//x is the amount of philosphers
	int x;
	//if there is a command line arg, set x to the command line arg
	if(argc == 2){
		sscanf(argv[1],"%d",&x);
	}
	//else, default is 5
	else{
		x = 5;
	}
	//set size to x-1, everything is 0 indexed
	size = x-1;
	//create array of pthreads philosphers. each thread represents a philosphers
	pthread_t philosophers[x];
	//set each element of array to 0. It has size x
	int i = 0;
	int array[x];
	for(i;i < x;i++){
		array[i] = 0;
	}
	//set chopsticks to array
	chopsticks = array;
	i = 0;
	int food_array[x];
	//initialize all the elements in food_array size x to 0 and set food to food_array
	for(i = 0;i < x;i++){
		food_array[i] = 0;
	}
	food = food_array;
	//initialize each a mutex for each philosopher x times
	while(i < x){
		int mtx_error = pthread_mutex_init(&mtx,NULL);
		//error check
		if(mtx_error == -1){
			int error = errno;
			fprintf(stderr,"Error %d = %s\n",error,strerror(error));
		}
		i++;
	}//array_int is a an array of size x that stores the ith element of its for loop in each element. It used is in indexing the pthread
	i = 0;
	int array_int[x];
	for(i = 0; i < x; i++){
		array_int[i] = i;
	}
	i = 0;
	//create a pthread using the philosphers function and the ith array_int element in a while loop that increments i until i < x. The ith element in the philsophers array carries the saved pthread
	while(i < x){
		int create = pthread_create(&philosophers[i],NULL,philosopher,&array_int[i]);
		//error check
		if(create == -1){
			int error = errno;
			fprintf(stderr,"Error %d = %s\n",error,strerror(error));
		}
		i++;
	}
	i = 0;
	//join the pthreads together and wait for them to finish. pthread_join is similiar to wait, block exectuion until the thread is done. All x amount of pthreads are joined in the while loop
	while(i < x){
		int join = pthread_join(philosophers[i],NULL);
		//error check
		if(join == -1){
			int error = errno;
			fprintf(stderr,"Error %d = %s\n",error,strerror(error));
		}
		i++;
	}
	i = 0;
	while(i < x){
		int mutex_destroy = pthread_mutex_destroy(&mtx);
		if(mutex_destroy == -1){
			int error = errno;
			fprintf(stderr,"Error %d = %s\n",error,strerror(error));
		}
		i++;
	}

}

/* successive calls to randomGaussian produce integer return values */
/* having a gaussian distribution with the given mean and standard  */
/* deviation.  Return values may be negative.                       */

int randomGaussian(int mean, int stddev) {
	double mu = 0.5 + (double) mean;
	double sigma = fabs((double) stddev);
	double f1 = sqrt(-2.0 * log((double) rand() / (double) RAND_MAX));
	double f2 = 2.0 * 3.14159265359 * (double) rand() / (double) RAND_MAX;
	if (rand() & (1 << 5)) 
		return (int) floor(mu + sigma * cos(f2) * f1);
	else            
		return (int) floor(mu + sigma * sin(f2) * f1);
}

