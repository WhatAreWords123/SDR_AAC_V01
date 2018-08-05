#ifndef __IWDG_H_
#define __IWDG_H_
#include "config.h"

#define WINDOW_VALUE											0x42
#define COUNTER_INIT       								0x7F

#define BIT_MASK													((uint8_t)0x7F)
#define WWDG_CR_WDGA_DATE									((uint8_t)0x80) /*!< WDGA bit mask */
#define WWDG_CR_T6_DATE										((uint8_t)0x40) /*!< T6 bit mask */
#define WWDG_WR_RESET_VALUE								((uint8_t)0x7F) /*!< T bits mask */

void System_WWDG_Init(uint8_t Counter,uint8_t WindowValue);
void System_WWDG_Disable(uint8_t Counter,uint8_t WindowValue);
void WWDG_SetCounter(void);

#endif