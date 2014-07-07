#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include "atomvm.h"



/** Forward declarations */
DWORD WINAPI cntrl_thread_proc (LPVOID lpParameter) ;



/** Local data */
static HANDLE           _cntrl_thread ;
static HATOMVM          _atomvm ;


/**
 * \b syscall_log
 *
 * Dont run this functions from inside a vm context. 
 *
 */
uint32_t
syscall_log (uint32_t level, uint32_t buffer)
{
    printf( (char*)buffer) ;
    return 0 ;
}



/**
 * \b dbgPrintf
 *
 * Same as printf. 
 *
 */
int 
halDbgPrintf (char *format, ...)
{
	va_list			args;
	static char		msg[512] ;
    int count ;

    va_start (args, format) ;
    if ((count = vsnprintf ((char*)msg, 512, (char*)format, args)))  {
        atomvmSyscallRequest  (syscall_log, 0,  (uint32_t)msg) ;
    }

    return count ;
}


void
halDbgAssert (char *format, ...)
{
   _CrtDbgBreak() ;
}

DWORD WINAPI 
cntrl_thread_proc (LPVOID lpParameter)
 {
    /*
     * This function never returns, it runs the VM by calling __atomvmReset(). 
     */
	atomvmCtrlRun ((HATOMVM)lpParameter, 0) ;
	return 0 ;
 }


extern void sysTickVector(uint32_t vect) ;

void halInit(void)
{
	atomvmCtrlCreate (&_atomvm) ;
	_cntrl_thread = CreateThread (NULL, 0, cntrl_thread_proc, (uint32_t*)_atomvm, CREATE_SUSPENDED, NULL) ;
	ResumeThread (_cntrl_thread) ;

    /* This generates our system tick "interrupt" */
	while (1)  {
		Sleep(10) ;
		atomvmCtrlIntRequest (_atomvm, sysTickVector, 0) ;
	}

}



/** @} */
