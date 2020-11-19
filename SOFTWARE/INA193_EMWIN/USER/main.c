#include "led.h"
#include "delay.h"
#include "key.h"
#include "adc.h"
#include "sys.h"
#include "ILI93xx.h"
#include "usart.h"	 
#include "24cxx.h"
#include "flash.h"
#include "touch.h"
#include "timer.h"
#include "malloc.h"
#include "GUI.h"
#include "includes.h"
#include "GUIDemo.h"
/************************************************
 ALIENTEK MiniSTM32������STemWinʵ��
 STemWin+UCOSIII��ֲ
 
 UCOSIII���������ȼ��û�������ʹ�ã�ALIENTEK
 ����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
 ���ȼ�0���жϷ������������� OS_IntQTask()
 ���ȼ�1��ʱ�ӽ������� OS_TickTask()
 ���ȼ�2����ʱ���� OS_TmrTask()
 ���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
 ���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
//�������ȼ�
#define START_TASK_PRIO				3
//�����ջ��С	
#define START_STK_SIZE 				128
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//TOUCH����
//�����������ȼ�
#define TOUCH_TASK_PRIO				4
//�����ջ��С
#define TOUCH_STK_SIZE				128
//������ƿ�
OS_TCB TouchTaskTCB;
//�����ջ
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//touch����
void touch_task(void *p_arg);

//LED0����
//�����������ȼ�
#define LED0_TASK_PRIO 				5
//�����ջ��С
#define LED0_STK_SIZE				128
//������ƿ�
OS_TCB Led0TaskTCB;
//�����ջ
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
//led0����
void led0_task(void *p_arg);

//ADC����
//�����������ȼ�
#define ADC_TASK_PRIO       			5 
//���������ջ��С
#define ADC_STK_SIZE  					128       
//������ƿ�
OS_TCB ADCTaskTCB;
//�����ջ
__align(8) CPU_STK ADC_TASK_STK[ADC_STK_SIZE];
//������
void ADC_task(void *pdata);

u16 adcx;
float WORK_CURR;
float WORK_VOLT;
float WORK_WATT;

int main(void)
{	 
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init();	    	//��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();			    //LED�˿ڳ�ʼ��
    Adc_Init();		  		//ADC��ʼ��
	TFTLCD_Init();			//LCD��ʼ��	
	KEY_Init();	 			//������ʼ��
	mem_init();				//��ʼ���ڲ��ڴ��
//	TP_Init();				//��������ʼ��
	
	OSInit(&err);			//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();	//�����ٽ���
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

//��ʼ������
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
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//ʹ��CRCʱ�ӣ�����STemWin����ʹ�� 
	WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();  			//STemWin��ʼ��
    
	OS_CRITICAL_ENTER();	//�����ٽ���
	//ADC����	
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
	//����������
	OSTaskCreate((OS_TCB*     )&TouchTaskTCB,		
				 (CPU_CHAR*   )"Touch task", 		
                 (OS_TASK_PTR )touch_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )TOUCH_TASK_PRIO,     
                 (CPU_STK*    )&TOUCH_TASK_STK[0],	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE/10,	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);			 
	//LED0����
	OSTaskCreate((OS_TCB*     )&Led0TaskTCB,		
				 (CPU_CHAR*   )"Led0 task", 		
                 (OS_TASK_PTR )led0_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )LED0_TASK_PRIO,     
                 (CPU_STK*    )&LED0_TASK_STK[0],	
                 (CPU_STK_SIZE)LED0_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED0_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
}


/**
* @brief ADC����
* @param Adc_task
* @return 

* @details 
* @see 
*/
void ADC_task(void *p_arg){
	char aa[6];
    uint16_t i =  0;
    p_arg = p_arg;	

	GUI_SetBkColor(GUI_WHITE);
    GUI_Clear();
    
    GUI_SetFont(&GUI_Font16_ASCII); //��������
	GUI_SetPenSize(5);				//���ñʴ�С
	GUI_SetColor(GUI_BLACK);			//��ɫ����
	GUI_DrawLine(0,210,320,210);	//����
	GUI_SetColor(GUI_BLACK);       //����ǰ��ɫ(���ı������ߵ���ɫ)
    GUI_DispStringAt("VOLT:",   0, 220);
    GUI_DispStringAt("V",      88, 220);
    GUI_DispStringAt("CURR:", 108, 220);
    GUI_DispStringAt("A",     196, 220);
    GUI_DispStringAt("WATT:", 216, 220);
    GUI_DispStringAt("W",     304, 220);    
    
//    LCD_ShowString(4,220,40,16,16,"VOLT:");
//    LCD_ShowString(92,220,8,16,16,"V");
//    LCD_ShowString(112,220,40,16,16,"CURR:");
//    LCD_ShowString(200,220,8,16,16,"A");
//    LCD_ShowString(220,220,40,16,16,"WATT:");
//    LCD_ShowString(308,220,8,16,16,"W");
    
	while(1){
        GUI_SetPenSize(3);				//���ñʴ�С
        i++;
        
        GUI_SetColor(GUI_ORANGE);       //����ǰ��ɫ(���ı������ߵ���ɫ)
		adcx=Get_Adc_Average(ADC_Channel_1,20);
//        GUI_DispDecAt(adcx, 44 ,204 ,4);    
		WORK_VOLT=(float)adcx*(3.288/4096)*11;//ʵ�ʵ�ѹֵ
        sprintf(aa,"%2.3f",WORK_VOLT);//����������תΪָ����ʽ���ַ���
        GUI_DispStringAt(aa, 44, 220); 
        GUI_DrawPoint(i, 200 - (int)(WORK_VOLT * 200 / 24));

        GUI_SetColor(GUI_LIGHTBLUE);       //����ǰ��ɫ(���ı������ߵ���ɫ)
		adcx=Get_Adc_Average(ADC_Channel_2,20);
//        GUI_DispDecAt(adcx, 152 ,204 ,4);
        if(adcx <= 8){
            adcx = 0;
        }
		WORK_CURR=(float)adcx*(3.288/4096);
        sprintf(aa,"%2.3f",WORK_CURR);//����������תΪָ����ʽ���ַ���
        GUI_DispStringAt(aa, 152, 220);
        GUI_DrawPoint(i, 200 - (int)((WORK_CURR / 2) * 200));
        
        GUI_SetColor(GUI_LIGHTGREEN);       //����ǰ��ɫ(���ı������ߵ���ɫ)
		WORK_WATT=WORK_VOLT * WORK_CURR;
        sprintf(aa,"%2.3f",WORK_WATT);//����������תΪָ����ʽ���ַ���
        GUI_DispStringAt(aa, 260, 220);
        GUI_DrawPoint(i, 200 - (int)((WORK_WATT / 48 ) * 200));

        if(i == 320){
            i = 0;
            GUI_ClearRect(0,0,320,205);
        }
////		printf(">>ADC TASK.\r\n");
	}
}

//TOUCH����
void touch_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		GUI_TOUCH_Exec();	
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//��ʱ5ms
	}
}

//LED0����
void led0_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//��ʱ500ms
	}
}




