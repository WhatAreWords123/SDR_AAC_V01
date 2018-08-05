#include "iwdg.h"

/**
  * @brief  None
  * @param  None
  * @retval None
  */
void System_WWDG_Init(uint8_t Counter,uint8_t WindowValue)
{
	WWDG_WR = WWDG_WR_RESET_VALUE;
	WWDG_CR = (uint8_t)((uint8_t)(WWDG_CR_WDGA_DATE | WWDG_CR_T6_DATE) | (uint8_t)Counter);
	WWDG_WR = (uint8_t)((uint8_t)(~WWDG_CR_WDGA_DATE) & (uint8_t)(WWDG_CR_T6_DATE | WindowValue));
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void System_WWDG_Disable(void)
{

}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
uint8_t WWDG_GetCounter(void)
{
	return WWDG_CR;
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void WWDG_SetCounter(void)
{
	uint8_t NUM = false;

	NUM = WWDG_GetCounter();
	
	if(( NUM & 0x7F) < WINDOW_VALUE){
	   /* Write to T[6:0] bits to configure the counter value, no need to do
	     a read-modify-write; writing a 0 to WDGA bit does nothing */
		WWDG_CR = (uint8_t)(COUNTER_INIT & (uint8_t)BIT_MASK);
	}
}