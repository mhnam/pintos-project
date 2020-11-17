#include "userprog/syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "filesys/inode.h"
#include "filesys/off_t.h"

static int arg_size[SYS_MAX_NUM];

struct file 
  {
    struct inode *inode;        /* File's inode. */
    off_t pos;                  /* Current position. */
    bool deny_write;            /* Has file_deny_write() been called? */
  };

struct lock filesys_lock;
int readcnt;

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
	
	lock_init(&filesys_lock);
	intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
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
			f->eax = create((const char*)*(uint32_t *)(f->esp +16), (unsigned)*(uint32_t *)(f->esp + 20));
			break;
			
		case SYS_REMOVE:
			f->eax = remove((const char*)*(uint32_t *)(f->esp + 4));
			break;
			
		case SYS_OPEN:
      f->eax = open((const char*)*(uint32_t *)(f->esp + 4));
			break;
			
		case SYS_FILESIZE:
      f->eax = filesize((int)*(uint32_t *)(f->esp + 4));
			break;
			
		case SYS_READ:
			f->eax = read((int)*(uint32_t *)(f->esp + 20), (void *)*(uint32_t *)(f->esp + 24), (unsigned)*((uint32_t *)(f->esp + 28)));
			break;
			
		case SYS_WRITE:
			f->eax = write((int)*(uint32_t *)(f->esp + 20), (void *)*(uint32_t *)(f->esp + 24), (unsigned)*((uint32_t *)(f->esp + 28)));
			break;
			
		case SYS_SEEK:
      seek((int)*(uint32_t *)(f->esp + 16), (unsigned)*(uint32_t *)(f->esp + 20));
			break;
			
		case SYS_TELL:
      f->eax = tell((int)*(uint32_t *)(f->esp + 4));
			break;
			
		case SYS_CLOSE:
      close((int)*(uint32_t *)(f->esp + 4));
			break;
			
		case SYS_FIBONACCI:
      f->eax = fibonacci((int)*(uint32_t *)(f->esp + 4));
			break;
			
		case SYS_MAX_OF_FOUR_INT:
      f->eax = max_of_four_int((int)*(uint32_t *)(f->esp + 28), (int)*(uint32_t *)(f->esp + 32), (int)*(uint32_t *)(f->esp + 36), (int)*(uint32_t *)(f->esp + 40));
			break;
		
	}
}

void halt(void){
	shutdown_power_off();
}

void exit(int status){
	printf("%s: exit(%d)\n", thread_name(), status);
	thread_current()->exit_status = status;
	int i = 3;
	do{
		if(thread_current()->fd[i] != NULL)
			close(i);
		i++;
	}while(i<128);
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
	if(!file)	exit(-1);
  return filesys_create(file, initial_size);
}

bool remove (const char *file){
	if(!file)	exit(-1);
  return filesys_remove(file);
}

int open (const char *file){
  int i;
	
	if(!is_user_vaddr(file)) exit(-1);
	if(!file)	exit(-1);
	
	lock_acquire(&filesys_lock);
	struct file* fp = filesys_open(file);
	lock_release(&filesys_lock);

	if (!fp) return -1;
	else{ /*put new file into threads' fd and return such fd*/
		for(i = 3; i < 128; i++){
			if(thread_current()->fd[i] == NULL) {
				if(strcmp(thread_current()->name, file) == 0) file_deny_write(fp);
				thread_current()->fd[i] = fp; 
				return i;
			}
		}
	}
}

int filesize (int fd){
		struct file* file = thread_current()->fd[fd];
		if(!file)	exit(-1);
  return file_length(file);
}

int read (int fd, void *buffer, unsigned length){
  int i, ret;

	if(!is_user_vaddr(buffer)) exit(-1);
	if(!buffer || fd == 1)	exit(-1);
	
	lock_acquire(&filesys_lock);
  if (fd == 0) {
    for (i = 0; i < length; i++) {
      if (input_getc() == '\0') break;
    }
		ret = i;
  }
	else if (fd > 2) {
		struct file* file = thread_current()->fd[fd];
		if(!file){
			lock_release(&filesys_lock);
			exit(-1);
		}
		ret = file_read(file, buffer, length);
  }
	lock_release(&filesys_lock);
	return ret;
}

int write (int fd, const void *buffer, unsigned length){
	int ret;

	if(!is_user_vaddr(buffer)) exit(-1);
	if(!buffer || fd == 2)	exit(-1);

	if (fd == 1) {
    putbuf(buffer, length);
    return length;
  }
	else if (fd > 2) {
		lock_acquire(&filesys_lock);

		if(thread_current()->fd[fd] == NULL){
			lock_release(&filesys_lock);
			exit(-1);
		}
		if(thread_current()->fd[fd]->deny_write)
			file_deny_write(thread_current()->fd[fd]);
		ret = file_write(thread_current()->fd[fd], buffer, length);
		lock_release(&filesys_lock);
		return ret;
  }
}

void seek (int fd, unsigned position){
	struct file* file = thread_current()->fd[fd];
	if(!file)	exit(-1);
  file_seek(file, position);
}

unsigned tell (int fd){
	struct file* file = thread_current()->fd[fd];
	if(!file)	exit(-1);
	return file_tell(file);
}

void close (int fd){
	struct file* file = thread_current()->fd[fd];
	if(!file)	exit(-1);
	thread_current()->fd[fd] = NULL; /*mark that the file is closed*/
	return file_close(file);
}