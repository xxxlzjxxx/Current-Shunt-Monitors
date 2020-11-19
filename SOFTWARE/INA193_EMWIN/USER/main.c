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
 ALIENTEK MiniSTM32开发板STemWin实验
 STemWin+UCOSIII移植
 
 UCOSIII中以下优先级用户程序不能使用，ALIENTEK
 将这些优先级分配给了UCOSIII的5个系统内部任务
 优先级0：中断服务服务管理任务 OS_IntQTask()
 优先级1：时钟节拍任务 OS_TickTask()
 优先级2：定时任务 OS_TmrTask()
 优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
 优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
//任务优先级
#define START_TASK_PRIO				3
//任务堆栈大小	
#define START_STK_SIZE 				128
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//TOUCH任务
//设置任务优先级
#define TOUCH_TASK_PRIO				4
//任务堆栈大小
#define TOUCH_STK_SIZE				128
//任务控制块
OS_TCB TouchTaskTCB;
//任务堆栈
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//touch任务
void touch_task(void *p_arg);

//LED0任务
//设置任务优先级
#define LED0_TASK_PRIO 				5
//任务堆栈大小
#define LED0_STK_SIZE				128
//任务控制块
OS_TCB Led0TaskTCB;
//任务堆栈
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
//led0任务
void led0_task(void *p_arg);

//ADC任务
//设置任务优先级
#define ADC_TASK_PRIO       			5 
//设置任务堆栈大小
#define ADC_STK_SIZE  					128       
//任务控制块
OS_TCB ADCTaskTCB;
//任务堆栈
__align(8) CPU_STK ADC_TASK_STK[ADC_STK_SIZE];
//任务函数
void ADC_task(void *pdata);

u16 adcx;
float WORK_CURR;
float WORK_VOLT;
float WORK_WATT;

int main(void)
{	 
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init();	    	//延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();			    //LED端口初始化
    Adc_Init();		  		//ADC初始化
	TFTLCD_Init();			//LCD初始化	
	KEY_Init();	 			//按键初始化
	mem_init();				//初始化内部内存池
//	TP_Init();				//触摸屏初始化
	
	OSInit(&err);			//初始化UCOSIII
	OS_CRITICAL_ENTER();	//进入临界区
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

//开始任务函数
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
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//使能CRC时钟，否则STemWin不能使用 
	WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();  			//STemWin初始化
    
	OS_CRITICAL_ENTER();	//进入临界区
	//ADC任务	
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
	//触摸屏任务
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
	//LED0任务
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
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//退出临界区
}


/**
* @brief ADC任务
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
    
    GUI_SetFont(&GUI_Font16_ASCII); //设置字体
	GUI_SetPenSize(5);				//设置笔大小
	GUI_SetColor(GUI_BLACK);			//红色字体
	GUI_DrawLine(0,210,320,210);	//绘线
	GUI_SetColor(GUI_BLACK);       //设置前景色(如文本，画线等颜色)
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
        GUI_SetPenSize(3);				//设置笔大小
        i++;
        
        GUI_SetColor(GUI_ORANGE);       //设置前景色(如文本，画线等颜色)
		adcx=Get_Adc_Average(ADC_Channel_1,20);
//        GUI_DispDecAt(adcx, 44 ,204 ,4);    
		WORK_VOLT=(float)adcx*(3.288/4096)*11;//实际电压值
        sprintf(aa,"%2.3f",WORK_VOLT);//浮点型数据转为指定格式的字符串
        GUI_DispStringAt(aa, 44, 220); 
        GUI_DrawPoint(i, 200 - (int)(WORK_VOLT * 200 / 24));

        GUI_SetColor(GUI_LIGHTBLUE);       //设置前景色(如文本，画线等颜色)
		adcx=Get_Adc_Average(ADC_Channel_2,20);
//        GUI_DispDecAt(adcx, 152 ,204 ,4);
        if(adcx <= 8){
            adcx = 0;
        }
		WORK_CURR=(float)adcx*(3.288/4096);
        sprintf(aa,"%2.3f",WORK_CURR);//浮点型数据转为指定格式的字符串
        GUI_DispStringAt(aa, 152, 220);
        GUI_DrawPoint(i, 200 - (int)((WORK_CURR / 2) * 200));
        
        GUI_SetColor(GUI_LIGHTGREEN);       //设置前景色(如文本，画线等颜色)
		WORK_WATT=WORK_VOLT * WORK_CURR;
        sprintf(aa,"%2.3f",WORK_WATT);//浮点型数据转为指定格式的字符串
        GUI_DispStringAt(aa, 260, 220);
        GUI_DrawPoint(i, 200 - (int)((WORK_WATT / 48 ) * 200));

        if(i == 320){
            i = 0;
            GUI_ClearRect(0,0,320,205);
        }
////		printf(">>ADC TASK.\r\n");
	}
}

//TOUCH任务
void touch_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		GUI_TOUCH_Exec();	
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//延时5ms
	}
}

//LED0任务
void led0_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//延时500ms
	}
}




