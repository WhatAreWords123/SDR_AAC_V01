#include "adc.h"
#include "gpio.h"

/**
  * @brief  None
  * @param  None
  * @retval None
  */
void ADC_Init(void)
{
  ADC_CR1 = 0x40;                       //8分频系数   单次转换模式
  ADC_CR2 = 0x30;                       
  ADC_TDRH = 0x01;                      //禁止高位施密特触发功能
  ADC_TDRL = 0x01;                      //禁止低位施密特触发功能
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void ADC_OFF_CMD(void)
{
  ADC_CR1 &= ~0x01;                    //失能ADC
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
uint16_t Read_ADC(uint8_t ch)
{

	uint8_t m,n,i;
  uint8_t  ADC_Num_H = false;
  uint8_t  ADC_Num_L = false;
	uint16_t AD_Date;
	uint16_t AD_Date_Array[6];
	uint16_t AD_Date1, AD_Date2;

  ADC_CSR = 0x00;                       //清除一下通道
  ADC_CSR = ch&0x0F;                    //ADC转换通道
//  ADC_CSR = ch&0x80;                    //清除转换结束标志位

		
	for(m=0; m<=5; m++)
	{
	  ADC_CR1 |= 0x01;                      									//使能ADC并开始转换
	  for(i=0; i<200; i++);                 									//延时等待
	  ADC_CR1 |= 0x01;                      									//使能ADC并开始转换
		while((ADC_CSR & ADC_Over) != ADC_Over);    						//等待ADC转换结束
		ADC_CSR &= ~ADC_Over;                       						//清除转换结束标志位
	  ADC_Num_H = ADC_DRH;
	  ADC_Num_L = ADC_DRL;		
		AD_Date_Array[m] = ((uint16_t)ADC_Num_H << 2) + (uint16_t)ADC_Num_L;
		ADC_CR1 = 0x00;
	}

	for(m=0; m<=4; m++)
	{
		for(n=m+1; n<=5; n++)
		{
			AD_Date1 = AD_Date_Array[m];
			AD_Date2 = AD_Date_Array[n];
			if(AD_Date1 > AD_Date2)
			{
				AD_Date_Array[m] = AD_Date2;
				AD_Date_Array[n] = AD_Date1;
			}
		}
	}
	AD_Date = false;

	for(m=2; m<=3; m++)
	{
		AD_Date += AD_Date_Array[m];
	}
  AD_Date = AD_Date >> 1;
	
  return AD_Date;
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
static void Fast_slow_charge_discharge_judge(void)
{
	qc_detection.QC_Gather_finish = true;
	
	if(qc_detection.ADC_QC_Voltage > Speed_Voltage){
		qc_detection.Mode = Speed_mode;
	}
	else if(qc_detection.ADC_QC_Voltage < low_speed_Voltage){
		qc_detection.Mode = low_speed_mode;
	}
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void TYPE_C_sleep_filter(void)
{
	if(type_c.ADC_TYPE_C_Voltage > TYPE_C_SLEEP){
		type_c.Load_cnt++;
	}
#if 0
	if(type_c.ADC_TYPE_C_Voltage < TYPE_C_SLEEP){
#else
	else{
#endif
		type_c.No_load_cnt++;	
	}
	if(type_c.No_load_cnt >= 500){
		type_c.Sleep_flay = true;
		type_c.No_load_cnt = false;
	}
	if(type_c.Load_cnt >= 200){
		type_c.Sleep_flay = false;
		type_c.Load_cnt = false;
	}
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void A1_sleep_filter(void)
{
	if(a_detection.ADC_A1_AD_Voltage > A_load){
		a_detection.A1_Load_cnt++;
	}
#if 0
	if(a_detection.ADC_A1_AD_Voltage < A_SLEEP){
#else
	else{
#endif
		a_detection.A1_No_load_cnt++;
	}

	if(a_detection.A1_No_load_cnt >= 500){
		a_detection.A1_Sleep_flay = true;
		a_detection.A1_No_load_cnt = false;
	}
	if(a_detection.A1_Load_cnt >= 200){
		a_detection.A1_Sleep_flay = false;
		a_detection.A1_Load_cnt = false;
		a_detection.A1_No_load_cnt = false;
	}
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void A2_sleep_filter(void)
{

	if(a_detection.ADC_A2_AD_Voltage > A_load){
		a_detection.A2_Load_cnt++;
	}
#if 0
	else if(a_detection.ADC_A2_AD_Voltage < A_SLEEP){
#else
	else{
#endif
		a_detection.A2_No_load_cnt++;	
	}
	
	if(a_detection.A2_No_load_cnt >= 200){
		a_detection.A2_Sleep_flay = true;
		a_detection.A2_No_load_cnt = false;
	}
	if(a_detection.A2_Load_cnt >= 500){
		a_detection.A2_Sleep_flay = false;
		a_detection.A2_Load_cnt = false;
	}
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void Port_monitoring(void)
{
	if(system.Charge_For_Discharge == Charge_State){
		if(qc_detection.Mode == Speed_mode){
			system.LED_Temporary_Init = LED4_OUT;
			LED4_Init_Judge();
		}else{
			system.LED_Temporary_Init = LED4_INPUT;
			LED4_Init_Judge();
		}
		if((battery.Battery_voltage > MAX_VOLTAGE) && (PG == true)){
			battery.Battery_full_time_out = true;
			if(battery.Battery_full_locking == true){
				battery.Battery_State = Battery_Full;
			}
		}else{
			battery.Battery_State = Battery_Charge;
		}
	}else{//system.Charge_For_Discharge == Discharge_State
		if((qc_detection.QC_Gather_finish==true)&&(a_detection.ADC_A1_Gather_finish==true)
			&&(a_detection.ADC_A2_Gather_finish==true)){
#if 0
		if(((type_c.Sleep_flay == true)||(STAT2 != true))
			&&(a_detection.A1_Sleep_flay == true)&&(a_detection.A2_Sleep_flay == true)&&(system.Charge_For_Discharge == Discharge_State)){
#else
		if(((type_c.ADC_TYPE_C_Voltage < TYPE_C_SLEEP)||(STAT2 != true))
			&&(a_detection.ADC_A1_AD_Voltage < A_SLEEP)&&(a_detection.ADC_A2_AD_Voltage < A_SLEEP)&&(system.Charge_For_Discharge == Discharge_State)){
#endif
				system.System_sleep_countdown = true;				
			}else{
				system.System_sleep_countdown = false;
				system.System_sleep_countdown_cnt = false;
			}
		}
		if(qc_detection.Mode == Speed_mode){
			system.LED_Temporary_Init = LED4_OUT;
			LED4_Init_Judge();
			if(a_detection.ADC_A1_AD_Voltage > A1_overcurrent){
				//A1过流事件
				if(++a_detection.A1_overcurrent_cnt >= 20){
					a_detection.A1_overcurrent_cnt = false;
					system.System_State = System_Sleep;
				}
			}
			if(a_detection.ADC_A2_AD_Voltage > A2_overcurrent){
				//A2过流事件
				if(++a_detection.A2_overcurrent_cnt >= 20){
					a_detection.A2_overcurrent_cnt = false;
					system.System_State = System_Sleep;
				}
			}
		}else{//qc_detection.Mode == low_speed_mode
			system.LED_Temporary_Init = LED4_INPUT;
			LED4_Init_Judge();
			if(a_detection.ADC_A1_AD_Voltage > A2_overcurrent){
				//A1过流事件
				if(++a_detection.A1_overcurrent_cnt >= 20){
					a_detection.A1_overcurrent_cnt = false;
					system.System_State = System_Sleep;
				}
			}
			if(a_detection.ADC_A2_AD_Voltage > A2_overcurrent){
				//A2过流事件
				if(++a_detection.A2_overcurrent_cnt >= 20){
					a_detection.A2_overcurrent_cnt = false;
					system.System_State = System_Sleep;
				}
			}
		}

		if(type_c.ADC_TYPE_C_Voltage > TYPE_C_overcurrent){
			if(++type_c.C_overcurrent_cnt >= 20){
				type_c.C_overcurrent_cnt = false;
				system.System_State = System_Sleep;
			}
		}
	}
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void Adc_Task(void)
{
  if(adc.Flay_Adc_gather == true)
	{
    switch(adc.Adc_Query)
		{
      case 0: battery.Battery_voltage = Read_ADC(ADC_VB); adc.Adc_Query++; break;
			case 1: qc_detection.ADC_QC_Voltage = Read_ADC(ADC_QC); Fast_slow_charge_discharge_judge(); adc.Adc_Query++; break;
			case 2: a_detection.ADC_A1_AD_Voltage = Read_ADC(ADC_A1_AD); a_detection.ADC_A1_Gather_finish = true; A1_sleep_filter(); adc.Adc_Query++; break;
			case 3: a_detection.ADC_A2_AD_Voltage = Read_ADC(ADC_A2_AD); a_detection.ADC_A2_Gather_finish = true;	A2_sleep_filter(); adc.Adc_Query++; break;
			case 4:	if(STAT2 == true){type_c.ADC_TYPE_C_Voltage = Read_ADC(TYPE_AD);} TYPE_C_sleep_filter(); adc.Adc_Query=false; break;
			default:break;
    }
		adc.Flay_Adc_gather = false;
  }
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void Battery_Volume(void)
{
	if(battery.Battery_Level_Update == true){
#if BATTER_WARNING
		if((battery.Battery_voltage <= Battery_abnormal)&&(system.Charge_For_Discharge == Discharge_State)){
#else
		if(system.Charge_For_Discharge == Discharge_State){
#endif
			battery.Battery_warning = NORMAL;
			if(battery.Battery_voltage >= Battery_Level_4)
				battery.Battery_energy_buf = Quantity_Electricity_100;
			else{
				if(battery.Battery_voltage >= Battery_Level_3)
					battery.Battery_energy_buf = Quantity_Electricity_75;
				else{
					if(battery.Battery_voltage >= Battery_Level_2)
						battery.Battery_energy_buf = Quantity_Electricity_50;
					else{
						if(battery.Battery_voltage >= Battery_Level_1)
							battery.Battery_energy_buf = Quantity_Electricity_25;
						else{
								battery.Battery_energy_buf = Quantity_Electricity_5;
								if(battery.Battery_voltage < Battery_Level_0){
									if(system.Charge_For_Discharge == Discharge_State)
										battery.Batter_Low_Pressure = Batter_Low;
										system.System_State = System_Sleep;
							}
						}
					}
				}
			}
			if(battery.Battery_energy_buf	<= battery.Current_Display){
				battery.Current_Display = battery.Battery_energy_buf;
				system.NotifyLight_EN = true;
			}
		}
		else if(system.Charge_For_Discharge == Charge_State){
			if(battery.Battery_voltage >= Battery_charge_Level_4)
				battery.Battery_energy_buf = Quantity_Electricity_100;
			else{
				if(battery.Battery_voltage >= Battery_charge_Level_3)
					battery.Battery_energy_buf = Quantity_Electricity_75;
				else{
					if(battery.Battery_voltage >= Battery_charge_Level_2)
						battery.Battery_energy_buf = Quantity_Electricity_50;
					else{
						if(battery.Battery_voltage >= Battery_charge_Level_1)
							battery.Battery_energy_buf = Quantity_Electricity_25;
						else{
								battery.Battery_energy_buf = Quantity_Electricity_5;
						}
					}
				}
			}
			if(battery.Battery_energy_buf > battery.Current_Display){
				battery.Current_Display = battery.Battery_energy_buf;
				system.NotifyLight_EN = true;
			}
		}else{
			battery.Battery_warning = WARNING;
			system.NotifyLight_EN = false;
		}
		battery.Battery_Level_Update = false;
	}
}
