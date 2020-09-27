#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/select.h>
#include "structs.h"

#define PERMS 0666


void deleteLList( List** head) 
{ 
   List* current = *head; 
   List* next; 
   while (current != NULL)  
   { 
       next = current->next; 
       free(current->country);
       free(current); 
       current = next; 
   }  
   *head = NULL; 
} 

List* insertList_parent(List** head, List *current_node){
	// Insert a new node in the end of the list
	List* help_ptr = *head;
	if(help_ptr == NULL){
		(*head)=current_node;
		return *head;
	}	
	help_ptr->next= current_node;
	return current_node;
}

int compareDates(Date* entry1, Date *entry2){
	if(entry2->year == 0)
		return 0;
	if(entry1->year == entry2->year)
		if(entry1->month == entry2->month)
			if(entry1->day == entry2->day)
				return 2;

	if (entry1->year < entry2->year)
		return 1;
	else if (entry1->year > entry2->year)
		return 0;
	else{
		if (entry1->month < entry2->month)
			return 1;
		else if (entry1->month > entry2->month)
			return 0;
		else{
			if (entry1->day < entry2->day)
				return 1;
			else if ( entry1-> day > entry2->day)
				return 0;
			else 
				return 0;
		}
	}
}


char* LastcharDel(char* name)
{
    int i = 0;
    while(name[i] != '\0')
    {
        i++;
    }
    name[i-1] = '\0';
    return name;
}




void reverse(char s[]){
     int i, j;
     char c;
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

 void itoa(int n, char s[]){
     int i, sign;
     if ((sign = n) < 0) 
         n = -n;         
     i = 0;
     do {     
         s[i++] = n % 10 + '0';  
     } while ((n /= 10) > 0); 
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }


int main(int argc , char*argv[]){
	if(argc != 11){
		perror("wrong number of arguments");
		return -1;
	}
	int i, numWorkers,bufferSize , filedesc, serverPort;
	pid_t pid;
	char* input_dir, *serverIP;
	int total = 0,success=0, fail=0;
	for(i=1; i<=9; i=i+2){
		if(strcmp(argv[i], "-w")==0){
			numWorkers=atoi(argv[i+1]);		
		}	
		else if(strcmp(argv[i], "-i")==0){
			input_dir=malloc(sizeof(char)*strlen(argv[i+1])+1);
			strcpy(input_dir,argv[i+1]);	
		}
		else if(strcmp(argv[i], "-b")==0){
			bufferSize=atoi(argv[i+1]);
		}
		else if(strcmp(argv[i],"-p") == 0){
			serverPort =atoi(argv[i+1]);
		}
		else if(strcmp(argv[i],"-s") == 0){
			serverIP = malloc(sizeof(char)*strlen(argv[i+1])+1);
			strcpy(serverIP,argv[i+1]);
		} 
	}
	printf("numWorkers are %d\n", numWorkers);
	printf("input_dir is %s\n",input_dir);
	printf("buffersize is %d\n",bufferSize );
	printf("serverIP is %s\n",serverIP);
	int proc_id[numWorkers]; //array with the proc ids of each child
	int fd5;
 	DIR *dp = NULL;
 	struct dirent *dir;
	dp = opendir(input_dir); 
	if(dp==NULL){
		perror("input directory does not exist\n");
		return -1;
	}

	List *head = NULL ,*head2 = NULL;
	int count = -2 , keepHead = 0 , current_workers = 1;

 	List* node = NULL;	
	while ((dir = readdir(dp)) != NULL){ //reaf the directory
        count++;
        if (strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") !=0 ){
	        node = malloc(sizeof(struct list));
	        node->country = malloc(sizeof(char)*strlen(dir->d_name)+1);
	        strcpy(node->country,dir->d_name);
	        if (current_workers != numWorkers){
	        	node->num_work = current_workers;
	        	current_workers++;
	        }
	        else{
	        	node->num_work = current_workers;
	        	current_workers = 1;
	        }
	        node->next = NULL;
	        head = insertList_parent(&head,node);
	        if(keepHead == 0){
	        	head2 = head;
	        	keepHead++;
	        }
	    }
    }
    List* ptr = head2;
    // share the jobs
    while(ptr!=NULL){
    	printf("%s in worker %d\n",ptr->country,ptr->num_work);
    	ptr = ptr->next;
    }
    printf("\n");
    closedir(dp);
    int fd , bytes, len, status;
   	for(i=1; i<=numWorkers; i++){
	   		//create fifo's
	   		char* buf1 = malloc(sizeof(char)*15);  
	   	    snprintf(buf1, 15, "rec_fifo%d", i);
	   		mkfifo(buf1,PERMS);
	   		//create children
			pid = fork(); 
			proc_id[i] = pid;
			if(pid == -1){
				perror("error in fork");
				exit(EXIT_FAILURE);
			}
			else if(pid > 0){
			 // parent's code 
				if ((fd = open(buf1, O_WRONLY ,PERMS)) == -1){
					perror("opening pipe in parent");
					exit(EXIT_FAILURE);
				}
				List* hh = head2;
				char* length = NULL;
				while(hh != NULL){
					if ((hh->num_work) == i){
						len = strlen(hh->country)+1;
						int num = len;
						int digits = 0;
						while(num>0){
							num = num/10;
							digits++;
						}
						//printf("%d with %d digits \n",len,digits );
						length = malloc(sizeof(char)*100);
						memset(length,'\0',digits);
						itoa(len,length);
						strcat(length,hh->country);
						if ((bytes = write(fd, length ,strlen(length)+1)) == -1){
							perror("write pipe");
							exit(EXIT_FAILURE);
						}
						//printf("stelnw %s\n",length );
						write(fd,&serverPort,sizeof(int));
						int sizeIp = strlen(serverIP);
						write(fd,&sizeIp,sizeof(int));
						write(fd,serverIP,sizeIp);
						free(length);
					}
					hh = hh->next;
				}
				free(hh);
				free(buf1);
				close(fd);
			}
			else{
				int *ptr = malloc(sizeof(int));
				int *maxWorkers = malloc(sizeof(int));
				maxWorkers = &numWorkers;
				ptr = &i;
				//printf(" worker no : %d\n", i);
				fflush(stdout);
				execl("./worker","worker",input_dir,ptr,maxWorkers, NULL);
				exit(0);
			}
		close(fd);
	}
	//check if a child has died with SIGCHLD

	for(i=1;i<=numWorkers;i++){ //unlink the fifos
		char* buf1 = malloc(sizeof(char)*15);
   	    snprintf(buf1, 15, "rec_fifo%d", i);
   		unlink(buf1);
   		free(buf1);
	}
	free(input_dir);
	while(wait(&status)>0);
	wait(&status);
	//deallocate the memory
	//deleteLList(&head2);
	return 1;
}