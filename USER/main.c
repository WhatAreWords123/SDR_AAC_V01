#include "main.h"
#include "delay.h"
#include "gpio.h"
#include "time.h"
#include "adc.h"

_System system;
_ADC adc;
_Battery battery;

/**
  * @brief  Configure System_Variable_Init
  * @param  None
  * @retval None
  */
static void System_Variable_Init(void)
{
	system.System_State = System_Run;
}
/**
  * @brief  SClK_Initial() => 初始化系统时钟，系统时钟 = 16MHZ
  * @param  None
  * @retval None 
  */
static void SClK_Initial(void)
{      
	//启用内部高速晶振且无分频16MHz
	CLK_ICKR |= 0x01;             //开启内部HSI  
	while(!(CLK_ICKR&0x02));      //HSI准备就绪
	CLK_SWR = 0xe1;               //HSI为主时钟源
	CLK_CKDIVR = 0x00;            //HSI不分频
}
/**
  * @brief  void ClockConfig(void) => 开启所有外设时钟
  * @param  None
  * @retval None
  */
static void ClockConfig_ON(void)
{
  CLK_PCKENR1 = 0xFF;
  CLK_PCKENR2 = 0xFF;
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
static void ClockConfig_OFF(void)
{
  CLK_PCKENR1 = 0x00;
  CLK_PCKENR2 = 0x00;
}
static void Key_Interrupt_Enable(void)
{
	PA_DDR &= ~0x08;
	PA_CR2 |= 0x08;
	EXTI_CR1 |= 0x02;
}
static void TYPE_C_Interrupt_Enable(void)
{
	PC_DDR &= ~0x30;
	PC_CR2 |= 0x30;
	EXTI_CR1 |= 0x30;	
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
static void Charge_For_Discharge_Detection(void)
{
	if((C_DIR == false) && (STAT2 == true)){
		SEL = false;
		system.Charge_For_Discharge = Charge_State;
	}else{
		SEL = true;
		system.Charge_For_Discharge = Discharge_State;
	}
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
static void System_Initial(void)
{
	System_Variable_Init();
	SClK_Initial();
	ClockConfig_ON();
	GPIO_Init();
	Time2_Init();
	ADC_Init();
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void main(void)
{
	System_Initial();
	battery.Current_Display = 6;
	adc.Flay_Adc_gather = true;
	delay_ms(150);
	battery.Battery_Level_Update = true;
	asm("rim");                                 //开全局中断 
	while(1){
		if(system.System_State == System_Run){
			Charge_For_Discharge_Detection();
			Adc_Task();
			Battery_Volume();
		}else{//system.System_State == System_Sleep
			
		}
	}
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
#pragma vector=EXTI0_vector//0x05
__interrupt void Exti0_OVR_IRQHandler(void){
//Don't do the action
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
#pragma vector=EXTI2_vector//0x07
__interrupt void Exti2_OVR_IRQHandler(void){
//Don't do the action
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
#pragma vector=EXTI3_vector//0x08
__interrupt void Exti3_OVR_IRQHandler(void){
//Don't do the action
}
