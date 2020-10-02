#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "list.h"
#include "hash.h"
#include "bitmap.h"

#define MAX_INPUT_SIZE 500
#define MAX_REQUEST_TOKEN 20
#define MAX_DATA_STRUCTURE_NUM 30

struct request{
	int cnt;
	char* token[MAX_REQUEST_TOKEN];
};

typedef struct new_data{
	int type;
	char name[100];
	void* data;
	int exist;
};

int namecheck(char* name);
void create(struct request req);
void dumpdata(struct request req);
void delete(struct request req);
void delete_all();