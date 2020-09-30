#include "testlib.h"

int main(){
	struct request req;
	static char input[MAX_INPUT_SIZE]; /*array for input sting*/
	struct new_data* new[MAX_DATA_STRUCTURE_NUM];
	int new_cnt = 0;
	int i;
	int fl = 1;
	
	//initialise()

	while(1){
		fgets(input, MAX_INPUT_SIZE, stdin);
		req.cnt = 0;
		req.token[req.cnt] = strtok(input, " \t\n");
		
		while(req.token[req.cnt]==NULL){
			req.token[++req.cnt] = strtok(NULL, " \t\n");
		}
		
		if(strcmp(req.token[0], "create")==0){
			for (i = 0; i < new_cnt; i++) { /*chk whether given name is valid*/
				if ((strcmp(req.token[2], new[i].name) == 0) && new[i].exist == 1) {
					fl = -1;
					break;
				}
			}
			if (fl){
//				new = (struct new_data*)realloc(new, sizeof(struct new_data)*(new_cnt+2)); /*add one extra space for data that will be added later*/
				create(req, &new[new_cnt++]);
			}
			else if (fl == -1)
				break;
		}
		else if(strcmp(req.token[0], "dumpdata")==0){
			dumpdata(req, new, new_cnt);
		}
		else if(strcmp(req.token[0], "delete")==0){
			delete(req, new, new_cnt);
		}
		else if(strcmp(req.token[0], "quit")==0){
			delete_all(new, new_cnt);
			free(new);
			break;
		}
	}
	return 0;
}

void create(struct request req, struct new_data* new){
	size_t size;
	int i;
	
	if (strcmp(req.token[1], "list") == 0) {
		new->type = 0;
		strcpy(new->name, req.token[2]);
		new->data = malloc(sizeof(struct list));
		list_init((struct list*)new->data);
		new->exist = 1;
	}
	else if (strcmp(req.token[1], "hashtable") == 0) {
		new->type = 1;
		strcpy(new->name, req.token[2]);
		new->data = malloc(sizeof(struct hash));
//			hash_init((struct hash*)new->data, hash_func, hash_less, NULL);
		new->exist = 1;
	}
	else if (strcmp(req.token[1], "bitmap") == 0) {
		new->type = 2;
		strcpy(new->name, req.token[2]);
		size = atoi(req.token[3]);
		new->data = bitmap_create(size);
		new->exist = 1;
	}
}

void dumpdata(struct request req, struct new_data* new, int new_cnt){
	struct list_elem* tmp_elem;
	struct list_item* tmp_listitem;
	struct hash_iterator* iter = malloc(sizeof(struct hash_iterator));
	struct hash_item* tmp_hashitem;
	struct bitmap* tmp_bit;
	int i;
	int type = -1;
	int cnt = 0;
	int fl = -1;
	size_t size;
	
	for(i = 0; i < new_cnt; i++){ /*find whether given varialbe exist*/
		if(strcmp(new[i].name, req.token[1]) == 0){
			type = new[i].type;
			fl = 1;
			break;
		}
	}
	if (fl) {
		switch(type){
			case 0: /*list*/
				for(tmp_elem = list_begin((struct list*)new[i].data); tmp_elem != list_end((struct list*)new[i].data); tmp_elem = list_next(tmp_elem)){
					tmp_listitem = list_entry(tmp_elem, struct list_item, elem);
					fprintf(stdout, "%d ", tmp_listitem->data);
					cnt++;
				}
				if(cnt > 0)
					fprintf(stdout, "\n");
				break;
			
			case 1: /*hashtable*/
				hash_first(iter, (struct hash*)new[i].data);
				while(hash_next(iter)){
					tmp_hashitem = hash_entry(hash_cur(iter), struct hash_item, elem);
					fprintf(stdout, "%d ", tmp_hashitem->data);
					cnt++;
				}
				if(cnt > 0)
					fprintf(stdout, "\n");
				break;
				
			case 2: /*bitmap*/
				tmp_bit = new[i].data;
				size = bitmap_size(tmp_bit);
				for(i = 0; i < (int)size; i++){
					if(bitmap_test(tmp_bit, i))
						fprintf(stdout, "1");
					else
						fprintf(stdout, "0");
				}
				if(size > 0)
					fprintf(stdout, "\n");
				break;
		}
	}
}

void delete(struct request req, struct new_data* new, int new_cnt){
	struct list* tmp_list;
	struct list_elem* tmp_elem;
	struct list_item* tmp_listitem;
	struct hash* tmp_hash;
	struct bitmap* tmp_bit;
	int i;
	int fl = -1;
	int type;

	for(i = 0; i < new_cnt; i++){
		if(strcmp(new[i].name, req.token[1]) == 0){
			new[i].exist = 0;
			fl = 1;
			type = new[i].type;
			break;
		}
	}
	if (fl) {
		switch(type){
			case 0:
				tmp_list = (struct list*)new[i].data;
				while(!list_empty(tmp_list)){
					tmp_elem = list_pop_front(tmp_list);
					tmp_listitem = list_entry(tmp_elem, struct list_item, elem);
					free(tmp_listitem);
				}
				free(tmp_list);
				break;
				
			case 1:
				tmp_hash = (struct hash*)new[i].data;
//				hash_destroy(tmp_hash, hash_action_destructor);			
				break;
				
			case 2:
				tmp_bit = (struct bitmap*)new[i].data;
				bitmap_destroy(tmp_bit);
				break;
		}
	}
}

void delete_all(struct new_data* new, int new_cnt) {
	struct list* tmp_list;
	struct list_elem* tmp_elem;
	struct list_item* tmp_listitem;
	struct hash* tmp_hash;
	struct bitmap* tmp_bit;
	int i;
	int type;

	for (i = 0; i < new_cnt; i++) {
		if (new[i].exist == 1) {
			type = new[i].type;
			switch(type){
				case 0:
					tmp_list = (struct list*)new[i].data;
					while(!list_empty(tmp_list)){
						tmp_elem = list_pop_front(tmp_list);
						tmp_listitem = list_entry(tmp_elem, struct list_item, elem);
						free(tmp_listitem);
					}
					free(tmp_list);
					break;

				case 1:
					tmp_hash = (struct hash*)new[i].data;
//					hash_destroy(tmp_hash, hash_action_destructor);
					break;

				case 2:
					tmp_bit = (struct bitmap*)new[i].data;
					bitmap_destroy(tmp_bit);
					break;
			}
		}
	}
}