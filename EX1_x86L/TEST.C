/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #1
*********************************************************************************************************
*/

#include "includes.h"
#include "Utils.h"
/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE                 8*1024       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                        	10       /* Number of identical tasks                          */
#define  uCOS				  	1
/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_STK        TaskStartStk[TASK_STK_SIZE];
char          TaskData[N_TASKS];                      /* Parameters to pass to each task               */
OS_EVENT     *RandomSem;

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

        void  Task(void *data);                       /* Function prototypes of tasks                  */
        void  TaskStart(void *data);                  /* Function prototypes of Startup task           */
static  void  TaskStartCreateTasks(void);
static  void  TaskStartDispInit(void);
static  void  TaskStartDisp(void);

/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

int  main (void)
{
    PC_DispClrScr();      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    //PC_VectSet(uCOS, (void *)OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    RandomSem   = OSSemCreate(1);                          /* Random number semaphore                  */

    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStart();                                             /* Start multitasking                       */
}


/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
void  TaskStart (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    char       s[100];
    INT16S     key;


    pdata = pdata;                                         /* Prevent compiler warning                 */

    TaskStartDispInit();                                   /* Initialize the display                   */

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, (void *)OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    OSStatInit();                                          /* Initialize uC/OS-II's statistics         */

    TaskStartCreateTasks();                                /* Create all the application tasks         */

    for (;;) {
        TaskStartDisp();                                  /* Update the display                       */


        if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn();                            /* Return to DOS                            */
            }
        }

        OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
        OSTimeDlyHMSM(0, 0, 1, 0);                         /* Wait one second                          */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        INITIALIZE THE DISPLAY
*********************************************************************************************************
*/

static  void  TaskStartDispInit (void)
{
/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
    PC_DispStr( 0,  0, "                         uC/OS-II, The Real-Time Kernel                         ", COLOR_WHITE , COLOR_RED );  
    PC_DispStr( 0,  1, "                                Jean J. Labrosse                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0,  2, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0,  3, "                                    EXAMPLE #1                                  ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0,  4, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0,  5, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0,  6, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0,  7, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0,  8, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0,  9, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 10, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 11, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 12, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 13, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 14, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 15, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 16, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 17, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 18, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 19, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 20, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 21, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 22, "#Tasks          :        CPU Usage:     %                                       ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 23, "#Task switch/sec:                                                               ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 24, "                            <-PRESS 'Ctrl C' TO QUIT->                             ", COLOR_BLACK , COLOR_LIGHT_GRAY ); 
/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           UPDATE THE DISPLAY
*********************************************************************************************************
*/

static  void  TaskStartDisp (void)
{
    char   s[80];
	
    sprintf(s, "%5d", OSTaskCtr);                                  /* Display #tasks running               */  
    PC_DispStr(18, 22, s, COLOR_YELLOW , COLOR_BLUE);  
  
#if OS_TASK_STAT_EN > 0  
    sprintf(s, "%3d", OSCPUUsage);                                 /* Display CPU usage in %               */  
    PC_DispStr(36, 22, s, COLOR_YELLOW , COLOR_BLUE);  
#endif  
  
    sprintf(s, "%5d", OSCtxSwCtr);                                 /* Display #context switches per second */  
    PC_DispStr(18, 23, s, COLOR_YELLOW , COLOR_BLUE);  
  
    sprintf(s, "V%1d.%02d", OSVersion() / 100, OSVersion() % 100); /* Display uC/OS-II's version number    */  
    PC_DispStr(75, 24, s, COLOR_YELLOW , COLOR_BLUE); 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks (void)
{
    INT8U  i;


    for (i = 0; i < N_TASKS; i++) {                        /* Create N_TASKS identical tasks           */
        TaskData[i] = '0' + i;                             /* Each task will display its own letter    */
        OSTaskCreate(Task, (void *)&TaskData[i], &TaskStk[i][TASK_STK_SIZE - 1], i + 1);
    }
}

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/

void  Task (void *pdata)
{
    INT8U  x;
    INT8U  y;
    INT8U  err;


    for (;;) {
        OSSemPend(RandomSem, 0, &err);           /* Acquire semaphore to perform random numbers        */
        x = random(80);                          /* Find X position where task number will appear      */
        y = random(16);                          /* Find Y position where task number will appear      */
        OSSemPost(RandomSem);                    /* Release semaphore                                  */
                                                 /* Display the task number on the screen              */
        PC_DispChar(x, y + 5, *(char *)pdata, COLOR_BLACK , COLOR_LIGHT_GRAY);
        OSTimeDly(1);                            /* Delay 1 clock tick                                 */
    }
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

