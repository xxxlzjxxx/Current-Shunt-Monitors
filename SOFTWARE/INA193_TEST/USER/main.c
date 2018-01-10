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
//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		512
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);	
 			   
//LED0任务
//设置任务优先级
#define LED0_TASK_PRIO       			4 
//设置任务堆栈大小
#define LED0_STK_SIZE  		    		128
//任务控制块
OS_TCB Led0TaskTCB;
//任务堆栈	
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
//任务函数
void led0_task(void *pdata);


//ADC任务
//设置任务优先级
#define ADC_TASK_PRIO       			5 
//设置任务堆栈大小
#define ADC_STK_SIZE  					128
//任务控制块
OS_TCB ADCTaskTCB;
//任务堆栈
CPU_STK ADC_TASK_STK[ADC_STK_SIZE];
//任务函数
void ADC_task(void *pdata);

u16 adcx;
float WORK_CURR;
float WORK_VOLT;

int main(void)
{ 	
	OS_ERR err;
	CPU_SR_ALLOC();
	
 	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为9600
	LED_Init();		  		//初始化与LED连接的硬件接口
	TFTLCD_Init();			   	//初始化LCD 	
	KEY_Init();				//按键初始化	
	Adc_Init();		  		//ADC初始化	 
	tp_dev.init();			//触摸屏初始化 

	OSInit(&err);		//初始化UCOSIII
	OS_CRITICAL_ENTER();//进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);  //开启UCOSIII
				 
	while(1);
}

/**
* @brief 开始任务
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
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//进入临界区
	//创建LED0任务
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
				 
	//创建LED1任务
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
			 								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//进入临界区
}
/**
* @brief LED0任务
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
* @brief ADC任务
* @param Adc_task
* @return 

* @details 
* @see 
*/
void ADC_task(void *p_arg){
	p_arg = p_arg;	
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
