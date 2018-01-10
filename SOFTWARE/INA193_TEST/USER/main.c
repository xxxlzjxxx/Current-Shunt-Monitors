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
//ALIENTEK Mini STM32开发板范例代码21
//触摸屏实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
 

/////////////////////////UCOSII任务设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);	
 			   
//LED0任务
//设置任务优先级
#define LED0_TASK_PRIO       			7 
//设置任务堆栈大小
#define LED0_STK_SIZE  		    		64
//任务堆栈	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//任务函数
void led0_task(void *pdata);


//ADC任务
//设置任务优先级
#define ADC_TASK_PRIO       			6 
//设置任务堆栈大小
#define ADC_STK_SIZE  					64
//任务堆栈
OS_STK ADC_TASK_STK[ADC_STK_SIZE];
//任务函数
void Adc_task(void *pdata);

u16 adcx;
float WORK_CURR;
float WORK_VOLT;

int main(void)
{ 	

	
 	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为9600
	LED_Init();		  		//初始化与LED连接的硬件接口
	TFTLCD_Init();			   	//初始化LCD 	
	KEY_Init();				//按键初始化	
	Adc_Init();		  		//ADC初始化	 
	tp_dev.init();			//触摸屏初始化 

	OSInit();
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();	
}

/**
* @brief 开始任务
* @param start_task
* @return 

* @details 
* @see 
*/
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
  	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
 	OSTaskCreate(Adc_task,(void *)0,(OS_STK*)&ADC_TASK_STK[ADC_STK_SIZE-1],ADC_TASK_PRIO);	 				   
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}
/**
* @brief LED0任务
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
* @brief ADC任务
* @param Adc_task
* @return 

* @details 
* @see 
*/
void Adc_task(void *pdata){	 	
	while(1){
		adcx=Get_Adc_Average(ADC_Channel_1,10);
		LCD_ShowxNum(156,130,adcx,4,16,0);//显示ADC的值
		WORK_VOLT=(float)adcx*(3.3/4096)*11;
		adcx=WORK_VOLT;
		LCD_ShowxNum(140,150,adcx,2,16,0);//显示电压值
		WORK_VOLT-=adcx;
		WORK_VOLT*=1000;
		LCD_ShowxNum(164,150,WORK_CURR,3,16,0X80);
		
		adcx=Get_Adc_Average(ADC_Channel_2,10);
		LCD_ShowxNum(156,170,adcx,4,16,0);//显示ADC的值
		WORK_CURR=(float)adcx*(3.3/4096)/2;
		adcx=WORK_CURR;
		LCD_ShowxNum(140,190,adcx,2,16,0);//显示电压值
		WORK_CURR-=adcx;
		WORK_CURR*=1000;
		LCD_ShowxNum(164,190,WORK_CURR,3,16,0X80);
		printf(">>ADC TASK.\r\n");
	};
}
