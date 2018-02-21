#include "includes.h"
#include <stdio.h>

#define TASK_STK_SIZE (8*1024)
OS_STK   StartTaskStk[TASK_STK_SIZE];



void  StartTask (void *pdata)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif
    pdata = pdata;

    linuxInitInt();/* Initialize the Linux timer */
    OSStatInit();/* Initialize uC/OS-II's statistics*/

    for (;;)
    {
    	printf("sc\n");
 	OSTimeDly(100);
    }
}

OS_STK   TaskStk1[TASK_STK_SIZE];
OS_STK   TaskStk2[TASK_STK_SIZE];


OS_EVENT * s_r;
OS_EVENT * s_e;

static INT8U a = 0;

void task1(void *pdata)
{
	INT8U err;
	while(1)
	{
		printf("pend s_e start\r\n");
		OSSemPend(s_e, 0, &err);
		printf("pend s_e end\r\n");
		
		a = 1;
		printf("task1:%d\r\n", a);
		OSTimeDly(100);
		
		printf("post s_r start\r\n");
		OSSemPost(s_r);
		printf("post s_r end\r\n");
	}
}

void task2(void *pdata)
{
	INT8U err;
	while(1)
	{
		printf("pend s_r start\r\n");
		OSSemPend(s_r, 0, &err);
		printf("pend s_r end\r\n");
		
		a = 0;
		printf("task2:%d\r\n", a);

		printf("post s_e start\r\n");
		OSSemPost(s_e);
		printf("post s_e end\r\n");		
	}
}

int main (void)
{
    OSInit( );						

    OSTaskCreate(task1,			
    	(void*)0,					
    	&TaskStk2[TASK_STK_SIZE - 1],		
    	0);


    OSTaskCreate(task2,			
    	(void*)0,					
    	&TaskStk1[TASK_STK_SIZE - 1],		
    	0);
	

	s_r = OSSemCreate(1);
	s_e = OSSemCreate(0);

    OSStart( );				

    return 0;
}

/*$PAGE*/  
/* 
********************************************************************************************************* 
*                                                 HOOK
********************************************************************************************************* 
*/  

/**                                          TASK CREATION HOOK
 *
 * This function is called when a task is created.
 * Arguments  : ptcb   is a pointer to the task control block of the task being created.
 * Note(s)    : 1) Interrupts are disabled during this call.
 */
#if OS_CPU_HOOKS_EN > 0
void OSTaskCreateHook (OS_TCB *ptcb)
{
    ptcb = ptcb;
}
#endif

/**                                          TASK DELETION HOOK
 *
 * This function is called when a task is deleted.
 * \arg ptcb   is a pointer to the task control block of the task being deleted.
 * Note(s)    : 1) Interrupts are disabled during this call.
 */
#if OS_CPU_HOOKS_EN > 0
void OSTaskDelHook (OS_TCB *ptcb)
{
    ptcb = ptcb;       /* Prevent compiler warning */
}
#endif

/**                                           TASK SWITCH HOOK
 *
 * This function is called when a task switch is performed.  This allows
 * you to perform other operations during a context switch.
 *
 * Note(s)    : 1) Interrupts are disabled during this call.
 *              2) It is assumed that the global pointer 'OSTCBHighRdy' points to the
 *              TCB of the task that will be 'switched in' (i.e. the highest priority
 *              task) and, 'OSTCBCur' points to the task being switched out (i.e. the
 *              preempted task).
 */
#if (OS_CPU_HOOKS_EN > 0) && (OS_TASK_SW_HOOK_EN > 0)
void OSTaskSwHook (void)
{
}
#endif

/**                                           STATISTIC TASK HOOK
 *
 * This function is called every second by uC/OS-II's statistics task.
 * This allows your application to add functionality to the statistics task.
 */
#if (OS_TASK_STAT_HOOK_EN > 0)
void OSTaskStatHook (void)
{
}
#endif

/**                                               TICK HOOK
 *
 * This function is called every tick.
 * Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
 */
#if (OS_CPU_HOOKS_EN > 0) && (OS_TIME_TICK_HOOK_EN > 0)
void OSTimeTickHook (void)
{
}
#endif

/**                                     OS INITIALIZATION HOOK
 *                                            (BEGINNING)
 *
 * This function is called by OSInit() at the beginning of OSInit().
 * Note(s)    : 1) Interrupts should be disabled during this call.
 */
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
void OSInitHookBegin (void)
{
    linuxInit(); ///< installs the syscall handler
}
#endif

/**                                     OS INITIALIZATION HOOK
 *                                               (END)
 *
 * This function is called by OSInit() at the end of OSInit().
 * Note(s)    : 1) Interrupts should be disabled during this call.
 */
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
void OSInitHookEnd (void)
{
}
#endif

/**                                          IDLE TASK HOOK
 *
 * This function is called by the idle task.  This hook has been added to
 * allow you to do such things as STOP the CPU to conserve power.
 * Note(s)    : 1) Interrupts are enabled during this call.
 *
 *      On real hardware a power saving function would be selected.
 *      On Linux, we sleep a little to lower the load on the system
 *      or, we block until next signal is delivered by calling select().
 *
 *      If either of these methods causes problems just leave the
 *      function empty.
 */
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION >= 251
void OSTaskIdleHook (void)
{
    //sleep(1); ///< as done in Topsy
    //printf("a\n");
    //select(0, NULL, NULL, NULL, NULL); ///< as done in eCos
}
#endif

/**                                           OSTCBInit() HOOK
 *
 * This function is called by OS_TCBInit() after setting up most of the TCB.
 * \arg ptcb    is a pointer to the TCB of the task being created.
 * Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
 */
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
void OSTCBInitHook (OS_TCB *ptcb)
{
    ptcb = ptcb;
}
#endif

