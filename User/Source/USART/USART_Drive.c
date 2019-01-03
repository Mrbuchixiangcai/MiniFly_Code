/*************************************************************/
/*ͷ�ļ�Header File*******************************************/
/*************************************************************/
#include "app_main.h"
#include <stdio.h>

/*************************************************************/
/*�궨��Macro Definition**************************************/
/*************************************************************/
//���ʹ��FreeRTOS,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
	#include "FreeRTOS.h"
#endif

/*************************************************************/
/*���Ͷ���Byte Definition*************************************/
/*************************************************************/
UART_HandleTypeDef Uart1_Handler;
/*************************************************************/
/*��־λ����Flags Definition**********************************/
/*************************************************************/

/*************************************************************/
/*��������Variable Definition*********************************/
/*************************************************************/

/*************************************************************/
/*���鶨��Array Definition************************************/
/*************************************************************/

/*************************************************************/
/*��������Function Declaration********************************/
/*************************************************************/

/*************************************************************/
/*��������Function Definition*********************************/
/*************************************************************/
/*******************************************************************************
* ����ԭ�ͣ�
* ���������
* ���������
* �������ܣ��������´��룬֧��printf������������Ҫѡ��use MicroLIB
* ����ֵ˵����
* �������ڣ�
* �����ˣ�
* �޸�����
* �޸��ˣ�
* ��N���޸ģ�
* �޸�ԭ��
* ��ע��
*******************************************************************************/
#if 1
	#pragma import(__use_no_semihosting)
	//��׼����Ҫ��֧�ֺ���
	struct __FILE
	{
		int handle;
	};
	
	FILE __stdout;
	//����_sys_exit()�Ա���ʹ�ð�����ģʽ
	void _sys_exit(int x)
	{
		x = x;
	}
//	extern int consolePutchar(int ch);
	//���¶���fputc����
	int fputc(int ch,FILE *f)
	{
		while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
		USART1->DR = (uint8_t) ch;   
//		consolePutchar(ch);//��ӡ����λ��
		return ch;
	}
#endif

/*******************************************************************************
* ����ԭ�ͣ�
* ���������
* ���������
* �������ܣ����ڳ�ʼ��
* ����ֵ˵����
* �������ڣ�
* �����ˣ�
* �޸�����
* �޸��ˣ�
* ��N���޸ģ�
* �޸�ԭ��
* ��ע��
*******************************************************************************/
void UART1_Init(void)
{
	Uart1_Handler.Instance=USART1;					 //������һ�����ڣ����
	Uart1_Handler.Init.BaudRate=115200;				 //������
	Uart1_Handler.Init.WordLength=UART_WORDLENGTH_8B;//�ֳ�Ϊ8bit
	Uart1_Handler.Init.StopBits=UART_STOPBITS_1;	 //1bitֹͣλ
	Uart1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;//Ӳ������
	Uart1_Handler.Init.Mode=UART_MODE_TX_RX;		 //���ù���Ϊ���ͺͽ���
	Uart1_Handler.Init.Parity=UART_PARITY_NONE;		 //��ʹ����żУ��λ
	
	HAL_UART_Init(&Uart1_Handler);
}

/*******************************************************************************
* ����ԭ�ͣ�
* ���������
* ���������
* �������ܣ�
* ����ֵ˵����
* �������ڣ�
* �����ˣ�
* �޸�����
* �޸��ˣ�
* ��N���޸ģ�
* �޸�ԭ��
* ��ע���˺�����HAL_UART_Init()�б�����
*******************************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)//����Ƕ˿�1
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
		__HAL_RCC_GPIOB_CLK_ENABLE();			//ʹ��GPIOBʱ��
		__HAL_RCC_USART1_CLK_ENABLE();			//ʹ��USART1ʱ��
		
		GPIO_Initure.Pin=GPIO_PIN_3;			//PB3  RX
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
		GPIO_Initure.Pull=GPIO_PULLUP;			//����
		GPIO_Initure.Speed=GPIO_SPEED_HIGH;		//����
		GPIO_Initure.Alternate=GPIO_AF7_USART1;	//����ΪUSART1
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//��ʼ��PB3

		GPIO_Initure.Pin=GPIO_PIN_15;			//PA15 TX
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA15
		
		HAL_NVIC_SetPriority(USART1_IRQn,15,0);	//���ô����ж���ռ���ȼ�Ϊ3����Ӧ���ȼ�Ϊ3
		HAL_NVIC_EnableIRQ(USART1_IRQn);		//ʹ���ж�ͨ��
	}
}

/*******************************************************************
 * ����ԭ�ͣ�
 * ���������
 * ���������
 * �������ܣ�ÿ���յ�һ���ַ��ͱ�������жϣ�����˺�����Ȼ���ڽ���
 * 			 HAL_UART_IRQHandler()�������˺�������ж����ͽ����ж�
 *			 ����ǿ����˽����жϣ��ͻ����UART_Receive_IT()������
 *			 Ȼ����UART_Receive_IT()�����е��ûص�����
 *			 HAL_UART_RxCpltCallback();
 * ����ֵ˵����
 * �������ڣ�
 * �����ˣ�
 * �޸�����
 * �޸��ˣ�
 * ��N���޸ģ�
 * �޸�ԭ��
 * ��ע��
 *******************************************************************/
void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&Uart1_Handler);
	//HAL_UART_Receive_IT(&Uart1_Handler,rdata,sizeof(rdata));//��ȡ���յ�������
}

/*******************************************************************
 * ����ԭ�ͣ�
 * ���������
 * ���������
 * �������ܣ���HAL_UART_Receive_IT(&Uart1_Handler,rdata,sizeof(rdata))
 *			 �е�sizeof(rdata)Ϊ0ʱ�Ż���ûص����������ر��жϣ���
 *			 ��USART1_IRQHandler()��һ��,����Ҫ��USART1_IRQHandler()
 *			 ���������һ��HAL_UART_Receive_IT(&Uart1_Handler,rdata,sizeof(rdata));
 *			 �������¿����ж�
 * ����ֵ˵����
 * �������ڣ�
 * �����ˣ�
 * �޸�����
 * �޸��ˣ�
 * ��N���޸ģ�
 * �޸�ԭ��
 * ��ע��
 *******************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t rec;
	if(huart->Instance==USART1)
	{
		rec=*((huart->pRxBuffPtr)-1);//pRxBuffPtr-1��ָ����ǰ���ˣ�����Ϊ��HAL_UART_Receive_IT���������
									 //pRxBuffPtr��ָ���һ����"��һ���洢��ַ"���Բ��������һ��Ҳ����
									 //д��"--(huart->pRxBuffPtr)"
		//rec=tdata[0];//��HAL_UART_Receive_IT()�����н��յ��Ļḳֵ��rdata���飬���Կ���ֱ��ȡ
		//HAL_UART_Transmit(&Uart1_Handler,&rec,1,1000);//����֮�������͸���λ��
	}
}



