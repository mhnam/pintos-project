#include "testlib.h"

static struct new_data new[MAX_DATA_STRUCTURE_NUM];
static int new_cnt = 0;

int main(){
	struct request req;
	static char input[MAX_INPUT_SIZE]; /*array for input sting*/
	int i, j, k, l;
	int fl = 1;
	int tmploc1, tmploc2, tmpdata;
	size_t size;
	struct list_item* new_listitem;
	struct list_elem* tmp_listelem1;
	struct list_elem* tmp_listelem2;
	struct list_elem* tmp_listelem3;
	
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
			//free(new);
			break;
		}

		else if(strcmp(req.token[0], "list_push_back")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				new_listitem = malloc(sizeof(struct list_item));
				new_listitem->data = atoi(req.token[2]);
				list_push_back(new[i].data, (struct list_elem*)new_listitem);
			}
		}

		else if(strcmp(req.token[0], "list_push_front")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				new_listitem = malloc(sizeof(struct list_item));
				new_listitem->data = atoi(req.token[2]);
				list_push_front(new[i].data, (struct list_elem*)new_listitem);
			}
		}
		
		else if(strcmp(req.token[0], "list_insert")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				new_listitem = malloc(sizeof(struct list_item));
				new_listitem->data = atoi(req.token[3]);
				list_insert(nth_elem(new[i].data, atoi(req.token[2])), (struct list_elem*)new_listitem);
			}
		}

		else if(strcmp(req.token[0], "list_insert_ordered")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				new_listitem = malloc(sizeof(struct list_item));
				new_listitem->data = atoi(req.token[2]);
				list_insert_ordered(new[i].data, (struct list_elem*)new_listitem, list_less, NULL);
			}
		}
		
		else if(strcmp(req.token[0], "list_empty")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				if(list_empty(new[i].data))
					fprintf(stdout, "true\n");
				else
					fprintf(stdout, "false\n");
			}
		}
		
		else if(strcmp(req.token[0], "list_front")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				tmp_listelem1 = list_front(new[i].data);
				fprintf(stdout, "%d\n", list_entry(tmp_listelem1, struct list_item, elem)->data)
			}
		}

		else if(strcmp(req.token[0], "list_back")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				tmp_listelem1 = list_back(new[i].data);
				fprintf(stdout, "%d\n", list_entry(tmp_listelem1, struct list_item, elem)->data)
			}
		}
		
		else if(strcmp(req.token[0], "list_size")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				fprintf(stdout, "%zu\n", list_size(new[i].data));
			}
		}
	
		else if(strcmp(req.token[0], "list_max")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				tmp_listelem1 = list_max(new[i].data, list_less, NULL);
				fprintf(stdout, "%d\n", list_entry(tmp_listelem1, struct list_item, elem)->data);
			}
		}
	
		else if(strcmp(req.token[0], "list_min")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				tmp_listelem1 = list_min(new[i].data, list_less, NULL);
				fprintf(stdout, "%d\n", list_entry(tmp_listelem1, struct list_item, elem)->data);
			}
		}
	
		else if(strcmp(req.token[0], "list_pop_front")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				tmp_listelem1 = list_pop_front(new[i].data);
				free(list_entry(tmp_listelem1, struct list_item, elem));
			}
		}
	
		else if(strcmp(req.token[0], "list_pop_back")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				tmp_listelem1 = list_pop_back(new[i].data);
				free(list_entry(tmp_listelem1, struct list_item, elem));
			}
		}

		else if(strcmp(req.token[0], "list_push_front")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				new_listitem = malloc(sizeof(struct list_item));
				new_listitem->data = atoi(req.token[2]);
				list_push_front(new[i].data, (struct list_elem*)new_listitem);
			}
		}

		else if(strcmp(req.token[0], "list_push_back")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				new_listitem = malloc(sizeof(struct list_item));
				new_listitem->data = atoi(req.token[2]);
				list_push_back(new[i].data, (struct list_elem*)new_listitem);
			}
		}

		else if(strcmp(req.token[0], "list_remove")==0){
			i = namecheck(req.token[1]);
			if(i > -1 && atoi(req.token[2]) < (int)(list_size(new[i].data))){
				tmp_listelem1 = nth_elem(new[i].data, atoi(req.token[2]));
				list_remove(tmp_listelem1);
				free(list_entry(tmp_listelem1, struct list_item, elem));
			}
		}
		
		else if(strcmp(req.token[0], "list_reverse")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				list_reverse(new[i].data);
			}
		}

		else if(strcmp(req.token[0], "list_shuffle")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				list_shuffle(new[i].data);
			}
		}
		
		else if(strcmp(req.token[0], "list_sort")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				list_sort(new[i].data, list_less, NULL);
			}
		}

		else if(strcmp(req.token[0], "list_splice")==0){
			i = namecheck(req.token[1]);
			j = namecheck(req.token[3]);
			if(i > -1 && j > -1){
				tmp_listelem1 = nth_elem(new[i].data, atoi(req.token[2]));
				tmp_listelem2 = nth_elem(new[j].data, atoi(req.token[4]));
				tmp_listelem3 = nth_elem(new[j].data, atoi(req.token[5]));
				list_splice(tmp_listelem1, tmp_listelem2, tmp_listelem3);
			}
		}
		
		else if(strcmp(req.token[0], "list_swap")==0){
			i = namecheck(req.token[1]);
			if(i > -1 && atoi(req.token[2])<list_size(new[i].data) && atoi(req.token[3])<list_size(new[i].data)){
				tmp_listelem1 = nth_elem(new[i].data, atoi(req.token[2]));
				tmp_listelem2 = nth_elem(new[i].data, atoi(req.token[3]));
				list_swap(tmp_listelem1, tmp_listelem2);
			}
		}
		
		else if(strcmp(req.token[0], "list_unique")==0){
			i = namecheck(req.token[1]);
			if(req.cnt == 3){
				j = namecheck(req.token[2]);
				if(i > -1 && j > -1){
					list_unique(new[i].data, new[j].data, list_less, NULL);
				}
			}
			else if(req.cnt == 2){
				if(i > -1){
					list_unique(new[i].data, NULL, list_less, NULL);
				}
			}
		}
		
		else if(strcmp(req.token[0], "bitmap_mark")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				bitmap_mark(new[i].data, atoi(req.token[2]));
			}
		}
		
		else if(strcmp(req.token[0], "bitmap_all")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				j = bitmap_all(new[i].data, atoi(req.token[2]), atoi(req.token[3]));
				if(j == 1)
					fprintf(stdout, "true\n");
				else if(j == 0)
					fprintf(stdout, "false\n");
			}
		}

		else if(strcmp(req.token[0], "bitmap_any")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				j = bitmap_any(new[i].data, atoi(req.token[2]), atoi(req.token[3]));
				if(j == 1)
					fprintf(stdout, "true\n");
				else if(j == 0)
					fprintf(stdout, "false\n");
			}
		}
		
		else if(strcmp(req.token[0], "bitmap_count")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				if(strcmp(req.token[4], "true") == 0)
					size = bitmap_count(new[i].data, atoi(req.token[2]), atoi(req.token[3]), 1);
				else if(strcmp(req.token[4], "false") == 0)
					size = bitmap_count(new[i].data, atoi(req.token[2]), atoi(req.token[3]), 0);
				fprintf(stdout, "%zu\n", size);
			}
		}

		else if(strcmp(req.token[0], "bitmap_contains")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				if(strcmp(req.token[4], "true") == 0)
					j = bitmap_contains(new[i].data, atoi(req.token[2]), atoi(req.token[3]), 1);
				else if(strcmp(req.token[4], "false") == 0)
					j = bitmap_contains(new[i].data, atoi(req.token[2]), atoi(req.token[3]), 0);
				if(j == 1)
					fprintf(stdout, "true\n");
				else if(j == 0)
					fprintf(stdout, "false\n");
			}
		}

		else if(strcmp(req.token[0], "bitmap_dump")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				bitmap_dump(new[i].data);
			}
		}

		else if(strcmp(req.token[0], "bitmap_expand")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				bitmap_expand(new[i].data, atoi(req.token[2]));
			}
		}

		else if(strcmp(req.token[0], "bitmap_flip")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				bitmap_flip(new[i].data, atoi(req.token[2]));
			}
		}
		
		else if(strcmp(req.token[0], "bitmap_none")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				j = bitmap_none(new[i].data, atoi(req.token[2]), atoi(req.token[3]));
				if(j == 1)
					fprintf(stdout, "true\n");
				else if(j == 0)
					fprintf(stdout, "false\n");
			}
		}

		else if(strcmp(req.token[0], "bitmap_reset")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				bitmap_reset(new[i].data, atoi(req.token[2]));
			}
		}
		
		else if(strcmp(req.token[0], "bitmap_scan")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				if(strcmp(req.token[4], "true") == 0)
					size = bitmap_scan(new[i].data, atoi(req.token[2]), atoi(req.token[3]), 1);
				else if(strcmp(req.token[4], "false") == 0)
					size = bitmap_scan(new[i].data, atoi(req.token[2]), atoi(req.token[3]), 0);
				fprintf(stdout, "%zu\n", size);
			}
		}

		else if(strcmp(req.token[0], "bitmap_scan_and_flip")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				if(strcmp(req.token[4], "true") == 0)
					size = bitmap_scan_and_flip(new[i].data, atoi(req.token[2]), atoi(req.token[3]), 1);
				else if(strcmp(req.token[4], "false") == 0)
					size = bitmap_scan_and_flip(new[i].data, atoi(req.token[2]), atoi(req.token[3]), 0);
				fprintf(stdout, "%zu\n", size);
			}
		}

		else if(strcmp(req.token[0], "bitmap_set")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				if(strcmp(req.token[3], "true") == 0)
					bitmap_set(new[i].data, atoi(req.token[2]), 1);
				else if(strcmp(req.token[3], "false") == 0)
					bitmap_set(new[i].data, atoi(req.token[2]), 0);
			}
		}

		else if(strcmp(req.token[0], "bitmap_set_all")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				if(strcmp(req.token[2], "true") == 0)
					bitmap_set_all(new[i].data, 1);
				else if(strcmp(req.token[2], "false") == 0)
					bitmap_set_all(new[i].data, 0);
			}
		}

		else if(strcmp(req.token[0], "bitmap_set_multiple")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				if(strcmp(req.token[4], "true") == 0)
					bitmap_set_multiple(new[i].data, atoi(req.token[2]), atoi(req.token[3]), 1);
				else if(strcmp(req.token[4], "false") == 0)
					bitmap_set_multiple(new[i].data, atoi(req.token[2]), atoi(req.token[3]), 0);
			}
		}

		else if(strcmp(req.token[0], "bitmap_size")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				size = bitmap_size(new[i].data);
				fprintf(stdout, "%zu\n", size);
			}
		}
		
		else if(strcmp(req.token[0], "bitmap_test")==0){
			i = namecheck(req.token[1]);
			if(i > -1){
				j = bitmap_test(new[i].data, atoi(req.token[2]));
				if(j == 1)
					fprintf(stdout, "true\n");
				else if(j == 0)
					fprintf(stdout, "false\n");
			}
		}
		
	/*hash table*/
	
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
			new[new_cnt].type = 0;
			strcpy(new[new_cnt].name, req.token[2]);
			new[new_cnt].data = malloc(sizeof(struct list));
			list_init((struct list*)new[new_cnt].data);
			new[new_cnt].exist = 1;
		}
		else if (strcmp(req.token[1], "hashtable") == 0) {
			new[new_cnt].type = 1;
			strcpy(new[new_cnt].name, req.token[2]);
			new[new_cnt].data = malloc(sizeof(struct hash));
	//			hash_init((struct hash*)new->data, hash_func, hash_less, NULL);
			new[new_cnt].exist = 1;
		}
		else if (strcmp(req.token[1], "bitmap") == 0) {
			new[new_cnt].type = 2;
			strcpy(new[new_cnt].name, req.token[2]);
			size = atoi(req.token[3]);
			new[new_cnt].data = bitmap_create(size);
			new[new_cnt].exist = 1;
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

/* ACCORDING TO LIST.H:
	Compares the value of two list elements A and B, given
	auxiliary data AUX.  Returns true if A is less than B, or
	false if A is greater than or equal to B. */

bool list_less(const struct list_elem *a, const struct list_elem *b, void *aux){
	struct list_item* tmp_listitem1 = list_entry(a, struct list_item, elem);
	struct list_item* tmp_listitem2 = list_entry(b, struct list_item, elem);
	
	if(tmp_listitem1->data < tmp_listitem2->data)
		return true;
	else if(tmp_listitem1->data >= tmp_listitem2->data)
		return false;
}