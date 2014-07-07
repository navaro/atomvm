#include <stdlib.h>
#include <stdint.h>
#include "atomvm.h"
#include "example_hal.h"

#define EXAMPLE_STACK_SIZE              1024
volatile unsigned long  _sys_ticks = 0 ;
static uint32_t         _thread_stack[EXAMPLE_STACK_SIZE] ;

static HATOMVM_CONTEXT  _hatomvm_context_thread ;
static HATOMVM_CONTEXT  _hatomvm_context_main ;

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

    halDbgPrintf ("Example VM in Thread. systicks = %d\r\n", _sys_ticks) ;

    /*
     * Just sit in a loop and print a message every now and then.
     */
    while (1) {
        if (!(_sys_ticks % 100)) {
            if (!logged) {
                halDbgPrintf ("Example VM running in the thread context. systicks = %d\r\n", _sys_ticks) ;
                logged = 1 ;
            }
        }
        else {
            logged = 0 ;
        }

        if (_sys_ticks > 500) {
            /* switch back to main context */
            atomvmContextSwitch (_hatomvm_context_thread, _hatomvm_context_main) ;
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
    halDbgPrintf ("Example VM in main. systicks = %d\r\n", _sys_ticks) ;

    /*
     * Create a "thread" saved in _hatomvm_context_thread and run it. 
     * We save the main context in _hatomvm_context_main so we can return 
     * here at a later stage.
     */
    _hatomvm_context_thread = atomvmContextCreate (0) ;
    _hatomvm_context_main = atomvmContextCreate (0) ;
    atomvmContextInit (_hatomvm_context_thread, &_thread_stack[EXAMPLE_STACK_SIZE-1], Thread, 0, 0) ;

    atomvmContextSwitch (_hatomvm_context_main, _hatomvm_context_thread) ;

    halDbgPrintf ("Example VM back in main. systicks = %d\r\n", _sys_ticks) ;

    while (1) {
        if (!(_sys_ticks%100)) {
            halDbgPrintf ("Example VM masking interrupts... systicks = %d\r\n", _sys_ticks) ;
            atomvmInterruptMask (1) ;
            _sys_ticks++; /* since interupts is disabled, this message will never be printed again. */
        }
    }


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
