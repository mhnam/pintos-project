# Document for Project #3 (Threads)
## 개발 목표
본 프로젝트를 통해서 구현하는 내용은 핀토스 운영체제에서 스케줄링의 기본 단위가 되는 thread와 관련하여, 좀더 효율적인 방식으로 작동하게 함을 목적으로 한다. 현재 핀토스에서는 각 thread의 우선순위가 고려되지 않고, 들어온 순서대로 일정한 time slice를 할당하여 순환적으로 수행하는 Round-robin 방식이 기본적인 스케줄링 알고리즘으로 구현되어 있다. 따라서 본 프로젝트에서는 현재 수행중인 thread보다 더 높은 우선순위의 thread가 들어오면 먼저 수행되는 것을 허용하는 선점형 Priority Scheduling을 구현하는 것을 목표로 한다. 더불어 현재 while loop으로 busy waiting을 하는 방식으로 구현된 Alarm clock을 좀더 효율적으로 수정하고, Multi-level feedback queue를 도입한 BSD scheduler도 함께 구현한다.


## 	개발 범위 및 내용
### 개발 범위
#### Alarm Clock
핀토스에서는 busy waiting 방식으로 일정 시간 동안 thread가 작동되지 않도록 sleep시키는 timer_sleep() 함수가 ``devices/timer.c``에 구현되어 있다. 그러나 이로 인하여 필요없이 context switch를 한 이후에 충분한 시간이 되지 않은 경우 다시 thread_yield를 하는 비효율이 있다. 따라서 thread 구조체에 필요한 정보를 저장하고, 아예 BLOCKED state으로 전환하여 새로운 queue에 sleep 된 thread를 관리하도록 구현한다. 이후 매 tick마다 작동하는 timer_interrupt() 함수를 활용하여, 일정 시간이 지나면 다시 READY state으로 깨워 다시 ready queue에 넣어주는 방식으로 비료율을 해결할 수 있다.

### Priority Scheduling
핀토스에서는 각 thread의 우선순위가 고려되지 않고, 들어온 순서대로 일정한 time slice를 할당하여 순환적으로 수행하는 Round-robin 방식이 기본적인 스케줄링 알고리즘으로 구현되어 있다. 따라서 본 프로젝트에서는 현재 수행중인 thread보다 더 높은 우선순위의 thread가 들어오면 먼저 수행되는 것을 허용하는 선점형 Priority Scheduling을 구현한다. 이를 통하여 우선순위에 따라 thread가 수행될 수 있다. 

### Advanced Scheduler
BSD 스케줄러은 여러개의 multi-level queue로 구성되어 각 queue마다 다른 scheduling을 하는 방식이다. 이를 통하여 기존에 priority로만 우선순위가 결정되는 기존 알고리즘의 한계를 극복하고, 여러가지 thread 특성에 맞추어 다양한 스케줄링 알고리즘을 구현할 수 있게 된다.

### 개발 내용
#### Alarm Clock: Blocked 상태의 스레드를 어떻게 깨울 수 있는지
우선 thread가 timer_sleep() 함수로 잠들면 잠들기 시작한 시간과 잠들어 있어야 하는 시간을 알수 없으므로 thread 구조체에 새로운 변수를 선언하여, 두 가지 변수를 저장시킨다. 이후 BLOCKED state으로 전환한 후 sleep 된 thread를 쌓아두는 queue에 저장해둔다.
이후 매 tick마다 작동하는 timer_interrupt() 함수를 활용하여, 매 tick마다 sleep된 thread를 모아둔 queue를 돌아보며, 저장된 thread가 필요한 시간을 지났는지 확인하고, 지난 경우 다시 READY state 바꾸고, 다시 ready queue에 넣어주는 방식으로 blocked된 thread를 깨울 수 있다.

#### Priority Scheduling: Ready list에 running thread보다 높은 priority를 가진 thread가 들어올 경우 priority scheduling에 따르면 어떻게 해야하는지
Running thread 보다 높은 priority를 가진 thread가 들어오는 상황은 새로운 thread가 생성되거나, BLOCKED state에 있던 thread가 unblock되거나, 특정 thread의 priority가 변경되는 경우가 있다. 해당 상황이 발생할 때, thread_current()->priority와 새롭게 추가될 thread의 priority를 비교하여, 새로운 thread의 priority가 높을 경우 thread_yield() 하도록 구현하면, 더 높은 prioity가 ready list에 추가될 때, 항상 ready list에서 가장 높은 priority가 실행될 수 있다.

#### Advanced Scheduler: Advanced Scheduler에서 priority 계산에 필요한 각 요소
새로운 priority는 다음과 같은 식에 따라 계산된다. ``PRI_MAX-((recent_cpu)/4)-(nice×2)``

따라서 priority 계산을 위해서는 각 thread의 recent_cpu와 nice 값이 필요하다. 이 식에서 알 수 있듯이, cpu 사용이 많을 수록 nice가 높을 수록 priority가 작아지는 것을 확인할 수 있다. 즉 우선순위가 낮아서, 해당 알고리즘에서는 cpu 사용이 높은 thread 일수록 우호적이지 않다는 사실을 알 수 있다.


##	개발 방법
### Alarm Clock
우선 thread가 timer_sleep() 함수로 잠들면 잠들기 시작한 시간과 잠들어 있어야 하는 시간을 알수 없으므로 thread 구조체에 새로운 변수를 선언해야한다. 이를 위하여 ``threads/thread.h``에 int64_t sleep_time과 int64_t start_sleep_time을 포함시킨다. 더불어 새로운 thread가 만들어 질 때, 해당 변수를 초기화 하기 위하여 ``threads/thread.c``에 포함된 init_thread() 함수에 새로운 변수를 모두 0으로 초기화해주는 코드를 포함하도록 수정한다.

이후 ``devices/timer.c``에서 잠들어 있는 thread들을 관리하기 위하여 struct list sleeped_list를 선언하고, timer_init() 함수에 해당 list를 초기화한다. 이후 timer_sleep() 함수에서는 잠들어 있을 기간과 잠들기 시작한 시간을 새로운 변수 sleep_time과 start_sleep_time에 저장하고, 잠들어 있는 thread만 모아둔 queue (sleeped_list)에 넣는다. List로 구현되었으므로, list_push_back 함수를 활용한다. 이후 thread_block() 함수를 이용하여 thread를 BLOCKED state으로 전환한다.

이후 잠든 thread가 일정 시간이 지난 이후 깨어날 수 있도록 매 tick마다 작동하는 timer_interrupt() 함수를 활용한다. 매 tick마다 sleep된 thread를 모아둔 queue를 돌아보며, 저장된 thread가 필요한 시간을 지났는지 확인하고, 지난 경우 다시 READY state 바꾸고, 다시 ready queue에 넣어주는 방식으로 blocked된 thread를 깨우도록 추가한다.

###	Priority Scheduling
우선 thread scheduling과 관련한 코드는 ``threads/thread.c``에 구현되어 있으므로, 해당 코드를 중점적으로 수정하였다. 특히 ready list의 첫번째 성분을 실행하면 항상 ready list에서 가장 높은 priority를 가진 thread가 실행될 수 있도록 ready list가 항상 priority 순서 대로 정렬하도록 코드를 수정했다. 이를 구현하기 위해 ready list에 thread를 추가할 때 list_insert_ordered()를 활용하였으며, 이를 위하여 thread의 priority를 비교하여, Boolean 값을 반환하는 priority_compare() 함수를 구현했다.

Thread scheduling은 새로운 thread가 생성되거나, running thread가 양보되는 상황, blocked된 thread가 다시 ready state이 되거나, thread의 priority가 변경되는 상황에 발생하므로, thread_create(), thread_yield(), thread_unblock(), thread_set_priority() 함수를 주로 수정하였다. 

###	Advanced Scheduler
우선 각 thread마다 cpu 사용 시간과 다른 thread에 대한 niceness를 저장해야하므로 이를 위하여 ``threads/thread.h``에 int recent_cpu와 int nice를 포함시킨다. 더불어 새로운 thread가 만들어 질 때, 해당 변수를 부모로 부터 상속받을 수 있도록 하기 위하여 ``threads/thread.c``에 포함된 init_thread() 함수에 새로운 변수를 모두 현재 실행중인 thread의 것을 그대로 받도록 초기화해주는 코드를 포함하도록 수정한다. 더불어 첫번째 생성된 thread에 한하여 0으로 초기화 해야하므로 ``threads/thread.c``에 포함된 thread_init() 함수에서는 모두 0으로 초기화 해주었다. 더불어 전역 변수로 선언된 load_avg도 0으로 초기화 해준다.

이후 실제 load_avg, recent_cpu와 이를 기반으로 정의된 새로운 priority를 계산하는 함수 – update_values(), update_priority() – 를 구현하였다. 다만 핀토스 커널에서는 floating point 연산을 지원하지 않으므로, fixed point 실수가 포함된 연산을 진행할 때 자료형을 일치시키기 위하여 각종 연산에 대한 함수를 정의가 필요하다. 끝으로 매 초마다 update_values() 함수가, 매 4개 tick 마다 update_priority() 함수가 실행될 수 있도록 ``Alarm Clock`` 구현을 하면서 활용하였던 ``/devices/timer.c``에 있는 timer_interrupt() 함수를 활용하였다.

그 밖에 실행중인 thread의 nice, load_avg, recent_cpu, priority 변수를 가져오기 위한 함수를 ``/threads/thread.c``에 추가적으로 구현하였다.

##	제작 내용
###	Alarm Clock
Busy waiting 방식이 아닌 Alarm Clock 구현을 위하여 sleep된 thread의 sleep이 진행될 시간과 sleep이 시작된 시간을 저장하기 위하여 ``threads/thread.h``에 선언되어 있는 struct thread 내에 각각 int64_t sleep_time와 int64_t start_sleep_time을 포함시켰으며, 구체적인 모습은 다음과 같다.

struct 구조체 안에 새로운 원소를 선언함에 따라 ``/thread/thread.c`` 안에 있는 init_thread() 함수에서 초기화하는 코드를 추가하였다.

Sleep timer를 busy waiting이 아닌 BLOCK state으로 바꾼 후 이들을 하나의 queue에 모아두기 위하여 sleeped list를 선언하였다. 더불어 ``/devices/timer.c``에 있는 timer_init() 함수에서 이를 list_init() 함수를 활용하여 초기화해주었다.

Busy waiting 방식으로 구현되어 있는 timer_sleep() 함수는 ``/threads/thread.h``에 수정한 thread 구조체 내에 sleep을 시작하는 시간과 sleep이 진행되는 시간을 각각 start_sleep_time과 sleep_time 구조체에 넣어준다. 이후 앞서 선언한 sleeped_list에 해당  thread를 넣어준 후, thread_block() 함수를 활용하여 BLOCK state로 바꾸어준다.

앞서 구현한 코드만으로는 실제 busy waiting으로 잠들어 있는 thread를 깨워주는 역할을 해주지 못한다. 이를 해결하기 위하여 매 tick 마다 interrupt를 걸어주는 timer_interrupt() 함수를 활용한다. 해당 함수는 자동적으로 매 tick마다 실행되므로, 매 tick마다 잠들어 있는 thread 들이 담겨 있는 sleeped_list를 돌아보며, 잠들어 있는 시간보다 넘게 잠들어 있는 thread가 있는지 확인하고, 조건에 만족하는 thread에 대해서는 thread_unblock() 함수를 통하여 READY state으로 바꾸고, sleeped_list에서 제거해준다.

### ii.	Priority Scheduling
Priority에 따라서 scheduling이 일어날 수 있도록 구현하기 위하여 새롭게 scheduling이 필요한 상황 – thread_create(), thread_yield(), thread_unblock(), thread_set_priority() – 을 모두 수정해주었다.

가장 먼저 새로운 thread가 만들어지는 경우에는 현재 진행 중인 running_thread 보다 priority가 높은 경우에는 바로 실행되어야 하므로, priority가 높은지 확인하고, 맞으면 thread_yield()하도록 하였다.

나머지 상황들 - thread_yield(), thread_unblock(), thread_set_priority() – 에 대해서는 ready queue에 thread의 순서를 priority 내림차순으로 정렬함으로써 가장 앞에 있는 thread가 가장 priority가 높은 thread가 오도록 하여, 기존 round robin 알고리즘에서 크게 벗어나지 않도록 구성하였다. 이를 위하여 새로운 thread를 ready queue에 넣을 때, list_insert_ordered를 활용하였으며, 이를 활용하기 위하여 첫번째 인자가 더 클 경우 1을 return하는 priority_compare 함수를 정의하였다.

이후 앞서 정의한 priority_compare 함수를 활용해, 새로운 thread를 ready queue 즉, ready_list에 넣을 때 priority 내림차순으로 정렬되도록 수정하였다.

끝으로 semaphore를 활용할 때 priority와 충돌하는 문제 – semaphore가 생기는 경우  priority 정렬을 보장하지 않음 - 에 관한 test case ``priority_sema``을 위하여  ``/threads/synch.c``에 있는 sema_up() 함수를 수정하였다. sema_up()에 의하여 기다리는 thread들을 저장해두는 waiters에서 항상 가장 높은 priority를 가진 thread를 찾아서 실행해주도록 구성하였다.

더불어 priority aging과 관련해서는 다음 Advanced Scheduler를 구현하면서 priority를 재계산하는 것을 통하여 aging을 구현하였으므로, 다음 절에서 구체적으로 설명하였다.

### Advanced Scheduler
Advanced Scheduler 구현을 위해서는 각 thread 별로 CPU를 사용한 시간과 다른 thread에 대한 niceness가 추가적으로 필요하다. 이를 위하여 ``threads/thread.h``에 선언되어 있는 struct thread 내에 각각 int recent_cpu와 int nice를 포함시켰으며, 구체적인 모습은 다음과 같다.

struct 구조체 안에 새로운 원소를 선언함에 따라 ``/thread/thread.c`` 안에 있는 init_thread() 함수에서 초기화하는 코드를 추가하였다. 이 때 recent_cpu와 nice는 모두 thread 를 만든 부모의 것을 그래도 물려받으므로, running_thread()의 것을 그대로 물려받는다.

새로운 thread에 한하여 nice와 recent_cpu는 모두 0으로 설정하라고 하였으므로, 첫번째 thread를 초기화하는 thread_init() 함수에서는 이를 0으로 초기화 한다. 또한 전역변수로 선언된 load_avg 변수 역시 0으로 초기화 해준다.

핀토스 내에서는 커널의 성능을 위하여 모두 fixed point float를 사용하며, 연산한다. 이에 따라 실수를 포함하는 숫자와 연산을 하기 위해서는 반드시 정수변수를 변환하는 과정이 필요하며, 기존 연산을 그대로 사용하는데 어려움이 있다. 이에 따라서 핀토스 매뉴얼 B.6 (pp. 94-95)을 참고하여, 아래와 같이 연산을 위한 함수를 선언하였다. 이때 CONVERT는 핀토스 내에서 사용되는 fixed point 자료형이 14개의 소수점을 가지고 있으므로, 정수 1을 fixed point 자료형으로 바꾸기 위해서는 14칸 왼쪽으로 미뤄줘야한다. 따라서 자료형 변환을 위하여 1을 14칸 왼쪽으로 밀어주는 값을 constant로 지정하였다. 이는 십진수로는 2의 14승에 해당하는 값이다.

BSD 스케줄링에서 주요하게 활용되고 있는 load_avg 변수와 recent_cpu 값을 계산하는 update_values() 함수와 모든 thread의 priority를 계산하여 수정해주는 update_priority() 함수의 모습이다. all_list에 저장되어 있는 모든 thread에 대해서 값을 수정해준다. 이 때 정수형과 fixed point 실수형 변수가 섞여 있으므로, 위에서 선언한 함수를 활용하여 계산하였다. 

한편 Priority의 경우에는 가장 큰 값과 작은 값을 넘지 않는 다는 보장이 없으므로, 계산된 결과가 최댓값이나 최솟값을 넘지 않는지 확인하여 결과를 수정해주었다. 

끝으로 매뉴얼에서 nice, load_avg, recent_cpu, priority에 대해서 현재 실행중인 thread의 해당 값을 가져오는 함수를 정의하도록 요구하였으므로, 아래와 같이 정의하였다. 실수형으로 정의된 load_avg와 recent_cpu 역시 정수형으로 반환하도록 요구하였으므로, CONVERT (2의 14승) 값으로 나누어 변환해주었다.

더불어 BSD 스케줄러 구성을 위하여 nice 변수를 수정하는 것에 대하여 새로운 priority를 계산하고, 현재 실행중인 thread보다 더 높은 thread가 있을 경우 실행 중인 thread를 바꾸어주는 thread_set_nice() 함수를 구현하였다.

앞서 load_avg, recent_cpu와 priority를 재계산하는 함수를 실제로 매 초마다, 4개 tick마다 실행될 수 있도록 timer_interrupt() 함수를 활용하였다. Timer_interrupt는 매 tick마다 수행되므로 tick이 4의 배수이면 priority를 수정해주고, TIMER_FREQ (1초의 단위가 되는 tick의 개수, 현재 100으로 설정됨)의 배수가 되면 load_avg와 recent_cpu를 수정해주도록 구현하였다.
