#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "Utils.h"

#define  COLOR_BLACK     0         /* VT102 Color Codes  */
#define  COLOR_RED       1
#define  COLOR_GREEN     2
#define  COLOR_BROWN     3
#define  COLOR_BLUE      4
#define  COLOR_MAGENTA   5
#define  COLOR_CYAN      6
#define  COLOR_WHITE     7
#define  COLOR_LIGHT_GRAY 7
#define  COLOR_YELLOW	0

OS_EVENT       *DispStrSem;
INT32U          RndNext;          /* Used by random generator */


/** hmmm... PC_... are used in sample programs in the book */
static struct timeval _t;
int PC_GetKey(void* x) { return 0; }
void PC_DOSSaveReturn() {}
void PC_DOSReturn() {}
void PC_ElapsedInit() {}
int PC_ElapsedStop() { 
	struct timeval x; 
	gettimeofday(&x, NULL); 
	return (1000000*(x.tv_sec - _t.tv_sec) + (x.tv_usec - _t.tv_usec)); 
}
void PC_ElapsedStart() { 
	gettimeofday(&_t, NULL); 
}
void PC_GetDateTime(char* s) {
	struct timeval x;
	gettimeofday(&x, NULL);
	ctime_r(&x.tv_sec, s);
}
void OSTaskStkInit_FPE_x86() {}

void PC_VectSet(int x, void* f) {
	linuxInitInt();
}

void PC_SetTickRate(int ticks) {
	ualarm(1000000/ticks, 1000000/ticks);
}

/*
 * UART 'emulation' for VT100
 */
void PutChar (char c)
{
  write(1, &c, 1);
}

void PutString (const char *s)
{
    while (*s != '\0') {
        PutChar(*s++);
    }
}

void SPrintDec(char *s, INT16U x, INT8U n)
{
    INT8U i;

    s[n] = '\0';
    for (i = 0; i < n; i++) {
        s[n - i - 1] = '0' + (x % 10);
        x /= 10;
    }
    for (i = 0; i < (n - 1); i++) {
        if (s[i] == '0') {
            s[i] = ' ';
        } else {
            break;
        }
    }
}

INT8U random (INT8U n)
{
    RndNext = RndNext * 1103515245 + 12345;
    return ((INT8U)(RndNext / 256) % (n + 1));
}

void PutDec (INT8U x2)
{
    INT8U x0;
    INT8U x1;

    x0  = (x2 % 10);
    x2 /= 10;
    x1  = (x2 % 10);
    x2 /= 10;
    if (x2) {
        PutChar(x2 + '0');
    }
    if (x1 || x2) {
        PutChar(x1 + '0');
    }
    PutChar(x0 + '0');
}

void PC_Attribute (INT8U fgcolor, INT8U bgcolor)
{/*
    PutChar(0x1b);
    PutChar('[');
    PutDec(30 + fgcolor);
    PutChar(';');
    PutDec(40 + bgcolor);
    PutChar('m');*/
}

void PC_DispClrScr (void)
{
    PC_Attribute(COLOR_WHITE, COLOR_BLACK);
    PutString("\x1B[2J");
}

void PC_DispChar (INT8U x, INT8U y, char c, INT8U fgcolor, INT8U bgcolor)
{
    INT8U err;

    OSSemPend(DispStrSem, 0, &err);                     /* Acquire semaphore to display string              */
    PC_Attribute(fgcolor, bgcolor);
    PutChar(0x1B);
    PutChar('[');
    PutDec(y);
    PutChar(';');
    PutDec(x);
    PutChar('H');
    PutChar(c);
    OSSemPost(DispStrSem);                              /* Release semaphore                                */
}

void PC_DispStr (INT8U x, INT8U y, char *s, INT8U fgcolor, INT8U bgcolor)
{
    INT8U err;

    OSSemPend(DispStrSem, 0, &err);                     /* Acquire semaphore to display string              */
    PC_Attribute(fgcolor, bgcolor);
    PutChar(0x1B);
    PutChar('[');
    PutDec(y);
    PutChar(';');
    PutDec(x);
    PutChar('H');
    PutString(s);
    OSSemPost(DispStrSem);                              /* Release semaphore                                */
}