//MUST BE MODIFIED (COPIED FROM GITHUB)

#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

/*handler*/
inline static bool chk_address(const void *);
static void get_argument(void *esp, int *arg, int count);
static void syscall_handler(struct intr_frame *);

/*syscalls*/
static void syscall_halt(void *arg_top);
static void syscall_exit(void *arg_top);
static void syscall_exec(void *arg_top);
static void syscall_wait(void *arg_top);
static void syscall_read(void *arg_top);
static void syscall_write(void *arg_top);

static void syscall_fibonacci(void *arg_top);
static void syscall_max_of_four_int (void *arg_top);


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/* check whether given address is user area;
otherwise exit process (i.e., inbetween 0x8048000~0xc0000000) */
inline static bool chk_address(const void *ptr)
{
  return is_user_vaddr(ptr);
}

/* save  */
static void get_argument(void *esp, int *arg, int count){
	
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  printf ("system call!\n");
  thread_exit ();
	/*check whether esp and ptr are user space; otherwise page fault*/
	
	/*user stack data -> kernel space*/
	
	/*getting syscall num from user stack*/
  int syscall_num = * (uint32_t *) f->esp;
	switch(syscall_num){
		/*save return to eax*/

    case SYS_HALT:
			syscall_halt();
			break;
		case SYS_EXIT:
			syscall_exit();
			break;
		case SYS_EXEC: 
			syscall_exec();
			break;
		case SYS_WAIT:
			syscall_wait();
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
			syscall_read();
			break;
		case SYS_WRITE: 
			syscall_write();
			break;
		case SYS_SEEK: 
			break;
		case SYS_TELL: 
			break;
		case SYS_CLOSE: 
			break;
		case SYS_FIBONACCI:
			break;
		case SYS_MAX_OF_FOUR_INT:
			break;
			
			/*default: exit()*/

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

static void syscall_halt(void *arg_top){
	shutdown_power_off();
}

static void syscall_exit(void *arg_top){
	/* get thread structure */
	/* printout termination message */
	/* exit thread */
	
  /* Load syscall arguments */
  int status = * (int *) SYS_ARG_PTR (arg_top, 0);

  struct thread *cur = thread_current ();
  cur->normal_exit = true;
  cur->exit_code = status;
  thread_exit();}

/* create child process
uese process_execute in userprog/process.c */
static void syscall_exec(void *arg_top){
  /* Load syscall arguments */
  const char *file = * (char **) SYS_ARG_PTR (arg_top, 0);

  if (! chk_valid_ptr (file))
    SYS_RETURN (-1);

  SYS_RETURN ( process_execute(file) ); 	
}

static void syscall_wait(void *arg_top){
  /* Load syscall arguments. */
  pid_t pid = * (pid_t *) SYS_ARG_PTR (arg_top, 0); 

  SYS_RETURN ( process_wait(pid) );
}

static void syscall_read(void *arg_top){
  // must be modified...
  /* Load syscall arguments */
  int fd = * (int *) SYS_ARG_PTR (arg_top, 0);
  void *buffer = * (void **) SYS_ARG_PTR (arg_top, 1);
  unsigned size = * (unsigned *) SYS_ARG_PTR (arg_top, 2);

  int i;

  if (! chk_valid_ptr (buffer))
    SYS_RETURN (-1);

  if (fd == 0 && buffer != NULL)
    {
      for(i = 0; i < size; ++i)
        *(char*)(buffer + i) = input_getc();
      SYS_RETURN (i);
    }

  SYS_RETURN (-1);
}

static void syscall_write(void *arg_top){
  /* Load syscall arguments */
  int fd = * (int *) SYS_ARG_PTR (arg_top, 0);
  void *buffer = * (void **) SYS_ARG_PTR (arg_top, 1);
  unsigned size = * (unsigned *) SYS_ARG_PTR (arg_top, 2);

  if (! chk_valid_ptr (buffer))
    SYS_RETURN (-1);

  if (fd == 1 && buffer != NULL)
    {
      putbuf (buffer, size);
      SYS_RETURN (size);
    }

  SYS_RETURN (0);
}

static void syscall_fibonacci(void *arg_top){
  // Load syscall arguments.
  int n = * (int *) SYS_ARG_PTR (arg_top, 0);

  int a, b, c, i;

  a = 0; b = c = 1;

  if(n == 1) 
    SYS_RETURN (1);

  for(i = 1; i < n; ++i)
    {
      c = a + b;
      a = b;
      b = c;
    }

  SYS_RETURN (c);
}

static void
syscall_sum_of_four_integers (void *arg_top)
{
  /* Load syscall arguments. */
  int a = * (int *) SYS_ARG_PTR (arg_top, 0);
  int b = * (int *) SYS_ARG_PTR (arg_top, 1);
  int c = * (int *) SYS_ARG_PTR (arg_top, 2);
  int d = * (int *) SYS_ARG_PTR (arg_top, 3);

  SYS_RETURN (a+b+c+d);
}

static void syscall_max_of_four_int (void *arg_top){
	
}


