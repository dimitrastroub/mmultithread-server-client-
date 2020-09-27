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

pthread_mutex_t mutex;
pthread_mutex_t rd;
pthread_mutex_t add;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
pthread_mutex_t list;
pool_t pool;

int numFiles = 0;

typedef struct work_diseases{
	char* dis;
	char* line1;
	char* line2;
	char* line3;
	char* line4;
	int first_range;
	int sec_range;
	int third_range;
	int fourth_range;
	int total;
	struct work_diseases* next_disease;
}Work_diseases;

typedef struct work_dates{
	char* filename;
	Date* filedate;
	struct work_diseases* wds;	
	struct work_dates *next_date;
}Work_dates;

typedef struct work_countries{
	char* country;
	int counter;
	struct work_dates* wd;
	struct work_countries* next_country;
}Work_countries;


void initialize(pool_t * pool, int bufferSize) {
	pool->data = malloc(sizeof(Fd)*bufferSize);
	pool->size = bufferSize;
	pool->start = 0;
	pool->end = -1;
	pool->count = 0;
}

void place(pool_t * pool, Fd* data) {
	pthread_mutex_lock(&mutex);
	while (pool->count >= pool->size) {
		pthread_cond_wait(&cond_nonfull, &mutex);
	}
	pool->end = (pool->end + 1)%pool->size;
	pool->data[pool->end] = data;
	pool->count++;
	pthread_mutex_unlock(&mutex);
}

Fd* obtain(pool_t * pool) {
	Fd* data;
	pthread_mutex_lock(&mutex);
	while (pool->count <= 0) {
		pthread_cond_wait(&cond_nonempty, &mutex);
	}
	data = pool->data[pool->start];
	pool->start = (pool->start + 1)%pool->size;
	pool->count--;
	pthread_mutex_unlock(&mutex);
	return data;
}

void add_firstC( Work_countries **head,Work_countries *new_node ) 
{
   new_node->next_country = *head;
   *head = new_node;
}

void add_firstD( Work_dates **head,Work_dates *new_node ) 
{
   new_node->next_date = *head;
   *head = new_node;
}

void add_firstDs( Work_diseases **head,Work_diseases *new_node ) 
{
   new_node->next_disease = *head;
   *head = new_node;
}

void add_port( PortList **head,PortList *new_node ) 
{
   new_node->next = *head;
   *head = new_node;
}

int TraverseCList(char* country, Work_countries* head){
	Work_countries *ptr = head;
	while(ptr!=NULL){
		if (strcmp(ptr->country, country) == 0){
			return 1;
		}
		ptr = ptr->next_country;
	}
	return 0;
}
int exist(PortList * head ,int data ){
	PortList *ptr = head;
	while(ptr!=NULL){
		if(ptr->port == data)
			return 1;
		ptr = ptr->next;
	}
	return 0;
}
PortList *head = NULL;

Work_countries* array = NULL;

void *thread_f(void *argp){ /* Thread function */
	while(1){
		int current_fd;
		Fd *pac = malloc(sizeof(Fd));
		pac = obtain(&pool);
		current_fd = pac->fd;
		if (pac->type == 0){
			pthread_mutex_lock(&mutex);
			char buffer_s[1024];
			pthread_cond_signal(&cond_nonfull);
			int maxWorkers;
		    read(current_fd,&maxWorkers,sizeof(int));
		    int len ;
		    int bytestoread = read(current_fd,&len ,sizeof(int));
		    int valread = read( current_fd , buffer_s, sizeof(char)*len); 
		    buffer_s[valread]='\0';
		    char* token = NULL, *keep_file = NULL;
		    if (valread < 0) 
		    	perror("ERROR reading from socket");
		   	else
		   		printf("%s",buffer_s); //print th stats
		    int cond;
		    //print the lsit
		    read(current_fd,&cond,sizeof(int));
		    int eachPort; 
		    read(current_fd,&eachPort,sizeof(int));
		    int pointer;
		    read(current_fd,&pointer,sizeof(int));
		   	if (pointer == 1){
		   		PortList *node = malloc(sizeof(PortList));
		   		node->worker = cond;
		   		node->port = eachPort;
		   		node->next = NULL;
		   		pthread_mutex_lock(&list);
		   		if (exist(head,node->port) == 0)
		   			add_port(&head,node);
		   		pthread_mutex_unlock(&list);
		   	}
		   	pthread_mutex_unlock(&mutex);
//		   	close(current_fd);
		}
		else if (pac->type == 1){
			pthread_mutex_lock(&mutex);
			char buffer_s[1024];
			int size_query , bytestoread;
			read(current_fd,&size_query,sizeof(int));
			memset(buffer_s, '\0', 1023);
			bytestoread = read(current_fd,buffer_s,size_query);
			//printf("bytes to read are %d\n",bytestoread );
			buffer_s[bytestoread] = '\0';
			//printf("%s\n",buffer_s);
			pthread_mutex_unlock(&mutex);
			PortList * ptr  = head;
			char* ch = malloc(sizeof(char)*30);
			char* rest = malloc(sizeof(char)*1024);
			char* perm = strtok_r(buffer_s," ",&rest);
			strcpy(ch,perm);	
			printf("%s\n",ch);
			if(strcmp(ch,"/diseaseFrequency") == 0){
				while(ptr!=NULL){
					int wok, desc;
					struct sockaddr_in serv_addr;
					bzero(&serv_addr, sizeof(struct sockaddr_in));
			    	if ((wok = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
			        	printf("\n Socket creation error \n"); 
			        	exit(EXIT_FAILURE);
			    	} 
			    	serv_addr.sin_family = AF_INET; 
			    	serv_addr.sin_addr.s_addr = INADDR_ANY; 
			    	
			    	//find the IP address of each worker
			    	/*
			    	if ((getpeername(int wok, struct sockaddr *serv_addr, socklen_t*sizeof(serv_addr)) < 0){ 
			        	perror("Connection\n"); 
			        	exit(EXIT_FAILURE);
			    	} */

			    	serv_addr.sin_port = ptr->port; 
			    	if (connect(wok, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ 
			        	perror("Connection\n"); 
			        	exit(EXIT_FAILURE);
			    	}
			    	desc = 1;
		    		int bytestowrite = write(wok,&desc,sizeof(int));
					char *token;   
					char* rest2 = strdup(rest);
				    token = strtok(rest2," ");
				    int counter = 0;	   
					while( token != NULL ){
						counter++;
					    int len = strlen(token);
					    write(wok,&len,sizeof(int));
					   	write(wok,token,len);
					    token = strtok(NULL," ");
					   }
 					if (counter == 4){
 						char* token = "-";
 						int len = 1;
 						write(wok,&len,sizeof(int));
 						write(wok," ",len);
 					}
 					int found;
 					read(wok,&found,sizeof(int));
 					if (found == 1){
 						int ans;
 						read(wok,&ans,sizeof(int));
 						printf("%d\n",ans);
 						write(current_fd,&ans,sizeof(int));
 					}
		    		close(wok);
					ptr = ptr->next;  
				}
		   	}
		   	else if(strcmp(ch,"/topk-AgeRanges") == 0){
				while(ptr!=NULL){
					int wok, desc;
					struct sockaddr_in serv_addr;
					bzero(&serv_addr, sizeof(struct sockaddr_in));
			    	if ((wok = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
			        	printf("\n Socket creation error \n"); 
			        	exit(EXIT_FAILURE);
			    	} 
			    	serv_addr.sin_family = AF_INET; 
			    	serv_addr.sin_addr.s_addr = INADDR_ANY; 
			    	serv_addr.sin_port = ptr->port; 
			    	if (connect(wok, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ 
			        	perror("Connection\n"); 
			        	exit(EXIT_FAILURE);
			    	}
			    	desc = 2;
		    		write(wok,&desc,sizeof(int));
					char *token;   
					char* rest2 = strdup(rest);
				    token = strtok(rest2," ");
				    int counter = 0;	   
					while( token != NULL ){
						counter++;
					    int len = strlen(token);
					    write(wok,&len,sizeof(int));
					   	write(wok,token,len);
					    token = strtok(NULL," ");
					}		    		
		    		close(wok);
					ptr = ptr->next;  
				}
		   	}
		    else if(strcmp(ch,"/numPatientAdmissions") == 0){
		    	int ans = 0;
				while(ptr!=NULL){
					int wok, desc;
					struct sockaddr_in serv_addr;
					bzero(&serv_addr, sizeof(struct sockaddr_in));
			    	if ((wok = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
			        	printf("\n Socket creation error \n"); 
			        	exit(EXIT_FAILURE);
			    	} 
			    	serv_addr.sin_family = AF_INET; 
			    	serv_addr.sin_addr.s_addr = INADDR_ANY; 
			    	serv_addr.sin_port = ptr->port; 
			    	if (connect(wok, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ 
			        	perror("Connection\n"); 
			        	exit(EXIT_FAILURE);
			    	}
			    	desc = 3;
		    		write(wok,&desc,sizeof(int));
					char *token;   
					char* rest2 = strdup(rest);
				    token = strtok(rest2," ");
				    int counter = 0;	   
					while( token != NULL ){
						counter++;
					    int len = strlen(token);
					    write(wok,&len,sizeof(int));
					   	write(wok,token,len);
					    token = strtok(NULL," ");
					   }
					 char* country = malloc(sizeof(char)*30);
 					if (counter == 3){
 						char* token = strdup("error");
 						int len = strlen("error");
 						write(wok,&len,sizeof(int));
 						write(wok,"error",len);
 					}
 					int found;
 					read(wok,&found,sizeof(int));
 					if (found == 1){
 						int an;
 						read(wok,&an,sizeof(int));
 						ans = ans + an;
 					}
		    		close(wok);
					ptr = ptr->next;  
				}
				printf("patients %d\n", ans);
				write(current_fd,&ans,sizeof(int));
		    } 
		   	else if(strcmp(ch,"/searchPatientRecord") == 0){
				while(ptr!=NULL){
					int wok, desc;
					struct sockaddr_in serv_addr;
					bzero(&serv_addr, sizeof(struct sockaddr_in));
			    	if ((wok = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
			        	printf("\n Socket creation error \n"); 
			        	exit(EXIT_FAILURE);
			    	} 
			    	serv_addr.sin_family = AF_INET; 
			    	serv_addr.sin_addr.s_addr = INADDR_ANY; 
			    	serv_addr.sin_port = ptr->port; 
			    	if (connect(wok, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ 
			        	perror("Connection\n"); 
			        	exit(EXIT_FAILURE);
			    	}
			    	desc = 4;
		    		write(wok,&desc,sizeof(int));
		  			char* tok = strtok(rest," ");
		  			int len = strlen(tok);
		  			write(wok,&len,sizeof(int));
		  			write(wok,tok,len);
		  			int found;
		  			read(wok,&found,sizeof(int));
		  			if(found== 1){
			  			int bytestoread, val2;
						char* buf_send;
						buf_send = malloc(sizeof(char)*1024);
						memset(buf_send, '\0', 1024);
						bytestoread = read(wok,&val2,sizeof(int));
						bytestoread = read(wok,buf_send,val2);
						buf_send[bytestoread] = '\0';
						if (found == 1)
							printf("%s ",buf_send);

						bytestoread = read(wok,&val2,sizeof(int));
						bytestoread = read(wok,buf_send,val2); 
						buf_send[bytestoread]='\0';
						if (found == 1)
							printf("%s ",buf_send );

						bytestoread = read(wok,&val2,sizeof(int));
						bytestoread = read(wok,buf_send,val2);
						buf_send[bytestoread] = '\0';
						if (found == 1)
							printf("%s ",buf_send );

						bytestoread = read(wok,&val2,sizeof(int));
						bytestoread = read(wok,buf_send,val2);
						buf_send[bytestoread] = '\0';
						if (found == 1)
							printf("%s ",buf_send );

						int age; 
						bytestoread = read(wok,&age,sizeof(int));
						buf_send[bytestoread]='\0';
						if (found == 1)
							printf("%d ",age);

						int day1,month1,year1;
						bytestoread = read(wok,&day1,sizeof(int));
						bytestoread = read(wok,&month1,sizeof(int));
						bytestoread = read(wok,&year1,sizeof(int));
						if (found == 1)
							printf("%d-%d-%d ",day1,month1,year1 );

						int day2,month2,year2;
						bytestoread = read(wok,&day2,sizeof(int));
						bytestoread = read(wok,&month2,sizeof(int));
						bytestoread = read(wok,&year2,sizeof(int));
						if (found == 1)
							if (year2 == 0)
								printf("---\n");
							else
								printf("%d-%d-%d\n",day2,month2,year2 );
						free(buf_send);
		  			}
		    		close(wok);
					ptr = ptr->next;  
				}
		   	}
		    else if(strcmp(ch,"/numPatientDischarges") == 0){
		    	int ans = 0;
				while(ptr!=NULL){
					int wok, desc;
					struct sockaddr_in serv_addr;
					bzero(&serv_addr, sizeof(struct sockaddr_in));
			    	if ((wok = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
			        	printf("\n Socket creation error \n"); 
			        	exit(EXIT_FAILURE);
			    	} 
			    	serv_addr.sin_family = AF_INET; 
			    	serv_addr.sin_addr.s_addr = INADDR_ANY; 
			    	serv_addr.sin_port = ptr->port; 
			    	if (connect(wok, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ 
			        	perror("Connection\n"); 
			        	exit(EXIT_FAILURE);
			    	}
			    	desc = 5;
		    		write(wok,&desc,sizeof(int));
					char *token;   
					char* rest2 = strdup(rest);
				    token = strtok(rest2," ");
				    int counter = 0;	   
					while( token != NULL ){
						counter++;
					    int len = strlen(token);
					    write(wok,&len,sizeof(int));
					   	write(wok,token,len);
					    token = strtok(NULL," ");
					   }
					 char* country = malloc(sizeof(char)*30);
 					if (counter == 3){
 						char* token = strdup("error");
 						int len = strlen("error");
 						write(wok,&len,sizeof(int));
 						write(wok,"error",len);
 					}
 					int found;
 					read(wok,&found,sizeof(int));
 					if (found == 1){
 						int an;
 						read(wok,&an,sizeof(int));
 						ans = ans + an;
 					}
		    		close(wok);
					ptr = ptr->next;  
				}
				printf("patients %d\n", ans);
				write(current_fd,&ans,sizeof(int));
		    }
		    close(current_fd); 
	 	}
	 	//save stats
	}
	pthread_exit((void *) 47);
}

int main(int argc ,char* argv[]){
	if (argc!= 9){
		perror("wrong number of arguments");
		exit(0);
	}
	int i, queryPortNum, statisticsPortNum, numThreads , bufferSize;
	for(i=1; i<=8; i=i+2){
		if(strcmp(argv[i],"-q")==0){
			queryPortNum=atoi(argv[i+1]);		
		}
		else if(strcmp(argv[i],"-s")==0){
			statisticsPortNum=atoi(argv[i+1]);
		}
		else if(strcmp(argv[i],"-w") == 0){
			numThreads =atoi(argv[i+1]);
		}
		else if(strcmp(argv[i],"-b") == 0){
			bufferSize = atoi(argv[i+1]);
		} 
	}

	//create the buffer
	///pool_t* pool;
	initialize(&pool, bufferSize);

	pthread_mutex_init(&mutex, 0);
	pthread_mutex_init(&list,0);
	pthread_cond_init(&cond_nonempty, 0);
	pthread_cond_init(&cond_nonfull, 0);
	
	int server_fd, valread; 
	int new_socket, new_querry;
	Fd * pac = malloc(sizeof(Fd));
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
  	
    //socket for stats
	  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( statisticsPortNum ); 
      printf("server listen for stats in port %d...\n", address.sin_port );
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){ 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 10) < 0) { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    }

    // Querry Stats
    int querySock, new_socket2;
    struct sockaddr_in query;
    int querylen = sizeof(querylen);

 	if ((querySock = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }     
    if (setsockopt(querySock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    query.sin_family = AF_INET; 
    query.sin_addr.s_addr = INADDR_ANY; 
    query.sin_port = htons( queryPortNum ); 
    printf("server listen for queries in port %d...\n", query.sin_port );
    // Forcefully attaching socket to the port 8080 
    if (bind(querySock, (struct sockaddr *)&query, sizeof(query))<0){ 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(querySock, 10) < 0) { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    }
	pthread_t *threads;
  	int err;
	threads = malloc(numThreads*sizeof(pthread_t));
	//pthread_t* thrd =malloc(numThreads*sizeof(pthread_t));
	//Create the threads
  	for(i=0;i < numThreads;i++){
  		if (err = pthread_create(threads+i, NULL, &thread_f,NULL)){
	  		perror("pthread_create");
	  		exit(1);
  		}
	}
    while(1){
    	fd_set readfds;
        FD_ZERO(&readfds);          /* initialize the fd set */
        FD_SET(server_fd, &readfds);
        FD_SET(querySock, &readfds); /* add socket fd */

        int max_sd = server_fd;
        if(max_sd < querySock)
            max_sd = querySock;

        int result = select(max_sd + 1, &readfds, 0, 0, 0);
		if(result<0){
			perror("select() failed");
			exit(EXIT_FAILURE);
		}
		if(FD_ISSET(server_fd,&readfds)){
	  		if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  (socklen_t*)&addrlen))<0) { 
	        	perror("accept"); 
	        	exit(EXIT_FAILURE); 
	    	}
	    	pac->fd = new_socket;
	    	pac->type = 0;
	   		place(&pool,pac);
	    	pthread_cond_signal(&cond_nonempty);
	    }
	    else if(FD_ISSET(querySock,&readfds)){
	    	if ((new_socket = accept(querySock, (struct sockaddr *)&query,  (socklen_t*)&querylen))<0) { 
	        	perror("accept"); 
	        	exit(EXIT_FAILURE); 
	    	}
		    pac->fd = new_socket;
		    pac->type = 1;
		   	place(&pool,pac);
		    pthread_cond_signal(&cond_nonempty);
	    }
	}
	for (int i = 0; i < numThreads; ++i){
		if (err = pthread_join(threads[i], NULL)){
 			exit(1);
 		}
	}
}