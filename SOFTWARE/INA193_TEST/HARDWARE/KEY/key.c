#include "key.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//�������� ��������		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/06
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									   
//////////////////////////////////////////////////////////////////////////////////	 
 	    
//������ʼ������ 
//PA0.15��PC5 ���ó�����
void KEY_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD,ENABLE);//ʹ��PORTA,PORTCʱ��

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//�ر�jtag��ʹ��SWD��������SWDģʽ����
	 
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.0
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15 | GPIO_Pin_12;//PA15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA15	
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5 | GPIO_Pin_4;//PC5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC5
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;//PC5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIOC5
} 
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//����ֵ��
//0��û���κΰ�������
//KEY0_PRES��KEY0����
//KEY1_PRES��KEY1����
//WKUP_PRES��WK_UP���� 
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>WK_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode){
		key_up=1;  //֧������	
	}		
	if(key_up&&(KEY_UP==1||KEY_DOWN==0||KEY_LEFT==0||KEY_RIGHT==0||KEY_BACK==0||KEY_MAIN==0)){
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY_DOWN==0){
			return KEY_DOWN_PRES;
		}
		else if(KEY_LEFT==0){
			return KEY_LEFT_PRES;
		}	
		else if(KEY_RIGHT==0){
			return KEY_RIGHT_PRES;
		}
		else if(KEY_BACK==0){
			return KEY_BACK_PRES;
		}
		else if(KEY_MAIN==0){
			return KEY_MAIN_PRES;
		}		
		else if(KEY_UP==1){
			return KEY_UP_PRES;
		}			
	}
	else if(KEY_DOWN==1&&KEY_LEFT==1&&KEY_RIGHT==1&&KEY_BACK==1&&KEY_MAIN==1&&KEY_UP==0){
		key_up=1; 
	}		
	return 0;// �ް�������
}
