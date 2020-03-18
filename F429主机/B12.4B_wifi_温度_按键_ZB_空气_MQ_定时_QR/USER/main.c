#include "includes.h"

//������
#include "gizwits_product.h"
#include "common.h"
#include "./Bsp/humiture/humiture.h"

//ȫ�ֱ���*********************************************
DHT11_Data_TypeDef DHT11_Data;
uint32_t IOT_DHT11_wendu;
uint32_t valueDHT11_shidu;
uint32_t E1ST=1;
uint32_t E2ST=0 ;
uint32_t MQ_W=0;
uint32_t POW_H=0;
uint32_t POW_L=0;
uint32_t IOT_Time_OnOff=0;
uint32_t IOT_Time_Off_Minute=0;
uint32_t IOT_CSQ_FLAG=0;


float POW_T=0;
int pow_x;
//����ZIGBEE���ݰ�
#if 1
uint8_t ZIGBEE_R_TEMP[30]={0x00};//zigbee���ڽ��ջ���
uint8_t R_flag=0;
uint8_t NOFINS_flag=0;//��δ�������־\

u8 SW1_ST[11]={0xFE, 0x08,	0xA1,	0xA1,	0x22,	0x22,	0x01,	0x00,	0x00,	0x00,	0xFF};
//u8 SW1_OFF[11]={0xFE, 0x08,	0xA1,	0xA1,	0x22,	0x22,	0x00,	0x00,	0x00,	0x00,	0xFF};
u8 LIGHT_LED2[8]={0xFE ,0x05 ,0x91 ,0x20 ,0x22 ,0x22 ,0x0A ,0xFF};
u8 testzb[11]={0xFE, 0x08,	0xA1,	0xA1,	0x22,	0x22,	0xAB,0XCD,0XEF,0XE1,	0xFF};

u8 SW2_ON[8]={0XFE, 0X05 , 0X90 , 0X44 , 0X33 , 0X33 , 0X11 , 0XFF};
u8 SW2_OFF[8]={0XFE, 0X05 , 0X90 , 0X44 , 0X33 , 0X33 , 0X10 , 0XFF};

u8 MQ_ST[11]={0xFE, 0x08,	0xA4,	0xA4,	0x44,	0x44,	0x00,	0x00,	0x00,	0x00,	0xFF};//MQ��ѯ �������� ֻҪ����11λ�����ҷ��͵���ȷ��ַ



void ZIGBEE_Sned(u8 buf[])  
{  
    u32 i;  
    for(i=0;i<11;i++)  
			USART_SendData(USART3,buf[i]);  
          
}


//uint32_t E1ST_flag=0;//zigbee����ǰ�ı�־λ
//uint32_t E2ST_flag=0;


#endif

int32_t testtime;
int32_t tests=0;
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/* --------------- APPLICATION GLOBALS ---------------- */
static  OS_TCB       AppTaskStartTCB;
static  CPU_STK      AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB       AppTaskObj0TCB;  //�������� ����
static  CPU_STK      AppTaskObj0Stk[APP_CFG_TASK_OBJ0_STK_SIZE];

static  OS_TCB       AppTaskGUITCB;
__align(8) static  CPU_STK      AppTaskGUIStk[APP_CFG_TASK_GUI_STK_SIZE];



//��ȡDHT11��������������*******************
//�������ȼ�
#define GET_HUMTEMLLIGHT_TASK_PRIO    15
//�����ջ��С
#define GET_HUMTEMLLIGHT_STK_SIZE     128
//������ƿ�
static OS_TCB GET_HUMTEMLLIGHT_TaskTCB;
//���������ջ
static CPU_STK GET_HUMTEMLLIGHT_TASK_STK[GET_HUMTEMLLIGHT_STK_SIZE];
//����������
void Get_HUMTEMLLIGHT_task(void *p_arg);

//�����ƴ�������*******************
//�������ȼ�
#define IOT_HANDLE_TASK_PRIO    15
//�����ջ��С
#define IOT_HANDLE_STK_SIZE     1024
//������ƿ�
static OS_TCB IOT_HANDLE_TaskTCB;
//���������ջ
static CPU_STK IOT_HANDLE_TASK_STK[IOT_HANDLE_STK_SIZE];
//����������
void IOT_HANDLE_task(void *p_arg);


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void  AppTaskStart   (void  *p_arg);
static  void  AppTaskCreate  (void);
static  void  AppObjCreate   (void);

static  void  AppTaskObj0    (void  *p_arg);
static  void  AppTaskGUI     (void  *p_arg);

KEY Key1,Key2;
uint8_t   tpad_flag=0;
/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*
*********************************************************************************************************
*/

int main(void)
{
	OS_ERR   err;
#if (CPU_CFG_NAME_EN == DEF_ENABLED)
	CPU_ERR  cpu_err;
#endif                 

	Mem_Init();                                                 /* Initialize Memory Managment Module                   */
	Math_Init();                                                /* Initialize Mathematical Module                       */

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
	CPU_NameSet((CPU_CHAR *)"STM32F429II",
							(CPU_ERR  *)&cpu_err);
#endif

	BSP_IntDisAll();                                            /* Disable all Interrupts.                              */

	OSInit(&err);                                               /* Init uC/OS-III.                                      */

	App_OS_SetAllHooks();

	OSTaskCreate(&AppTaskStartTCB,                              /* Create the start task                                */
								"App Task Start",
								AppTaskStart,
								0u,
								APP_CFG_TASK_START_PRIO,
								&AppTaskStartStk[0u],
								AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
								APP_CFG_TASK_START_STK_SIZE,
								0u,
								0u,
								0u,
							 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							 &err);

  if(err!=OS_ERR_NONE)  bsp_result|=BSP_OS;
	OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
	
	while (DEF_ON) {                                            /* Should Never Get Here.                               */
			;
	}
}

//������ʱɨ��Ķ�ʱ��
static void inputscan(void)
{
  OS_ERR  err;
  
  OSSchedLock(&err);
	GT9xx_GetOnePiont();//��������ʱɨ��
  OSSchedUnlock(&err); 
  
//	Key_RefreshState(&Key1);//ˢ�°���״̬
//	Key_RefreshState(&Key2);//ˢ�°���״̬
//	if(Key_AccessTimes(&Key1,KEY_ACCESS_READ)!=0)//���������¹�
//	{
//		LED2_TOGGLE;
//		Key_AccessTimes(&Key1,KEY_ACCESS_WRITE_CLEAR);
//	}
//	if(Key_AccessTimes(&Key2,KEY_ACCESS_READ)!=0)//���������¹�
//	{
//		LED3_TOGGLE;
//		Key_AccessTimes(&Key2,KEY_ACCESS_WRITE_CLEAR);
//	}
}

/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{

	OS_ERR      err;
 (void)p_arg;
 
	BSP_Init();                                                 /* Initialize BSP functions                             */
	CPU_Init();                                                 /* Initialize the uC/CPU services                       */

	
//������*****************************

		timerInit();             //������������ʱ����ʼ��
	  uartInit();
    userInit();
    gizwitsInit();
		TEMP_HUM_GPIO_Config();//��ʪ��
	
	  GIZWITS_LOG("MCU Init Success \n");
	
#if OS_CFG_STAT_TASK_EN > 0u
	OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
	CPU_IntDisMeasMaxCurReset();
#endif

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
	APP_TRACE_DBG(("Creating Application kernel objects\n\r"));
#endif	
	AppObjCreate(); 
/* Create Applicaiton kernel objects                    */
#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
	APP_TRACE_DBG(("Creating Application Tasks\n\r"));
#endif	
	AppTaskCreate();                                            /* Create Application tasks                             */

	
	while (DEF_TRUE)
	{                                          /* Task body, always written as an infinite loop.       */
    inputscan();
		OSTimeDlyHMSM(0u, 0u, 0u, 20u,
									OS_OPT_TIME_HMSM_STRICT,
									&err);
	}
}

/*
*********************************************************************************************************
*                                          AppTaskCreate()
*
* Description : Create Application Tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
	OS_ERR  os_err;

	 /* ����ʱ��Ƭ��ת���� */
 OSSchedRoundRobinCfg((CPU_BOOLEAN )DEF_ENABLED, //ʹ��ʱ��Ƭ��ת����
 (OS_TICK )1, //�� OSCfg_TickRate_Hz / 10��ΪĬ��ʱ��Ƭֵ
 (OS_ERR *)&os_err ); //���ش�������
	
	
	/* ---------- CREATE KERNEL OBJECTS TEST TASK --------- */
	OSTaskCreate((OS_TCB       *)&AppTaskObj0TCB,
               (CPU_CHAR     *)"Kernel Objects Task 0",
               (OS_TASK_PTR   )AppTaskObj0,
               (void         *)0,
               (OS_PRIO       )APP_CFG_TASK_OBJ0_PRIO,
               (CPU_STK      *)&AppTaskObj0Stk[0],
               (CPU_STK_SIZE  )AppTaskObj0Stk[APP_CFG_TASK_OBJ0_STK_SIZE / 10u],
               (CPU_STK_SIZE  )APP_CFG_TASK_OBJ0_STK_SIZE,
               (OS_MSG_QTY    )0u,
               (OS_TICK       )0u,
               (void         *)0,
               (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR       *)&os_err);
							 
  OSTaskCreate((OS_TCB       *)&AppTaskGUITCB,
               (CPU_CHAR     *)"Kernel Objects Task 0",
               (OS_TASK_PTR   )AppTaskGUI,
               (void         *)0,
               (OS_PRIO       )APP_CFG_TASK_GUI_PRIO,
               (CPU_STK      *)&AppTaskGUIStk[0],
               (CPU_STK_SIZE  )AppTaskGUIStk[APP_CFG_TASK_GUI_STK_SIZE / 10u],
               (CPU_STK_SIZE  )APP_CFG_TASK_GUI_STK_SIZE,
               (OS_MSG_QTY    )0u,
               (OS_TICK       )0u,
               (void         *)0,
               (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR       *)&os_err);
							 
							 
							 	//�����ɼ��¶�ʪ������					 
		OSTaskCreate(
								 (OS_TCB*				)&GET_HUMTEMLLIGHT_TaskTCB,		
								 (CPU_CHAR* 		)"GET_HUMTEMLLIGHT_Task", 		
                 (OS_TASK_PTR 	)Get_HUMTEMLLIGHT_task, 			
                 (void* 				)0,					
                 (OS_PRIO	  		)GET_HUMTEMLLIGHT_TASK_PRIO,     
                 (CPU_STK* 			)&GET_HUMTEMLLIGHT_TASK_STK[0],	
                 (CPU_STK_SIZE	)GET_HUMTEMLLIGHT_STK_SIZE/10,	
                 (CPU_STK_SIZE	)GET_HUMTEMLLIGHT_STK_SIZE,		
                 (OS_MSG_QTY  	)0,					
                 (OS_TICK	  		)0,					
                 (void*					)0,					
                 (OS_OPT      	)OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR* 			)&os_err
							);	
							 
							 //����IOT_HANDLE����						 
		OSTaskCreate(
								 (OS_TCB*				)&IOT_HANDLE_TaskTCB,		
								 (CPU_CHAR* 		)"IOT_HANDLE_task", 		
                 (OS_TASK_PTR 	)IOT_HANDLE_task, 			
                 (void* 				)0,					
                 (OS_PRIO	  		)IOT_HANDLE_TASK_PRIO,     
                 (CPU_STK* 			)&IOT_HANDLE_TASK_STK[0],	
                 (CPU_STK_SIZE	)IOT_HANDLE_STK_SIZE/10,	
                 (CPU_STK_SIZE	)IOT_HANDLE_STK_SIZE,		
                 (OS_MSG_QTY  	)0,					
                 (OS_TICK	  		)0,					
                 (void*					)0,					
                 (OS_OPT      	)OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR* 			)&os_err);				
}


/*
*********************************************************************************************************
*                                          AppObjCreate()
*
* Description : Create Application Kernel Objects.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{
//  KeyCreate(&Key1,GetPinStateOfKey1);
//	KeyCreate(&Key2,GetPinStateOfKey2);
}
/*
*********************************************************************************************************
*                                          AppTaskObj0()
*
* Description : Test uC/OS-III objects.
*
* Argument(s) : p_arg is the argument passed to 'AppTaskObj0' by 'OSTaskCreate()'.
*
* Return(s)   : none
*
* Caller(s)   : This is a task
*
* Note(s)     : none.���ݰ�������������
*********************************************************************************************************
*/

//����
#if 0
static  void  AppTaskObj0 (void  *p_arg)
{
  OS_ERR      err;
	static uint8_t tpad_count=0;
	(void)p_arg;
	
	while(TPAD_Init())
	{
		tpad_count++;
		if(tpad_count>=10)
    {
      bsp_result |= BSP_TPAD;
      break;
		}
     OSTimeDlyHMSM(0, 0, 1, 0,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
	}	
	
	while(1)
	{	printf("����1\n");
		if((tpad_count<10)&&TPAD_Scan(0))
		{
			//����
//		GIZWITS_LOG("KEY2 PRESS LONG ,AirLink mode\n");
//    #if !MODULE_TYPE
//    gizwitsSetMode(WIFI_AIRLINK_MODE);
//    #endif

			BEEP_ON;
      if(UserApp_Flag)tpad_flag=1;
			
		}
		OSTimeDlyHMSM(0, 0, 0, 300,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
		BEEP_OFF;
	}   
}

#endif

//ZIGBEE ���� ���ȼ�16
#if 1

static  void  AppTaskObj0 (void  *p_arg)
{
  OS_ERR  err;
	(void)p_arg;
	

	while(1)
	{	
//		printf("TTASK1\n");
		//BEEP_ON;

			OSSchedLock(&err);//�����ٽ���

			SW1_ST[6]=E1ST;
			USART3_Sned_Char_Buff(SW1_ST,11);
//			E1ST_flag=E1ST;

		if(E2ST)
			USART3_Sned_Char_Buff(SW2_ON,8);
		else
			USART3_Sned_Char_Buff(SW2_OFF,8);
		
		
			USART3_Sned_Char_Buff(MQ_ST,11);
		
			OSSchedUnlock(&err);	//�˳��ٽ���
			
			if(IOT_Time_OnOff==1&&E1ST==0)
			{
				IOT_Time_Off_Minute--;
				if(IOT_Time_Off_Minute==0)
				{
					IOT_Time_OnOff=0;
					E1ST=1;
				}
			}
		
			OSTimeDlyHMSM(0, 0, 1, 0,
												OS_OPT_TIME_HMSM_STRICT,
												&err);
	}
		//BEEP_OFF;  
}

#endif




/*
����������  UCOSIII IOT�����������ڲɼ�DHT11�¶�ʪ��
�������ȼ���
��ڲ�����  ��
����ֵ��    ��
*/

void Get_HUMTEMLLIGHT_task(void *p_arg)
{
		OS_ERR err;
	u8 flll=0;

		while(1)
		{
//			printf("TTASK2\n");
			
			OSSchedLock(&err);//�����ٽ���
			
			if(Read_DHT11(&DHT11_Data) == 0) //��ȡʧ��
			{
				printf("DHT11_ERROR\n");
			}
//			
			OSSchedUnlock(&err);	//�˳��ٽ���
			IOT_DHT11_wendu=DHT11_Data.temp_int;
			valueDHT11_shidu=DHT11_Data.humi_int;
			
//			printf("temp=%d\n",IOT_DHT11_wendu);
//			
//			POW_H=SW1_ST[8];
//			POW_L=SW1_ST[9];
//			
//			for(flll=0;flll<11;flll++)
//				printf("%x ",SW1_ST[flll]);
//			printf("\n ");
			
//			USART3_Sned_Char_Buff(MQ_ST,11);
//			
//			printf("POW_H=%x\n",POW_H);printf("POW_L=%x\n",POW_L);
			
			pow_x=rand()%7;
			POW_T=1+pow_x*0.1;
			
			
			
			if(IOT_CSQ_FLAG)//���ʪ�ȴ���80�򿪳�ʪ��������2��
			{
				if(valueDHT11_shidu>80)
				{
					E2ST=0;
				}
			}
			
			
			
			
			if(MQ_W)//����п�ȼ���壬��������
			{
				BEEP_ON;
				
				OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);//1��
				
				BEEP_OFF;
				
				MQ_W=0;
			}
			OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_HMSM_STRICT,&err);//5��
		}
}	



/*
����������  UCOSIII IOT���������������������ݴ�������������ϵͳӦ�ò�
�������ȼ���15
��ڲ�����  ��
����ֵ��    ��
*/
uint8_t testeew[5]={0XFF,5,8,7,0XFF};
void IOT_HANDLE_task(void *p_arg)
{
	OS_ERR err;

	while(1)
	{
//		printf("TTASK3\n");
		
//	OSSchedLock(&err);//�����ٽ���
			gizwitsHandle((dataPoint_t *)&currentDataPoint);
			userHandle();
//			OSSchedUnlock(&err);	//�˳��ٽ���
//uartWrite(testeew,5);
			OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
	}
	
}

/**************  ���ش���   ***************/
void hardwareerr(void)
{
	OS_ERR  err;
  printf("bsp_result->(0x%04X)\n",bsp_result);
  if((bsp_result&(~BSP_NETWORK))==0) return;
  if(!((bsp_result&BSP_SDRAM)||(bsp_result&BSP_GUI)))
  {
    char tempstr[20];
    sprintf(tempstr,"error->(0x%04X)",bsp_result);
    GUI_Clear();
    GUI_SetColor(GUI_RED);
    GUI_SetFont(GUI_FONT_24_ASCII);
		GUI_DispStringHCenterAt(tempstr, 400,120);
    
    LCD_BL_ON();
  }
  while(1)
  {
    printf("bsp_result->(0x%04X)\n",bsp_result);
    LED1_ON;
    OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
    LED1_OFF;
    OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
  }
}

/*
*********************************************************************************************************
*                                          AppTaskObj1()
*
* Description : Test uC/OS-III objects.
*
* Argument(s) : p_arg is the argument passed to 'AppTaskObj1' by 'OSTaskCreate()'.
*
* Return(s)   : none
*
* Caller(s)   : This is a task
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  AppTaskGUI (void  *p_arg)
{
  (void)p_arg;
  if(bsp_result!=BSP_OK)
	{
		hardwareerr();
	}
  while(1)
  {printf("TTASK4\n");
    emWinMainApp();
		
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
