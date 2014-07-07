#include <stdlib.h>
#include <stdint.h>
#include "atomvm.h"
#include "example_hal.h"

#define EXAMPLE_STACK_SIZE              1024
volatile unsigned long  _sys_ticks = 0 ;
static uint32_t         _thread_stack[EXAMPLE_STACK_SIZE] ;

void sysTickVector (uint32_t vect)
{
    _sys_ticks++ ;
}

/*
 * This is a thread cotext
 */
static void Thread(uint32_t arg) 
{
    int logged = 0 ;

    /*
     * Just sit in a loop and print a message every now and then.
     */
    while (1) {
        if (!(_sys_ticks % 100)) {
            if (!logged) {
                halDbgPrintf ("Example VM Tread. systicks = %d\r\n", _sys_ticks) ;
                logged = 1 ;
            }
        }
        else {
            logged = 0 ;
        }
    }

    return ;
}

/*
 * This is the main application entry point.
 */
int
main (void)
{
/*
 * halInit will call __atomvmReset() so you can use this code on your 
 * embedded MPU and in your Visual Studio project.
 */
      halInit();
#ifdef WIN32
}
void 
__atomvmReset ()
#endif
{
    HATOMVM_CONTEXT thread ;

    halDbgPrintf ("Example VM startup. systicks = %d\r\n", _sys_ticks) ;

    /*
     * Create a "thread" and run it.
     */
    thread = atomvmContextCreate (0) ;
    atomvmContextInit (thread, &_thread_stack[EXAMPLE_STACK_SIZE-1], Thread, 0, 0) ;
    atomvmContextSwitch (0, thread) ;

#ifndef WIN32
  return 0 ;
#endif
}

#ifndef WIN32
}
#else
void 
__atomvmClose ()
{
}
#endif
