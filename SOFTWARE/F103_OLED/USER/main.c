/*******************************************************************************
* @file     main.c                                                               
* @brief    ������                                                   
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
// 	u8 t = 0;
//	u16 i = 99;
 
	delay_init();	     //��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	IIC_Init();
	SHT_Init();
	OLED_Init();			//��ʼ��OLED
	usmart_dev.init(72);	//��ʼ��USMART	
//	OLED_ShowString(0,0,"ALIENTEK",24);  
//	OLED_ShowString(0,24, "0.96' OLED TEST",16);  
// 	OLED_ShowString(0,40,"ATOM 2014/3/7",12);  
// 	OLED_ShowString(0,52,"ASCII:",12);  
// 	OLED_ShowString(64,52,"CODE:",12);  
//	OLED_Refresh_Gram();//������ʾ��OLED
//	while(RTC_Init()){ 
//		OLED_ShowString(0,0,"RTC ERROR!   ",12);	
//		delay_ms(1000);
//		OLED_ShowString(0,0,"RTC Trying...",12);
//		delay_ms(500);
//	}	
///��ʾʱ��
//	OLED_ShowString(0,0,"    -  -  ",12);
//	OLED_ShowString(32,16,"  :  :  ",16);
//	OLED_Refresh_Gram();//������ʾ��OLED
	while(1){		  	
//		if(t!=calendar.sec){
//			t=calendar.sec;
//			OLED_ShowNum(0,0,calendar.w_year,4,12);									  
//			OLED_ShowNum(30,0,calendar.w_month,2,12);									  
//			OLED_ShowNum(48,0,calendar.w_date,2,12);	 
//			switch(calendar.week){
//				case 0:
//					OLED_ShowString(98,0,"Sun. ",12);
//					break;
//				case 1:
//					OLED_ShowString(98,0,"Mon. ",12);
//					break;
//				case 2:
//					OLED_ShowString(98,0,"Tues.",12);
//					break;
//				case 3:
//					OLED_ShowString(98,0,"Wed. ",12);
//					break;
//				case 4:
//					OLED_ShowString(98,0,"Thur.",12);
//					break;
//				case 5:
//					OLED_ShowString(98,0,"Fri. ",12);
//					break;
//				case 6:
//					OLED_ShowString(98,0,"Sat. ",12);
//					break;  
//			}
//			OLED_ShowNum(32,16,calendar.hour,2,16);									  
//			OLED_ShowNum(56,16,calendar.min,2,16);									  
//			OLED_ShowNum(80,16,calendar.sec,2,16);
//			OLED_Refresh_Gram();//������ʾ��OLED
//		}
		delay_ms(1000);

		oled_task();
	}	  
}
/*******************************************************************************
* @param		����
* @brief		ժҪ
* @arg			�б�˵������
* @return       ����ֵ˵��
* @retval		����ֵ����˵��
* @see			�ο�
* @attention	ע��
* @note			ע��	
* @author   	����                                                                                                       *                                                            
* @date     	����            				
*******************************************************************************/
void oled_task(void){	
	SHT_GetValue();
///�¶�����У׼	
	Tem_Value = Tem_Value - 5;
///���ڴ�ӡ��ǰ����
	printf("tem:%.2f C    ",Tem_Value);
	printf("rh:%.2f RH\r\n",RH_Value);
	OLED_ShowString(0,8,"Temp:",16);
	OLED_ShowString(0,32,"Humi:",16);
	OLED_ShowString(104,8," C",16);
	OLED_ShowString(104,32,"%RH",16);
	
	
	adcx = Tem_Value;
	OLED_ShowNum(42,8,adcx,3,16);	//��ʾASCII�ַ�����ֵ
	Tem_Value-=adcx;
	Tem_Value*=1000;
	OLED_ShowChar(66,8,'.',16,1);//��ʾASCII�ַ�	
	OLED_ShowNum(74,8,Tem_Value,3,16);	//��ʾASCII�ַ�����ֵ
	
	adcx = RH_Value;
	OLED_ShowNum(42,32,adcx,3,16);	//��ʾASCII�ַ�����ֵ
	RH_Value-=adcx;
	RH_Value*=1000;
	OLED_ShowChar(66,32,'.',16,1);//��ʾASCII�ַ�	
	OLED_ShowNum(74,32,RH_Value,3,16);	//��ʾASCII�ַ�����ֵ
	
	OLED_Refresh_Gram();//������ʾ��OLED
}



