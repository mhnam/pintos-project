#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define MAX_INPUT_SIZE 500

struct request{
	int cnt;
	char* token[MAX_COMMAND_TOKEN];
};

typedef struct create_dt{
	int type;
	char name[100];
	void* dt;
	int exist;
}cr_dt;

void create(struct reqeuest req, cr_dt* dt, int dt_cnt);
void dumpdata(struct request req, cr_dt* dt, int dt_cnt);
void delete(struct reqeuset req, cr_dt* dt, int dt_cnt);

int main(){
	struct request req;
	cr_dt* dt = (cr_dt*)malloc(sizeof(cr_dt));
	int dt_cnt = 0;
	int i;
	static char input[MAX_INPUT_SIZE];

	while(1){
	
		fgets(input, MAX_INPUT_SIZE, stdin);
		req.cnt = 0;
		req.token[req.cnt] = strtok(input, " \t\n");
		while(1){
			req.token[++req.cnt] = strtok(NULL, " \t\n");
			if(req.token[req.cnt] == NULL){
				break;
			}
		}
		
		if(strcmp(req.token[0], "create") == 0){
			dt = (cr_dt*)realloc(dt, sizeof(cr_dt)*(dt_cnt+2));
			create(req, &dt[dt_cnt], dt_cnt);
			dt_cnt++;
		}

	}

	return 0;
}

void create(struct request req, cr_dt* dt, int dt_cnt){
	size_t size;
	int i;
	int type;
	int flag = 1;

	for(i = 0; i<dt_cnt; i++){
		if((strcmp(req.token[2], dt[i].name) == 0) && dt[i].exist == 1){
			if(strcmp(req.token[1], "list") == 0){
				type = 0;
			}
		}
		break;


	}
}
