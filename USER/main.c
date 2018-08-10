#include "main.h"
#include "delay.h"
#include "gpio.h"
#include "time.h"
#include "adc.h"
#include "key.h"
#include "iwdg.h"

_ADC adc;
_KEY key;
_System system;
_TYPE_C type_c;
_Battery battery;
_Qc_Detection qc_detection;
_A_Detection a_detection;

/**
  * @brief  Configure System_Variable_Init
  * @param  None
  * @retval None
  */
static void System_Variable_Init(void)
{
	system.System_State = System_Run;

	key.Forced_shutdown = false;

	system.System_sleep_countdown = false;

	system.Hardware_Sleep = false;
	system.Hardware_Sleep_dowm_cnt = false;
	
	qc_detection.QC_Gather_finish = false;
	a_detection.ADC_A1_Gather_finish = false;
	a_detection.ADC_A2_Gather_finish = false;
	
	a_detection.A1_overcurrent_cnt = false;
	a_detection.A2_overcurrent_cnt = false;
	a_detection.ADC_A1_AD_Voltage = false;
	a_detection.ADC_A2_AD_Voltage = false;

	a_detection.A1_No_load_cnt = false;
	a_detection.A1_Load_cnt = false;
	a_detection.A1_Sleep_flay = false;

	a_detection.A2_Load_cnt = false;
	a_detection.A2_Sleep_flay = false;
	a_detection.A2_No_load_cnt = false;
	
	system.System_sleep_countdown_cnt = false;
	adc.Adc_gather_cnt = false;
	adc.Adc_Query=false;

	type_c.Load_cnt = false;
	type_c.Sleep_flay = false;
	type_c.No_load_cnt = false;
	type_c.C_overcurrent_cnt = false;
	type_c.ADC_TYPE_C_Voltage = false;
	
	qc_detection.QC_Gather_finish = false;

	battery.warning_temp = false;
	battery.Lndicator_light_cnt = false;
	battery.Charge_Current_warning = false;
	battery.Battery_Full_Accumulative = false;
	battery.Battery_warning_blink_time = false;
	
	battery.Batter_Low_Pressure = Batter_Normal;
	system.NotifyLight_EN = false;
	battery.Batter_Low_Filtration = false;

	a_detection.A_load_status = false;
	a_detection.A_out_status = true;
	a_detection.A_out_disable_countdown_cnt = false;
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
	PC_DDR &= ~0x20;
	PC_CR2 |= 0x20;
	EXTI_CR1 |= 0x10;	
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
static void Sleep_task(void)
{
	CE = true;
	LED1 = false;
	LED2 = false;
	LED3 = false;
	LED4 = false;
	SEL = false;
	A_EN = false;
#if WWDG_ENABLE
	WWDG_SetCounter();
#endif
	if(system.Hardware_Sleep == true){
		ADC_OFF_CMD();
		Tim2_DeInit();
		SLEEP:
		asm("sim");                                     //关闭全局中断
		if(battery.Batter_Low_Pressure != Batter_Low){
			Key_Interrupt_Enable();
		}
		TYPE_C_Interrupt_Enable();
		asm("rim");                                     //开全局中断 
#if WWDG_ENABLE
		System_WWDG_Disable(COUNTER_INIT,WINDOW_VALUE);
#endif
	  ClockConfig_OFF();                              //关闭所有外设时钟  
	  asm("halt");                                    //进入停机模式
	  ClockConfig_ON();
		if(battery.Batter_Low_Pressure == Batter_Low){
			if((STAT2 == true) && (C_DIR != false)){
				goto SLEEP;
			}
		}
		System_Initial();
		qc_detection.QC_Gather_finish = false;
		adc.Flay_Adc_gather = true;
		battery.Battery_Level_Update = true;
		system.NotifyLight_EN = true;
	}
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void main(void)
{
	System_Initial();
	adc.Flay_Adc_gather = true;
	battery.Battery_warning = false;
	delay_ms(150);
#if WWDG_ENABLE
	System_WWDG_Init(COUNTER_INIT,WINDOW_VALUE);
#endif
	battery.Current_Display = Quantity_Electricity_100;
	battery.Battery_Level_Update = true;
	asm("rim");                                 //开全局中断 
	while(1){
#if 1
		if(system.System_State == System_Run){
			Charge_For_Discharge_Detection();
			Key_event();
			if(key.Forced_shutdown!=true){
			Adc_Task();
			Battery_Volume();
			Port_monitoring();
			}
#if WWDG_ENABLE
			WWDG_SetCounter();
#endif
		}else{//system.System_State == System_Sleep
			Sleep_task();
		}
#else
		WWDG_SetCounter();
		delay_ms(20);
#endif
	}
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
#pragma vector=EXTI0_vector//0x05  A
__interrupt void Exti0_OVR_IRQHandler(void){
//Don't do the action
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
#pragma vector=EXTI2_vector//0x07 C
__interrupt void Exti2_OVR_IRQHandler(void){
//Don't do the action
}
#if 0
/**
  * @brief  None
  * @param  None
  * @retval None
  */
#pragma vector=EXTI3_vector//0x08
__interrupt void Exti3_OVR_IRQHandler(void){
//Don't do the action
}
#endif
