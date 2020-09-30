#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "list.h"
#include "hash.h"
#include "bitmap.h"

#define MAX_INPUT_SIZE 500
#define MAX_REQUEST_TOKEN 20

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

void create(struct request req, struct new_data* new, int new_cnt);
void dumpdata(struct request req, struct new_data* new, int new_cnt);
void delete(struct request req, struct new_data* new, int new_cnt);
void delete_all(struct new_data* new, int new_cnt);