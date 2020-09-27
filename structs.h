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
#include <stdbool.h> 
#include <dirent.h>
#include <assert.h>

typedef struct circular_buf_t circular_buf_t;
typedef circular_buf_t* cbuf_handle_t;

typedef struct date{
	int day;
	int month;
	int year;
}Date;

typedef struct list{ //list with the countries
	int num_work;
	char* country;
	struct list* next;
}List;

typedef struct range_list{
	int value;
	int type;
	struct range_list *next;
}Range_list;

typedef struct record{
	char* recordID;
	char* situation;
	char* patientFirstName;
	char* patientLastName;
	char* diseaseID;
	char* country;
	Date* entryDate;
	Date* exitDate;
	int  age;
	struct record* nextRecord;
}Record;

typedef struct avlNode{
	int ht;
	Record* ptr;
	struct avlNode* right;
	struct avlNode* left;
}AvlNode;

typedef struct bucketEntry{
	char* name;
	AvlNode *avlNode;
	struct bucketEntry* nextBucketEntry;
}BucketEntry;

typedef struct bucket{
	BucketEntry* bucketEntryList;
	int entries;
	struct bucket* nextBucket;
}Bucket;

typedef struct hashNode{
	Bucket* bucketList;
}HashNode;

typedef struct buffer{
	int fd;
	struct buffer* next;
}Buffer;

typedef struct thread_c{
	pthread_t thread;
	struct thread_c* next;
}Thread_c;

typedef struct portList{
	int worker;
	int port;
	struct portList* next;
}PortList;

typedef struct fd{
	int fd;
	int type;
}Fd;

typedef struct {
	Fd **data;
	int start;
	int end;
	int count;
	int size;
} pool_t;

typedef struct {
	char** data;
	int start;
	int end;
	int count;
	int size;
}pool_t_client;

