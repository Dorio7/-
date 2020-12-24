#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_gpio.h"

uint32_t MQ=0;
u8 MQ_ST[11]={0xFE, 0x08,	0xA4,	0xA4,	0x11,	0x11,	0x00,	0x00,	0x00,	0x00,	0xFF};//����

uint8_t ZIGBEE_R_TEMP[30]={0x00};//zigbee���ڽ��ջ���
uint8_t R_flag=0;
uint8_t NOFINS_flag=0;//��δ�������־


//��ʱ
void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
} 


int main(void)
{	
	u8 i;
	SystemInit();
  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
  USART_Config();
	USART2_Config();
	
	C_GPIO_Config();
	

  while(1)
	{	
		Delay(0X1FFFFf);

		if(GPIO_ReadInputDataBit(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == 0 )  
		{
			MQ=0X01;
		}
		else
			MQ=0X00;
	
		MQ_ST[6]=MQ;
		if(NOFINS_flag==1)
		{
			Delay(0X1FFFFf);
			USART1_Sned_Char_Buff(MQ_ST,11);
			NOFINS_flag=0;
		}
			
	}	
}
