

#include <stdint.h>

/*
 * This is the API for this example HAL running on the atomvm virtual machine.
 */
extern void     halInit(void) ;
extern int      halDbgPrintf (char *format, ...) ;
extern void     halDbgAssert (char *format, ...) ;
/* ... */


/*
 * sysTickVector - To be implemented for this HAL
 */
extern void     sysTickVector(uint32_t vect) ;


