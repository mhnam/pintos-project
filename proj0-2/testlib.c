#include "testlib.h"

static struct new_data new[MAX_DATA_STRUCTURE_NUM];
static int new_cnt = 0;

int main(){
	struct request req;
	static char input[MAX_INPUT_SIZE]; /*array for input sting*/
	int i;
	int fl = 1;
	int tmploc1, tmploc2, tmpdata;
	struct list_item* new_listitem;
	
	//initialise()

	while(1){
		fgets(input, MAX_INPUT_SIZE, stdin);
		req.cnt = 0;
		req.token[req.cnt] = strtok(input, " \t\n");
		
		while(1){
			req.token[++req.cnt] = strtok(NULL, " \t\n");
			if(req.token[req.cnt]==NULL)
				break;
		}
		
		if(strcmp(req.token[0], "create")==0)
			create(req);
		
		else if(strcmp(req.token[0], "dumpdata")==0)
			dumpdata(req);
		
		else if(strcmp(req.token[0], "delete")==0)
			delete(req);
		
		else if(strcmp(req.token[0], "quit")==0){
			delete_all();
			free(new);
			break;
		}

		else if(strcmp(req.token[0], "list_push_back")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				new_listitem = malloc(sizeof(struct list_item));
				new_listitem->data = atoi(req.token[2]);
				list_push_back((struct list*)new[i].data, (struct list_elem*)new_listitem);			
			}
		}

		else if(strcmp(req.token[0], "list_push_front")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				new_listitem = malloc(sizeof(struct list_item));
				new_listitem->data = atoi(req.token[2]);
				list_push_back((struct list*)new[i].data, (struct list_elem*)new_listitem);			
			}
		}
	}
	
	return 0;
}

int namecheck(char* name){
	int i;
	int fl = -1;
	
	for (i = 0; i < MAX_DATA_STRUCTURE_NUM; i++) {
		if ((strcmp(name, new[i].name) == 0) && new[i].exist == 1) {
			fl = i;
			break;
		}
	}
	return fl;
}

void create(struct request req){
	size_t size;
	int fl;
	
	fl = namecheck(req.token[2]);
	if(fl == -1){
		new_cnt++;
		if (strcmp(req.token[1], "list") == 0) {
			new[new_cnt]->type = 0;
			strcpy(new[new_cnt]->name, req.token[2]);
			new[new_cnt]->data = malloc(sizeof(struct list));
			list_init((struct list*)new[new_cnt]->data);
			new[new_cnt]->exist = 1;
		}
		else if (strcmp(req.token[1], "hashtable") == 0) {
			new[new_cnt]->type = 1;
			strcpy(new[new_cnt]->name, req.token[2]);
			new[new_cnt]->data = malloc(sizeof(struct hash));
	//			hash_init((struct hash*)new->data, hash_func, hash_less, NULL);
			new->exist = 1;
		}
		else if (strcmp(req.token[1], "bitmap") == 0) {
			new[new_cnt]->type = 2;
			strcpy(new[new_cnt]->name, req.token[2]);
			size = atoi(req.token[3]);
			new[new_cnt]->data = bitmap_create(size);
			new[new_cnt]->exist = 1;
		}
	}
}

void dumpdata(struct request req){
	struct list_elem* tmp_elem;
	struct list_item* tmp_listitem;
	struct hash_iterator* iter = malloc(sizeof(struct hash_iterator));
	struct hash_item* tmp_hashitem;
	struct bitmap* tmp_bit;
	int i;
	int type;
	int cnt = 0;
	size_t size;
	
	i = namecheck(req.token[1]);
	
	if(i > -1){
		type = new[i].type;	
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
				for(cnt = 0; cnt < (int)size; cnt++){
					if(bitmap_test(tmp_bit, cnt))
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

void delete(struct request req){
	struct list* tmp_list;
	struct list_elem* tmp_elem;
	struct list_item* tmp_listitem;
	struct hash* tmp_hash;
	struct bitmap* tmp_bit;
	int i;
	int fl = -1;
	int type;
	
	i = namecheck(req.token[1]);
	if(i > -1){
		new[i].exist = 0;
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
//				hash_destroy(tmp_hash, hash_action_destructor);			
				break;
				
			case 2:
				tmp_bit = (struct bitmap*)new[i].data;
				bitmap_destroy(tmp_bit);
				break;
		}
	}
}

void delete_all(void) {
	struct list* tmp_list;
	struct list_elem* tmp_elem;
	struct list_item* tmp_listitem;
	struct hash* tmp_hash;
	struct bitmap* tmp_bit;
	int i;
	int type;

	for (i = 0; i < MAX_DATA_STRUCTURE_NUM; i++) {
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