/*******************************************************************************
* @file     main.c                                                               
* @brief    主函数                                                   
* @author   xxxlzjxxx                                                                                                      *
* @version  V1.0                                                              
* @date     2018-08-02                                                        
* @license  GNU General Public License (GPL)                                                                                                               
*******************************************************************************/
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "oled.h"
#include "TemAndHum.h"

u16 adcx;

void oled_task(void);

int main(void)
{	
 	u8 t=0;	
 
	delay_init();	     //延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	uart_init(115200);	 	//串口初始化为115200
	IIC_Init();
	SHT_Init();
	OLED_Init();			//初始化OLED     
//	OLED_ShowString(0,0,"ALIENTEK",24);  
//	OLED_ShowString(0,24, "0.96' OLED TEST",16);  
// 	OLED_ShowString(0,40,"ATOM 2014/3/7",12);  
// 	OLED_ShowString(0,52,"ASCII:",12);  
// 	OLED_ShowString(64,52,"CODE:",12);  
//	OLED_Refresh_Gram();//更新显示到OLED	   
	while(1) 
	{		
//		OLED_ShowChar(36,52,t,12,1);//显示ASCII字符	
//		OLED_ShowNum(94,52,t,3,12);	//显示ASCII字符的码值    
//		OLED_Refresh_Gram();//更新显示到OLED
//		t++;
//		if(t>'~')t=' ';  
		delay_ms(1000);
		oled_task();
	}	  
}
/*******************************************************************************
* @param		参数
* @brief		摘要
* @arg			列表说明参数
* @return       返回值说明
* @retval		返回值类型说明
* @see			参看
* @attention	注意
* @note			注解	
* @author   	作者                                                                                                       *                                                            
* @date     	日期            				
*******************************************************************************/
void oled_task(void){	
	SHT_GetValue();
	///串口打印当前数据
	printf("tem:%.2f C    ",Tem_Value);
	printf("rh:%.2f RH\r\n",RH_Value);
	OLED_ShowString(0,0,"SHT30 TEST",24);
	OLED_ShowString(0,24,"Temp:",16);
	OLED_ShowString(0,40,"Humi:",16);
	OLED_ShowString(96,24,"  C",16);
	OLED_ShowString(96,40," RH",16);
	
	
	adcx = Tem_Value;
	OLED_ShowNum(40,24,adcx,3,16);	//显示ASCII字符的码值
	Tem_Value-=adcx;
	Tem_Value*=1000;
	OLED_ShowChar(64,24,'.',16,1);//显示ASCII字符	
	OLED_ShowNum(72,24,Tem_Value,3,16);	//显示ASCII字符的码值
	
	adcx = RH_Value;
	OLED_ShowNum(40,40,adcx,3,16);	//显示ASCII字符的码值
	RH_Value-=adcx;
	RH_Value*=1000;
	OLED_ShowChar(64,40,'.',16,1);//显示ASCII字符	
	OLED_ShowNum(72,40,RH_Value,3,16);	//显示ASCII字符的码值
	
	OLED_Refresh_Gram();//更新显示到OLED
}



