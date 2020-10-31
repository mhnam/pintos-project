#include "userprog/syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static int arg_size[SYS_MAX_NUM];

//THINGS TO DO:
//	* IMPLEMENT ``GET_ARGUMENT`` TO GET PROPER DATA ADDR FROM USER STACK FOR EACH FUNCTION
//	* PUT ``CHK_ADDRESS`` INTO ``GET_ARGUMENT`` AND CHK WHETHER ADDR IS PROPER, NOT IN SWITCH ARGUMENT

/*handler*/
void chk_address(const void *addr);
static void get_argument(void *esp, int *arg, int count);
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
}

/* check whether given address is user area;
otherwise exit process (i.e., inbetween 0x8048000~0xc0000000) */
void chk_address(struct intr_frame *f){
	int i, j = 20;
	int syscall_num = * (uint32_t *) f->esp;
	if(arg_size[syscall_num] == 1){
		if(is_user_vaddr(f->esp+4) == 0)
			exit(-1);
	}
	else{
		foreach(i=0; i<arg_size[syscall_num]; i++){
			if(is_user_vaddr(f->esp + j) == 0)
				exit(-1);
			j+=4;
		}
	}
}

static void get_argument(void *esp, int *arg, int count){
	
}

static void
syscall_handler (struct intr_frame *f) 
{
//  printf ("system call!\n");
//  printf("syscall : %d\n", *(uint32_t *)(f->esp));
//  hex_dump(f->esp, f->esp, 100, 1); 
	/*check whether esp and ptr are user space; otherwise page fault*/
	
	/*user stack data -> kernel space*/
	
	/*getting syscall num from user stack*/
  int syscall_num = * (uint32_t *) f->esp;
	chk_address(f);
	
	switch(syscall_num){
		/*save return to eax*/
    case SYS_HALT:
//			halt();
			shutdown_power_off();
			break;
			
		case SYS_EXIT:
			syscall_exit(f);
			break;
			
		case SYS_EXEC:
			syscall_exec(f);
			break;
			
		case SYS_WAIT:
			syscall_wait(f);
			break;
			
		case SYS_CREATE:
			break;
			
		case SYS_REMOVE: 
			break;
			
		case SYS_OPEN: 
			break;
			
		case SYS_FILESIZE: 
			break;
			
		case SYS_READ: 
			syscall_read(f);
			break;
			
		case SYS_WRITE:
			syscall_write(f);
			break;
			
		case SYS_SEEK: 
			break;
			
		case SYS_TELL: 
			break;
			
		case SYS_CLOSE: 
			break;
			
		case SYS_FIBONACCI:
			syscall_fibonacci(f);
			break;
			
		case SYS_MAX_OF_FOUR_INT:
			syscall_max_of_four_int(f);
			break;
	}
	
	/*
  uint32_t syscall_num;

  //hex_dump((uintptr_t) f->esp, (const char *) f->esp, (uintptr_t) PHYS_BASE - (uintptr_t) f->esp, true);


  if (syscall_num >= SYS_MAX_NUM || syscall_table[syscall_num] == NULL)
    thread_exit ();
  else {
      void *arg_top;
      
      sys_ret = (int*) &f->eax;
      arg_top = (void*) ((uintptr_t) f->esp + STACK_BLOCK + esp_fix_val[syscall_num]);

      if (! is_user_vaddr((void*) ((uintptr_t) arg_top + arg_size[syscall_num] - STACK_BLOCK)))
        thread_exit ();

      syscall_table[syscall_num](arg_top);
	}
  */
}

void syscall_exit (struct intr_frame *f){
	int status;
	status = *(uint32_t *)(f->esp + 4);
	printf("%s: exit(%d)\n", thread_name(), status);
	thread_current()->exit_status = status;
	thread_exit();
}

/* create child process
uese process_execute in userprog/process.c */
void syscall_exec (struct intr_frame *f){
	const char *file = (const char *)*(uint32_t *)(f->esp + 4);
	f->eax = process_execute(file);
}

void syscall_wait (struct intr_frame *f){
	pid_t = (pid_t)*(uint32_t *)(f->esp + 4);
	f->eax = process_wait(pid);
}

void syscall_read (struct intr_frame *f){
	int fd, i;
	void *buffer;
	unsigned length;
	
	fd = (int)*(uint32_t *)(f->esp + 20);
	buffer = (void *)*(uint32_t *)(f->esp + 24);
	length = (unsigned)*((uint32_t *)(f->esp + 28));
	
	if(fd == 0 && buffer != NULL){
		for(i = 0; i <= (int)length; i++)
			*(char*)(buffer + i) = input_getc();
	f->eax = i;
	}	
	else f->eax = -1;
}

void syscall_write (struct intr_frame *f){
	int fd;
	void *buffer;
	unsigned length;
	
	fd = (int)*(uint32_t *)(f->esp + 20);
	buffer = (void *)*(uint32_t *)(f->esp + 24);
	length = (unsigned)*((uint32_t *)(f->esp + 28));
	
	if(fd == 1){
		putbuf(buffer, length);
		f->eax = length;
	}
	
	f->eax = -1;
}

void syscall_fibonacci (struct intr_frame *f){
	int n = (int)*(uint32_t *)(f->esp + 4);
  int a = 0;
	int b = 1;
	int c = 1;
	int i;

  if(n == 1) 
    f->eax = 1;

  for(i = 1; i < n; i++){
      c = a + b; a = b; b = c;
	}
	
	f->eax = c;
}

int syscall_max_of_four_int (struct intr_frame *f){
	int a = (int)*(uint32_t *)(f->esp + 20);
	int b = (int)*(uint32_t *)(f->esp + 24);
	int c = (int)*(uint32_t *)(f->esp + 28);
	int d = (int)*(uint32_t *)(f->esp + 32);
	int max = a;
	
	if(b > max)
		max = b;
	if(c > max)
		max = c;
	if(d > max)
		max = d;
	
	f->eax = max;
}