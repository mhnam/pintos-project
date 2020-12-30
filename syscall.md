# Document for Project #1 (System Call)
## 개발 목표
본 프로젝트를 통해서 구현하는 내용은 유저모드에서 실행되는 유저 프로그램으로부터 OS에 의해 제공되는 기능을 사용하기 위해 요청하기 위해 사용되는 시스템 콜이 정상적으로 작동하게 함을 목적으로 한다. 현재 핀토스에서는 유저 프로그램을 불러들이고, 실행시키는 것에 대해서는 구현이 되어있지만, 사용자로부터 명령어를 받아드려서 그에 해당되는 유저 프로그램을 불러들이도록 하는 부분이 구현되어 있지 않다. 또한 유저 프로그램이 실행되면서 발생하는 시스템 콜을 처리하는 부분의 구현이 필요하다. 따라서 본 프로젝트를 통해서는 사용자로부터 입력된 명령어를 적절히 가공하고, 메모리 스택을 구성하여 명령어를 쌓고, 필요한 유저 프로그램을 실행시키도록 하고, 유저 프로그램이 실행되면서 발생하는 각각의 시스템 콜들이 수행되기 위한 handler 구현을 목표로 한다. 더 나아가 4개의 수를 받아 최댓값을 구하고, 특성 번째의 피보나치 수열의 결과를 구하는 2개의 추가적인 시스템 콜을 구현한다.

## 개발 범위 및 내용
## 개발 범위
### 1. Argument Passing
사용자로부터 띄어쓰기로 구분되어 있는 명령어 조합을 입력을 입력으로 받으면, 띄어쓰기를 기준으로 명령어를 구분하여 자르고, 유저의 가상메모리 영역의 스택에 각각의 명령어를 ``80x86 calling convention``에 따라 쌓는다.

### 2. User Memory Access
유저프로그램이 실행되는 과정에서 시스템 콜이 발생하면, 커널 모드에서 처리해야하지만 운영체제의 안전성을 위하여 유저모드와 커널모드가 구분되어 있어, 유저 프로그램은 시스템 콜 번호와 시스템 콜을 처리하는데 필요한 파라미터들을 유저 가상 메모리 영역에 저장한다. 그러나 시스템 콜은 커널 모드에서 처리되므로 유저 메모리 영역에 접근이 필요하므로, 시스템 콜 요청시 유저 메모리 영역의 스택에 접근하여 필요한 argument를 가져온다. 더불어 유저 메모리에서 시스템 콜이 유저 영역이 아닌 커널 영역을 접근을 요청하는 경우에는 해당 프로세스를 즉시 종료하여 커널 영역을 보호한다.

### 3. System Calls
유저프로그램에 의해 시스템 콜 요청될 때 System calls handler을 통해 요청된 시스템 콜에 적합한 내용이 처리될 수 있도록 만든다. 이때 시스템 콜은 ``halt, exit, exec, wait, read, write, fibonacci, max_of_four_int`` 총 8개로 한정하여 커널 모드에서 해당 요청을 수행하여 유저모드에 반환한다.

## 개발 내용
### 1. Argument Passing
커널 내 스택에 ``argument``를 쌓는 과정은 다음과 같다. 먼저 사용자로 부터 받은 명령어 집합을 띄어쓰기 단위로 분리하여, 마지막 ``argument``으로 부터 유저 스택에 쌓아 올린다. 이후에 모든 ``argument``가 스택에 쌓인 다음 4비트식 스택 접근을 용이하기 하게 만들기 위하여, 4비트 단위로 끝나지 않는 경우에는 모자른 개수만큼 0을 넣는다. 이후에 각 ``argument``의 시작 주소를 쌓아준다. 이때 마지막 ``argument``는 실제 사용자가 입력한 ``argument``가 아닌 ``NULL pointer sentinel``이다. 그 이후에는 첫번째 ``argument``의 주소를 담고 있는 포인터를 스택에 쌓고, ``argument``의 개수와 ``fake return address 0``을 넣어 마무리 짓는다.

### 2. User Memory Access
1) Pintos 상에서의 ``invalid memory access`` 개념
Pintos 상에서 유저프로그램이 프로세스로 등록이 되면 각 프로세스마다 4GB짜리 유저 가상 메모리가 생긴다. 3GB를 physical baseline로서 0GB부터 3GB를 유저 영역에 매핑시키고, 그 이상의 3GB부터 4GB 영역까지를 커널 영역에 매핑되도록 구성되어 있다. 따라서 유저 프로그램이 커널 영역 즉 Phyisical baseline 위의 영역의 메모리를 접근하는 것은 곧 커널 영역에 접근하는 것이므로, invalid memory access 상황이라고 볼 수 있다.

2) ``Invalid memory access``를 막기 위한 방법
앞서 설명한 ``Pintos`` 상에서 각 프로세스의 가상 메모리를 관리하는 방식에 따라서, 유저 가상 메모리 영역에서 해당 메모리 주소가 ``Physical baseline``보다 아래에 있는지를 확인함으로써 커널 영역이 아닌 유저메모리를 정상적으로 접근하여 ``invalid memory access``가 아님을 보장받을 수 있다. 이는 ``/src/threads/vaddr.c``에 있는 ``is_user_vaddr( )`` 함수를 활용하면 용이하다.

### 3. System Calls
#### 시스템 콜의 필요성
위의 User Memory Access에서 설명한 바와 같이 Pintos를 비롯한 OS의 실행의 안전성을 보장하기 위하여 유저 모드와 커널 모드 두가지로 운영이 되고 있다. 유저 프로그램이 실행되는 유저 모드 상에서는 앞서 설명한 바와 같이 유저 영역과 커널 영역으로 나뉘어 관리되는 메모리의 커널 영역에 접근하는 것이 불가능하다. 따라서 유저 프로그램이 유저 모드에서 실행되는 동안에는 결코 커널 영역 메모리에 있는 즉, OS가 제공하는 기능을 이용하는 것은 불가능하다. 따라서 유저 프로그램에서 OS가 제공하는 프로그램을 활용하기 위해서 이러한 시스템 콜 인터페이스가 필요한 것이다.

#### 이번 프로젝트에서 개발할 시스템 콜
본 프로젝트에서는 아래 6가지 기본적인 시스템 콜과 2가지 추가적인 시스템 콜을 구현한다. 구체적인 시스템 콜의 내용은 다음과 같다.
- ``halt``: halt( )는 핀토스를 종료시키는 시스템 콜이다.
- ``exit``: exit(exit status)는 현재 실행중인 유저 프로그램을 종료시키고, exit status를 부모 process에 알리며, 해당 process가 사용하던 모든 자원을 반환하고, 콘솔 창에 exit status를 알려주는 메시지가 출력하도록 하는 시스템 콜이다.
- ``exec``: exec(file)는 새로운 자식 process가 생성하고, 해당 사용자 프로그램 (file)이 실행되도록 하는 시스템 콜이다.
- ``wait``: wait(pid)는 부모 thread가 자식 thread의 일을 마칠 때까지 기다리도록 하는 시스템 콜로서, 우선 pid가 유효한 자식 thread id 인지를 확인한 후, 자식 thread가 일을 마칠때까지 기다린 후, 종료되면 exit status를 가져오는 시스템 콜이다.
- ``read``: read(fd, *buffer, length)는 사용자 프로그램으로부터 입력받은 내용을 standard input으로 읽어드리는 시스템 콜이다.
- ``write``: write(fd, *buffer, length)는 사용자 프로그램으로부터 입력받은 내용을 standard output으로 콘솔 창에 띄워주는 시스템 콜이다.
- ``fibonacci``: fibonacci(n)은 n번째 피보나치 수열의 결과를 반환해주는 시스템 콜이다.
- ``max_of_four_int``: max_of_four_int(a, b, c, d)는 4개의 정수를 입력으로 받아, 그 중에서 가장 큰 정수를 받환해주는 시스템 콜이다.

#### 시스템 콜 진행 과정
유저 레벨에서 시스템 콜 API를 호출한 이후 커널을 거쳐 다시 유저 레벨로 돌아올 때까지 과정은 다음과 같은 순서로 진행된다.
-	``/src/examples/``에 있는 유저 프로그램에서 OS상의 도움이 필요한 작업에 대한 함수를 호출한다.
-	호출된 함수는 ``/src/lib/user/syscall.c`` 파일 내에 정의되어 있으며, 구체적으로 각 함수는 유저모드에서 유저 스택에 시스템 콜 번호와 argument를 push하여 저장한 이후에 interrupt를 걸어 커널 모드에 시스템 콜이 발생했음을 알려준다.
-	``Interrupt``가 걸리면 ``/src/threads/intr-stubs.s`` 어셈블리 파일에 의해 ``/src/threads/interrupt.c``에 있는 ``interrupt handler``가 수행되도록 한다. 이 ``interrupt handler``에 의하여 시스템 콜에 의한 interrupt를 수행하기 위한 service routine 즉, system call handler로 넘어간다.
-	``System callhandler``는 ``/src/userprog/syscall.c``에 있는 ``syscall_handler( )`` 함수에 의하여 시스템 콜의 요구사항을 수행하도록 한다. 이때 ``systemcall handler``는 ``interrupt frame`` 내에 있는 ``esp`` 포인터, 즉 유저 스택의 ``system call`` 번호를 저장한 곳의 위치를 가리키는 포인터를 이용하여 어떤 시스템 콜인지, 무슨 ``argument``가 왔는지를 확인할 수 있다.
-	``Systemcall handler``에 의해 시스템 콜을 처리한 이후에는 역시 ``interrupt frame``의 ``eax`` 변수에 저장하여 ``return`` 해줌으로써 ``eax`` 레지스터에 반환 값을 저장할 수 있고, 이에 따라 유저 모드에서도 시스템 콜의 결과를 확인할 수 있는 것이다.
-	모든 시스템 콜이 처리된 이후에는 다시 ``/src/lib/user/syscall.c`` 파일 내에 있는 함수에 의해 유저 스택에 쌓고 표시한 esp 포인터를 다시 복원함으로써 본래 상태로 돌아가며, 그 반환 값은 ``eax`` 레지스터에 보관되어 있다.

## 개발 방법
### 1. Argument Passing
``src/userprog/process.c``에 있는 ``load (const char *file_name, void (**eip) (void), void **esp)`` 함수가 사용자로부터 받은 명령어의 집합인 ``file_name``을 전달받아서, 필요한 유저프로그램을 수행한다. 따라서 이 곳에 2.B의 ``Argument passing`` 부분에 언급한 바와 같은 ``80x86 convention``에 따라 사용자로 부터 받은 명령어 집합을 띄어쓰기 단위로 나누어, 유저스택에 쌓아주는 과정을 구현이 필요하다. 이를 위해서는 먼저 전달받은 명령어 집합인 ``file_name``을 띄어쓰기 단위로 분리하여, 첫번째 ``argument``로 유저프로그램을 불러들이고, 모든 명령어 집합을 유저 가상 메모리의 스택에 쌓는 기능을 구현한다.

### 2. User Memory Access
유저 프로그램에서 잘못된 메모리 영역을 참조하는 경우는 크게 두가지 가능성으로 생각해볼 수 있다. 첫번째는 argument 상에서 유저 메모리가 아닌 커널 영역을 참조하는 경우이다. 따라서 이 경우에는 ``/src/userprog/syscell.c`` 파일의 ``syscall_handler( )`` 함수 내에서 본격적인 시스템 콜 처리 작업에 앞서 시스템 콜 처리를 위해 필요한 모든 파라미터들이 유효한 유저 메모리 안을 참조하는지 점검하는 함수를 구현을 통해 해결할 수 있다. 이때 ``/src/threads/vaddr.c``에서 ``is_user_vaddr( )`` 함수가 주어진 메모리 주소가 유저 메모리 영역인지를 확인해주므로, 이를 활용하여 구현한다.

두번째 가능성은 유저 프로그램의 종류에 상관없이 유저 메모리 밖을 참조하는 경우이다. 이때 ``/src/userprog/exception.c``에서 ``page_fault( )`` 함수가 이미 오류 상황 판단하고 있으므로, user 영역 밖을 참조하는 경우에 한하여 -1로 프로세스를 종료시키도록 구현한다.

### 3. System Calls
유저 프로그램으로부터 시스템 콜이 요청되면 해당 시스템 콜 처리를 위한 명령어 집합을 실행토록 하는 ``/src/userprog/syscall.c``의 ``syscall_handler (struct intr_frame *f)`` 함수와 같은 위치에 각각의 시스템 콜마다 수행해야하는 내용을 함수로서 구현해야한다.

또한 추가적인 시스템 콜을 구현할 경우에는 ``/src/lib/syscall.c``, ``/src/lib/syscall.h`` 파일에 유저 프로그램으로부터 새로운 시스템 콜을 사용하는 운영체제의 API가 요청되었을 때, 어떻게 내부적으로 커널모드에서 수행될 수 있는 시스템 콜로 변환하고, 메모리에 넣어야 하는지를 함수로서 구현해야한다. 더불어 ``/src/lib/syscall-nr.h`` 파일에서 새로운 시스템 콜이 불릴 때 어떠한 고유 번호로 요청할 것인지 입력되어야 한다.

이 밖에도 process wait 시스템 콜과 관련하여 ``/src/userprog/process.c``의 ``process_wait( )``과 ``process_exit( )`` 함수를 구현하였으며, ``process_wait( )``과 관련하여 semaphore로 구현하기 위해서 ``/src/threads/thread.h``에 있는 thread 구조체에 semaphore와 child를 관리하기 위한 list와 list_elem 성분, 그리고 본인의 exit status를 부모에게 알려줄 수 있도록 exit status를 저장하는 변수도 추가하였다. 이에 따란 process를 처음 생성했을 때 thread를 초기화하는 ``thread_init( )`` 함수에 새로운 변수들을 초기화하는 부분들을 추가하고,  Semaphore 구현과 관련해서는 ``/src/threads/synch.c``에 구현되어 있는 ``sema_up( )``과 ``sema_down( )`` 함수를 활용한다.

## 제작 내용
### 1. Argument Passing
#### load( )
사용자로부터 받은 명령어 집합을 사용자 가상 메모리 스택에 쌓고, 필요한 사용자 프로그램을 실행시키는 것을 목적으로 하며, ``/src/userprog/process.c``에 있는 ``load`` 함수를 수정하였다.
크게 두 부분을 수정하였다. 먼저 사용자 명령어에 따라 적절한 유저 프로그램을 여는 ``file = filesys_open ();`` 부분 이전에 strtok_r() 함수를 활용하여 사용자가 입력한 명령어를 띄어쓰기 단위로 token을 나누어 argv[] 배열에 넣고, 그 첫번째 원소를 filesys_open 함수의 파라미터로 보내는 내용을 구현하였다. 다음으로는 ``setup_stack (esp)``을 통해 생성한 사용자 스택에 argv[] 배열의 내용(명령어)과 해당 내용이 보관된 메모리 주소 등의 명령어 관련 내용을 저장하는 부분을 구현하였다.

### 2. User Memory Access
유저 프로그램에서 잘못된 메모리 영역을 참조하는 경우는 크게 두가지 가능성으로 생각해볼 수 있다. 첫번째는 argument 상에서 유저 메모리가 아닌 커널 영역을 참조하는 경우이다. 따라서 이 경우에는 ``/src/userprog/syscell.c`` 파일의 ``syscall_handler( )`` 함수 내에서 본격적인 시스템 콜 처리 작업에 앞서 인자로 받은 intr_frame을 통째로 ``chk_address( )`` 함수에 넘겨 각 유저 프로그램 종류에 따라 필요한 파라미터의 개수 만큼 유저 메모리 내에서 참조하는지를 점검하고, 아니면 -1로 프로세스를 종료시키는 함수를 구현하였다. 이때 ``/src/threads/vaddr.c``에서 ``is_user_vaddr( )`` 함수가 주어진 메모리 주소가 유저 메모리 영역인지를 확인해주므로, 이를 활용하여 구현하였다.

두번째 가능성은 유저 프로그램의 종류에 상관없이 유저 메모리 밖을 참조하는 경우가 존재할 수 있다. 이때 ``/src/userprog/exception.c``에서 ``page_fault( )`` 함수가 이미 오류 상황 판단하고 있으므로, user 영역 밖을 참조하는 경우에 한하여 역시 -1로 프로세스를 종료시키도록 구현하였다.

### 3. System Calls
#### syscall_handler( )
``/src/examples`` 디렉토리에 있는 유저 프로그램에서 시스템의 I/O를 요구하는 함수를 부르면 ``/src/lib/user/syscall.c``에 저장되어있는 함수에 따라 어셈블리어로 시스템 콜을 부른다. 이때 시스템 콜 번호(``/src/lib/syscall-nr.h``에 따름) 와 argument는 유저 스택에 저장되고, 저장 위치가 esp 레지스터로서 저장된다. 이는 궁극적으로  ``/src/userprog/syscall.c``에 있는 syscall_handler() 함수로 전달되므로, 가장 먼저 해당 함수를 수정이 필요하다.

해당 함수는 유저 프로그램에서 쌓은 스택과 관련한 레지스터의 값을 전달받는데, 그 안에 유저 스택에서 시스템 콜 번호를 가리키는 esp 레지스터를 포함하고 있다. 따라서  마지막 항목을 표시하는 esp 레지스터가 가리키는 값 (* (uint32_t *) f->esp)를 참고하여 어떤 시스템 콜을 부르고 있는지 switch 문으로 상황을 구분 짓고, 각 시스템 콜 마다 적절한 ``service routine``을 수행하도록 구현하였다.

#### halt()
``/src/userprog/syscall.c`` 파일에서 ``void halt (void)`` 형태의 함수로 구현하였으며, ``shutdown_power_off( )``를 활용했다.

#### exit()
``/src/userprog/syscall.c`` 파일에서 ``void exit (int status)`` 형태의 함수로 구현하였으며, ``thread_exit( )``를 활용하였다. 이 밖에도 이후 wait 시스템 콜 구현과 관련하여 ``/src/userprog/process.c`` 파일에서의 ``process_wait( )`` 함수에서 ``thread`` 구조체에 ``exit_status``가 필요하므로, thread 종료 시 종료 상태를 저장하도록 구현했다.

#### exec()
``/src/userprog/syscall.c`` 파일에서 ``pid_t exec (const char *file)`` 형태의 함수로 구현하였으며, ``process_exec( )``을 활용하였다. 

#### wait()
``/src/userprog/syscall.c`` 파일에서 ``void wait (pid_t pid)`` 형태의 함수로 구현하였으며, ``process_wait( )``을 활용하였다. 

#### read()
``/src/userprog/syscall.c`` 파일에서 ``Int read (int fd, void *buffer, unsigned length)`` 형태의 함수로 구현하였으며, ``/src/devices/input.c`` 파일에 있는 ``putbuf ( )`` 함수를 활용하여 콘솔창에 결과를 띄우는 ``STDOUT``을 구현하였다.

#### write()
``/src/userprog/syscall.c`` 파일에서 ``int write (int fd, const void *buffer, unsigned length);`` 형태의 함수로 구현하였으며, ``/src/lib/kernel/console.c`` 파일에 있는 ``input_getc( )`` 함수를 활용하여 사용자로부터 STDIN을 읽어 드릴 수 있도록 구현하였다.

### 4. Additional System calls
추가적인 시스템 콜을 구현하기 위해서는 ``/src/userprog/syscall.c`` 파일에 단순히 시스템 콜이 들어왔을 때 처리하는 것 이외에 몇가지 추가적인 작업이 필요하다. 가장 먼저 유저 프로그램으로 부터 전달받을 시스템 콜 번호를 부여하였다.

더불어 유저 프로그램으로 부터 해당 시스템 콜을 부르기 위한 함수(API)를 구현하자. 특히 ``max_of_four_int``는 다른 시스템콜과 다르게 4개의 인자가 필요하므로, 4개 인자를 받은 새로운 ``syscall4(NUMBER, ARG0, ARG1, ARG2, ARG3)``도 함께 구현하였다.

끝으로 해당 시스템 콜이 불렸을 때, 처리하기 위한 service routine을 앞선 다른 시스템콜들과 마찬가지로 ``/src/userprog/syscall.c`` 파일에 구현하였다.

구체적으로 ``int fibonacci(int n)`` 형태의 함수로 사용자로부터 n을 전달받으면, n번째 피보나치 수열의 값을 다시 유저 프로그램으로 반환하도록 구현하였다. 또한 ``int max_of_four_int(int a, int b, int c, int d)`` 형태의 함수로 사용자로부터 4개의 정수를 전달받으면, 그중 가장 큰 정수를 다시 유저 프로그램으로 반환하도록 구현하였다. 

구현한 새로운 시스템 콜이 잘 작동하는지 확인하기 위해서는 해당 시스템 콜을 부르는 사용자 프로그램이 필요하다. 따라서 ``/src/examples/additional.c``에 아래와 같이 새로운 사용자 프로그램을 추가해주었다.

#### 시험 및 평가 내용
해당 시스템 콜 설명에 작성되어 있는 ``pintos –filesys-size=2 -p ../examples/additional -a additional -- -f -q run ‘additional 10 20 62 40’``을 수행하면, 기대되는 바와 같이 첫번째 원소는 10번째 피보나치 수열의 값인 55가, 두번째는 10, 20 ,62, 40 중에 가장 큰 정수인 62가 출력됨을 확인할 수 있다. 
