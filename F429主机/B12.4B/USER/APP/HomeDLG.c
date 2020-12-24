#include  "app.h"

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0        (GUI_ID_USER + 0x00)

#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x02)
#define ID_BUTTON_2 (GUI_ID_USER + 0x05)
#define ID_TEXT_0 (GUI_ID_USER + 0x03)
#define ID_TEXT_1 (GUI_ID_USER + 0x04)
#define SWPOS_x 0 //����x����

static WM_HTIMER hTimerTime;  //��ʱ����300ms


//����
GUI_BITMAP buttonbmp_tab[2];
extern GUI_CONST_STORAGE GUI_BITMAP bmdd2;
//λͼ********************************************************************************************************************************
extern GUI_CONST_STORAGE GUI_BITMAP bmBUTTONOFF;
extern GUI_CONST_STORAGE GUI_BITMAP bmBUTTONON;
extern GUI_CONST_STORAGE GUI_BITMAP bmwendu;
extern GUI_CONST_STORAGE GUI_BITMAP bmSD7;//ʪ��
extern GUI_CONST_STORAGE GUI_BITMAP bmKQZL;//��������
extern GUI_CONST_STORAGE GUI_BITMAP bmGLDS;//���ʶ�ʱ
extern uint32_t IOT_Time_OnOff;
extern uint32_t IOT_Time_Off_Minute;


extern GUI_CONST_STORAGE GUI_BITMAP bmsdtx;//��ʪ���ı�

extern uint32_t IOT_CSQ_FLAG;//��ʪ���ж�

extern RTC_TIME rtc_time;//ʱ��

//������IOT
extern uint32_t IOT_DHT11_wendu;//�¶�
extern uint32_t valueDHT11_shidu;//ʪ��
extern uint8_t E1ST;
extern uint8_t E2ST;
extern float POW_T;
extern int pow_x;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Home", ID_FRAMEWIN_0, 0, 0, 800, 480, 0, 0x0, 0 },
	
	//LED��ť
	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_0, SWPOS_x+60, 122, 110, 40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_1, SWPOS_x+60, 251, 110, 40, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_0, SWPOS_x, 132, 60, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_1, SWPOS_x, 261, 60, 20, 0, 0x64, 0 },
	
	//ʱ��
	{ TEXT_CreateIndirect,     "Clock",       GUI_ID_TEXT0,    300, 12,  200, 40,  0, 0x64, 0 },

	//ʪ��
	{ TEXT_CreateIndirect, "Text", GUI_ID_TEXT1, 87, 375, 60, 20, 0, 0x64, 0 },
	
	//����
	{ TEXT_CreateIndirect, "Text", GUI_ID_TEXT2, 710, 115, 60, 20, 0, 0x64, 0 },
	
	//��ʱ
	{ TEXT_CreateIndirect, "Text", GUI_ID_TEXT3, 710, 145, 60, 20, 0, 0x64, 0 },
	
	//��ʪ��
	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_2, 610, 251, 110, 40, 0, 0x0, 0 },
	
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/


/*********************************************************************
*
*       _cbDialog�ص�����
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
	  uint8_t sec, min, hour;
	  char text_buffer[20]={0};
		char text1_buffer[2]={0};
  // USER END

switch (pMsg->MsgId) {
		case WM_DELETE:
		// �ر�
		OS_INFO("HomeDLG delete\n");
		Flag_ICON6 = 0;
		UserApp_Flag = 0;
    tpad_flag=0;  
		WM_DeleteTimer(hTimerTime);//�ض�ʱ���¶�ʱ��

	  break;
  case WM_INIT_DIALOG://��ʼ��
		
			// �����رհ���
			hItem = pMsg->hWin;
			FRAMEWIN_SetText(hItem,"HOME");
			FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);

			//��ʼ��BUTTON0
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
			BUTTON_SetBitmapEx(hItem,0,&buttonbmp_tab[0],0,0);
			BUTTON_SetText(hItem, "");
			
			//��ʼ��BUTTON1
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
			BUTTON_SetBitmapEx(hItem,0,&buttonbmp_tab[0],0,0);
			BUTTON_SetText(hItem, "");
			
			//��ʼ��TEXT0
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
			TEXT_SetText(hItem, "SW1");
			
			//��ʼ��TEXT1
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
			TEXT_SetText(hItem, "SW2");
			
			//ʱ����ʾ�ı�
			hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
			TEXT_SetFont(hItem, GUI_FONT_24B_1);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetTextColor(hItem, GUI_CYAN);
			RTC_GetTime(RTC_Format_BIN, &rtc_time.RTC_Time);
        sec    =  rtc_time.RTC_Time.RTC_Seconds;
        min    =  rtc_time.RTC_Time.RTC_Minutes;
        hour   =  rtc_time.RTC_Time.RTC_Hours;
			sprintf(text_buffer,"%02d:%02d:%02d",hour,min,sec);
			TEXT_SetText(hItem, text_buffer);
			
			//��ʼ��ʪ���ı�
			hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetFont(hItem, GUI_FONT_32_ASCII);
			TEXT_SetText(hItem, "50");
	
			//��ʼ�������ı�
			hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetFont(hItem, GUI_FONT_24_ASCII );
			TEXT_SetText(hItem, "0 W");
	
			//��ʼ����ʱ�ı�
			hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT3);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetFont(hItem, GUI_FONT_24_ASCII );
			TEXT_SetText(hItem, "OFF");
			
			
			//��ʼ��BUTTON2
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
			BUTTON_SetBitmapEx(hItem,0,&buttonbmp_tab[0],0,0);
			BUTTON_SetText(hItem, "");
			
    break;
		
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    
		switch(Id) 
			{
				case ID_BUTTON_0: //BUTTON_0��֪ͨ���룬����LED1
					hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //��ť�����²��ͷ�
								LED1_TOGGLE;
								if(E1ST)
									E1ST=0;
								else
									E1ST=1;
								BUTTON_SetBitmapEx(hItem,0,E1ST?&buttonbmp_tab[1]:&buttonbmp_tab[0],0,0);	
							break;	
					}
					break;
				case ID_BUTTON_1: //BUTTON_1��֪ͨ���룬����BEEP
					hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
								LED1_TOGGLE;
						
								if(E2ST)
									E2ST=0;
								else
									E2ST=1;
								
								BUTTON_SetBitmapEx(hItem,0,E2ST?&buttonbmp_tab[1]:&buttonbmp_tab[0],0,0);	
							break;
					}
					break;
					
					case ID_BUTTON_2: //BUTTON_2��֪ͨ���룬����BEEP
					hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							
								LED1_TOGGLE;
						
								if(IOT_CSQ_FLAG)
									IOT_CSQ_FLAG=0;
								else
									IOT_CSQ_FLAG=1;
								
								BUTTON_SetBitmapEx(hItem,0,IOT_CSQ_FLAG?&buttonbmp_tab[0]:&buttonbmp_tab[1],0,0);	
							break;
					}
					break;
			}
			break;
		
		//��ʱˢ��
	  case WM_TIMER:
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
			BUTTON_SetBitmapEx(hItem,0,E1ST?&buttonbmp_tab[1]:&buttonbmp_tab[0],0,0);	
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
			BUTTON_SetBitmapEx(hItem,0,E2ST?&buttonbmp_tab[1]:&buttonbmp_tab[0],0,0);	
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
			BUTTON_SetBitmapEx(hItem,0,IOT_CSQ_FLAG?&buttonbmp_tab[0]:&buttonbmp_tab[1],0,0);	
		
//			printf("IOT %d\n",IOT_CSQ_FLAG);
//			printf("E %d\n",E2ST);
		
			RTC_GetTime(RTC_Format_BIN, &rtc_time.RTC_Time);
			sec    =  rtc_time.RTC_Time.RTC_Seconds;
			min    =  rtc_time.RTC_Time.RTC_Minutes;
			hour   =  rtc_time.RTC_Time.RTC_Hours;
			sprintf(text_buffer,"%02d:%02d:%02d",hour,min,sec);
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0),text_buffer);
		
		
			sprintf(text1_buffer,"%02d",valueDHT11_shidu);
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1),text1_buffer);
		
			if(E1ST)
				sprintf(text1_buffer,"0 W");
			else
				sprintf(text1_buffer,"1.%d W",pow_x);
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2),text1_buffer);
			
			if(IOT_Time_OnOff==1&&E1ST==0)
				sprintf(text1_buffer,"%d",IOT_Time_Off_Minute);
			else
				sprintf(text1_buffer,"OFF");
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT3),text1_buffer);
			
			
			WM_RestartTimer(hTimerTime,200);

    break;
		
		case WM_PAINT:			 
    GUI_SetBkColor(GUI_BLACK);
		GUI_Clear();
		//�ϲ�������
		GUI_DrawGradientH(400, 0, 800,3,0xF5F200, 0x000000);
		GUI_DrawGradientH(000, 0, 400,3 ,0x000000, 0xF5F200);
		//�ײ��ǻ�����ϵͳ����
		GUI_DrawBitmap(&bmdd2,265,404);
		//�¶ȵ�ͼ
		GUI_DrawBitmap(&bmwendu,250,50);
		//ʪ�ȵ�ͼ
		GUI_DrawBitmap(&bmSD7,55,330);
		//����������ͼ
		GUI_DrawBitmap(&bmKQZL,610,330);
		//���ʶ�ʱ
		GUI_DrawBitmap(&bmGLDS,550,100);
		//��ʪ���ı�
		GUI_DrawBitmap(&bmsdtx,575,187);
		
		
		//�¶�ָ��
    GUI_SetColor(0xF5F200);  
		GUI_SetPenSize(4);
		GUI_DrawLine(398, /* X ��ʼλ�� */
								 200, /* Y ��ʼλ�� */
							 	 398+55*cos((125.0-(IOT_DHT11_wendu/40.0)*140.0)/180*3.1416), /* X ����λ�� */
							   200-55*sin((125.0-(IOT_DHT11_wendu/40.0)*140.0)/180*3.1416)); /* Y ����λ�� */
		GUI_SetPenSize(18);
		GUI_DrawPoint(398,200);
		
    GUI_SetFont(&XBF_XINSONGTI19);

		break;

  default:
    WM_DefaultProc(pMsg);
    break;
  }
}


/*********************************************************************
*
*       CreateHome
*/
void FUN_ICON6Clicked(void){

  WM_HWIN hWin;
	OS_INFO("HomeDLG create\n");
  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, HDTWIN, 0, 0);
	
	buttonbmp_tab[1]=bmBUTTONOFF;
	buttonbmp_tab[0]=bmBUTTONON;
	
  hTimerTime=WM_CreateTimer(WM_GetClientWindow(hWin), 0, 200, 0); //��ʱ����
	
	while(Flag_ICON6)
  {
    if(tpad_flag)WM_DeleteWindow(hWin);  
    GUI_Delay(5);
  }
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
