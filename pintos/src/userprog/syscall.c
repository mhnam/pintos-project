#include "userprog/syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static int arg_size[SYS_MAX_NUM];

/*****
THINGS TO DO:
* IMPLEMENT ``GET_ARGUMENT`` TO GET PROPER DATA ADDR FROM USER STACK FOR EACH FUNCTION
*****/

/*handler*/
void chk_address(struct intr_frame *f);
//static void get_argument(void *esp, int *arg, int count);
static void syscall_handler(struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
	arg_size[SYS_EXIT] = 1;
	arg_size[SYS_EXEC] = 1;
	arg_size[SYS_WAIT] = 1;
	arg_size[SYS_READ] = 3;
	arg_size[SYS_WRITE] = 3;
	arg_size[SYS_FIBONACCI] = 1;
	arg_size[SYS_MAX_OF_FOUR_INT] = 4;
	arg_size[SYS_CREATE] = 2;
	arg_size[SYS_REMOVE] = 1;
	arg_size[SYS_OPEN] = 1;
	arg_size[SYS_FILESIZE] = 1;
	arg_size[SYS_SEEK] = 2;
	arg_size[SYS_TELL] = 1;
	arg_size[SYS_CLOSE] = 1;
}

/* check whether given address is user area;
otherwise exit process (i.e., inbetween 0x8048000~0xc0000000) */
void chk_address(struct intr_frame *f){
	int i, j = 20;
	int syscall_num = * (uint32_t *) f->esp;
	if(arg_size[syscall_num] == 1){
		if(is_user_vaddr(f->esp + 4) == 0)
			exit(-1);
	}
	else{
		for(i=0; i<arg_size[syscall_num]; i++){
			if(is_user_vaddr(f->esp + j) == 0)
				exit(-1);
			j+=4;
		}
	}
}

/*
static void get_argument(void *esp, int *arg, int count){

}
*/

static void
syscall_handler (struct intr_frame *f) 
{
	//check whether esp and ptr are user space; otherwise page fault
	chk_address(f);

	//getting syscall num from user stack
  int syscall_num = * (uint32_t *) f->esp;
	
	switch(syscall_num){
		/*save return to eax*/
    case SYS_HALT:
			halt();
			break;
			
		case SYS_EXIT:
			exit(*(uint32_t *)(f->esp + 4));
			break;
			
		case SYS_EXEC:
      f->eax = exec((const char *)*(uint32_t *)(f->esp + 4));
			break;
			
		case SYS_WAIT:
      f->eax = wait((pid_t)*(uint32_t *)(f->esp + 4));
			break;
			
		case SYS_CREATE:
      f->eax = create((const char *)*(uint32_t *)(f->esp + 4), (unsigned)*(uint32_t *)(f->esp + 8));
			break;
			
		case SYS_REMOVE:
      f->eax = remove((const char *)*(uint32_t *)(f->esp + 4));
			break;
			
		case SYS_OPEN: 
      f->eax = open((pid_t)*(uint32_t *)(f->esp + 4));
			break;
			
		case SYS_FILESIZE: 
      f->eax = filesize((pid_t)*(uint32_t *)(f->esp + 4));
			break;
			
		case SYS_READ: 
			f->eax = read((int)*(uint32_t *)(f->esp + 20), (void *)*(uint32_t *)(f->esp + 24), (unsigned)*((uint32_t *)(f->esp + 28)));
			break;
			
		case SYS_WRITE:
			f->eax = write((int)*(uint32_t *)(f->esp + 20), (void *)*(uint32_t *)(f->esp + 24), (unsigned)*((uint32_t *)(f->esp + 28)));
			break;
			
		case SYS_SEEK: 
      f->eax = seek((int)*(uint32_t *)(f->esp + 4), (unsigned)*(uint32_t *)(f->esp + 8));
			break;
			
		case SYS_TELL: 
      f->eax = tell((int)*(uint32_t *)(f->esp + 4));
			break;
			
		case SYS_CLOSE: 
      f->eax = close((int)*(uint32_t *)(f->esp + 4));
			break;
			
		case SYS_FIBONACCI:
      f->eax = fibonacci((int)*(uint32_t *)(f->esp + 4));
			break;
			
		case SYS_MAX_OF_FOUR_INT:
      f->eax = max_of_four_int((int)*(uint32_t *)(f->esp + 20), (int)*(uint32_t *)(f->esp + 24), (int)*(uint32_t *)(f->esp + 28), (int)*(uint32_t *)(f->esp + 32));
			break;
		
	}
}

void halt(void){
	shutdown_power_off();
}

void exit(int status){
	int i = 3; /*why 3??*/
	printf("%s: exit(%d)\n", thread_name(), status);
	thread_current()->exit_status = status;
	do{
		if(!thread_current()->fd[i])
			close(i);
		i++;
	}while(i<=128);
	thread_exit();
}

/* create child process
uese process_execute in userprog/process.c */
pid_t exec (const char *file){
	return process_execute(file);
}

int wait (pid_t pid){
	return process_wait(pid);
}

int read (int fd, void *buffer, unsigned length){
	int i;
	
		if(fd == 0 && buffer != NULL){
			for(i = 0; i <= (int)length; i++)
				*(char*)(buffer + i) = input_getc();
		return i;
		}	
	
		else return -1;
}

int write (int fd, const void *buffer, unsigned length){
	if(fd == 1){
		putbuf(buffer, length);
		return length;
	}
	return -1;
}

int fibonacci(int n){
  int a = 0;
	int b = 1;
	int c = 1;
	int i;

  if(n == 1) 
    return 1;

  for(i = 1; i < n; i++){
      c = a + b; a = b; b = c;
	}
	
	return c;
}

int max_of_four_int(int a, int b, int c, int d){
	int max = a;
	
	if(b > max)
		max = b;
	if(c > max)
		max = c;
	if(d > max)
		max = d;
	
	return max;
}

bool create (const char *file, unsigned initial_size){
	return filesys_create(file, initial_size);
}

bool remove (const char *file){
	return filesys_remove(file);
}

int open (const char *file){
	
}

int filesize (int fd){
  struct thread *cur = thread_current ();
	return file_length(cur->fd[fd]);
}

void seek (int fd, unsigned position){
  struct thread *cur = thread_current ();
	return file_seek(cur->fd[fd], position);
}

unsigned tell (int fd){
  struct thread *cur = thread_current ();
	return file_tell(cur->fd[fd]);
}

void close (int fd){
  struct thread *cur = thread_current ();
	return file_close(cur->fd[fd]);
}