#include "delay.h"

/*================================================================================
函数功能：us延时
输入参数：无
输出参数：无
备    注：粗略延时
================================================================================*/
#if 1
void delay_us(void)
{ 
    asm("nop"); //一个asm("nop")函数经过示波器测试代表100ns
    asm("nop");
    asm("nop");
    asm("nop"); 
}
#endif
/*================================================================================
函数功能：ms延时
输入参数：无
输出参数：无
备    注：粗略延时
================================================================================*/
#if 1
void delay_ms(unsigned int time)
{
    unsigned int i;
    while(time--)  
    for(i=900;i>0;i--)
    delay_us(); 
}
#endif
/******************************************************************************/
/* Function name:       Delay_ms                                              */
/* Descriptions:        毫秒级延时函数函数，16MHz时钟                         */
/* input parameters:    nCount延时时间                                        */
/* output parameters:   无                                                    */
/* Returned value:      无                                                    */
/******************************************************************************/
#if 0
void delay_ms(unsigned int nCount)
{ 
  uint16_t i=0,j=0;
  for(i=0;i<nCount;i++)
  {
  for(j=0;j<1142;j++)
  {;}
  }
}
#endif
