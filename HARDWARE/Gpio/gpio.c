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
  PA_DDR |= 0x06;                    //PA1 PA2���ģʽ
	PA_CR1 |= 0x06;                    //�������
	PA_CR2 |= 0x06;                    //����ٶ�10Mhz

  PB_DDR |= 0x20;                    //PB5���ģʽ
	PB_CR1 |= 0x20;                    //�������
	PB_CR2 |= 0x20;                    //����ٶ�10Mhz

  PC_DDR |= 0xC0;                    //PC6 PC7���ģʽ
	PC_CR1 |= 0xC0;                    //�������
	PC_CR2 |= 0xC0;                    //����ٶ�10Mhz

  PD_DDR |= 0x10;                    //PD4���ģʽ
	PD_CR1 |= 0x10;                    //�������
	PD_CR2 |= 0x10;                    //����ٶ�10Mhz

	PA_DDR &= ~0x08;
	PA_CR1 |= 0x08;                    //PA3 ��������
	PA_CR2 &= ~0x08;

	PB_CR1 |= 0x10;                    //PB4��������

	PC_CR1 |= 0x08;                    //PC3��������
	
	PC_DDR &= ~0x20;
	PC_CR1 |= 0x20;                    //PC5��������
	PC_CR2 &= ~0x20;

  PD_DDR |= 0x02;                    //PD1���ģʽ
	PD_CR1 |= 0x02;                    //�������
	PD_CR2 |= 0x02;                    //����ٶ�10Mhz

	CE = false;

	LED1 = false;
	LED2 = false;
	LED3 = false;
	LED4 = false;

	SEL = true;
	A_EN = true;
	
}