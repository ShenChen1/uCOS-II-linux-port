#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "includes.h"

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


int PC_GetKey(void* x);
void PC_DOSSaveReturn();
void PC_DOSReturn();
void PC_ElapsedInit();
int PC_ElapsedStop();
void PC_ElapsedStart();
void PC_GetDateTime(char* s);
void OSTaskStkInit_FPE_x86();
void PC_VectSet(int x, void* f);
void PC_SetTickRate(int ticks);
void PutChar (char c);
void PutString (const char *s);
void SPrintDec(char *s, INT16U x, INT8U n);
INT8U random (INT8U n);
void PutDec (INT8U x2);
void PC_Attribute (INT8U fgcolor, INT8U bgcolor);
void PC_DispClrScr (void);
void PC_DispChar (INT8U x, INT8U y, char c, INT8U fgcolor, INT8U bgcolor);
void PC_DispStr (INT8U x, INT8U y, char *s, INT8U fgcolor, INT8U bgcolor);

#endif