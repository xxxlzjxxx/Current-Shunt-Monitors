#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK miniSTM32������
//������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 


//#define KEY0 PCin(5)   	
//#define KEY1 PAin(15)	 
//#define WK_UP  PAin(0)	 
 
#define KEY_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//��ȡ����2 
#define KEY_DOWN  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12)//��ȡ����1
#define KEY_LEFT  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)//��ȡ����0
#define KEY_RIGHT  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)//��ȡ����1

#define KEY_BACK  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)//��ȡ����0
#define KEY_MAIN  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2)//��ȡ����1
 

#define KEY_UP_PRES		1		//KEY0  
#define KEY_DOWN_PRES	2		//KEY1 
#define KEY_LEFT_PRES	3		//WK_UP
#define KEY_RIGHT_PRES	4		//KEY0 
#define KEY_BACK_PRES	5		//KEY0 
#define KEY_MAIN_PRES	6		//KEY0 

void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8 mode);  	//����ɨ�躯��					    
#endif
