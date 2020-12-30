# Document for Project #2 (Filesystem)
## 개발 목표
본 프로젝트를 통해서 구현하는 내용은 유저모드에서 실행되는 유저 프로그램 중에서 파일과 관련된 파일 시스템 전반을 정상적으로 작동하게 함을 목적으로 한다. 현재 핀토스에서는 standard input, output에 대해서만 처리가 가능한 상태이지만, 핀토스 내부에서 파일을 읽고, 쓰는 등에 대한 부분이 구현되어 있지 않다. 따라서 본 프로젝트를 통해서는 컴퓨터 내의 다양한 파일과 관련된 시스템 콜들이 수행될 수 있도록 thread에서 사용하는 파일을 인지하도록 한 후, 필요한 파일 시스템 관련 유저 프로그램을 실행시키도록 하고, 유저 프로그램이 실행되면서 발생하는 각각의 시스템 콜들이 수행되기 위한 handler 구현을 목표로 한다. 더 나아가 동일한 파일을 여러 thread에서 동시에 접근하여 일관성이 훼손되는 경우를 제어하도록 ``synchronization``을 구현한다.

## 개발 범위 및 내용
### 개발 범위
#### File Descriptor
핀토스에서는 각 thread는 독립적인 file descriptor를 가지고 보조기억장치에 있는 파일에 접근하기 위한 정보를 관리한다. 이에 따라 ``threads/thread.h``에서 thread 구조체에 고정된 크기의 file descriptor를 구현한다. 이후 해당 구조체를 이용하여 각종 파일 시스템과 관련된 시스템 콜 구현에 활용한다.

#### System Calls
본 프로젝트에서는 9가지의 시스템 콜 - bool create (const char *file, unsigned initial_size); bool remove (const char *file); int open (const char *file); int filesize (int fd); int read (int fd, void *buffer, unsigned length); int write (int fd, const void *buffer, unsigned length); void seek (int fd, unsigned position); unsigned tell (int fd); void close (int fd); - 을 구현하며, 이는 ``filesys/file.c``에 구현된 코드를 활용한다. 

#### ``synchronization`` in Filesystem
핀토스는 Multi-thread를 지원하므로, 동시에 여러 thread가 돌아가면서 프로그램을 수행한다. 이에 따라 한 파일에 대해 여러 thread가 접근하여 수정할 여지가 존재하며, 특히 동시에 여러 thread가 하나의 파일을 수정하거나, 읽는 도중에 수정이 진행될 경우 파일의 일관성이 사라질 우려가 발생한다. 이에 따라 동시에 여러 thread에 의해 쓰기가 진행되거나, 쓰는 도중에 읽거나, 읽는 도중에 쓰는 행위가 발생하지 않도록 사전에 방지할 필요가 있다.

### 개발 내용
#### File Descriptor
각 ``thread`` 마다 독립적인 ``file descriptor``를 유지한다고 하였으므로, ``thread.h``에 선언된 ``thread`` 구조체 내에 ``file descriptor``를 ``array``로 선언한다. 이는 동시에 여러 파일을 열어 볼 경우 random access가 용이하며, pintos manual 상에 최대 128개의 파일을 열 수 있다고 가정함에 따라 정적으로 그 크기를 고정해도 무방하다.

#### System Calls
본 프로젝트에서는 아래 9가지 기본적인 시스템 콜을 구현한다. 구체적인 시스템 콜의 내용은 다음과 같다.
-	``create (const char *file, unsigned initial_size)``: initial_size 바이트 크기를 가지는 ``file``이라는 새로운 파일을 생성한다. 성공할 경우 ``true``를, 실패할 경우 ``false``를 반환한다.
-	``remove (const char *file)``: ``file``이라는 이름의 파일을 삭제하고, 성공할 경우 ``true``를 그 밖의 경우에는 ``false``를 반환한다.
-	``open (const char *file)``: ``file``이라는 이름의 파일을 열고 음수가 아닌 정수가 ``file descriptor``로서 반환되며, 열기를 실패할 경우 -1을 반환한다.
-	``filesize (int fd)``: ``file descriptor`` (fd)로 열린 파일의 크기를 바이트로 반환한다.
-	``read (int fd, void *buffer, unsigned length)``: file이라는 이름의 파일을 ``file descriptor (fd)``로 열어 파일에서 읽은 바이트를 반환하되 읽지 못한 경우 -1을 반환한다.
-	``write (int fd, const void *buffer, unsigned length)``: length 크기의 데이터를 buffer에서 ``file descriptor (fd)`` 파일로 쓰도록 하는 시스템 콜로서, 실제로 쓴 바이트 수를 반환한다. 실패한 경우 -1을 반환한다.
-	``seek (int fd, unsigned position)``: ``file descriptor (fd)``로 열린 파일에서 ``position``에 따라 다음에 읽거나 쓰여진 다음 바이트의 위치를 변경한다. 
-	``tell (int fd)``: ``file descriptor (fd)``로 열린 파일에서 다음에 읽거나 쓸 위치를 정수로 반환한다.
-	``close (int fd)``: ``file descriptor (fd)``로 열린 파일을 닫는다.

#### synchronization in Filesystem
앞서 설명한 바와 같이 파일 시스템에서 ``synchronization issue``가 발생하는 부분은 한 ``thread``에서 파일을 쓰고 있을 때, 동일한 파일을 쓰거나 읽는 경우에 발생한다. 또한 한 ``thread``에서 파일을 읽고 있을 때, 동일한 파일을 쓰는 경우에 문제가 생긴다. 따라서 lock과 ``Semaphore``를 사용하여 다음과 같이 해결할 수 있다. 이를 구현하기 위해서는 ``threads/synch.c``를 참고할 수 있다.

##### 1. Lock
우선 lock은 ``Semaphore``와 달리 0 또는 1의 값만 가질 수 있다. 따라서 한번에 하나 이상의 ``critical section``이 수행되는 것을 막을 수 있다. 위에서 설명하였듯이 ``writing``중에 ``writing``이나 reading 모두가 불가능 하고, ``reading`` 중에 ``writing``이 불가능 하므로, 실제로 ``writing``나 ``reading``이 수행되는 동안 어떠한 ``writing``이나 ``reading``도 수행될 수 없도록 배제함으로서 해당 문제를 해결할 수 있다.
##### 2. ``Semaphore``
앞서 살펴본 lock은 ``Semaphore``의 특수한 경우이므로, 전반적인 아이디어는 비슷하다. 다만 ``Semaphore``를 통해 확장 가능한 부분은 동시에 여러 개의 reading을 가능케 할 수 있다는 점이다. 동시에 여러 thread에서 동일한 파일을 읽는 것은 문제가 발생하지 않고, 오히려 성능을 증가시킬 수 있으므로, lock보다 더 효율적인 ``synchronization`` 방식이라고 할 수 있다. Lock에서와 마찬가지로 writing이 진행되는 것을 막는 것과 일반 목적의 mutex를 두 개의 lock외에 reading의 개수를 세어 최초의 reading일 경우 새로운 writing을 막고, 마지막 reading일 경우 이후 writing을 허용하는 방식으로 구현할 수 있다.


## 개발 방법
###	File Descriptor
각 ``thread`` 별 ``file descriptor``를 위한 ``array``를 포함시키기 위하여 ``threads/thread.h``에 선언되어 있는 ``struct thread`` 내에 ``file descriptor``를 위하여 ``struct file* fd[128]``을 포함시킨다. 더불어 새로운 thread가 만들어 질 때, 해당 변수를 초기화 하기 위하여 ``threads/thread.c``에 포함된 ``init_thread()`` 함수에 ``fd[128]``의 원소들을 모두 ``NULL``로 초기화 해주는 코드를 포함하도록 수정한다.

###	System Call
OS 내의 파일 시스템 운영을 위해 필요한 ``system call`` 구현을 위하여 ``userprog/systemcall.h``에 새로운 시스템 콜 처리를 위한 함수를 선언하고, 이를 ``userprog/systemcall.c``에 정의한다. 또한 project 1에서와 마찬가지로 해당 파일에 있는 ``systemcall_handler()``에 포함시켜야 한다. 또한 thread 구조체에 파일이 새롭게 생겼으므로, process를 종료시킬 때, 모든 파일을 닫을 수 있도록 project 1에서 구현한 ``exit()`` 함수를 수정한다. 참고로 시스템 콜 처리를 위한 함수는 ``filesys/file.c``와 ``filesys/file.h``에 존재하므로 참고할 수 있다.
 
### ``synchronization``
동일한 파일을 동시에 여러 thread에서 접근하는 것을 막기 위한 ``synchronization``은 앞서 설명한 system call 구현에서 진행되므로, 수정하거나, 추가할 부분은 윗 절을 참고할 수 있다. 더 나아가, project 1을 진행하면서 파일을 로드하는 중인 ``child processor``보다 ``parent processor``가 먼저 종료되는 경우 오류가 발생할 수 있으므로, 이에 대한 해결이 필요하다. 이는 project 1에서 주요하게 다룬 ``userprog/process.c``를 수정이 필요하다. 더 나아가 이를 위한 ``Semaphore``를 추가하기 위해서는 역시 ``threads/thread.c`` 와 ``threads/thread.h``를 수정해야한다.

## 제작 내용
i.	File Descriptor
각 ``thread`` 별 ``file descriptor``를 위한 ``array``를 포함시키기 위하여 ``threads/thread.h``에 선언되어 있는 ``struct thread`` 내에 ``file descriptor``를 위하여 ``struct file* fd[128]``을 포함시켰으며, 구체적인 모습은 다음과 같다.

더불어 새로운 ``thread``가 만들어 질 때, 해당 변수를 초기화 하기 위하여 ``threads/thread.c``에 포함된 ``init_thread()`` 함수에 ``fd[128]``의 원소들을 모두 NULL로 초기화 해주는 코드를 포함하도록 수정하였다.

### System Call
OS 내의 파일 시스템 운영을 위해 필요한 system call 구현을 위하여 다음과 같이  ``userprog/systemcall.h``에 새로운 시스템 콜 처리를 위한 함수를 선언하였다.

해당 함수는 다음과 같이 ``userprog/systemcall.c``에 정의하였다. 이때 모든 systemcall은 ``filesys/file.h``에 선언되어 있는 함수들을 바탕으로 작동하므로, #include "filesys/file.h"등과 같이 관련된 파일을 포함시켰다. Systemcall은 userprogram에서 메모리를 통해 전달받은 parameter들을 file.h에 선언되어 있는 함수 형태에 맞추어 전달해주도록 구성하였다. 그 밖에 해당 thread에게 할당되지 않은 영역의 옳지 못한 메모리 영역을 참조하거나, 파일 parameter가 NULL인 경우에 대한 예외처리를 구현하였다. 추가적으로 open, write, read() 함수에 대해서 ``synchronization``을 ``Semaphore``를 이용하여 구현하였다. 구체적인 내용은 다음 절에서 설명한다.

또한 새로운 systemcall을 정의하였으므로, Project 1에서 진행한 것과 동일하게 실제로 userprogram으로 부터 위에서 선언한 systemcall을 실제로 전달받았을 때 해당 systemcall 함수를 호출할 수 있도록 하는 handler를 아래와 같이 수정하였다. 

``void chk_address(struct intr_frame *f)``은 각 systemcall의 parameter의 개수에 맞추어 모든 인자가 유효한지를 검사하고, 그렇지 않으면 ``exit(-1)``을 하도록 하는 반환하는 함수이다. 새로운 ``systemcall``이 선언되었으므로, 초기화하는 ``syscall_init (void)`` 함수에서 새로운 ``system call``에 대한 인자 개수를 알려주고, 이를 참조하여 ``void chk_address(struct intr_frame *f)`` 함수가 실행된다.

더불어 ``syscall_handler (struct intr_frame *f)`` 함수에서는 switch 문으로 개별 system call의 함수로 연결되므로, switch 문을 아래와 같이 확장하였다.

끝으로, 파일이 열려있는 상태에서 thread가 종료될 경우를 대비하여 project1에서 구현한 system call 중에서 exit 함수를 아래와 같이 수정하였다. 앞서 선언한 fd[] array 중에서 어떠한 값이 들어있으면 앞서 정의한 ``close()`` 함수를 이용하여 파일을 닫도록 하였다.

### ``synchronization``
동일한 파일을 동시에 여러 thread에서 접근하는 것을 막기 위하여 ``userprog/syscall.c``에 있는 open, read, write 함수에 ``threads/synch.h``에 정의되어 있는 lock을 활용하여 ``synchronization``을 구현하였다. Lock을 걸고 싶을 경우 lock_acquire(&filesys_lock)을, lock을 해제하고 싶을 경우 lock_release(&filesys_lock)을 사용하여 lock의 상태를 조절할 수 있다. 

``Semaphore``가 아닌 filesys_lock이라는 하나의 lock 변수만을 활용하여 구현하였으므로, Critical section은 parameter들이 유효한지를 확인하는 한 이후 파일이 활용되기 시작하는 지점부터 return하기 직전까지로 정의하였고, 더불어 정상적으로 파일이 읽히지 않은 경우 exit(-1)로 해당 시스템콜을 종료하기 직전에 모두 lock을 풀어주는 방식으로 구현하였다. 처음에는 효율성을 제고하기 위하여 stdin과 stdout을 사용하는 경우 lock을 걸지 않기 위하여 fd가 2 이상은 상황에만 lock을 걸어주려고 하였으나, 모든 가능한 경우에서 test가 모두 통과하는 경우는 앞서 설명한 가장 보수적으로 Critical section을 설정한 경우 밖에 없었다. 이에 따라 현재 test는 통과하지만 아직까지 해당 문제를 해결하지는 못하였음을 밝힌다.

끝으로 ``synchronization`` 이슈는 아니지만, 가상 메모리가 없는 제한된 메모리 상황에서 메모리 누수가 있는지를 확인하는 multi-oom test case와 관련해서 수정한 내용은 다음과 같다. 우선 부모 thread가 빠르게 execute 되어 child thread가 load가 채 되기도 전에 사라지는 경우 부모 thread는 기존과 동일하게 wait을 하지 않기 때문에 ``start_process()``와 process_execute() 함수에서 해당 상황에 대한 예외처리를 아래와 같이 구현하였다. 

우전 ``process_execute()`` 함수에 의하여 새로운 thread가 생기면 이후 child thread는 ``start_process()`` 함수에서 실제 thread가 실행이 된다. 부모 thread의 경우에는 이후 코드로 넘어간다면 ``sema_down(&child->wait_child)``에 의하여 이후로 진행되지 못할 것이다. 왜냐하면 ``threads/thread.c``의 ``init_thread()`` 함수에서 확인할 수 있듯이 최초로 thread가 만들어 질 때, 0으로 초기화 하기 때문이다. 이는 이후 child thread 실행 과정 중에서 thread 수행을 위하여 필요한 파일을 ``load``가 완료되면 ``sema_up(&wait_child)``에 의하여 ``wait_child Semaphore``에 의해 실행되지 못하던 부분이 실행될 수 있도록 허용한다. 즉, 부모 ``thread``가 처리를 먼저 마쳐서. 자식. Thread가 채 ``load``가 완료되지 않아 기다리는 것을 모르는 경우를 배제할 수 있는 것이다. 

끝으로 ``thread structure`` 중 ``fl``라는 ``Boolean`` 자료형에 저장되어 있는 ``load`` 성공 여부를 참조하여, 자식 ``thread``가 성공적으로 ``load``가 된 경우에는 ``child_list``에 넣고, 그렇지 않은 경우에는 에러라고 알려주는 방식으로 처리하였다.
