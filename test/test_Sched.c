#include <system.h>
#include <trace.h>



WORD ticks;
_SYS_TASK task_list;
_SYS_TASK task_list1;

void task_handler(void)
{
    volatile WORD last_ticks = _Sys_SchedPollTicks();
    TRACE("task_handler at", last_ticks);
}

void task_handler1(void)
{
    volatile WORD last_ticks1 = _Sys_SchedGetTicks();
    TRACE("task_handler 1 at", last_ticks1);
}

void Sys_Scheduler_Start(void)
{
    _Sys_SchedSetResolution(10000);
    _Sys_SchedRegister(&task_list, task_handler, 100);
    _Sys_SchedRegister(&task_list1, task_handler1, 500);

}


void test_Schedule(void)
{
    Sys_Scheduler_Start();
}