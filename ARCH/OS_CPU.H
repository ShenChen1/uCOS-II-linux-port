
#include <unistd.h>
#include <signal.h>
#include <string.h>  ///< memcpy, memset used in os_task.c !

#ifndef OS_CPU_A	/* skip the rest if we're including from the assembler file */

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT  extern
#endif

/** DATA TYPES (Compiler and architecture specific)
 */
typedef unsigned char  		BOOLEAN;
typedef unsigned char  		INT8U;      /* Unsigned  8 bit quantity          */
typedef signed   char  		INT8S;      /* Signed    8 bit quantity          */
typedef unsigned short int   	INT16U;     /* Unsigned 16 bit quantity          */
typedef signed   short int   	INT16S;     /* Signed   16 bit quantity          */
typedef unsigned int  		INT32U;     /* Unsigned 32 bit quantity          */
typedef signed   int  		INT32S;     /* Signed   32 bit quantity          */
typedef float          		FP32;       /* Single precision floating point   */
typedef INT32U  		OS_STK;     /* Each stack entry is 32 bit wide   */
typedef sigset_t		OS_CPU_SR;  ///< the 'status register' corresponds to signals


/** used in os_cpu_c */
#if (OS_CPU_HOOKS_EN > 0) && (OS_TASK_SW_HOOK_EN > 0)
    void OSTaskSwHook (void);
#endif

/** linuxInit does the setup of the signal handler. */
void linuxInit(void);

/** linuxInitInt starts periodic interrupts. */
void linuxInitInt(void);

/** Macro to block interrupts (on Linux: signals)
 *
 * Critical method 1 which does not restore the signal state may lead to hanging
 * timer interrupts, especially when debugging (i.e. real time is much faster than debug
 * time).
 */
#define OS_CRITICAL_METHOD 3

/** We add all the virtual interrupt signals to the mask and save the old process
 * signal mask to the backup 'status register'.
 */
#define OS_ENTER_CRITICAL() { 	sigset_t set; \
				sigemptyset(&set); \
				sigaddset(&set, SIGALRM); \
				sigaddset(&set, SIGUSR1); \
				sigprocmask(SIG_SETMASK, &set, &cpu_sr); \
				}
/** Macro to unblock interrupts
 *
 * Here we just restore the state that was returned by previous call to sigprocmask
 */
#define OS_EXIT_CRITICAL() {	sigprocmask(SIG_SETMASK, &cpu_sr, NULL); \
				}
/**
 * Stack grows from HIGH to LOW memory on linux x86
 */
#define  OS_STK_GROWTH      1

/**
 * This macro posts a Linux signal to ourselves; it is returned in the handler with the
 * threads context. The SIGUSR1 handler saves the context and then calls OSCtxSw().
 *
 * (use OSCtxSw() directly when no sw int available)
 */
#define OS_TASK_SW() { kill(getpid(), SIGUSR1); }

#endif //OS_CPU_A
