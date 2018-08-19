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
		type_c.No_load_cnt--;
	}
	else{
		type_c.No_load_cnt++;	
	}
	if(type_c.No_load_cnt >= 100){
		type_c.Sleep_flay = true;
		type_c.No_load_cnt = 50;
	}else if(type_c.No_load_cnt < 0){
		type_c.No_load_cnt = false;
		type_c.Sleep_flay = false;
	}
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void A1_sleep_filter(void)
{
#if _A_QC_COMPENSATE
	if(qc_detection.Mode == Speed_mode){
		a_detection.A1_AD_Compensate = 0x02;
	}else{
		a_detection.A1_AD_Compensate = false;
	}
#endif
	if((a_detection.ADC_A1_AD_Voltage + a_detection.A1_AD_Compensate)  > A_load){
		a_detection.A1_No_load_cnt--;
	}
	else{
		a_detection.A1_No_load_cnt++;
	}
	if(a_detection.A1_No_load_cnt >= 100){
		a_detection.A1_Sleep_flay = true;
		a_detection.A1_No_load_cnt = 50;
	}else if(a_detection.A1_No_load_cnt < 0){
		a_detection.A1_Sleep_flay = false;
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
		a_detection.A2_No_load_cnt--;
	}
	else{
		a_detection.A2_No_load_cnt++;	
	}
	if(a_detection.A2_No_load_cnt >= 100){
		a_detection.A2_Sleep_flay = true;
		a_detection.A2_No_load_cnt = 50;
	}else if(a_detection.A2_No_load_cnt < 0){
		a_detection.A2_No_load_cnt = false;
		a_detection.A2_Sleep_flay = false;
	}
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void Speed_mode_Current_monitoring(void)
{
	LED4 = true;
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
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void low_speed_mode_Current_monitoring(void)
{
	LED4 = false;
	if(a_detection.ADC_A1_AD_Voltage > A1_overcurrent_low_speed){
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
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void PD_led_show(void)
{
	if(qc_detection.Mode == Speed_mode){
		LED4 = true;
	}else{//qc_detection.Mode == low_speed_mode
		LED4 = false;
	}
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void Charge_mode_restriction(void)
{
	if(battery.Current_Display >= Quantity_Electricity_50){
		A_EN = true;
	}else if(battery.Current_Display < Quantity_Electricity_5){
		A_EN = false;
	}
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void A_load_monitoring(void)
{
	if(a_detection.A_out_status == true){
#if 0
		if(((type_c.Sleep_flay == false)||(STAT2 != true))
			&&(a_detection.A1_Sleep_flay == true)&&(a_detection.A2_Sleep_flay == true)&&(system.Charge_For_Discharge == Discharge_State)){
			a_detection.A_load_status = A_NO_LOAD_STATUS;
#else
		if((a_detection.A1_Sleep_flay == true)&&(a_detection.A2_Sleep_flay == true)){
			a_detection.A_load_status = A_NO_LOAD_STATUS;
#endif
		}else{
			a_detection.A_load_status = A_LOAD_STATUS;
			a_detection.A_out_disable_countdown_cnt = false;
			A_EN = true;
		}
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
		if((battery.Battery_voltage > MAX_VOLTAGE) && (PG == true)){
			battery.Battery_full_time_out = true;
			if(battery.Battery_full_locking == true){
				battery.Battery_State = Battery_Full;
			}
		}else{
			battery.Battery_State = Battery_Charge;
		}
		PD_led_show();
		A_load_monitoring();
//		Charge_mode_restriction();
		type_c.C_overcurrent_cnt = false;
		a_detection.A1_overcurrent_cnt = false;
		a_detection.A2_overcurrent_cnt = false;
	}else{//system.Charge_For_Discharge == Discharge_State
		if((qc_detection.QC_Gather_finish==true)&&(a_detection.ADC_A1_Gather_finish==true)
			&&(a_detection.ADC_A2_Gather_finish==true)){
#if 1
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
				A_load_monitoring();
			}
		}
		if(qc_detection.Mode == Speed_mode){
			Speed_mode_Current_monitoring();
		}else{//qc_detection.Mode == low_speed_mode
			low_speed_mode_Current_monitoring();
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
void Discharge_Curves(void)
{
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
									if(battery.Batter_Low_Filtration == true){
									battery.Batter_Low_Pressure = Batter_Low;
									system.System_State = System_Sleep;
								}
					}
				}
			}
		}
	}
	if(battery.Battery_energy_buf	<= battery.Current_Display){
		battery.Current_Display = battery.Battery_energy_buf;
		system.NotifyLight_EN = true;
		battery.Batter_Low_Filtration = true;
	}
}
/**
  * @brief  None
  * @param  None
  * @retval None
  */
void Charge_Curves(void)
{
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
			Discharge_Curves();
		}
		else if(system.Charge_For_Discharge == Charge_State){
			Charge_Curves();
			battery.Batter_Low_Filtration = true;
			battery.Batter_Low_Pressure = Batter_Normal;
		}else{
			battery.Battery_warning = WARNING;
			system.NotifyLight_EN = false;
		}
		battery.Battery_Level_Update = false;
	}
}
