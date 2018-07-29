#ifndef __CONFIG_H
#define __CONFIG_H
#include "iostm8s003f3.h"


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
	C_DIR												PB4											input
	KEY													PA3											input
	STAT2												PC5											input/ti
	SEL													PD1
	QC													PC4											input/adc/AIN3
	A_AD2												PD2											input/adc/AIN5
	A_AD1												PD3											input/adc/AIN4
	C_AD												PD5											input/adc/AIN5
	VB													PD6											input/adc/AIN6
*/

/*!< Signed integer types  */
typedef   signed char     int8_t;
typedef   signed int    	int16_t;
typedef   signed long     int32_t;

/*!< Unsigned integer types  */
typedef unsigned char     uint8_t;
typedef unsigned int    	uint16_t;
typedef unsigned long     uint32_t;

#define true					1
#define false					0

#define LED1												PA_ODR_ODR1
#define LED2												PA_ODR_ODR2
#define LED3												PC_ODR_ODR7
#define LED4												PC_ODR_ODR6
#define A_EN												PD_ODR_ODR4
#define SEL													PD_ODR_ODR1
#define STAT2												PC_IDR_IDR5
#define C_DIR												PB_IDR_IDR4
#define PG													PC_IDR_IDR3
#define CE													PB_ODR_ODR5
#define KEY													PA_IDR_IDR3

#define ADC_Over                    0x80                  //ADC转换结束

#define DEBUG												0

#define ADC_GATHER_TIME																			50								//50 * 1ms = 50ms
#define SLEEP_TIME																					15000							//15000	* 1ms = 15S

#define Discharge_State																			true
#define Charge_State																				false

#define Batter_Low																					true
#define Batter_Normal																				false

#define System_Sleep                                        false
#define System_Run                                          true

#define ADC_VB                    													0x06                  //ADC6通道
#define ADC_QC																							0x02									//ADC2通道
#define ADC_A1_AD																						0x04									//ADC4通道
#define ADC_A2_AD																						0x03									//ADC3通道
#define TYPE_AD																							0x05									//ADC5通道
#if 1
#define Battery_Level_0                                     (uint16_t)0x1BD          //2.175V
#define Battery_Level_1																			(uint16_t)0x1CC          //2.250V
#define Battery_Level_2                                     (uint16_t)0x1FA          //2.475V
#define Battery_Level_3                                     (uint16_t)0x228          //2.700V
#define Battery_Level_4                                     (uint16_t)0x24D          //2.880V
#else
#define Battery_Level_0                                     (uint16_t)0x1BD          //2.175V
#define Battery_Level_1																			(uint16_t)0x1FA          //2.475V
#define Battery_Level_2                                     (uint16_t)0x228          //2.700V
#define Battery_Level_3                                     (uint16_t)0x24D          //2.880V
#endif
#define Quantity_Electricity_100														5
#define Quantity_Electricity_75															4
#define Quantity_Electricity_50															3
#define Quantity_Electricity_25															2
#define Quantity_Electricity_5															1
#define Quantity_Electricity_0															0

#define Speed_Voltage																				(uint16_t)0x147					//1.6V
#define low_speed_Voltage																		(uint16_t)0x133					//1.5V
#define Overload_event																			(uint16_t)0x66					//0.5V
#define Idle_Voltage																				(uint16_t)0x0E					//0.07V
#define Load_Voltage																				(uint16_t)0x0E					//0.06V
#define TYPE_C_VOLTAGE																			(uint16_t)0x02					//0.01V

#define MAX_VOLTAGE																					(uint16_t)0x270					//3.05V
#define TYPE_C_SLEEP																				(uint16_t)0x04					//0.02V
#define A_SLEEP																							(uint16_t)0x09					//0.045V
#define A1_overcurrent																			(uint16_t)0xB4					//0.88V
#define A2_overcurrent																			(uint16_t)0xD4					//1.04V

#define Speed_mode																					true
#define low_speed_mode																			false

#define Battery_Full																				true
#define Battery_Charge																			false

#define A1_LOAD_STATE																				true
#define A1_IDLE_STATE																				false

typedef struct{
	uint8_t System_State;
	uint8_t NotifyLight_EN;
	uint8_t Charge_For_Discharge;
	uint8_t System_sleep_countdown;
	uint16_t System_sleep_countdown_cnt;
}_System;

typedef struct{
	uint8_t Flay_Adc_gather;
	uint8_t Adc_gather_cnt;
}_ADC;

typedef struct{
	uint8_t Delay_Detection_Battery_full_status;
	
	
	uint8_t Battery_full_locking;
	uint8_t Battery_full_time_out;
	uint8_t Battery_State;
	uint16_t Battery_Full_Accumulative;	

	uint16_t Discharge_Batter_Low_blink;
	uint16_t Charge_Batter_Low_blink;
	
	uint8_t Charge_Current_buf;
	
	uint16_t Lndicator_light_cnt;
	uint8_t Batter_Low_Pressure;
	uint8_t Current_Display;
	uint8_t Battery_energy_buf;
	uint8_t Battery_Level_Update;
	uint16_t Battery_voltage;
	uint16_t Battery_Compensate;
}_Battery;

typedef struct{
	uint8_t Mode;
	uint8_t QC_Gather_finish;
	uint16_t ADC_QC_Voltage;
}_Qc_Detection;

typedef struct{
	uint8_t A1_overcurrent_cnt;
	uint8_t A2_overcurrent_cnt;
	uint8_t ADC_A1_Gather_finish;
	uint8_t ADC_A2_Gather_finish;
	uint16_t ADC_A1_AD_Voltage;
	uint16_t ADC_A2_AD_Voltage;
}_A_Detection;

typedef struct{
	uint16_t ADC_TYPE_C_Voltage;
}_TYPE_C;

typedef struct{
	uint8_t Forced_shutdown;
	uint8_t Key_Time_cnt;
	uint8_t time_10ms_ok;
	uint8_t key;
}_KEY;

extern _ADC adc;
extern _KEY key;
extern _TYPE_C type_c;
extern _System system;
extern _Battery battery;
extern _Qc_Detection qc_detection;
extern _A_Detection a_detection;
#endif
