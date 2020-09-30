#include "main.h"

int main(void){
	struct command com;
	cr_dt* dt=(cr_dt*)malloc(sizeof(cr_dt));
	int dt_count=0, i;
	char input[300]={'\0'};
	int loc, loc2, data, ext, ext2;
	struct list_elem* e;
	struct list_elem* e2;
	struct list_elem* e3;
	struct list_item* new;
	struct list_item* item;
	int one, two;
	int tf;
	size_t size;
	int list_sp_st, list_sp_ed;
	struct hash_item* new2;
	struct hash_item* want;
	struct hash_elem* old;
	struct hash_elem* find;
	int bitmap_loc;
	int bitmap_st, bitmap_ed;
	int bit_size;
	while(1){
		ext=0;
		ext2 = 0;
		fgets(input, sizeof(char)*300, stdin);
		com.count=0;
		com.token[com.count]=strtok(input, " \t\n");
		while(1){
			com.count++;
			com.token[com.count]=strtok(NULL, " \t\n");
			if(com.token[com.count]==NULL){
				break;
			}
		}
		if(strcmp(com.token[0], "create")==0){
			dt=(cr_dt*)realloc(dt, sizeof(cr_dt)*(dt_count+2));
			create(com, &dt[dt_count], dt_count);
			dt_count++;
		}
		else if(strcmp(com.token[0], "dumpdata")==0){
			dumpdata(com, dt, dt_count);
		}
		else if(strcmp(com.token[0], "delete")==0){
			delete(com, dt, dt_count);
		}
		else if(strcmp(com.token[0], "quit")==0){
			delete_all(dt, dt_count);
			free(dt);
			break;
		}
		else if(strcmp(com.token[0], "list_insert")==0){
			loc=atoi(com.token[2]);
			data=atoi(com.token[3]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						e=find_nlist((struct list*)dt[i].dt, loc);
						ext=1;
						break;
					}
				}
			}
			if(ext==1){
				new=(struct list_item*)malloc(sizeof(struct list_item));
				new->data=data;
				list_insert(e, (struct list_elem*)new);
			}
		}
		else if(strcmp(com.token[0], "list_push_back")==0){
			data=atoi(com.token[2]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				new = (struct list_item*)malloc(sizeof(struct list_item));
				new->data = data;
				list_push_back(dt[i].dt, (struct list_elem*)new);
			}
			
		}
		else if(strcmp(com.token[0], "list_push_front")==0){
			data=atoi(com.token[2]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				new = (struct list_item*)malloc(sizeof(struct list_item));
				new->data = data;
				list_push_front(dt[i].dt, (struct list_elem*)new);
			}
			
		}
		else if(strcmp(com.token[0], "list_splice")==0){
			loc=atoi(com.token[2]);
			list_sp_st=atoi(com.token[4]);
			list_sp_ed=atoi(com.token[5]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						one=i;
						e=find_nlist((struct list*)dt[one].dt, loc);
						ext = 1;
						break;
					}
				}
			}
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[3])==0){
						two=i;
						e2=find_nlist((struct list*)dt[two].dt, list_sp_st);
						e3=find_nlist((struct list*)dt[two].dt, list_sp_ed);
						ext2 = 1;
						break;
					}
				}
			}
			if (ext == 1 && ext2 == 1) {
				list_splice(e, e2, e3);
			}
		}
		else if(strcmp(com.token[0], "list_swap")==0){
			loc=atoi(com.token[2]);
			loc2=atoi(com.token[3]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				e = find_nlist((struct list*)dt[i].dt, loc);
				e2 = find_nlist((struct list*)dt[i].dt, loc2);
				list_swap(e, e2);
			}
		}
		else if(strcmp(com.token[0], "list_shuffle")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				list_shuffle((struct list*)dt[i].dt);
			}
		}
		else if(strcmp(com.token[0], "list_unique")==0){
			if(com.count==3){
				for(i=0; i<dt_count; i++){
					if(dt[i].exist==1){
						if(strcmp(dt[i].name, com.token[1])==0){
							one=i;
							ext = 1;
							break;
						}
					}
				}
				for(i=0; i<dt_count; i++){
					if(dt[i].exist==1){
						if(strcmp(dt[i].name, com.token[2])==0){
							two=i;
							ext2 = 1;
							break;
						}
					}
				}
				if (ext == 1 && ext2 == 1) {
					list_unique(dt[one].dt, dt[two].dt, list_less, NULL);
				}
			}
			else if(com.count==2){
				for(i=0; i<dt_count; i++){
					if(dt[i].exist==1){
						if(strcmp(dt[i].name, com.token[1])==0){
							ext = 1;
							break;
						}
					}
				}
				if (ext == 1) {
					list_unique(dt[i].dt, NULL, list_less, NULL);
				}
			}
		}
		else if(strcmp(com.token[0], "list_sort")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				list_sort(dt[i].dt, list_less, NULL);
			}
		}
		else if(strcmp(com.token[0], "list_reverse")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				list_reverse(dt[i].dt);
			}
		}
		else if(strcmp(com.token[0], "list_insert_ordered")==0){
			data=atoi(com.token[2]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				new = (struct list_item*)malloc(sizeof(struct list_item));
				new->data = data;
				list_insert_ordered(dt[i].dt, (struct list_elem*)new, list_less, NULL);
			}
		}
		else if(strcmp(com.token[0], "list_remove")==0){
			loc=atoi(com.token[2]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						e=find_nlist((struct list*)dt[i].dt, loc);
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				list_remove(e);
				free(list_entry(e, struct list_item, elem));
			}
		}
		else if(strcmp(com.token[0], "list_front")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				e = list_front(dt[i].dt);
				item = list_entry(e, struct list_item, elem);
				fprintf(stdout, "%d\n", item->data);
			}
		}
		else if(strcmp(com.token[0], "list_back")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				e = list_back(dt[i].dt);
				item = list_entry(e, struct list_item, elem);
				fprintf(stdout, "%d\n", item->data);
			}
		}
		else if(strcmp(com.token[0], "list_pop_back")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				e = list_pop_back(dt[i].dt);
				free(list_entry(e, struct list_item, elem));
			}
		}
		else if(strcmp(com.token[0], "list_pop_front")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				e = list_pop_front(dt[i].dt);
				free(list_entry(e, struct list_item, elem));
			}
		}
		else if(strcmp(com.token[0], "list_empty")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				tf = list_empty(dt[i].dt);
				if (tf == 1) {
					fprintf(stdout, "true\n");
				}
				else {
					fprintf(stdout, "false\n");
				}
			}
		}
		else if(strcmp(com.token[0], "list_size")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				size = list_size(dt[i].dt);
				fprintf(stdout, "%zu\n", size);
			}
		}
		else if(strcmp(com.token[0], "list_max")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				e = list_max(dt[i].dt, list_less, NULL);
				item = list_entry(e, struct list_item, elem);
				fprintf(stdout, "%d\n", item->data);
			}
		}
		else if(strcmp(com.token[0], "list_min")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				e = list_min(dt[i].dt, list_less, NULL);
				item = list_entry(e, struct list_item, elem);
				fprintf(stdout, "%d\n", item->data);
			}
		}
		else if(strcmp(com.token[0], "hash_insert")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				data = atoi(com.token[2]);
				new2 = (struct hash_item*)malloc(sizeof(struct hash_item));
				new2->data = data;
				old = hash_insert((struct hash*)dt[i].dt, (struct hash_elem*)new2);
				if (old != NULL) {
					free(new2);
				}
			}
		}
		else if(strcmp(com.token[0], "hash_delete")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				data = atoi(com.token[2]);
				want = malloc(sizeof(struct hash_item));
				want->data = data;
				find = hash_delete((struct hash*)dt[i].dt, (struct hash_elem*)want);
				if (find != NULL) {
					free(find);
				}
			}
		}
		else if(strcmp(com.token[0], "hash_empty")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				tf = hash_empty((struct hash*)dt[i].dt);
				if (tf == 1) {
					fprintf(stdout, "true\n");
				}
				else {
					fprintf(stdout, "false\n");
				}
			}
		}
		else if(strcmp(com.token[0], "hash_find")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				data = atoi(com.token[2]);
				want = malloc(sizeof(struct hash_item));
				want->data = data;
				find = hash_find((struct hash*)dt[i].dt, (struct hash_elem*)want);
				if (find != NULL) {
					fprintf(stdout, "%d\n", data);
				}
			}
		}
		else if(strcmp(com.token[0], "hash_size")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				size = hash_size((struct hash*)dt[i].dt);
				fprintf(stdout, "%zu\n", size);
			}
		}
		else if(strcmp(com.token[0], "hash_clear")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				hash_clear((struct hash*)dt[i].dt, hash_action_destructor);
			}
		}
		else if(strcmp(com.token[0], "hash_replace")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				data = atoi(com.token[2]);
				new2 = (struct hash_item*)malloc(sizeof(struct hash_item));
				new2->data = data;
				old = hash_replace((struct hash*)dt[i].dt, (struct hash_elem*)new2);
				if (old != NULL) {
					free(old);
				}
			}
		}
		else if(strcmp(com.token[0], "hash_apply")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				if (strcmp(com.token[2], "square") == 0) {
					hash_apply((struct hash*)dt[i].dt, hash_action_square);
				}
				else if (strcmp(com.token[2], "triple") == 0) {
					hash_apply((struct hash*)dt[i].dt, hash_action_triple);
				}
			}
		}
		else if(strcmp(com.token[0], "bitmap_set")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				bitmap_loc = atoi(com.token[2]);
				if (strcmp(com.token[3], "true") == 0) {
					bitmap_set((struct bitmap*)dt[i].dt, bitmap_loc, 1);
				}
				else if (strcmp(com.token[3], "false") == 0) {
					bitmap_set((struct bitmap*)dt[i].dt, bitmap_loc, 0);
				}
			}
		}
		else if(strcmp(com.token[0], "bitmap_set_all")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				if (strcmp(com.token[2], "true") == 0) {
					bitmap_set_all((struct bitmap*)dt[i].dt, 1);
				}
				else if (strcmp(com.token[2], "false") == 0) {
					bitmap_set_all((struct bitmap*)dt[i].dt, 0);
				}
			}
		}
		else if(strcmp(com.token[0], "bitmap_size")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				size = bitmap_size((const struct bitmap*)dt[i].dt);
				fprintf(stdout, "%zu\n", size);
			}
		}
		else if(strcmp(com.token[0], "bitmap_mark")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				bitmap_loc = atoi(com.token[2]);
				bitmap_mark((struct bitmap*)dt[i].dt, bitmap_loc);
			}
		}
		else if(strcmp(com.token[0], "bitmap_count")==0){
			bitmap_st=atoi(com.token[2]);
			bitmap_ed=atoi(com.token[3]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				if (strcmp(com.token[4], "true") == 0) {
					size = bitmap_count((const struct bitmap*)dt[i].dt, bitmap_st, bitmap_ed, 1);
				}
				else if (strcmp(com.token[4], "false") == 0) {
					size = bitmap_count((const struct bitmap*)dt[i].dt, bitmap_st, bitmap_ed, 0);
				}
				fprintf(stdout, "%zu\n", size);
			}
		}
		else if(strcmp(com.token[0], "bitmap_test")==0){
			bitmap_loc=atoi(com.token[2]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				tf = bitmap_test((const struct bitmap*)dt[i].dt, bitmap_loc);
				if (tf == 1) {
					fprintf(stdout, "true\n");
				}
				else if (tf == 0) {
					fprintf(stdout, "false\n");
				}
			}
		}
		else if(strcmp(com.token[0], "bitmap_contains")==0){
			bitmap_st=atoi(com.token[2]);
			bitmap_ed=atoi(com.token[3]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				if (strcmp(com.token[4], "true") == 0) {
					tf = bitmap_contains((const struct bitmap*)dt[i].dt, bitmap_st, bitmap_ed, 1);
				}
				else if (strcmp(com.token[4], "false") == 0) {
					tf = bitmap_contains((const struct bitmap*)dt[i].dt, bitmap_st, bitmap_ed, 0);
				}
				if (tf == 1) {
					fprintf(stdout, "true\n");
				}
				else if (tf == 0) {
					fprintf(stdout, "false\n");
				}
			}
		}
		else if(strcmp(com.token[0], "bitmap_all")==0){
			bitmap_st=atoi(com.token[2]);
			bitmap_ed=atoi(com.token[3]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				tf = bitmap_all((const struct bitmap*)dt[i].dt, bitmap_st, bitmap_ed);
				if (tf == 1) {
					fprintf(stdout, "true\n");
				}
				else if (tf == 0) {
					fprintf(stdout, "false\n");
				}
			}
		}
		else if(strcmp(com.token[0], "bitmap_any")==0){
			bitmap_st=atoi(com.token[2]);
			bitmap_ed=atoi(com.token[3]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				tf = bitmap_any((const struct bitmap*)dt[i].dt, bitmap_st, bitmap_ed);
				if (tf == 1) {
					fprintf(stdout, "true\n");
				}
				else if (tf == 0) {
					fprintf(stdout, "false\n");
				}
			}
		}
		else if(strcmp(com.token[0], "bitmap_none")==0){
			bitmap_st=atoi(com.token[2]);
			bitmap_ed=atoi(com.token[3]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				tf = bitmap_none((const struct bitmap*)dt[i].dt, bitmap_st, bitmap_ed);
				if (tf == 1) {
					fprintf(stdout, "true\n");
				}
				else if (tf == 0) {
					fprintf(stdout, "false\n");
				}
			}
		}
		else if(strcmp(com.token[0], "bitmap_set_multiple")==0){
			bitmap_st=atoi(com.token[2]);
			bitmap_ed=atoi(com.token[3]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				if (strcmp(com.token[4], "true") == 0) {
					bitmap_set_multiple((struct bitmap*)dt[i].dt, bitmap_st, bitmap_ed, 1);
				}
				else if (strcmp(com.token[4], "false") == 0) {
					bitmap_set_multiple((struct bitmap*)dt[i].dt, bitmap_st, bitmap_ed, 0);
				}
			}
		}
		else if(strcmp(com.token[0], "bitmap_flip")==0){
			bitmap_loc=atoi(com.token[2]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				bitmap_flip((struct bitmap*)dt[i].dt, bitmap_loc);
			}
		}
		else if(strcmp(com.token[0], "bitmap_reset")==0){
			bitmap_loc=atoi(com.token[2]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				bitmap_reset((struct bitmap*)dt[i].dt, bitmap_loc);
			}
		}
		else if(strcmp(com.token[0], "bitmap_scan")==0){
			bitmap_st=atoi(com.token[2]);
			bitmap_ed=atoi(com.token[3]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				if (strcmp(com.token[4], "true") == 0) {
					size = bitmap_scan((const struct bitmap*)dt[i].dt, bitmap_st, bitmap_ed, 1);
				}
				else if (strcmp(com.token[4], "false") == 0) {
					size = bitmap_scan((const struct bitmap*)dt[i].dt, bitmap_st, bitmap_ed, 0);
				}
				if (size == BITMAP_ERROR) {
					fprintf(stdout, "%u\n", BITMAP_ERROR);
				}
				else {
					fprintf(stdout, "%zu\n", size);
				}
			}
		}
		else if(strcmp(com.token[0], "bitmap_scan_and_flip")==0){
			bitmap_st=atoi(com.token[2]);
			bitmap_ed=atoi(com.token[3]);
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				if (strcmp(com.token[4], "true") == 0) {
					size = bitmap_scan_and_flip((struct bitmap*)dt[i].dt, bitmap_st, bitmap_ed, 1);
				}
				else if (strcmp(com.token[4], "false") == 0) {
					size = bitmap_scan_and_flip((struct bitmap*)dt[i].dt, bitmap_st, bitmap_ed, 0);
				}
				if (size == BITMAP_ERROR) {
					fprintf(stdout, "%u\n", BITMAP_ERROR);
				}
				else {
					fprintf(stdout, "%zu\n", size);
				}
			}
		}
		else if(strcmp(com.token[0], "bitmap_dump")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				bitmap_dump((const struct bitmap*)dt[i].dt);
			}
		}
		else if(strcmp(com.token[0], "bitmap_expand")==0){
			for(i=0; i<dt_count; i++){
				if(dt[i].exist==1){
					if(strcmp(dt[i].name, com.token[1])==0){
						ext = 1;
						break;
					}
				}
			}
			if (ext == 1) {
				bit_size = atoi(com.token[2]);
				dt[i].dt = bitmap_expand((struct bitmap*)dt[i].dt, bit_size);
			}
		}

	}
	return 0;
}
bool list_less(const struct list_elem* a, const struct list_elem* b, void* aux){
	struct list_item* one=list_entry(a, struct list_item, elem);
	struct list_item* two=list_entry(b, struct list_item, elem);
	if(one->data<two->data){
		return true;
	}
	else if(one->data>=two->data){
		return false;
	}
}
unsigned hash_func(const struct hash_elem* e, void* aux){
	struct hash_item* item=hash_entry(e, struct hash_item, elem);
	return hash_int(item->data);
}
bool hash_less(const struct hash_elem* a, const struct hash_elem* b, void* aux){
	struct hash_item* one=hash_entry(a, struct hash_item, elem);
	struct hash_item* two=hash_entry(b, struct hash_item, elem);
	if(one->data<two->data){
		return true;
	}
	else if(one->data>=two->data){
		return false;
	}
}
void hash_action_square(struct hash_elem* e, void* aux){
	struct hash_item* item=hash_entry(e, struct hash_item, elem);
	item->data=item->data*item->data;
}
void hash_action_triple(struct hash_elem* e, void* aux){
	struct hash_item* item=hash_entry(e, struct hash_item, elem);
	item->data=item->data*item->data*item->data;
}
void hash_action_destructor(struct hash_elem* e, void* aux){
	struct hash_item* item=hash_entry(e, struct hash_item, elem);
	free(item);
}
void create(struct command com, cr_dt* dt, int dt_count){
	size_t size;
	int i;
	int type;
	int flag = 1;
	for (i = 0; i < dt_count; i++) {
		if ((strcmp(com.token[2], dt[i].name) == 0)&&dt[i].exist==1) {
			if (strcmp(com.token[1], "list") == 0) {
				type = 0;
			}
			else if (strcmp(com.token[1], "hashtable") == 0) {
				type = 1;
			}
			else if (strcmp(com.token[1], "bitmap") == 0) {
				type = 2;
			}
			break;
		}
	}
	if(dt[i].type==type){
		flag=0;
	}
	if (flag == 1) {
		if (strcmp(com.token[1], "list") == 0) {
			dt->type = 0;
			strcpy(dt->name, com.token[2]);
			dt->dt = malloc(sizeof(struct list));
			list_init((struct list*)dt->dt);
			dt->exist = 1;
		}
		else if (strcmp(com.token[1], "hashtable") == 0) {
			dt->type = 1;
			strcpy(dt->name, com.token[2]);
			dt->dt = malloc(sizeof(struct hash));
			hash_init((struct hash*)dt->dt, hash_func, hash_less, NULL);
			dt->exist = 1;
		}
		else if (strcmp(com.token[1], "bitmap") == 0) {
			dt->type = 2;
			strcpy(dt->name, com.token[2]);
			size = atoi(com.token[3]);
			dt->dt = bitmap_create(size);
			dt->exist = 1;
		}
	}
}
void dumpdata(struct command com, cr_dt* dt, int dt_count){
	struct list_elem* temp;
	struct list_item* item;
	int i;
	int type;
	int count=0;
	int flag = 0;
	struct hash_iterator* iter=malloc(sizeof(struct hash_iterator));
	struct hash_item* item2;
	struct bitmap* bit;
	size_t size;
	for(i=0; i<dt_count; i++){
		if(strcmp(dt[i].name, com.token[1])==0){
			type=dt[i].type;
			flag = 1;
			break;
		}
	}
	if (flag == 1) {
		if (type == 0) {
			for (temp = list_begin((struct list*)dt[i].dt); temp != list_end((struct list*)dt[i].dt); temp = list_next(temp)) {
				item = list_entry(temp, struct list_item, elem);
				fprintf(stdout, "%d ", item->data);
				count++;
			}
			if (count > 0) {
				fprintf(stdout, "\n");
			}
		}
		else if (type == 1) {
			hash_first(iter, (struct hash*)dt[i].dt);
			while (hash_next(iter)) {
				item2 = hash_entry(hash_cur(iter), struct hash_item, elem);
				fprintf(stdout, "%d ", item2->data);
				count++;
			}
			if (count > 0) {
				fprintf(stdout, "\n");
			}
		}
		else if (type == 2) {
			bit = dt[i].dt;
			size = bitmap_size(bit);
			for (i = 0; i < (int)size; i++) {
				if (bitmap_test(bit, i)) {
					fprintf(stdout, "1");
				}
				else {
					fprintf(stdout, "0");
				}
			}
			fprintf(stdout, "\n");
		}
	}
}
void delete(struct command com, cr_dt* dt, int dt_count){
	int i;
	int flag;
	int type;
	struct list* temp;
	struct list_elem* list_temp;
	struct list_item* item_temp;
	struct hash* hash_temp;
	struct bitmap* bitmap_temp;
	for(i=0; i<dt_count; i++){
		if(strcmp(dt[i].name, com.token[1])==0){
			dt[i].exist=0;
			flag = 1;
			type = dt[i].type;
			break;
		}
	}
	if (flag == 1) {
		if (type == 0) {
			temp = (struct list*)dt[i].dt;
			list_temp = list_begin(temp);
			while (!list_empty(temp)) {
				list_temp = list_pop_front(temp);
				item_temp = list_entry(list_temp, struct list_item, elem);
				free(item_temp);
			}
			free(temp);
		}
		else if (type == 1) {
			hash_temp = (struct hash*)dt[i].dt;
			hash_destroy(hash_temp, hash_action_destructor);
		}
		else if (type == 2) {
			bitmap_temp = (struct bitmap*)dt[i].dt;
			bitmap_destroy(bitmap_temp);
		}
	}
}
void delete_all(cr_dt* dt, int dt_count) {
	int i;
	int type;
	struct list* temp;
	struct list_elem* list_temp;
	struct list_item* item_temp;
	struct hash* hash_temp;
	struct bitmap* bitmap_temp;
	for (i = 0; i < dt_count; i++) {
		if (dt[i].exist == 1) {
			type = dt[i].type;
			if (type == 0) {
				temp = (struct list*)dt[i].dt;
				list_temp = list_begin(temp);
				while (!list_empty(temp)) {
					list_temp = list_pop_front(temp);
					item_temp = list_entry(list_temp, struct list_item, elem);
					free(item_temp);
				}
				free(temp);
			}
			else if (type == 1) {
				hash_temp = (struct hash*)dt[i].dt;
				hash_destroy(hash_temp, hash_action_destructor);
			}
			else if (type == 2) {
				bitmap_temp = (struct bitmap*)dt[i].dt;
				bitmap_destroy(bitmap_temp);
			}
		}
	}
}
struct list_elem* find_nlist(struct list* list, int loc){
	int i;
	struct list_elem* now=list_begin(list);
	for(i=0; i<loc; i++){
		now=list_next(now);
	}
	return now;
}
