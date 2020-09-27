#include<stdio.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <fcntl.h>
#include <dirent.h>
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <netdb.h> /* gethostbyaddr */
#include <stdint.h>
#include "structs.h"

pthread_mutex_t mutex; //into pool
pthread_mutex_t q_mtx; //queriesn
pthread_mutex_t lock;
pthread_cond_t waiting; //pool
pthread_cond_t connecting;

int threads_to_wait = false, ready = false;

typedef struct args{
	int port;
	char* line;
}args;

void *thread_client(void *argp){ /* Thread function */
	pthread_mutex_lock(&q_mtx); 
	args *arg; 	
	arg = argp;
 	ready = true;
    pthread_cond_signal(&waiting);
    while (!threads_to_wait){
   		pthread_cond_wait(&connecting,&q_mtx);
    };
	int sock = 0; 
	int valread;  
    struct sockaddr_in serv_addr; 
	bzero(&serv_addr, sizeof(struct sockaddr_in));
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
        printf("\n Socket creation error \n"); 
        exit(EXIT_FAILURE);
    } 
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    // /serv_addr.sin_addr.s_addr= inet_addr(serverIP);
    serv_addr.sin_port = htons(arg->port); 

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ 
        perror("\nConnection Failed \n"); 
        exit(EXIT_FAILURE);
    }
    //printf("client connecting to port... %d\n", serv_addr.sin_port);
	int size_querry = strlen(arg->line);
	int bytetowrite ;
	pthread_mutex_lock(&lock);
	write(sock,&size_querry,sizeof(int));
	do{
		bytetowrite = write(sock,(char*)arg->line,size_querry);
		printf("bytetowrite are %d\n",bytetowrite );
	}while(bytetowrite < 0);
	pthread_mutex_unlock(&lock);
	usleep(1000);
	pthread_mutex_unlock(&q_mtx); 
	pthread_exit(NULL);
}

void add_thread( Thread_c **head,Thread_c *new_node ) 
{
   new_node->next = *head;
   *head = new_node;
}

int main(int argc, char* argv[]){
	if (argc!= 9){
		perror("wrong number of arguments");
		exit(EXIT_FAILURE);
	}
	int i, servPort, numThreads;
	char* queryFile ,* servIP;
	for(i=1; i<=8; i=i+2){
		if(strcmp(argv[i],"-q")==0){
			queryFile = malloc(sizeof(char)*strlen(argv[i+1])+1);
			strcpy(queryFile,argv[i+1]);	
		}
		else if(strcmp(argv[i],"-sp")==0){
			servPort=atoi(argv[i+1]);
		}
		else if(strcmp(argv[i],"-w") == 0){
			numThreads =atoi(argv[i+1]);
		}
		else if(strcmp(argv[i],"-sip") == 0){
			servIP = malloc(sizeof(char)*strlen(argv[i+1])+1);
			strcpy(servIP,argv[i+1]);
		} 
	}
	printf("here\n");
	FILE *test = fopen("queryFile.txt","r");
	int cout = 0;
	char c;;
	for (c = getc(test); c != EOF; c = getc(test)) 
        if (c == '\n') // Increment count if this character is newline 
            cout = cout + 1; 
    // Close the file 
    fclose(test); 

	FILE *fp = fopen("queryFile.txt","r");
	pthread_mutex_init(&q_mtx, 0);
	pthread_mutex_init(&mutex,0);

	pthread_cond_init(&waiting,0);
	pthread_cond_init(&connecting,0);

	char* line = NULL;
	char* token = NULL;
	size_t len = 0;
	Thread_c *head_thread = NULL; // head of the list 

  	int err , counter = 0;
  	int rest_threads = 0;
  	int j = 0;
	pthread_t* thrd =malloc(cout*sizeof(pthread_t));
	threads_to_wait = 0;
	while(getline(&line,&len,fp) != -1){
		pthread_mutex_lock(&mutex);
		args *parg = malloc(sizeof(args));
		parg->port = servPort;
		parg->line = strdup(line);
		pthread_create(thrd+j, NULL, &thread_client,parg);
		while (!ready) {
			pthread_cond_wait(&waiting, &mutex);
		}
		pthread_mutex_unlock(&mutex);
		pthread_mutex_lock(&q_mtx);
		ready = false;	
		pthread_mutex_unlock(&q_mtx);
		j++;

		threads_to_wait = true;
		pthread_cond_broadcast(&connecting);
	}	
	for (int k = 0; k < cout; ++k){
		if (err = pthread_join(*(thrd+k), NULL)){
			perror("join");
			exit(EXIT_FAILURE);
		}
	}
}