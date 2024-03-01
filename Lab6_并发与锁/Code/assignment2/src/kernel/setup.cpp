#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "msg_queue.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;

int msg = 0;  // 消息
Msg_Queue que;  // 消息缓冲区
Semaphore empty_signal, full_signal, my_mutex; // 声明信号量 



int cheese_burger;

void produce(void *args)
{
    for(int e = 0; e < 10; e++){
	    	empty_signal.P(); // 等有空位 
		my_mutex.P(); // 等获得锁 
		que.push(msg++); 
		my_mutex.V(); // 解锁 
		full_signal.V(); // 满+=1 
		printf("produce: ");
		que.show();
	}
}

void consume(void *args)
{
    for(int e = 0; e < 10; e++){
		full_signal.P(); // 等有东西 
		my_mutex.P(); // 等获得锁 
	    	que.pop(); 
		my_mutex.V(); // 解锁 
		empty_signal.V(); // 空位+=1 
		printf("consume: ");
		que.show();
	}
}

void first_thread(void *arg)
{
    // 第1个线程不可以返回
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);
	
    my_mutex.initialize(1);  // 锁置 1
    empty_signal.initialize(CAPACITY);  // 空置 CAPACITY
    full_signal.initialize(0);  // 满置 0 
    que.init();

    programManager.executeThread(produce, nullptr, "second thread", 1);
    programManager.executeThread(consume, nullptr, "third thread", 1);

    asm_halt();
}

extern "C" void setup_kernel()
{
    // 中断管理器
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);

    // 输出管理器
    stdio.initialize();

    // 进程/线程管理器
    programManager.initialize();

    // 创建第一个线程
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 1);
    if (pid == -1)
    {
        printf("can not execute thread\n");
        asm_halt();
    }

    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = ListItem2PCB(item, tagInGeneralList);
    firstThread->status = RUNNING;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);

    asm_halt();
}
