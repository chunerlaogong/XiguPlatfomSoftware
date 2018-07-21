#ifndef _BSP_COMMON_H_
#define _BSP_COMMON_H_

#define BITs(X) (1<<(X))
#define S16H(X) (uint8_t)((X)>>8)
#define S16L(X) (uint8_t)(X)
#define _EINT() asm("rim")
#define _DINT()	asm("sim")
#define _WAIT()	asm("wfi")
#define _HALT()	asm("halt")
#endif   //_BSP_COMMON_H_

