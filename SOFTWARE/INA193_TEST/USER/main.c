#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "tftlcd.h"
#include "usart.h"	 
#include "24cxx.h"
#include "flash.h"
#include "touch.h"
#include "sram.h"
#include "adc.h"
#include "timer.h"
#include "malloc.h"
#include "includes.h"
//#include "GUI.h"
//#include "GUIDemo.h"
//ALIENTEK Mini STM32�����巶������21
//������ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
 

/////////////////////////UCOSII��������///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);	
 			   
//LED0����
//�����������ȼ�
#define LED0_TASK_PRIO       			7 
//���������ջ��С
#define LED0_STK_SIZE  		    		64
//�����ջ	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//������
void led0_task(void *pdata);


//ADC����
//�����������ȼ�
#define ADC_TASK_PRIO       			6 
//���������ջ��С
#define ADC_STK_SIZE  					64
//�����ջ
OS_STK ADC_TASK_STK[ADC_STK_SIZE];
//������
void Adc_task(void *pdata);

u16 adcx;
float WORK_CURR;
float WORK_VOLT;

int main(void)
{ 	

	
 	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	TFTLCD_Init();			   	//��ʼ��LCD 	
	KEY_Init();				//������ʼ��	
	Adc_Init();		  		//ADC��ʼ��	 
	tp_dev.init();			//��������ʼ�� 

	OSInit();
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();	
}

/**
* @brief ��ʼ����
* @param start_task
* @return 

* @details 
* @see 
*/
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
  	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
 	OSTaskCreate(Adc_task,(void *)0,(OS_STK*)&ADC_TASK_STK[ADC_STK_SIZE-1],ADC_TASK_PRIO);	 				   
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}
/**
* @brief LED0����
* @param led0_task
* @return 

* @details 
* @see 
*/
void led0_task(void *pdata){	 	
	while(1){
		LED0=0;
		delay_ms(80);
		LED0=1;
		delay_ms(920);
		printf(">>LED0 TASK.\r\n");
	};
}
/**
* @brief ADC����
* @param Adc_task
* @return 

* @details 
* @see 
*/
void Adc_task(void *pdata){	 	
	while(1){
		adcx=Get_Adc_Average(ADC_Channel_1,10);
		LCD_ShowxNum(156,130,adcx,4,16,0);//��ʾADC��ֵ
		WORK_VOLT=(float)adcx*(3.3/4096)*11;
		adcx=WORK_VOLT;
		LCD_ShowxNum(140,150,adcx,2,16,0);//��ʾ��ѹֵ
		WORK_VOLT-=adcx;
		WORK_VOLT*=1000;
		LCD_ShowxNum(164,150,WORK_CURR,3,16,0X80);
		
		adcx=Get_Adc_Average(ADC_Channel_2,10);
		LCD_ShowxNum(156,170,adcx,4,16,0);//��ʾADC��ֵ
		WORK_CURR=(float)adcx*(3.3/4096)/2;
		adcx=WORK_CURR;
		LCD_ShowxNum(140,190,adcx,2,16,0);//��ʾ��ѹֵ
		WORK_CURR-=adcx;
		WORK_CURR*=1000;
		LCD_ShowxNum(164,190,WORK_CURR,3,16,0X80);
		printf(">>ADC TASK.\r\n");
	};
}
