#include "time.h"

/**
  * @brief  TIM2定时器    	时钟源16M   16分频    1M时钟   1us
  * @brief  定时器初值    	65536 - 50  = 0xFFCD
  * @brief  定时时间      	1MS
  * @brief  自动重装载值   	0xFFCD
  */
#if 0
#define TIME_CNTRH														0xFF
#define TIME_CNTRL														0xCD
#else
#define TIME_CNTRH														0xFC
#define TIME_CNTRL														0x17
#endif

void Time2_Init(void)
{
  TIM2_IER |= 0x01;         																//使能更新中断
  TIM2_EGR |= 0x01;         																//产生更新事件
  TIM2_CNTRH = TIME_CNTRH;        													//计数器值   
  TIM2_CNTRL = TIME_CNTRL;        													//计数器值 
  TIM2_PSCR = 0x04;         																//分频值   2^4 = 16分频  
  TIM2_CR1 |= 0x01;         																//使能计数器
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void Tim2_DeInit(void)
{
  TIM2_CR1 &= 0xFE;                                          //失能Tim2计数器
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
static void Led_sacn(void)
{
	static uint8_t Discharge_sacn_cnt = false;
	static uint8_t Charge_sacn_cnt = false;
	static uint8_t Charge_buf = false;
	static uint8_t Charge_blink_static = false;
	static uint8_t buf = false;

	
	if(system.Charge_For_Discharge == Discharge_State){
		Charge_blink_static = false;
		Charge_sacn_cnt= false;
		buf = false;
		battery.Charge_Batter_Low_blink = false;
		battery.Charge_Current_buf = false;
		switch(Discharge_sacn_cnt){
			case 0: if(battery.Current_Display >= Quantity_Electricity_25)	{LED1=true;		LED2=false;	LED3=false;	}Discharge_sacn_cnt++;		break;
			case 1: if(battery.Current_Display >= Quantity_Electricity_50)	{LED1=false;	LED2=true;	LED3=true;	}Discharge_sacn_cnt++;		break;
			case 2: if(battery.Current_Display >= Quantity_Electricity_75)	{LED1=false;	LED2=true;	LED3=false;	}Discharge_sacn_cnt++;		break;
			case 3: if(battery.Current_Display >= Quantity_Electricity_100)	{LED1=true;		LED2=false;	LED3=true;	}Discharge_sacn_cnt=false;break;
			default: break;
		}
		if(battery.Current_Display == Quantity_Electricity_5){
			if(++battery.Discharge_Batter_Low_blink >= 500){
				battery.Discharge_Batter_Low_blink = false;
				LED1=!LED1;
			}
			LED2=false;
			LED3=false;
		}
	}else{//system.Charge_For_Discharge == Charge_State
		Discharge_sacn_cnt= false;
		battery.Discharge_Batter_Low_blink = false;
		
		switch(battery.Current_Display){
			case Quantity_Electricity_5:
			case Quantity_Electricity_25: 	Charge_buf = 1; break;
			case Quantity_Electricity_50: 	Charge_buf = 2; break;
			case Quantity_Electricity_75: 	Charge_buf = 3; break;
			case Quantity_Electricity_100: 	Charge_buf = 4; break;
			default: break;
		}
		
		if(Charge_buf != buf){
			buf = Charge_buf;
			battery.Charge_Current_buf = Charge_buf;
			Charge_blink_static = false;
		}
		
		switch(Charge_sacn_cnt){
			case 0: if(battery.Charge_Current_buf == 0)	{LED1=true;		LED2=true;	LED3=true;	}Charge_sacn_cnt++;			break;
			case 1: if(battery.Charge_Current_buf >= 1)	{LED1=true;		LED2=false;	LED3=false;	}Charge_sacn_cnt++;			break;
			case 2: if(battery.Charge_Current_buf >= 2)	{LED1=false;	LED2=true;	LED3=true;	}Charge_sacn_cnt++;			break;
			case 3: if(battery.Charge_Current_buf >= 3)	{LED1=false;	LED2=true;	LED3=false;	}Charge_sacn_cnt++;			break;
			case 4: if(battery.Charge_Current_buf >= 4)	{LED1=true;		LED2=false;	LED3=true;	}Charge_sacn_cnt++;		break;
			case 5: if(battery.Charge_Current_buf >= 5)	{LED1=true;		LED2=true;	LED3=true;	}Charge_sacn_cnt=false;	break;
			default: break;
		}
		
		if(++battery.Charge_Batter_Low_blink >= 250){
			battery.Charge_Batter_Low_blink = false;
			if(Charge_blink_static==0xFF){
				Charge_blink_static = ~Charge_blink_static;
				battery.Charge_Current_buf += 1;
			}else{
				Charge_blink_static = ~Charge_blink_static;
				battery.Charge_Current_buf -= 1;
			}
		}
	}
}
/**
  * @brief  TIM2更新时间中断    中断向量  0x0F
  * @param  None
  * @retval None	
  */
#pragma vector=TIM2_OVR_UIF_vector//0x0F
__interrupt void Time2_OVR_IRQHandler(void)
{
  TIM2_CNTRH = TIME_CNTRH;       															//计数器值   
  TIM2_CNTRL = TIME_CNTRL;       															//计数器值
	if(system.System_State == System_Run){

		if(system.NotifyLight_EN == true){
			Led_sacn();
		}
		
	  if(!adc.Flay_Adc_gather){
		  if(++adc.Adc_gather_cnt >= ADC_GATHER_TIME){
				adc.Adc_gather_cnt = false;
				adc.Flay_Adc_gather = true;
		  }
	  }
		
	if(!battery.Battery_Level_Update){
		if(++battery.Lndicator_light_cnt >= 1000){
			battery.Lndicator_light_cnt = false;
			battery.Battery_Level_Update = true;
			}
		}
	
	if((system.System_sleep_countdown == true)&&(system.Charge_For_Discharge == Discharge_State)){
		if(++system.System_sleep_countdown_cnt >= SLEEP_TIME){
			system.System_sleep_countdown_cnt = false;
			system.System_State = System_Sleep;
			}		
		}
	
	if((battery.Battery_full_time_out == true)&&(battery.Battery_State != Battery_Full)){
		if(++battery.Battery_Full_Accumulative >= 15000){
			battery.Battery_Full_Accumulative = false;
			battery.Battery_full_locking = true;
			battery.Battery_full_time_out = false;
			}
		}
	if(++key.Key_Time_cnt >= 10){
		key.Key_Time_cnt = false;
		key.time_10ms_ok = true;
		}
	}
	TIM2_SR1 = 0x00;         																		//清除更新时间标志位
}
