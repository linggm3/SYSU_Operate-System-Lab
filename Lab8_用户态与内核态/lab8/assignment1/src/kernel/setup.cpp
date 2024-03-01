#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "memory.h"
#include "syscall.h"
#include "tss.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;
// 内存管理器
MemoryManager memoryManager;
// 系统调用
SystemService systemService;
// Task State Segment
TSS tss;

int syscall_0(int first, int second, int third, int forth, int fifth)
{
    printf("systerm call 0: %d, %d, %d, %d, %d\n",
           first, second, third, forth, fifth);
    return first + second + third + forth + fifth;
}

int syscall_1(int n, int div_factor)
{
    int ans = 1;
    printf("systerm call 1: %d %d,  res: ", n, div_factor);
    for(int i = 1; i < n; i++){
        ans *= i;
        printf("%d ", ans);
    }
    ans /= div_factor;
    printf("%d \n", ans);
    return ans;
}

void first_process()
{
    asm_system_call(0, 132, 324, 12, 124, 18);
    asm_halt();
}

void second_process()
{
    asm_system_call(1, 11, 22);
    asm_halt();
}

void first_thread(void *arg)
{
    printf("start process\n");
    programManager.executeProcess((const char *)first_process, 1);
    programManager.executeProcess((const char *)second_process, 1);
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

    // 内存管理器
    memoryManager.initialize();

    // 初始化系统调用
    systemService.initialize();
    // 设置0号系统调用
    systemService.setSystemCall(0, (int)syscall_0);
    systemService.setSystemCall(1, (int)syscall_1);

    // 创建第一个线程
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 1);
    if (pid == -1)
    {
        printf("can not execute thread\n");
        asm_halt();
    }

    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = ListItem2PCB(item,tagInGeneralList);
    firstThread->status = RUNNING;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);

    asm_halt();
}
