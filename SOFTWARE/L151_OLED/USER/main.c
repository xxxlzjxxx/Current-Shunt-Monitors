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
#include "usmart.h"
#include "rtc.h"

u16 adcx;

void oled_task(void);

int main(void)
{	
 	u8 t = 0;
	u16 i = 199;
 
	delay_init();	     //延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	uart_init(115200);	 	//串口初始化为115200
	IIC_Init();
	SHT_Init();
	OLED_Init();			//初始化OLED
	usmart_dev.init(72);	//初始化USMART	
//	OLED_ShowString(0,0,"ALIENTEK",24);  
//	OLED_ShowString(0,24, "0.96' OLED TEST",16);  
// 	OLED_ShowString(0,40,"ATOM 2014/3/7",12);  
// 	OLED_ShowString(0,52,"ASCII:",12);  
// 	OLED_ShowString(64,52,"CODE:",12);  
//	OLED_Refresh_Gram();//更新显示到OLED
	while(RTC_Init()){ 
		OLED_ShowString(0,0,"RTC ERROR!   ",12);	
		delay_ms(1000);
		OLED_ShowString(0,0,"RTC Trying...",12);
		delay_ms(500);
	}	
///显示时间
	OLED_ShowString(0,0,"    -  -  ",12);
	OLED_ShowString(32,16,"  :  :  ",16);
	OLED_Refresh_Gram();//更新显示到OLED
	while(1){		  
		i++;	
		if(t!=calendar.sec){
			t=calendar.sec;
			OLED_ShowNum(0,0,calendar.w_year,4,12);									  
			OLED_ShowNum(30,0,calendar.w_month,2,12);									  
			OLED_ShowNum(48,0,calendar.w_date,2,12);	 
			switch(calendar.week){
				case 0:
					OLED_ShowString(98,0,"Sun. ",12);
					break;
				case 1:
					OLED_ShowString(98,0,"Mon. ",12);
					break;
				case 2:
					OLED_ShowString(98,0,"Tues.",12);
					break;
				case 3:
					OLED_ShowString(98,0,"Wed. ",12);
					break;
				case 4:
					OLED_ShowString(98,0,"Thur.",12);
					break;
				case 5:
					OLED_ShowString(98,0,"Fri. ",12);
					break;
				case 6:
					OLED_ShowString(98,0,"Sat. ",12);
					break;  
			}
			OLED_ShowNum(32,16,calendar.hour,2,16);									  
			OLED_ShowNum(56,16,calendar.min,2,16);									  
			OLED_ShowNum(80,16,calendar.sec,2,16);
			OLED_Refresh_Gram();//更新显示到OLED
		}
		delay_ms(10);
		while(i == 200){
			i = 0;
			oled_task();
		}
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
///温度数据校准	
	Tem_Value = Tem_Value - 5;
///串口打印当前数据
	printf("tem:%.2f C    ",Tem_Value);
	printf("rh:%.2f RH\r\n",RH_Value);
	OLED_ShowString(0,38,"Temp:",12);
	OLED_ShowString(0,50,"Humi:",12);
	OLED_ShowString(96,38,"  C",12);
	OLED_ShowString(96,50," RH",12);
	
	
	adcx = Tem_Value;
	OLED_ShowNum(40,38,adcx,3,12);	//显示ASCII字符的码值
	Tem_Value-=adcx;
	Tem_Value*=1000;
	OLED_ShowChar(64,38,'.',12,1);//显示ASCII字符	
	OLED_ShowNum(72,38,Tem_Value,3,12);	//显示ASCII字符的码值
	
	adcx = RH_Value;
	OLED_ShowNum(40,50,adcx,3,12);	//显示ASCII字符的码值
	RH_Value-=adcx;
	RH_Value*=1000;
	OLED_ShowChar(64,50,'.',12,1);//显示ASCII字符	
	OLED_ShowNum(72,50,RH_Value,3,12);	//显示ASCII字符的码值
	
	OLED_Refresh_Gram();//更新显示到OLED
}



