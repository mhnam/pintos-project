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
#ifdef VM
#include "vm/page.h"
#endif

static void syscall_handler (struct intr_frame *);

// 시스템 콜의 커널 쪽 서비스 루틴입니다.
// 이 함수들은 포인터가 가리키는 영역에 안전하게 접근할 수 있다고 가정합니다.
static void halt (void);
void exit (int);
static tid_t exec (const char *);
static int wait (tid_t);
static bool create (const char *, unsigned);
static bool remove (const char *);
static int open (const char *);
static int filesize (int);
static int read (int, void *, unsigned);
static int write (int, const void *, unsigned);
static void seek (int, unsigned);
static unsigned tell (int);
static void close (int);

// 파일 작업 락입니다.
// 파일 읽기 또는 쓰기 작업을 수행할 때 사용해야 합니다.
struct lock file_lock;

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");

  // 파일 작업 락 초기화
  lock_init (&file_lock);
}

// 주소 addr이 유효한 유저 모드 주소가 아니면 프로세스를 종료합니다.
// 시스템 콜을 안전하게 수행하기 위하여 사용합니다.
static inline void
check_address (void *addr)
{
  // 유저 영역 주소인지 확인한 다음, 올바른 가상 주소인지 확인합니다.
  if (!(
        is_user_vaddr (addr) &&
        addr >= (void *)0x08048000UL &&
        find_vme (addr)
      ))
    exit (-1);
}

// 4 바이트 값에 대한 안전한 포인터인지 검사합니다.
static inline void
check_address4 (void *addr)
{
  check_address (addr);
  check_address (addr + 3);
}

// 4바이트 인자를 1개에서 4개 사이에서 가져옵니다.
static inline void
get_arguments (int32_t *esp, int32_t *args, int count)
{
  ASSERT (1 <= count && count <= 4);
  while (count--)
  {
    check_address4 (++esp);
    *(args++) = *esp;
  }
}

// 아래 함수와 같으며, 주어진 크기를 가정합니다.
static inline void
check_user_string_l (const char *str, unsigned size)
{
  while (size--)
    check_address ((void *) (str++));  
}

// 널 문자로 종료되는 사용자 문자열의 유효성을 확인합니다.
static inline void
check_user_string (const char *str)
{
  for (; check_address ((void *) str), *str; str++);
}

// 아래 함수와 같으며, 주어진 크기를 가정합니다.
static inline char *
get_user_string_l (const char *str, unsigned size)
{
  char *buffer = 0;
  buffer = malloc (size);
  if (!buffer)
    return 0;
  memcpy (buffer, str, size);
  return buffer;
}

// 사용자 문자열을 가져옵니다. 새로운 메모리를 동적 할당합니다.
static inline char *
get_user_string (const char *str)
{
  unsigned size;
  char *buffer;
  size = strlen (str) + 1;
  buffer = get_user_string_l (str, size);
  return buffer; 
}

// 플래그가 맞을 때 동적 할당된 문자열을 해제하고 널 포인터를 대입합니다.
static inline void
free_single_user_string (char **args, int flag, int index)
{
  if (flag & (0b1000 >> index))
    {
      free (args[index]);
      args[index] = 0;
    }
}

// 플래그의 마지막 4비트에 따라서 문자열들을 해제합니다.
static inline void
free_user_strings (char **args, int flag)
{
  ASSERT (0 <= flag && flag <= 0b1111);
  free_single_user_string (args, flag, 0);
  free_single_user_string (args, flag, 1);
  free_single_user_string (args, flag, 2);
  free_single_user_string (args, flag, 3);
}

// 플래그가 맞을 때 사용자 문자열을 복사합니다.
// 작업 중 실패하면 내용을 되돌리고 종료합니다.
// 유효성을 검증한 다음 이 작업을 실행해야 합니다.
static inline void
get_single_user_string (char **args, int flag, int index)
{
  if (flag & (0b1000 >> index))
    {
      args[index] = get_user_string (args[index]);
      if (!args[index])
        {
          free_user_strings (args, flag & (0b11110000 >> index));
          exit(-1);
        }
    }
}

// 플래그가 맞을 때 사용자 문자열의 유효성을 확인합니다.
// 유효하지 않으면 종료합니다.
static inline void
check_single_user_string (char **args, int flag, int index)
{
  if (flag & (0b1000 >> index))
    check_user_string (args[index]);
}

// 플래그의 마지막 4비트에 따라서 사용자 문자열을 확인하고 가져옵니다.
// 새로운 메모리를 동적 할당합니다. 
static inline void
get_user_strings (char **args, int flag)
{
  ASSERT (0 <= flag && flag <= 0b1111);
  check_single_user_string (args, flag, 0);
  check_single_user_string (args, flag, 1);
  check_single_user_string (args, flag, 2);
  check_single_user_string (args, flag, 3);
  get_single_user_string (args, flag, 0);
  get_single_user_string (args, flag, 1);
  get_single_user_string (args, flag, 2);
  get_single_user_string (args, flag, 3);
}

static void
syscall_handler (struct intr_frame *f)
{
  int32_t args[4];
  check_address4 (f->esp);

  switch (*(int *) f->esp)
    {
      case SYS_HALT:
        halt ();
        break;
      case SYS_EXIT:
        get_arguments (f->esp, args, 1);
        exit (args[0]);
        break;
      case SYS_CREATE:
        get_arguments (f->esp, args, 2);
        get_user_strings ((char **) args, 0b1000);
        f->eax = create ((const char *) args[0], args[1]);
        free_user_strings ((char **) args, 0b1000);
        break;
      case SYS_REMOVE:
        get_arguments (f->esp, args, 1);
        get_user_strings ((char **) args, 0b1000);
        f->eax = remove ((const char *) args[0]);
        free_user_strings ((char **) args, 0b1000); 
        break;
      case SYS_WRITE:
        get_arguments (f->esp, args, 3);
        check_user_string_l ((const char *) args[1], (unsigned) args[2]);
        args[1] = (int) get_user_string_l ((const char *) args[1], (unsigned) args[2]);
        f->eax = write ((int) args[0], (const void *) args[1], (unsigned) args[2]);
        free ((void *) args[1]);
        args[1] = 0;
        break;
      case SYS_EXEC:
        get_arguments (f->esp, args, 1);
        get_user_strings ((char **) args, 0b1000);
        f->eax = exec ((const char *) args[0]);
        free_user_strings ((char **) args, 0b1000);
        break;
      case SYS_WAIT:
        get_arguments (f->esp, args, 1);
        f->eax = wait ((tid_t) args[0]);
        break;
      case SYS_OPEN:
        get_arguments (f->esp, args, 1);
        get_user_strings ((char **) args, 0b1000);
        f->eax = open ((const char *) args[0]);
        free_user_strings ((char **) args, 0b1000);
        break;
      case SYS_FILESIZE:
        get_arguments (f->esp, args, 1);
        f->eax = filesize ((int) args[0]);
        break;
      case SYS_READ:
        get_arguments (f->esp, args, 3);
        check_user_string_l ((const char *) args[1], (unsigned) args[2]);
        f->eax = read ((int) args[0], (void *) args[1], (unsigned) args[2]);
        break;
      case SYS_SEEK:
        get_arguments (f->esp, args, 2);
        seek ((int) args[0], (unsigned) args[1]);
        break;
      case SYS_TELL:
        get_arguments (f->esp, args, 1);
        f->eax = tell ((int) args[0]);
        break;
      case SYS_CLOSE:
        get_arguments (f->esp, args, 1);
        close ((int) args[0]);
        break;
      case SYS_MMAP:
      case SYS_MUNMAP:
      case SYS_CHDIR:
      case SYS_MKDIR:
      case SYS_READDIR:
      case SYS_ISDIR: 
      case SYS_INUMBER:
        // 시스템 콜 번호는 유효하나 아직 구현되지 않았습니다.
        printf("NotImplemented: %d\n", *(int *)f->esp);
      default:
        // 시스템 콜 번호가 유효하지 않습니다.
        exit(-1);
    }
}

static void
halt (void)
{
  shutdown_power_off ();
}

void
exit (int status)
{
  thread_current ()->exit_status = status;
  printf ("%s: exit(%d)\n", thread_name (), status);
  thread_exit ();
}

static tid_t
exec (const char *file)
{
  tid_t tid;
  struct thread *child;

  // 여기에서 실패하면 스레드 자료 구조 생성 실패입니다.
  if ((tid = process_execute (file)) == TID_ERROR)
    return TID_ERROR;

  child = thread_get_child (tid);
  ASSERT (child);

  sema_down (&child->load_sema);

  // 여기에서 실패하면 프로그램 적재 실패입니다.
  if (!child->load_succeeded)
    return TID_ERROR;

  return tid;
}

static int
wait (tid_t tid)
{
  return process_wait (tid);
}

static bool
create (const char *file, unsigned initial_size)
{
  return filesys_create (file, initial_size); 
}

static bool
remove (const char *file)
{
  return filesys_remove (file);
}

static int
open (const char *file)
{
  // process_add_file는 NULL에서 -1 반환하므로 안전합니다.
  return process_add_file (filesys_open (file));
}

static int
filesize (int fd)
{
  struct file *f = process_get_file (fd);
  if (f == NULL)
    return -1;
	return file_length (f);
}

static int
read (int fd, void *buffer, unsigned size)
{
  struct file *f;
  lock_acquire (&file_lock);
  if (fd == STDIN_FILENO)
  {
    // 표준 입력
    unsigned count = size;
    while (count--)
      *((char *)buffer++) = input_getc();
    lock_release (&file_lock);  
    return size;
  }
  if ((f = process_get_file (fd)) == NULL)
    {
      lock_release (&file_lock);
      return -1;
    }
  size = file_read (f, buffer, size);
  lock_release (&file_lock);
  return size;
}

static int
write (int fd, const void *buffer, unsigned size)
{
  struct file *f;
  lock_acquire (&file_lock);
  if (fd == STDOUT_FILENO)
    {
      putbuf (buffer, size);
      lock_release (&file_lock);
      return size;  
    }
  if ((f = process_get_file (fd)) == NULL)
    {
      lock_release (&file_lock);
      return 0;
    }
  size = file_write (f, buffer, size);
  lock_release (&file_lock);
  return size;
}

static void
seek (int fd, unsigned position)
{
  struct file *f = process_get_file (fd);
  if (f == NULL)
    return;
  file_seek (f, position);  
}

static unsigned
tell (int fd)
{
  struct file *f = process_get_file (fd);
  if (f == NULL)
    exit (-1);
  return file_tell (f);
}

static void
close (int fd)
{ 
  process_close_file (fd);
}


/*
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
void chk_address(void *addr);
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
void chk_address(void *addr){
	#ifdef VM
	if (!(is_user_vaddr (addr) && addr >= (void *)0x08048000UL && find_vme (addr)))
		exit(-1);
	#else
if (!(is_user_vaddr (addr) && addr >= (void *)0x08048000UL))
		exit(-1);
	#endif
}
/*
static void get_argument(void *esp, int *arg, int count){
  ASSERT (1 <= count && count <= 4);
  while (count--)
  {
    check_address (++esp);
    *(arg++) = *esp;
  }
}
*/
static void
syscall_handler (struct intr_frame *f) 
{
	//check whether esp and ptr are user space; otherwise page fault
	chk_address(f->esp);
	chk_address(f->esp + 3);

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
  int i;
	int ret = -1;

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
	int ret = -1;
	struct file* file = thread_current()->fd[fd];

	if(!is_user_vaddr(buffer)) exit(-1);
	if(!buffer || fd == 2)	exit(-1);
	
  lock_acquire(&filesys_lock);
	if (fd == 1) {
    putbuf(buffer, length);
		ret = length;
  }
	else if (fd > 2) {
		if(!file){
			lock_release(&filesys_lock);
			exit(-1);
		}
		if(file->deny_write)
			file_deny_write(file);
		ret = file_write(file, buffer, length);
  }
	lock_release(&filesys_lock);
	return ret;
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
*/