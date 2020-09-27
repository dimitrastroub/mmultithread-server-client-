#include<stdio.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <fcntl.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <netdb.h> /* gethostbyaddr */
#include <stdint.h>
#include "structs.h"

#define p 131

typedef struct work_diseases{
	char* dis;
	int first_range;
	int sec_range;
	int third_range;
	int fourth_range;
	struct work_diseases* next_disease;
}Work_diseases;

typedef struct work_dates{
	char* filename;
	struct work_diseases* wds;	
	struct work_dates *next_date;
}Work_dates;

typedef struct work_countries{
	char* country;
	struct work_dates* wd;
	struct work_countries* next_country;
}Work_countries;


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
Work_countries* insertC(Work_countries** head, Work_countries *current_record){
	// Insert a new record in the end of the list
	Work_countries* help_ptr = *head;
	if(help_ptr == NULL){
		(*head)=current_record;
		return *head;
	}	

	help_ptr->next_country = current_record;
	return current_record;
}

Work_dates* insertD(Work_dates** head, Work_dates *current_record){
	// Insert a new record in the end of the list
	Work_dates* help_ptr = *head;
	if(help_ptr == NULL){
		(*head)=current_record;
		return *head;
	}	

	help_ptr->next_date= current_record;
	return current_record;
}

Work_diseases* insertDs(Work_diseases** head, Work_diseases *current_record){
	// Insert a new record in the end of the list
	Work_diseases* help_ptr = *head;
	if(help_ptr == NULL){
		(*head)=current_record;
		return *head;
	}	

	help_ptr->next_disease= current_record;
	return current_record;
}

int TraverseList(char* diseaseID, Work_diseases* head){
	Work_diseases *ptr = head;
	while(ptr!=NULL){
		if (strcmp(ptr->dis, diseaseID) == 0){
			return 1;
		}
		ptr = ptr->next_disease;
	}
	return 0;
}

typedef struct file{
	char* filename;
	Date* fileDate;
	struct file* next_file;
}File;


void deleteList( Record** head) 
{ 
   Record* current = *head; 
   Record* next; 
   while (current != NULL)  
   { 
       next = current->nextRecord; 
       free(current->recordID);
       free(current->situation);
       free(current->patientFirstName);
       free(current->patientLastName);
       free(current->diseaseID);
       free(current->country);
       free(current->entryDate);
       free(current->exitDate);
       free(current); 
       current = next; 
   }    
   *head = NULL; 
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
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

void deleteTree(AvlNode* node)  
{ 
    if (node == NULL) return; 
    /* first delete both subtrees */
    deleteTree(node->left); 
    deleteTree(node->right); 
    /* then delete the node */

	//free(node->name);
	free(node);
}  

void exitHash(HashNode* diseaseHash, HashNode* counrtyHash){
	int i;
	for(i=0; i<5; i++){
		Bucket *ptr = diseaseHash[i].bucketList;
		Bucket *nextB;
		while(ptr!= NULL){
			nextB = ptr->nextBucket;
			BucketEntry* entry = ptr->bucketEntryList;
			BucketEntry* next ;
			while(entry!=NULL){
				next = entry->nextBucketEntry;
				free(entry->name);
				deleteTree(entry->avlNode);
				free(entry);
				entry = next;
			}
			free(ptr);
			ptr = nextB;
		}
	}
	for(i=0; i<5; i++){
		Bucket *ptr = counrtyHash[i].bucketList;
		Bucket *nextB;
		while(ptr!= NULL){
			nextB = ptr->nextBucket;
			BucketEntry* entry = ptr->bucketEntryList;
			BucketEntry* next ;
			while(entry!=NULL){
				next = entry->nextBucketEntry;
				free(entry->name);
				deleteTree(entry->avlNode);
				free(entry);
				entry = next;
			}
			free(ptr);
			ptr = nextB;
		}
	}
}	

Record* createRecord(char* recordID ,char* situation ,char*patientFirstName, char* patientLastName, char* diseaseID, char* country, Date* entryDate, Date* exitDate, int age){
	Record* current_record;

	current_record = malloc(sizeof(Record));	
	current_record->recordID= malloc(sizeof(char)*strlen(recordID)+1);
   	strcpy(current_record->recordID,recordID);
   	current_record->situation = malloc(sizeof(char)*strlen(situation)+1);
   	strcpy(current_record->situation,situation);
   	current_record->patientFirstName= malloc(sizeof(char)*strlen(patientFirstName)+1);
   	strcpy(current_record->patientFirstName,patientFirstName);
   	current_record->patientLastName= malloc(sizeof(char)*strlen(patientLastName)+1);
   	strcpy(current_record->patientLastName,patientLastName);
   	current_record->diseaseID= malloc(sizeof(char)*strlen(diseaseID)+1);
   	strcpy(current_record->diseaseID,diseaseID);
   	current_record->country= malloc(sizeof(char)*strlen(country)+1);
   	strcpy(current_record->country,country);

   	current_record->entryDate= malloc(sizeof(Date));
   	current_record->entryDate->day = entryDate->day;
   	current_record->entryDate->month = entryDate->month;
   	current_record->entryDate->year = entryDate->year;

   	current_record->exitDate= malloc(sizeof(Date));
   	current_record->exitDate->day = exitDate->day;
   	current_record->exitDate->month = exitDate->month;
   	current_record->exitDate->year = exitDate->year;

   	current_record->age = age;
   	current_record->nextRecord = NULL;

  	return current_record;
}

Record* insertRecord(Record** head, Record *current_record){
	// Insert a new record in the end of the list
	Record* help_ptr = *head;
	if(help_ptr == NULL){
		(*head)=current_record;
		return *head;
	}	

	help_ptr->nextRecord= current_record;
	return current_record;
}

void printRecordList(Record *head){
	Record *current_record = head;
	while(current_record != NULL){
		printf("%s ", current_record->recordID);
		printf("%s ", current_record->situation);
		printf("%s ",current_record->patientFirstName );
		printf("%s ",current_record->patientLastName );
		printf("%s ",current_record->diseaseID );
		printf("%s ",current_record->country );

		printf("%d-",current_record->entryDate->day);
		printf("%d-",current_record->entryDate->month);
		printf("%d ",current_record->entryDate->year);

		printf("%d-",current_record->exitDate->day);
		printf("%d-",current_record->exitDate->month);
		printf("%d ",current_record->exitDate->year);
		printf("%d ",current_record->age);
		printf("\n");
		current_record = current_record->nextRecord;
	}
	return;
}

int height(AvlNode *T){
	int lh,rh;
	if(T==NULL)
		return(0);
	
	if(T->left==NULL)
		lh=0;
	else
		lh=1+T->left->ht;
		
	if(T->right==NULL)
		rh=0;
	else
		rh=1+T->right->ht;
	
	if(lh>rh)
		return(lh);
	
	return(rh);
}

AvlNode * rotateright(AvlNode *x){
	AvlNode *y;
	y=x->left;
	x->left=y->right;
	y->right=x;
	x->ht=height(x);
	y->ht=height(y);
	return(y);
}
 
AvlNode * rotateleft(AvlNode *x){
	AvlNode *y;
	y=x->right;
	x->right=y->left;
	y->left=x;
	x->ht=height(x);
	y->ht=height(y);
	return(y);
}
 
AvlNode * RR(AvlNode *T){
	T=rotateleft(T);
	return(T);
}
 
AvlNode * LL(AvlNode *T){
	T=rotateright(T);
	return(T);
}
 
AvlNode * LR(AvlNode *T){
	T->left=rotateleft(T->left);
	T=rotateright(T);
	
	return(T);
}
 
AvlNode * RL(AvlNode *T){
	T->right=rotateright(T->right);
	T=rotateleft(T);
	return(T);
}
 
int BF(AvlNode *T){
	int lh,rh;
	if(T==NULL)
		return(0);
	if(T->left==NULL)
		lh=0;
	else
		lh=1+T->left->ht;
	if(T->right==NULL)
		rh=0;
	else
		rh=1+T->right->ht;
	return(lh-rh);
}

AvlNode* insertAvl(AvlNode *T,Record *rec){
//	if(searchAvl(T,rec) == 0){
		if(T==NULL){
			T=(AvlNode*)malloc(sizeof(AvlNode));
			//T->ptr = malloc(sizeof(Record));
			T->ptr = rec;
			T->left=NULL;
			T->right=NULL;
		}
		else
			if(compareDates(rec->entryDate,T->ptr->entryDate) != 1)		// insert in right subtree
			{
				T->right=insertAvl(T->right,rec);
				if(BF(T)==-2)
					if (compareDates(rec->entryDate,T->right->ptr->entryDate) != 1)
						T=RR(T);
					else
						T=RL(T);
			}
			else
				if(compareDates(rec->entryDate,T->ptr->entryDate) == 1)
				{
					T->left=insertAvl(T->left,rec);
					if(BF(T)==2)
						if(compareDates(rec->entryDate, T->left->ptr->entryDate) == 1)
							T=LL(T);
						else
							T=LR(T);
				}
			
			T->ht=height(T);
			
			return(T);
}



int searchEntries(BucketEntry *head, char* name){
	BucketEntry *ptr2 = head;
	while(ptr2!= NULL){
		if( strcmp(ptr2->name,name) == 0)
			return 1;
		ptr2 = ptr2->nextBucketEntry;
	}
	return 0;
}

int  searchHash(Bucket* head,char* name){
	Bucket *ptr = head;
	int key;
	//int counter = 0;
	while(ptr != NULL){
		key = searchEntries(ptr->bucketEntryList, name);
		if (key == 1)
			return 1;
		ptr = ptr->nextBucket;
	}
	return 0; 
}

Bucket* insertBucket(Bucket **bucketList, int max_entries, char*name, Record** head){
	int key = searchHash(*bucketList,name);
	//printf("kanw isnert thn arrwstia %s\n", name);
	if (key == 0){
		//Record* ptr = *head;
		if ((*bucketList) == NULL){
			(*bucketList) = malloc(sizeof(Bucket));
			(*bucketList)->entries = 1;
			(*bucketList)->nextBucket = NULL;

			
			(*bucketList)->bucketEntryList = malloc(sizeof(BucketEntry));
			(*bucketList)->bucketEntryList->name = malloc(sizeof(char)*strlen(name)+1);
			strcpy((*bucketList)->bucketEntryList->name,name);
			//(*bucketList)->bucketEntryList->name = name;
			(*bucketList)->bucketEntryList->avlNode = NULL;
			(*bucketList)->bucketEntryList->nextBucketEntry = NULL;
	
			while((*head)!=NULL){
				if(strcmp((*head)->diseaseID,name) == 0 || strcmp((*head)->country,name) == 0){
						(*bucketList)->bucketEntryList->avlNode = insertAvl((*bucketList)->bucketEntryList->avlNode,(*head));
				}
				(*head) = (*head)->nextRecord;
			}
			(*bucketList)->bucketEntryList->nextBucketEntry = NULL;
			return *bucketList;
		}
		Bucket* currentBucket = *bucketList;
		while(currentBucket->entries ==  max_entries){
			//Record *ptr2 = *head;
			if (currentBucket->nextBucket == NULL){
				currentBucket->nextBucket = malloc(sizeof(Bucket));
				currentBucket->nextBucket->entries = 1;
				currentBucket->nextBucket->nextBucket = NULL;

				currentBucket->nextBucket->bucketEntryList = malloc(sizeof(BucketEntry));
				currentBucket->nextBucket->nextBucket = NULL; //here
				currentBucket->nextBucket->bucketEntryList->name = malloc(sizeof(char)*strlen(name)+1);
				strcpy(currentBucket->nextBucket->bucketEntryList->name,name);
				//currentBucket->nextBucket->bucketEntryList->name = name;
				currentBucket->nextBucket->bucketEntryList->avlNode = NULL;
				currentBucket->nextBucket->bucketEntryList->nextBucketEntry = NULL; //here

				while((*head)!=NULL){
					if(strcmp((*head)->diseaseID,name) == 0 || strcmp((*head)->country,name) == 0){
						//Record *rec = head;
						//if(searchAvl(currentBucket->nextBucket->bucketEntryList->avlNode,rec) == 0)
							currentBucket->nextBucket->bucketEntryList->avlNode = insertAvl(currentBucket->nextBucket->bucketEntryList->avlNode,(*head));
							//return *bucketList;
					}
					(*head) = (*head)->nextRecord;
				}
				currentBucket->nextBucket->bucketEntryList->nextBucketEntry = NULL; 
				return *bucketList;
			}
			currentBucket = currentBucket->nextBucket;
		}
		BucketEntry *entry;
		entry = malloc(sizeof(BucketEntry));
		entry->name = malloc(sizeof(char)*strlen(name)+1);
		strcpy(entry->name,name);
		//entry->name = name;
		entry->nextBucketEntry = NULL; //here
		entry->avlNode = NULL; 
		entry->nextBucketEntry = currentBucket->bucketEntryList;

		currentBucket->bucketEntryList = entry;
		//Record *ptr3 = *head;

		while((*head)!=NULL){
			if(strcmp((*head)->diseaseID,name) == 0 || strcmp((*head)->country,name) == 0){
				//Record *rec = head;
				//if (searchAvl(currentBucket->bucketEntryList->avlNode,rec) == 0)
					currentBucket->bucketEntryList->avlNode = insertAvl(currentBucket->bucketEntryList->avlNode,(*head));
					//return *bucketList;
				//preorder(currentBucket->bucketEntryList->avlNode);
			}
			(*head) = (*head)->nextRecord;
		} 
		currentBucket->entries++;
		return *bucketList;
	}
	else{
		Bucket *bkt = *bucketList;
		//Record* ptr4 = *head;
		while(bkt!=NULL){
			BucketEntry *entry = bkt->bucketEntryList;
			while(entry !=NULL){
				if (strcmp(entry->name, name) == 0){ //vrhka se poio bucket ienai to sygkekrimeno disease/country
					while((*head)!=NULL){
						if(strcmp((*head)->diseaseID,name) == 0 || strcmp((*head)->country,name) == 0){
								entry->avlNode = insertAvl(entry->avlNode,(*head));
						}
						(*head) = (*head)->nextRecord;
					}
				}
				entry = entry->nextBucketEntry;
			}
			bkt = bkt->nextBucket;
		}
	return *bucketList;
	}
}

void inorder(AvlNode *T){
	if(T!=NULL){
		inorder(T->left);
		//printf("%s(Bf=%d)  ",T->ptr->patientLastName,BF(T));
		printf("%d-", T->ptr->entryDate->day);
		printf("%d-", T->ptr->entryDate->month);
		printf("%d\n", T->ptr->entryDate->year);		
		inorder(T->right);
	}
}
int patients = 0;
void inorder6(AvlNode *T,Date *entryDate, Date *exitDate, char*country)
{
	if(T!=NULL){
		inorder6(T->left,entryDate,exitDate,country);
		if(compareDates(entryDate,T->ptr->exitDate )== 1 && compareDates(T->ptr->exitDate, exitDate) == 0 ){
			patients++;}
		inorder6(T->right,entryDate,exitDate,country);
	}
}

int patients2 = 0;
void inorder7(AvlNode *T,Date *entryDate, Date *exitDate, char*country)
{
	if(T!=NULL){
		inorder7(T->left,entryDate,exitDate,country);
		if(compareDates(T->ptr->entryDate,entryDate )== 1 && compareDates(entryDate, T->ptr->exitDate) == 0 ){
			patients2++;}
			inorder7(T->right,entryDate,exitDate,country);
	}
}


void printEntries(BucketEntry *head){
	BucketEntry *ptr2 = head;
	while(ptr2!= NULL){
		printf("%s\n", ptr2->name);
		inorder(ptr2->avlNode);
		ptr2 = ptr2->nextBucketEntry;
	}
}

void printHash(Bucket* head){
	Bucket *ptr = head;
	int counter = 0;
	while(ptr != NULL){
		printEntries(ptr->bucketEntryList);
		ptr = ptr->nextBucket;
		counter++;
	} 
}

File* insertList_file(File** head, File *current_node){
	// Insert a new node in the end of the list
	File* help_ptr = *head;
	if(help_ptr == NULL){
		(*head)=current_node;
		return *head;
	}	
	help_ptr->next_file= current_node;
	return current_node;
}

void printList(File *head){
	File *node = head;
	while(node != NULL){
		printf("%d-%d-%d and whole filename: %s\n",node->fileDate->day,node->fileDate->month,node->fileDate->year,node->filename);
		node = node->next_file;
	}
	return;
}


int inorder4(AvlNode *T, int *counter,Date *entryDate, Date *exitDate)
{
	//Date* perm1 = entryDate;
	//Date *perm2 = exitDate;
	if(T!=NULL)
	{
		inorder4(T->left,counter,entryDate,exitDate);
		if(compareDates(entryDate,T->ptr->entryDate )== 1 && compareDates(T->ptr->entryDate, exitDate) == 1)
			(*counter)++;
		inorder4(T->right,counter,entryDate,exitDate);
	}
	return(*counter);
}

int inorder5(AvlNode *T, int *counter,Date *entryDate, Date *exitDate, char*country)
{
	//Date* perm1 = entryDate;
	//Date *perm2 = exitDate;
	if(T!=NULL)
	{
		inorder5(T->left,counter,entryDate,exitDate,country);
		if(compareDates(entryDate,T->ptr->entryDate )== 1 && compareDates(T->ptr->entryDate, exitDate) == 1 && strcmp(country,T->ptr->country) == 0){
			(*counter)++;
		}
		inorder5(T->right,counter,entryDate,exitDate,country);
	}
	return(*counter);
}


int diseaseFrequency(int s,char* virusName,int diseaseHashtableNumOfEntries,HashNode* diseaseHash,Date *entryDate, Date *exitDate){
	int i;
	int* counter = NULL;
	counter = malloc(sizeof(int));
	if(!counter){
		fprintf(stderr, "Error" );
		exit(EXIT_FAILURE);
	}
	(*counter) = 0;
	HashNode* hash1 = diseaseHash;
	for (i=0; i< diseaseHashtableNumOfEntries; i++){
		Bucket* ptr = hash1[i].bucketList;
		while(ptr!=NULL){
			BucketEntry *entry = ptr->bucketEntryList;
			while(entry!=NULL){
				if(strcmp(entry->name,virusName) == 0){
					(*counter) = inorder4(entry->avlNode,counter,entryDate,exitDate);
					return (*counter);
				}
				entry = entry->nextBucketEntry;
			}
			ptr = ptr->nextBucket;
		}
	}
	free(counter);
	return  0;
}

int diseaseFrequency2(int s,char* virusName,int diseaseHashtableNumOfEntries,HashNode* diseaseHash,Date *entryDate, Date *exitDate,char* country){
	int i;
	int* counter , found = 0;
	counter = malloc(sizeof(int));
	if(!counter){
		fprintf(stderr, "Error" );
		exit(EXIT_FAILURE);
	}
	(*counter) = 0;
	HashNode* hash1 = diseaseHash;
	for (i=0; i< diseaseHashtableNumOfEntries; i++){
		Bucket* ptr = hash1[i].bucketList;
		while(ptr!=NULL){
			BucketEntry *entry = ptr->bucketEntryList;
			while(entry!=NULL){
				if(strcmp(entry->name,virusName) == 0){
					(*counter) = inorder5(entry->avlNode,counter,entryDate,exitDate,country);
					return (*counter);
				}
				entry = entry->nextBucketEntry;
			}
			ptr = ptr->nextBucket;
		}
	}
	free(counter);
	return 0 ;
}

void swap(File *node1, File *node2){
	int ptr1, ptr2, ptr3;
	char *temp = node1->filename; 
	ptr1 = node1->fileDate->day;
	ptr2 = node1->fileDate->month;
	ptr3 = node1->fileDate->year;

  	node1->filename = node2->filename; 
  	node1->fileDate->day = node2->fileDate->day;
  	node1->fileDate->month = node2->fileDate->month;
  	node1->fileDate->year = node2->fileDate->year;

  	node2->filename = temp; 
  	node2->fileDate->day = ptr1;
  	node2->fileDate->month = ptr2;
  	node2->fileDate->year = ptr3;
}



void bubbleSort(File* start) 
{ 
    int swapped, i; 
    File *ptr1; 
    File *lptr = NULL; 

    if (start == NULL) 
        return; 
  
     
     do{
     	swapped = 0; 
        ptr1 = start; 
        while (ptr1->next_file != lptr) 
        { 
            if (compareDates(ptr1->fileDate, ptr1->next_file->fileDate) == 0){  
                swap(ptr1, ptr1->next_file); 
                swapped = 1; 
            } 
            ptr1 = ptr1->next_file; 
        } 
        lptr = ptr1; 
    } 
    while (swapped); 
}

int Hashfunction(char* entry, int buckets){
	//hash table function for strings

	unsigned int h=0, a=20,len,i;
	int value;
	len=strlen(entry);

	for(i=0;i<len;i++){
		value=(int)entry[i];
		h=(h*a + value ) % p;
	}
	return h%buckets;
}
//workers code

int main(int argc, char* argv[]){

	int serverPort ,sock;
	char* input_dir = malloc(sizeof(char)*strlen(argv[1])+1), *length;
	strcpy(input_dir,argv[1]);
	int fd,bytes,i = *argv[2],first_read;
	int maxWorkers = *argv[3];
	char buf1[15] ,buffer[8];
	Work_countries *node1 = NULL , *head_list1 = NULL, *head1 = NULL;
	int counter_list1 = 0;
	printf("child no %d is executing....\n",i  );
	Record* head_rec = NULL , *head2_rec = NULL;
	HashNode* diseaseHash = NULL, *counrtyHash = NULL; //2 hashtables , one for diseases and the other for the countries
	diseaseHash = malloc(5*sizeof(HashNode));  
	counrtyHash = malloc(5*sizeof(HashNode));
	char * serverIP;
	int rec_count = 0;
	snprintf(buf1, 15, "rec_fifo%d",i);
	if ((fd = open(buf1, O_RDONLY, 0666)) == -1){
		perror("open child side");
		exit(EXIT_FAILURE);
	}
    do{ //open each directory 
    	char* m = malloc(sizeof(char))+1;
   		if ((first_read = read(fd,m,1)) == -1){
			perror("read");
        	exit(EXIT_FAILURE);
		}
		m[first_read] = '\0';
		int len = atoi(m);
        if ((bytes = read(fd,buffer,(sizeof(char)*len))) == -1){
            perror("read");
            exit(EXIT_FAILURE);
       }
        buffer[bytes] ='\0';
        if(bytes!=0){
        			read(fd,&serverPort,sizeof(int));
		//printf("receive %d %d \n", serverPort, i);
		int sizeIP;
		read(fd,&sizeIP,sizeof(int));
		//printf("receive %d %d \n", sizeIP, i);
		serverIP = malloc(sizeof(char)*sizeIP);
		int bytestoread = read(fd,serverIP,sizeIP);
		serverIP[bytestoread] = '\0';
		//printf("received servrIP : %s\n", serverIP);
	       	DIR *dp = NULL;
	 		struct dirent *dir;
	 		//strcat each subdirectory
	 		char* str = malloc(sizeof(char)*80);
	 		strcpy(str,input_dir);
	 		strcat(str,"/");
	 		strcat(str,buffer);
	 		//printf("str is%s\n", str);
	 		dp = opendir(str);
			if(dp == NULL){
				perror("open directory \n");
				return -1;
			}
			File * head = NULL , *node = NULL , *hh = NULL;
			int keep = 0;
			while ((dir = readdir(dp)) != NULL){
				if (strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") !=0 ){
					node = malloc(sizeof(struct file));
					node->filename = malloc(sizeof(char)*strlen(dir->d_name)+1);
					strcpy(node->filename,dir->d_name);
					node->next_file = NULL;
		        	//printf("%s\n", dir->d_name); 
		        	node->fileDate = malloc(sizeof(Date));
					char s[2] = "-";
			   		char *token = NULL;
			   		char* temp_name = malloc(sizeof(node->filename)+1);
			   		strcpy(temp_name,node->filename);
			   		token = strtok(temp_name,s);
			   		int counter = 0;
			  		while( token != NULL ) {
			      	if (counter == 0){
			      		node->fileDate->day = atoi(token);
			      		counter++;
			      	}
			      	else if (counter == 1){
			      		node->fileDate->month = atoi(token);
			      		counter++;
			      	}else{
			      		node->fileDate->year = atoi(token);
			      		counter++;
			    	 }
			     	 token = strtok(NULL, s); 
			   		}
			   		head = insertList_file(&head,node); 
			   		if (keep == 0){
						hh = head;
						keep++;
					}
				}
			}

			bubbleSort(hh);
			//read sorted files 
			File *ptr = hh;
			char *temp_file ;
	 		node1 = malloc(sizeof(Work_countries));
	 		node1->country = malloc(sizeof(char)*strlen(buffer));
	 		node1->next_country = NULL;
	 		strcpy(node1->country,buffer);
	 		head_list1 = insertC(&head_list1,node1);
	 		if (counter_list1 == 0){
	 			head1 = head_list1;
	 			counter_list1++;
	 		}
	 		int counter_list2 = 0;
	 		Work_dates* head2 = NULL;
			while(ptr!= NULL){ //for each gile
				temp_file = malloc(sizeof(ptr->filename)+1);
				strcpy(temp_file,ptr->filename);
	 		    Work_dates* node2;
				node2 = malloc(sizeof(Work_dates));
				node2->filename = malloc(sizeof(char)*strlen(temp_file)+1);
				strcpy(node2->filename,temp_file);
				node2->next_date = NULL;
				node2->wds = NULL;
				head2 = insertD(&head2,node2);
				//printf("head is %p\n",head2);
				if(counter_list2 == 0){
					node1->wd = head2;
					counter_list2++;
				}
				char path[80];
				strcpy(path,str);
				strcat(path,"/");
				strcat(path,temp_file);
				//open it
				FILE* fp = fopen(path,"r");
				if (!fp){
				  fprintf(stderr, "Error opening file '%s'\n", "path.txt");
				  return EXIT_FAILURE;
				}
				//else printf("%s\n\n",path);		
				char* line = NULL;
				char* token = NULL;
				char* recordID = NULL,*patientFirstName = NULL,*patientLastName = NULL , *age = NULL, *diseaseID = NULL, *situation = NULL, *country = NULL;
				size_t len = 0;
				Work_diseases *head3 = NULL;
				int counter_list3 = 0;
				while(getline(&line,&len,fp) != -1){ //for each line of the file

	 			    char* line2 = malloc(sizeof(char)*strlen(line)+1);
	 			    strcpy(line2,line);
			  		char * token = strtok(line2, " ");
				   // loop through the string to extract all other tokens
			  		int counter = 0;
				   while( token != NULL ) { //gia kathe lexh ths grammhs
				   		//printf(" token is %s ",token );
				   		if(token !=NULL){
				    		//printf( "%s\n", token ); //printing each token
							switch(counter){
							case 0: recordID = malloc(sizeof(char)*strlen(token)+1);
								strcpy(recordID,token);
								break;
							case 1: situation = malloc(sizeof(char)*strlen(token)+1);
								strcpy(situation,token);
								break;
							case 2: patientFirstName = malloc(sizeof(char)*strlen(token)+1);
								strcpy(patientFirstName,token);
								break;
							case 3: patientLastName = malloc(sizeof(char)*strlen(token)+1);
								strcpy(patientLastName,token);
								break;
							case 4: diseaseID = malloc(sizeof(char)*strlen(token)+1);
								strcpy(diseaseID,token);
								//printf("%s\n",diseaseID );
								break;
							case 5: age = malloc(sizeof(char)*strlen(token)+1);
								strcpy(age,token);
								break;
							}
							counter++; 
				    	}
				    	//printf("%s\n", dis);
				    	token = strtok(NULL, " ");	
	 			    }
	 			    int age_int = atoi(age);
	 			    
		 		    Work_diseases* node3;
					node3 = malloc(sizeof(Work_diseases));
					node3->dis = malloc(sizeof(char)*strlen(diseaseID)+1);
					strcpy(node3->dis,diseaseID);
					Date* exitDate = malloc(sizeof(Date));
			    	Date* entryDate = malloc(sizeof(Date));			    		
			    	if (strcmp(situation,"ENTER") == 0){
			    		exitDate->day = 0;
			    		exitDate->month = 0;
			    		exitDate->year = 0;
			    		entryDate->day = ptr->fileDate->day;
			    		entryDate->month = ptr->fileDate->month;
			    		entryDate->year = ptr->fileDate->year;	
			    	}
			    	else{
    					exitDate->day = ptr->fileDate->day;
			    		exitDate->month = ptr->fileDate->month;
			    		exitDate->year = ptr->fileDate->year;
			    		entryDate->day = 0;
			    		entryDate->month =0;
			    		entryDate->year = 0;
			    	}
			    	if (strcmp(situation,"ENTER") == 0){
						if (TraverseList(diseaseID,node2->wds) == 0){ //if it does not exits, i create it
							head3 = insertDs(&head3,node3);
							if (age_int <= 20){
								node3->first_range = 1;
								node3->sec_range = 0;
								node3->third_range = 0;
								node3->fourth_range = 0;
							}
							else if (age_int <= 40){
								node3->first_range = 0;
								node3->sec_range = 1;
								node3->third_range = 0;
								node3->fourth_range = 0;
							}
							else if (age_int <= 60){
								node3->first_range = 0;
								node3->sec_range = 0;
								node3->third_range = 1;
								node3->fourth_range = 0;
							}
							else{
								node3->first_range = 0;
								node3->sec_range = 0;
								node3->third_range = 0;
								node3->fourth_range = 1;
							}
						}
						else{// it exists, so i search for it
							Work_diseases * perm = node2->wds;
							while(perm != NULL){
								if (strcmp(perm->dis,diseaseID) == 0){
									if (age_int <= 20)
										perm->first_range++;
									else if (age_int <= 40)
										perm->sec_range++;
									else if (age_int <= 60)
										perm->third_range++;
									else
										perm->fourth_range++;
									//break;
								}
								perm = perm->next_disease;	
							}
						}
						if(counter_list3 == 0){
							node2->wds = head3;
							counter_list3++;
						}
					}
			    	country = malloc(sizeof(char)*strlen(buffer)+1);
			    	strcpy(country,buffer);
			    	Record* new = createRecord(recordID,situation,patientFirstName,patientLastName,diseaseID,country,entryDate,exitDate,age_int);
			    	new->nextRecord  = NULL;
			    	int flag = 0;
			    	
			    	if (strcmp(new->situation,"ENTER") != 0){ 
				    	Record* cur = head_rec;
				    	while(cur!= NULL){
				    		if (strcmp(cur->recordID,new->recordID) == 0){ //an vrethei to record id
				    			cur->exitDate->day = new->exitDate->day;
				    			cur->exitDate->month = new->exitDate->month;
				    			cur->exitDate->year = new->exitDate->year;
				    		}
				    		cur = cur->nextRecord;
				    	}
		
					}
					else flag = 1; 
					if (flag == 1){
				    	head_rec = insertRecord(&head_rec,new);
				    	if (rec_count == 0){
				    		head2_rec = head_rec;
				    		rec_count++;
				    	}
					   int pos_dis = Hashfunction(diseaseID,5);
					   int pos_country = Hashfunction(country,5);
					   Record* new2 = new;
					   new2->nextRecord = NULL;
					   diseaseHash[pos_dis].bucketList = insertBucket(&(diseaseHash[pos_dis].bucketList),3, diseaseID,&new);
					   counrtyHash[pos_country].bucketList = insertBucket(&(counrtyHash[pos_country].bucketList),3, country,&new2); 
					}
					//else printf("Error\n");
				}
				ptr = ptr->next_file;
			}
			closedir(dp);
	    }
    }while (bytes > 0);
    Work_countries* ptr1 = head1;

    struct sockaddr_in worker;
	struct sockaddr *workerptr=(struct sockaddr *)&worker;
	int sock_work , new_sock , opt = 1;

	int workerlen = sizeof(worker);
	//create the socket	
	if ((sock_work = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket creation failed");
		  exit(1);
	}
	// Forcefully attaching socket to the port 
    if (setsockopt(sock_work, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){ 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    }
    worker.sin_family = AF_INET;
	worker.sin_addr.s_addr = htonl(INADDR_ANY);
	// /worker.sin_addr.s_addr= inet_addr(serverIP)
	worker.sin_port = htons(0);
	//bind the socket
	if (bind(sock_work,workerptr,  sizeof(worker))<0){ 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (getsockname(sock_work,workerptr,&workerlen)<0){
    	perror("getsockname");
    	exit(EXIT_FAILURE);
    }
   	printf("Socket  port for %d : %d\n",i, worker.sin_port);

	while(ptr1!=NULL){
		Work_dates * ptr2 = ptr1->wd;
		while(ptr2!=NULL){
			char* send_data = malloc(sizeof(char)*strlen(ptr2->filename)+1);
			strcpy(send_data,ptr2->filename);
			send_data = realloc(send_data,sizeof(char)*strlen("\n")+1);
			strcat(send_data,"\n");

			send_data = realloc(send_data,sizeof(char)*strlen(ptr1->country)+1);
			strcat(send_data,ptr1->country);
	
			send_data = realloc(send_data,sizeof(char)*strlen("\n")+1);
			strcat(send_data,"\n");
			Work_diseases *ptr3 = ptr2->wds;
			while(ptr3 != NULL){ //send_data statistics
				char* temp_disease = malloc(sizeof(char)*strlen(ptr3->dis)+1);
				strcpy(temp_disease,ptr3->dis);
				char* nl = malloc(sizeof(char)*strlen("\n")+1);
				strcpy(nl,"\n");
				strcat(send_data,temp_disease);
				strcat(send_data,nl);
				
				char* temp1, *temp2 ,*temp3 ,*temp4;
				temp1 = malloc(sizeof(char)*strlen("Age range 0-20 years: %d cases\n")+1);
				sprintf(temp1, "Age range 0-20 years: %d cases\n",ptr3->first_range);				
				//printf("Age range 0-20 years: %d cases\n",head1->wd->wds->first_range);
				strcat(send_data,temp1);
				
				temp2 = malloc(sizeof(char)*strlen("Age range 21-40 years: %d cases\n")+1);
				sprintf(temp2, "Age range 21-40 years: %d cases\n",ptr3->sec_range);
				//printf("Age range 21-40 years: %d cases\n",head1->wd->wds->sec_range);
				strcat(send_data,temp2);
				
				temp3 = malloc(sizeof(char)*strlen("Age range 41-60 years: %d cases\n")+1);
				sprintf(temp3, "Age range 41-60 years: %d cases\n",ptr3->third_range);
				//printf("Age range 41-60 years: %d cases\n",head1->wd->wds->third_range);
				strcat(send_data,temp3);

				temp4 = malloc(sizeof(char)*strlen("Age range 60+ years: %d cases\n")+1);
				sprintf(temp4, "Age range 60+ years: %d cases\n",ptr3->fourth_range);
				//printf("Age range 60+ years: %d cases\n",head1->wd->wds->fourth_range);
				strcat(send_data,temp4);

				ptr3 = ptr3->next_disease;
			}
			//printf("send_dataing child %d....................\n",i );
			int len = strlen(send_data); //mhkos aftounou poy thelw na steilw
			int num = len;
			int digits = 0;
			while(num>0){
				num = num/10;
				digits++;
			}

			sock = 0;
			int valread; 
		    struct sockaddr_in serv_addr; 
		    //char buffer_s[1024] = {0}; 
		    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
		        printf("\n Socket creation error \n"); 
		        return -1; 
		    } 
		    serv_addr.sin_family = AF_INET; 
		    serv_addr.sin_addr.s_addr = INADDR_ANY; 
		    serv_addr.sin_port = htons(serverPort); 
		    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ 
		        perror("\nConnection Failed \n"); 
		        return -1; 
		    } 
		    int length = strlen(send_data);
		    write(sock,&maxWorkers,sizeof(int));
		    write(sock ,&length, sizeof(int));
		    send(sock , send_data , strlen(send_data) , 0 ); 
		   // printf("send data are %s",send_data);
		  	int cont;
		  	if ( ptr2->next_date == NULL)
		  		cont = 1;
		  	else
		  		cont = 0;
		  	write(sock,&i,sizeof(int));
		  	write(sock,&worker.sin_port,sizeof(int));
		  	write(sock,&cont,sizeof(int));
			close(sock); 
			ptr2 = ptr2->next_date;
		} 
		ptr1 = ptr1->next_country;
	} 
    printf("Worker %d is listening in poer %d...\n",i,worker.sin_port );
    if (listen(sock_work,10) < 0){
		perror("listen");exit(-1);
	}
	int s;
	while(s = accept(sock_work,workerptr,&workerlen)){
		int desc;
		int bytestoread = read(s,&desc,sizeof(int));
		if (desc == 1){
			char* virusName = malloc(sizeof(char)*30);
			char* country = malloc(sizeof(char)*30);
			char* entrydate = malloc(sizeof(char)*30);
			char* exitdate = malloc(sizeof(char)*30);
			int len1, len2, len3,len4;
			read(s,&len1,sizeof(int));
			int b = read(s,virusName,len1);
			virusName[b] = '\0';
			read(s,&len2,sizeof(int));
			b = read(s,entrydate, len2);
			entrydate[b] = '\0';
			read(s,&len3,sizeof(int));
			b = read(s,exitdate,len3);
			exitdate[b] = '\0';
			read(s,&len4,sizeof(int));
			b = read(s,country,len4);
			country[b] = '\0';
			//printf("%s\n", virusName);
			//printf("%s\n", entrydate);
			//printf("%s\n", exitdate);
			//printf("%s\n",country );
			Date* entryDates;
		   	Date* exitDates;
			entryDates = malloc(sizeof(Date));
			char*token = strtok(entrydate,"-");
			int counter = 0;
			while( token != NULL ) {;
			    if (counter == 0){
			      	entryDates->day = atoi(token);
			      	counter++;
			    }
			    else if (counter == 1){
			      	entryDates->month = atoi(token);
			      	counter++;
			    }else{
			      	entryDates->year = atoi(token);
			      	counter++;
			    }
			    token = strtok(NULL,"-");
			}
		    exitDates = malloc(sizeof(Date));
		    char* token1 = strtok(exitdate,"-");
		    int counter1 = 0;
		    while( token1 != NULL ) {;
		    	if (counter1 == 0){
		      		exitDates->day = atoi(token1);
		      		counter1++;
		      	}
		      	else if (counter1 == 1){
		      		exitDates->month = atoi(token1);
		      		counter1++;
		      	}else{
		      		exitDates->year = atoi(token1);
		      		counter1++;
		      	}
		      	token1= strtok(NULL,"-");
		   	}
		   	int ans;
		   	Work_countries * pp = head1;
		   	int found = 0;
		   	while(pp != NULL){
		   		if( strcmp(country,pp->country) == 0){
		   			found = 1;
		   		}
		   		pp = pp->next_country;
		   	}
		   	write(s,&found,sizeof(int));
		   	if (strcmp(country,"-") == 0){
		   		ans = diseaseFrequency(s,virusName,5,diseaseHash,entryDates,exitDates); 
		   	}
		   	else{
		   		ans = diseaseFrequency2(s,virusName,5,diseaseHash,entryDates,exitDates,country);
		   	}
		   	if(found == 1)
		   		write(s,&ans,sizeof(int));
			free(virusName);
			free(entrydate);
			free(exitdate);
			free(country); 
		}
		else if(desc ==2 ){
			char* country = malloc(sizeof(char)*30);
			char* virusName = malloc(sizeof(char)*30);
			int k;
			char* k1 = malloc(sizeof(char)*30);
			char* entrydate = malloc(sizeof(char)*30);
			char* exitdate = malloc(sizeof(char)*30);
			int len0,len1, len2, len3,len4;
			read(s,&len0,sizeof(int));
			int b = read(s,k1,len0);
			k1[b] = '\0';
			k = atoi(k1);
			read(s,&len1,sizeof(int));
			b = read(s,country,len1);
			country[b] = '\0';
			read(s,&len2,sizeof(int));
			b = read(s,virusName, len2);
			virusName[b] = '\0';
			read(s,&len3,sizeof(int));
			b = read(s,entrydate,len3);
			entrydate[b] = '\0';
			read(s,&len4,sizeof(int));
			b = read(s,exitdate,len4);
			exitdate[b] = '\0';
			//printf("%s\n",country );
			//printf("%s\n", virusName);
			//printf("%s\n", entrydate);
			//printf("%s\n", exitdate);
			Date* entryDates;
		   	Date* exitDates;
			entryDates = malloc(sizeof(Date));
			char*token = strtok(entrydate,"-");
			int counter = 0;
			while( token != NULL ) {;
			    if (counter == 0){
			      	entryDates->day = atoi(token);
			      	counter++;
			    }
			    else if (counter == 1){
			      	entryDates->month = atoi(token);
			      	counter++;
			    }else{
			      	entryDates->year = atoi(token);
			      	counter++;
			    }
			    token = strtok(NULL,"-");
			}
		    exitDates = malloc(sizeof(Date));
		    char* token1 = strtok(exitdate,"-");
		    int counter1 = 0;
		    while( token1 != NULL ) {;
		    	if (counter1 == 0){
		      		exitDates->day = atoi(token1);
		      		counter1++;
		      	}
		      	else if (counter1 == 1){
		      		exitDates->month = atoi(token1);
		      		counter1++;
		      	}else{
		      		exitDates->year = atoi(token1);
		      		counter1++;
		      	}
		      	token1= strtok(NULL,"-");
		   	}
		   	int ans;
		   	//ans = tokpCountries2(5,counrtyHash,virusName, k,entryDates,exitDates);
		   	//printf("ans is %d\n", ans);
		   	free(country);
		   	free(entrydate);
		   	free(exitdate);
		   	free(k1);
		}
		else if(desc == 3){ //ADMITTIONS
			char* virusName = malloc(sizeof(char)*30);
			char* country = malloc(sizeof(char)*30);
			char* entrydate = malloc(sizeof(char)*30);
			char* exitdate = malloc(sizeof(char)*30);
			int len1, len2, len3,len4;
			read(s,&len1,sizeof(int));
			int b = read(s,virusName,len1);
			virusName[b] = '\0';
			read(s,&len2,sizeof(int));
			b = read(s,entrydate, len2);
			entrydate[b] = '\0';
			read(s,&len3,sizeof(int));
			b = read(s,exitdate,len3);
			exitdate[b] = '\0';
			read(s,&len4,sizeof(int));
			b = read(s,country,len4);
			country[b] = '\0';
			Date* entryDates;
		   	Date* exitDates;
			entryDates = malloc(sizeof(Date));
			char*token = strtok(entrydate,"-");
			int counter = 0;
			while( token != NULL ) {;
			    if (counter == 0){
			      	entryDates->day = atoi(token);
			      	counter++;
			    }
			    else if (counter == 1){
			      	entryDates->month = atoi(token);
			      	counter++;
			    }else{
			      	entryDates->year = atoi(token);
			      	counter++;
			    }
			    token = strtok(NULL,"-");
			}
		    exitDates = malloc(sizeof(Date));

		    char* token1 = strtok(exitdate,"-");
		    int counter1 = 0;
		    while( token1 != NULL ) {;
		    	if (counter1 == 0){
		      		exitDates->day = atoi(token1);
		      		counter1++;
		      	}
		      	else if (counter1 == 1){
		      		exitDates->month = atoi(token1);
		      		counter1++;
		      	}else{
		      		exitDates->year = atoi(token1);
		      		counter1++;
		      	}
		      	token1= strtok(NULL,"-");
		   	}
			int k;
		   	Work_countries * pp = head1;
		   	int found = 0;
			   	while(pp != NULL){
			   		char* cc = strtok(country,"\n");
			   		if( strcmp(cc,pp->country) == 0){
			   			found = 1;
			   		}
			   		pp = pp->next_country;
			   	}
			if (strcmp(country,"error") == 0) 
				found = 1;
		   	write(s,&found,sizeof(int));
		   	if (found == 1){
				for(k=0;k<5;k++){
					Bucket* b1 = counrtyHash[k].bucketList;
					while(b1!=NULL){
						BucketEntry *ptr = b1->bucketEntryList;
						while(ptr!=NULL){
							if(strcmp(country,"error")!=0){
								int a = atoi(ptr->name);
								int b = atoi(country);
								if(a == b){// th vrhka th xwra
									inorder7(ptr->avlNode,entryDates,exitDates,country);
								}
							}
							else{
								int a = atoi(ptr->name);
								int b = atoi(country);
								inorder7(ptr->avlNode,entryDates,exitDates,country);
							}
							ptr = ptr->nextBucketEntry;
						}
						b1 = b1->nextBucket;
					}
				}
				write(s,&patients2,sizeof(int));
				patients2 = 0;
			}
		}
		else if(desc == 4){			
			int len;
			char* buf = malloc(sizeof(char)*30);
			read(s,&len,sizeof(int));
			int b = read(s,buf,len);
			buf[b] = '\0';
			//int found = 0;
			int a = atoi(buf);
			int found = 0;
			Record *help = head2_rec;
			while(help!= NULL){
				int b = atoi(help->recordID);
				if(a== b)
					found = 1;
				help = help->nextRecord;
			}
			write(s,&found,sizeof(int));
			if( found == 1){
				Record *help = head2_rec;
				while(help!=NULL){
					int b = atoi(help->recordID);
					if(a == b){
						int len = strlen(help->recordID);
						write(s,&len,sizeof(int));
						write(s,help->recordID,sizeof(char)*len);

						len = strlen(help->patientFirstName);
						write(s,&len,sizeof(int));
						write(s,help->patientFirstName,sizeof(char)*len);

						len = strlen(help->patientLastName);
						write(s,&len,sizeof(int));
						write(s,help->patientLastName,sizeof(char)*len);

						len = strlen(help->diseaseID);
						write(s,&len,sizeof(int));
						write(s,help->diseaseID,sizeof(char)*len);

						write(s,&help->age,sizeof(int));
						
						write(s,&help->entryDate->day,sizeof(int));
						write(s,&help->entryDate->month,sizeof(int));
						write(s,&help->entryDate->year,sizeof(int));

						write(s,&help->exitDate->day,sizeof(int));
						write(s,&help->exitDate->month,sizeof(int));
						write(s,&help->exitDate->year,sizeof(int)); 
					} 
					help = help->nextRecord;
				}
			}
		}
		else if(desc == 5){
			char* virusName = malloc(sizeof(char)*30);
			char* country = malloc(sizeof(char)*30);
			char* entrydate = malloc(sizeof(char)*30);
			char* exitdate = malloc(sizeof(char)*30);
			int len1, len2, len3,len4;
			read(s,&len1,sizeof(int));
			int b = read(s,virusName,len1);
			virusName[b] = '\0';
			read(s,&len2,sizeof(int));
			b = read(s,entrydate, len2);
			entrydate[b] = '\0';
			read(s,&len3,sizeof(int));
			b = read(s,exitdate,len3);
			exitdate[b] = '\0';
			read(s,&len4,sizeof(int));
			b = read(s,country,len4);
			country[b] = '\0';
			Date* entryDates;
		   	Date* exitDates;
			entryDates = malloc(sizeof(Date));
			char*token = strtok(entrydate,"-");
			int counter = 0;
			while( token != NULL ) {;
			    if (counter == 0){
			      	entryDates->day = atoi(token);
			      	counter++;
			    }
			    else if (counter == 1){
			      	entryDates->month = atoi(token);
			      	counter++;
			    }else{
			      	entryDates->year = atoi(token);
			      	counter++;
			    }
			    token = strtok(NULL,"-");
			}
		    exitDates = malloc(sizeof(Date));

		    char* token1 = strtok(exitdate,"-");
		    int counter1 = 0;
		    while( token1 != NULL ) {;
		    	if (counter1 == 0){
		      		exitDates->day = atoi(token1);
		      		counter1++;
		      	}
		      	else if (counter1 == 1){
		      		exitDates->month = atoi(token1);
		      		counter1++;
		      	}else{
		      		exitDates->year = atoi(token1);
		      		counter1++;
		      	}
		      	token1= strtok(NULL,"-");
		   	}
			int k;
		   	Work_countries * pp = head1;
		   	int found = 0;
			   	while(pp != NULL){
			   		char* cc = strtok(country,"\n");
			   		if( strcmp(cc,pp->country) == 0){
			   			found = 1;
			   		}
			   		pp = pp->next_country;
			   	}
			if (strcmp(country,"error") == 0) 
				found = 1;
		   	write(s,&found,sizeof(int));
		   	if (found == 1){
				for(k=0;k<5;k++){
					Bucket* b1 = counrtyHash[k].bucketList;
					while(b1!=NULL){
						BucketEntry *ptr = b1->bucketEntryList;
						while(ptr!=NULL){
							if(strcmp(country,"error")!=0){
								int a = atoi(ptr->name);
								int b = atoi(country);
								if(a == b){// th vrhka th xwra
									inorder6(ptr->avlNode,entryDates,exitDates,country);
								}
							}
							else{
								int a = atoi(ptr->name);
								int b = atoi(country);
								inorder6(ptr->avlNode,entryDates,exitDates,country);
							}
							ptr = ptr->nextBucketEntry;
						}
						b1 = b1->nextBucket;
					}
				}
				write(s,&patients,sizeof(int));
			}
		}
	}
	close(fd);
	unlink(buf1);
  	deleteList(&head2_rec);
  	exitHash(diseaseHash,counrtyHash);
   	exit(0);
}