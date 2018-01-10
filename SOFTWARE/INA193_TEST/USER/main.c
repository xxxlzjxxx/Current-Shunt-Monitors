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
//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		512
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);	
 			   
//LED0����
//�����������ȼ�
#define LED0_TASK_PRIO       			4 
//���������ջ��С
#define LED0_STK_SIZE  		    		128
//������ƿ�
OS_TCB Led0TaskTCB;
//�����ջ	
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
//������
void led0_task(void *pdata);


//ADC����
//�����������ȼ�
#define ADC_TASK_PRIO       			5 
//���������ջ��С
#define ADC_STK_SIZE  					128
//������ƿ�
OS_TCB ADCTaskTCB;
//�����ջ
CPU_STK ADC_TASK_STK[ADC_STK_SIZE];
//������
void ADC_task(void *pdata);

u16 adcx;
float WORK_CURR;
float WORK_VOLT;

int main(void)
{ 	
	OS_ERR err;
	CPU_SR_ALLOC();
	
 	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	TFTLCD_Init();			   	//��ʼ��LCD 	
	KEY_Init();				//������ʼ��	
	Adc_Init();		  		//ADC��ʼ��	 
	tp_dev.init();			//��������ʼ�� 

	OSInit(&err);		//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
				 
	while(1);
}

/**
* @brief ��ʼ����
* @param start_task
* @return 

* @details 
* @see 
*/
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	//����LED0����
	OSTaskCreate((OS_TCB 	* )&Led0TaskTCB,		
				 (CPU_CHAR	* )"led0 task", 		
                 (OS_TASK_PTR )led0_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED0_TASK_PRIO,     
                 (CPU_STK   * )&LED0_TASK_STK[0],	
                 (CPU_STK_SIZE)LED0_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED0_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				
				 
	//����LED1����
	OSTaskCreate((OS_TCB 	* )&ADCTaskTCB,		
				 (CPU_CHAR	* )"ADC task", 		
                 (OS_TASK_PTR )ADC_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )ADC_TASK_PRIO,     	
                 (CPU_STK   * )&ADC_TASK_STK[0],	
                 (CPU_STK_SIZE)ADC_STK_SIZE/10,	
                 (CPU_STK_SIZE)ADC_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	
			 								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}
/**
* @brief LED0����
* @param led0_task
* @return 

* @details 
* @see 
*/
void led0_task(void *p_arg){
	OS_ERR err;
	p_arg = p_arg;	
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
void ADC_task(void *p_arg){
	p_arg = p_arg;	
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
