#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "list.h"
#include "hash.h"
#include "bitmap.h"

/*
struct list_item{
	struct list_elem elem;
	int data;
};
*/
typedef struct create_dt{
	int type;
	char name[100];
	void* dt;
	int exist;
}cr_dt;

struct command{
	int count;
	char* token[100];
};
/*
struct hash_item{
	struct hash_elem elem;
	int data;
};
*/
void create(struct command com, cr_dt* dt, int dt_count);
void dumpdata(struct command com, cr_dt* dt, int dt_count);
void delete(struct command com, cr_dt* dt, int dt_count);
void delete_all(cr_dt* dt, int dt_count);
struct list_elem* find_nlist(struct list* list, int loc);
bool list_less(const struct list_elem* a, const struct list_elem* b, void* aux);
unsigned hash_func(const struct hash_elem* e, void* aux);
bool hash_less(const struct hash_elem* a, const struct hash_elem* b, void* aux);
void hash_action_square(struct hash_elem* e, void* aux);
void hash_action_triple(struct hash_elem* e, void* aux);
void hash_action_destructor(struct hash_elem* e, void* aux);
