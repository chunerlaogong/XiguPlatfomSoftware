#ifndef _DRIVERS_COMMON_H_
#define _DRIVERS_COMMON_H_

/**************∫Í∂®“Â*********************/

#include "Delay.h"

#define _EINT()     asm("rim")
#define _DINT()	    asm("sim")
#define _WAIT()	    asm("wfi")
#define _HALT()	    asm("halt")
#define _EINTH()	asm("rim")
#define _DINTH()	asm("sim")





#endif   //_DRIVERS_COMMON_H_

