#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "list.h"
#include "hash.h"
#include "bitmap.h"
#include "hex_dump.h"

#define MAX_INPUT_SIZE 500
#define MAX_REQUEST_TOKEN 20
#define MAX_DATA_STRUCTURE_NUM 30

struct request{
	int cnt;
	char* token[MAX_REQUEST_TOKEN];
};

struct new_data{
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
bool list_less(const struct list_elem *, const struct list_elem *, void *);
unsigned hash_hash (const struct hash_elem *e, void *aux);
bool hash_less (const struct hash_elem *a, const struct hash_elem *b, void *aux);
void hash_action_square (struct hash_elem *e, void *aux);
void hash_action_triple (struct hash_elem *e, void *aux);
void hash_action_destructor(struct hash_elem* e, void* aux);
