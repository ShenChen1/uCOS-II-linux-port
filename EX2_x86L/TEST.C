/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #2
*********************************************************************************************************
*/

#include "includes.h"
#include "Utils.h"
/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/

#define          TASK_STK_SIZE     8*1024                /* Size of each task's stacks (# of WORDs)       */

#define          TASK_START_ID       0                /* Application tasks IDs                         */
#define          TASK_CLK_ID         1
#define          TASK_1_ID           2
#define          TASK_2_ID           3
#define          TASK_3_ID           4
#define          TASK_4_ID           5
#define          TASK_5_ID           6

#define          TASK_START_PRIO    10                /* Application tasks priorities                  */
#define          TASK_CLK_PRIO      11
#define          TASK_1_PRIO        12
#define          TASK_2_PRIO        13
#define          TASK_3_PRIO        14
#define          TASK_4_PRIO        15
#define          TASK_5_PRIO        16

/*
*********************************************************************************************************
*                                              VARIABLES
*********************************************************************************************************
*/

OS_STK        TaskStartStk[TASK_STK_SIZE];            /* Startup    task stack                         */
OS_STK        TaskClkStk[TASK_STK_SIZE];              /* Clock      task stack                         */
OS_STK        Task1Stk[TASK_STK_SIZE];                /* Task #1    task stack                         */
OS_STK        Task2Stk[TASK_STK_SIZE];                /* Task #2    task stack                         */
OS_STK        Task3Stk[TASK_STK_SIZE];                /* Task #3    task stack                         */
OS_STK        Task4Stk[TASK_STK_SIZE];                /* Task #4    task stack                         */
OS_STK        Task5Stk[TASK_STK_SIZE];                /* Task #5    task stack                         */

OS_EVENT     *AckMbox;                                /* Message mailboxes for Tasks #4 and #5         */
OS_EVENT     *TxMbox;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

        void  TaskStart(void *data);                  /* Function prototypes of tasks                  */
static  void  TaskStartCreateTasks(void);
static  void  TaskStartDispInit(void);
static  void  TaskStartDisp(void);
        void  TaskClk(void *data);
        void  Task1(void *data);
        void  Task2(void *data);
        void  Task3(void *data);
        void  Task4(void *data);
        void  Task5(void *data);

/*$PAGE*/
/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/

int main (void)
{
    OS_STK *ptos;
    OS_STK *pbos;
    INT32U  size;


    PC_DispClrScr();                        /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    //PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    PC_ElapsedInit();                                      /* Initialized elapsed time measurement     */

    ptos        = &TaskStartStk[TASK_STK_SIZE - 1];        /* TaskStart() will use Floating-Point      */
    pbos        = &TaskStartStk[0];
    size        = TASK_STK_SIZE;
    //OSTaskStkInit_FPE_x86(&ptos, &pbos, &size);            
    OSTaskCreateExt(TaskStart,
                   (void *)0,
                   ptos,
                   TASK_START_PRIO,
                   TASK_START_ID,
                   pbos,
                   size,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSStart();                                             /* Start multitasking                       */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                               STARTUP TASK
*********************************************************************************************************
*/

void  TaskStart (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    INT16S     key;


    pdata = pdata;                                         /* Prevent compiler warning                 */

    TaskStartDispInit();                                   /* Setup the display                        */

    OS_ENTER_CRITICAL();                                   /* Install uC/OS-II's clock tick ISR        */
    PC_VectSet(0x08, (void *)OSTickISR);
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    OSStatInit();                                          /* Initialize uC/OS-II's statistics         */

    AckMbox = OSMboxCreate((void *)0);                     /* Create 2 message mailboxes               */
    TxMbox  = OSMboxCreate((void *)0);

    TaskStartCreateTasks();                                /* Create all other tasks                   */

    for (;;) {
        TaskStartDisp();                                   /* Update the display                       */

        if (PC_GetKey(&key)) {                             /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn();                            /* Yes, return to DOS                       */
            }
        }

        OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
        OSTimeDly(OS_TICKS_PER_SEC);                       /* Wait one second                          */
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
    PC_DispStr( 0,  3, "                                    EXAMPLE #2                                  ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0,  4, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0,  5, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0,  6, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0,  7, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0,  8, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0,  9, "Task           Total Stack  Free Stack  Used Stack  ExecTime (uS)               ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 10, "-------------  -----------  ----------  ----------  -------------               ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 11, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 12, "TaskStart():                                                                    ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 13, "TaskClk()  :                                                                    ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 14, "Task1()    :                                                                    ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 15, "Task2()    :                                                                    ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 16, "Task3()    :                                                                    ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 17, "Task4()    :                                                                    ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 18, "Task5()    :                                                                    ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 19, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 20, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 21, "                                                                                ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 22, "#Tasks          :        CPU Usage:     %                                       ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 23, "#Task switch/sec:                                                               ", COLOR_BLACK , COLOR_LIGHT_GRAY);  
    PC_DispStr( 0, 24, "                            <-PRESS 'Ctrl C' TO QUIT->                          ", COLOR_BLACK , COLOR_LIGHT_GRAY );  
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
  
    sprintf(s, "%3d", OSCPUUsage);                                 /* Display CPU usage in %               */  
    PC_DispStr(36, 22, s, COLOR_YELLOW , COLOR_BLUE);  
  
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
    OSTaskCreateExt(TaskClk,
                   (void *)0,
                   &TaskClkStk[TASK_STK_SIZE - 1],
                   TASK_CLK_PRIO,
                   TASK_CLK_ID,
                   &TaskClkStk[0],
                   TASK_STK_SIZE,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(Task1,
                   (void *)0,
                   &Task1Stk[TASK_STK_SIZE - 1],
                   TASK_1_PRIO,
                   TASK_1_ID,
                   &Task1Stk[0],
                   TASK_STK_SIZE,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(Task2,
                   (void *)0,
                   &Task2Stk[TASK_STK_SIZE - 1],
                   TASK_2_PRIO,
                   TASK_2_ID,
                   &Task2Stk[0],
                   TASK_STK_SIZE,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(Task3,
                   (void *)0,
                   &Task3Stk[TASK_STK_SIZE - 1],
                   TASK_3_PRIO,
                   TASK_3_ID,
                   &Task3Stk[0],
                   TASK_STK_SIZE,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(Task4,
                   (void *)0,
                   &Task4Stk[TASK_STK_SIZE-1],
                   TASK_4_PRIO,
                   TASK_4_ID,
                   &Task4Stk[0],
                   TASK_STK_SIZE,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(Task5,
                   (void *)0,
                   &Task5Stk[TASK_STK_SIZE-1],
                   TASK_5_PRIO,
                   TASK_5_ID,
                   &Task5Stk[0],
                   TASK_STK_SIZE,
                   (void *)0,
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                               TASK #1
*
* Description: This task executes every 100 mS and measures the time it task to perform stack checking
*              for each of the 5 application tasks.  Also, this task displays the statistics related to
*              each task's stack usage.
*********************************************************************************************************
*/

void  Task1 (void *pdata)  
{  
    INT8U       err;  
    OS_STK_DATA data;                       /* Storage for task stack data                             */  
    INT16U      time;                       /* Execution time (in uS)                                  */  
    INT8U       i;  
    char        s[80];  
  
  
    pdata = pdata;  
    for (;;) {  
        for (i = 0; i < 7; i++) {  
            PC_ElapsedStart();  
            err  = OSTaskStkChk(TASK_START_PRIO + i, &data);  
            time = PC_ElapsedStop();  
            if (err == OS_NO_ERR) {  
                sprintf(s, "%4d        %4d        %4d        %6d",  
                        data.OSFree + data.OSUsed,  
                        data.OSFree,  
                        data.OSUsed,  
                        time);  
                PC_DispStr(19, 12 + i, s, COLOR_BLACK , COLOR_LIGHT_GRAY);  
            }  
        }  
        OSTimeDlyHMSM(0, 0, 0, 100);                       /* Delay for 100 mS                         */  
    }  
}  
/*$PAGE*/  
/* 
********************************************************************************************************* 
*                                               TASK #2 
* 
* Description: This task displays a clockwise rotating wheel on the screen. 
********************************************************************************************************* 
*/  
  
void  Task2 (void *data)  
{  
    data = data;  
    for (;;) {  
        PC_DispChar(70, 15, '|',  COLOR_YELLOW , COLOR_BLUE);  
        OSTimeDly(10);  
        PC_DispChar(70, 15, '/',  COLOR_YELLOW , COLOR_BLUE);  
        OSTimeDly(10);  
        PC_DispChar(70, 15, '-',  COLOR_YELLOW , COLOR_BLUE);  
        OSTimeDly(10);  
        PC_DispChar(70, 15, '\\', COLOR_YELLOW , COLOR_BLUE);  
        OSTimeDly(10);  
    }  
}  
/*$PAGE*/  
/* 
********************************************************************************************************* 
*                                               TASK #3 
* 
* Description: This task displays a counter-clockwise rotating wheel on the screen. 
* 
* Note(s)    : I allocated 500 bytes of storage on the stack to artificially 'eat' up stack space. 
********************************************************************************************************* 
*/  
  
void  Task3 (void *data)  
{  
    char    dummy[500];  
    INT16U  i;  
  
  
    data = data;  
    for (i = 0; i < 499; i++) {        /* Use up the stack with 'junk'                                 */  
        dummy[i] = '?';  
    }  
    for (;;) {  
        PC_DispChar(70, 16, '|',  COLOR_YELLOW , COLOR_BLUE);  
        OSTimeDly(20);  
        PC_DispChar(70, 16, '\\', COLOR_YELLOW , COLOR_BLUE);  
        OSTimeDly(20);  
        PC_DispChar(70, 16, '-',  COLOR_YELLOW , COLOR_BLUE);  
        OSTimeDly(20);  
        PC_DispChar(70, 16, '/',  COLOR_YELLOW , COLOR_BLUE);  
        OSTimeDly(20);  
    }  
}  
/*$PAGE*/  
/** 
 * ******************************************************************************************************** 
*                                               TASK #4 
* 
* Description: This task sends a message to Task #5.  The message consist of a character that needs to 
*              be displayed by Task #5.  This task then waits for an acknowledgement from Task #5 
*              indicating that the message has been displayed. 
* 
* \attention added local recursive function to show stack checking in action; if the random  
* maxima are chosen too large, however, you will crash the demo... 
 ********************************************************************************************************* 
*/  
void  Task4 (void *data)  
{  
 /*   void justForFun(int x) {  
        if (x > 0) {  
            justForFun(x - 1);  
        }  
    }  */
  
    char   txmsg;  
    INT8U  err;  
  
  
    data  = data;  
    txmsg = 'A';  
    for (;;) {  
        OSMboxPost(TxMbox, (void *)&txmsg);      /* Send message to Task #5                            */  
        OSMboxPend(AckMbox, 0, &err);            /* Wait for acknowledgement from Task #5              */  
        txmsg++;                                 /* Next message to send                               */  
        if (txmsg == 'Z') {  
            txmsg = 'A';                         /* Start new series of messages                       */  
        }  
        PC_DispChar(70, 18, '.', COLOR_YELLOW , COLOR_BLUE);  
  
    //justForFun(random(99) + random(99) + random(99) + random(99)); ///< this is like russian roulette ;-(  
     }  
}  
/*$PAGE*/  
/* 
********************************************************************************************************* 
*                                               TASK #5 
* 
* Description: This task displays messages sent by Task #4.  When the message is displayed, Task #5 
*              acknowledges Task #4. 
********************************************************************************************************* 
*/  
  
void  Task5 (void *data)  
{  
    char  *rxmsg;  
    INT8U  err;  
  
  
    data = data;  
    for (;;) {  
        rxmsg = (char *)OSMboxPend(TxMbox, 0, &err);                  /* Wait for message from Task #4 */  
        PC_DispChar(70, 18, *rxmsg, COLOR_YELLOW , COLOR_BLUE);  
        OSTimeDlyHMSM(0, 0, 1, 0);                                    /* Wait 1 second                 */  
        OSMboxPost(AckMbox, (void *)1);                               /* Acknowledge reception of msg  */  
    }  
}  
/*$PAGE*/  
/* 
********************************************************************************************************* 
*                                               CLOCK TASK 
********************************************************************************************************* 
*/  
  
void  TaskClk (void *data)  
{  
    char s[40];  
  
  
    data = data;  
    for (;;) {  
        PC_GetDateTime(s);  
        PC_DispStr(56, 23, s, COLOR_YELLOW , COLOR_BLUE);  
        OSTimeDly(OS_TICKS_PER_SEC);  
    }  
}


/*$PAGE*/  
/* 
********************************************************************************************************* 
*                                                HOOK
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

