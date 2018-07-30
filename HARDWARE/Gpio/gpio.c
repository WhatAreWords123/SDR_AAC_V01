#include "gpio.h"
/**
  * @brief  GPIO Definition
  * @param  None
  * @retval None
  */
/*
	LED1												PA1											out
	LED2												PA2											out
	LED3												PC7											out
	LED4												PC6											out
	CE													PB5											out
	A_EN												PD4											out
	PG													PC3											input
	STAT2												PC5											input/ti
	C_DIR												PB4											input
	KEY													PA3											input
	
	SEL													PD1											out

	QC													PC4											input/adc/AIN3
	A_AD2												PD2											input/adc/AIN5
	A_AD1												PD3											input/adc/AIN4
	C_AD												PD5											input/adc/AIN5
	VB													PD6											input/adc/AIN6
*/
void GPIO_Init(void)
{
  PA_DDR |= 0x06;
	PA_CR1 |= 0x06;
	PA_CR2 |= 0x06;

  PB_DDR |= 0x20;
	PB_CR1 |= 0x20;
	PB_CR2 |= 0x20;

  PC_DDR |= 0xC0;
	PC_CR1 |= 0xC0;
	PC_CR2 |= 0xC0;

  PD_DDR |= 0x10;
	PD_CR1 |= 0x10;
	PD_CR2 |= 0x10;

	PA_DDR &= ~0x08;
	PA_CR1 |= 0x08;
	PA_CR2 &= ~0x08;

	PB_CR1 |= 0x10;

	PC_CR1 |= 0x08;
	
	PC_DDR &= ~0x20;
	PC_CR1 |= 0x20;
	PC_CR2 &= ~0x20;

	
	LED4_Input_Init();
	CE = false;

	LED1 = false;
	LED2 = false;
	LED3 = false;

	SEL = true;
	A_EN = true;
	
}
/**
  * @brief  LED4_Out_Init
  * @param  None
  * @retval None
  */
void LED4_Out_Init(void)
{
  PD_DDR |= 0x02;                    //PD1_Out
	PD_CR1 |= 0x02;
	PD_CR2 |= 0x02;

	LED4 = true;
}
/**
  * @brief  LED4_Out_Init
  * @param  None
  * @retval None
  */
void LED4_Input_Init(void)
{
	LED4 = false;
  PD_DDR &= ~0x02;                   //PD1_Input
	PD_CR1 |= 0x02;
	PD_CR2 &= ~0x02;
}
/**
  * @brief  LED4_Init_Judge
  * @param  None
  * @retval None
  */
void LED4_Init_Judge(void)
{
	if(system.LED_Temporary_Init_buf != system.LED_Temporary_Init){
		system.LED_Temporary_Init_buf = system.LED_Temporary_Init;
		if(system.LED_Temporary_Init_buf == LED4_OUT){
			LED4_Out_Init();
		}else{
			LED4_Input_Init();
		}
	}
}