#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "lib/user/syscall.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "lib/kernel/console.h"
#include "userprog/process.h"
#define SYS_MAX_NUM 30

void syscall_init (void);

/*syscalls
void halt (void);
void exit (int status);
pid_t exec (const char *file);
int wait (pid_t);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd);
int read (int fd, void *buffer, unsigned length);
int write (int fd, const void *buffer, unsigned length);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);
int fibonacci(int n);
int max_of_four_int(int a, int b, int c, int d);
*/

void syscall_halt (struct intr_frame *f);
void syscall_exit (struct intr_frame *f);
void syscall_exec (struct intr_frame *f);
void syscall_wait (struct intr_frame *f);
void syscall_create (struct intr_frame *f);
void syscall_remove (struct intr_frame *f);
void syscall_open (struct intr_frame *f);
void syscall_filesize (struct intr_frame *f);
void syscall_read (struct intr_frame *f);
void syscall_write (struct intr_frame *f);
void syscall_seek (struct intr_frame *f);
void syscall_tell (struct intr_frame *f);
void syscall_close (struct intr_frame *f);
void syscall_fibonacci (struct intr_frame *f);
void syscall_max_of_four_int (struct intr_frame *f);

#endif /* userprog/syscall.h */
