/*************************************************************/
/*头文件Header File*******************************************/
/*************************************************************/
#include "app_main.h"
#include <stdio.h>

/*************************************************************/
/*宏定义Macro Definition**************************************/
/*************************************************************/
//如果使用FreeRTOS,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
	#include "FreeRTOS.h"
#endif

/*************************************************************/
/*类型定义Byte Definition*************************************/
/*************************************************************/
UART_HandleTypeDef Uart1_Handler;
/*************************************************************/
/*标志位定义Flags Definition**********************************/
/*************************************************************/

/*************************************************************/
/*变量定义Variable Definition*********************************/
/*************************************************************/

/*************************************************************/
/*数组定义Array Definition************************************/
/*************************************************************/

/*************************************************************/
/*函数声明Function Declaration********************************/
/*************************************************************/

/*************************************************************/
/*函数定义Function Definition*********************************/
/*************************************************************/
/*******************************************************************************
* 函数原型：
* 输入参数：
* 输出参数：
* 函数功能：加入以下代码，支持printf函数，而不需要选择use MicroLIB
* 返回值说明：
* 创建日期：
* 创建人：
* 修改日期
* 修改人：
* 第N次修改：
* 修改原因：
* 备注：
*******************************************************************************/
#if 1
	#pragma import(__use_no_semihosting)
	//标准库需要的支持函数
	struct __FILE
	{
		int handle;
	};
	
	FILE __stdout;
	//定义_sys_exit()以避免使用半主机模式
	void _sys_exit(int x)
	{
		x = x;
	}
//	extern int consolePutchar(int ch);
	//重新定义fputc函数
	int fputc(int ch,FILE *f)
	{
		while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
		USART1->DR = (uint8_t) ch;   
//		consolePutchar(ch);//打印到上位机
		return ch;
	}
#endif

/*******************************************************************************
* 函数原型：
* 输入参数：
* 输出参数：
* 函数功能：串口初始化
* 返回值说明：
* 创建日期：
* 创建人：
* 修改日期
* 修改人：
* 第N次修改：
* 修改原因：
* 备注：
*******************************************************************************/
void UART1_Init(void)
{
	Uart1_Handler.Instance=USART1;					 //调用哪一个串口，句柄
	Uart1_Handler.Init.BaudRate=115200;				 //波特率
	Uart1_Handler.Init.WordLength=UART_WORDLENGTH_8B;//字长为8bit
	Uart1_Handler.Init.StopBits=UART_STOPBITS_1;	 //1bit停止位
	Uart1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;//硬件流无
	Uart1_Handler.Init.Mode=UART_MODE_TX_RX;		 //复用功能为发送和接收
	Uart1_Handler.Init.Parity=UART_PARITY_NONE;		 //不使用奇偶校验位
	
	HAL_UART_Init(&Uart1_Handler);
}

/*******************************************************************************
* 函数原型：
* 输入参数：
* 输出参数：
* 函数功能：
* 返回值说明：
* 创建日期：
* 创建人：
* 修改日期
* 修改人：
* 第N次修改：
* 修改原因：
* 备注：此函数在HAL_UART_Init()中被调用
*******************************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)//如果是端口1
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_GPIOB_CLK_ENABLE();			//使能GPIOB时钟
		__HAL_RCC_USART1_CLK_ENABLE();			//使能USART1时钟
		
		GPIO_Initure.Pin=GPIO_PIN_3;			//PB3  RX
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_HIGH;		//高速
		GPIO_Initure.Alternate=GPIO_AF7_USART1;	//复用为USART1
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//初始化PB3

		GPIO_Initure.Pin=GPIO_PIN_15;			//PA15 TX
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA15
		
		HAL_NVIC_SetPriority(USART1_IRQn,15,0);	//设置串口中断抢占优先级为3，响应优先级为3
		HAL_NVIC_EnableIRQ(USART1_IRQn);		//使能中断通道
	}
}

/*******************************************************************
 * 函数原型：
 * 输入参数：
 * 输出参数：
 * 函数功能：每接收到一个字符就必须产生中断，进入此函数，然后在进入
 * 			 HAL_UART_IRQHandler()函数，此函数会对中断类型进行判断
 *			 如果是开启了接收中断，就会进入UART_Receive_IT()函数，
 *			 然后在UART_Receive_IT()函数中调用回调函数
 *			 HAL_UART_RxCpltCallback();
 * 返回值说明：
 * 创建日期：
 * 创建人：
 * 修改日期
 * 修改人：
 * 第N次修改：
 * 修改原因：
 * 备注：
 *******************************************************************/
void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&Uart1_Handler);
	//HAL_UART_Receive_IT(&Uart1_Handler,rdata,sizeof(rdata));//读取接收到的数据
}

/*******************************************************************
 * 函数原型：
 * 输入参数：
 * 输出参数：
 * 函数功能：当HAL_UART_Receive_IT(&Uart1_Handler,rdata,sizeof(rdata))
 *			 中的sizeof(rdata)为0时才会调用回调函数，并关闭中断，和
 *			 和USART1_IRQHandler()不一样,所以要在USART1_IRQHandler()
 *			 函数里面加一句HAL_UART_Receive_IT(&Uart1_Handler,rdata,sizeof(rdata));
 *			 这是重新开启中断
 * 返回值说明：
 * 创建日期：
 * 创建人：
 * 修改日期
 * 修改人：
 * 第N次修改：
 * 修改原因：
 * 备注：
 *******************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t rec;
	if(huart->Instance==USART1)
	{
		rec=*((huart->pRxBuffPtr)-1);//pRxBuffPtr-1是指针往前倒退，是因为在HAL_UART_Receive_IT这个函数中
									 //pRxBuffPtr的指针加一到了"下一个存储地址"所以才在这里减一，也可以
									 //写成"--(huart->pRxBuffPtr)"
		//rec=tdata[0];//在HAL_UART_Receive_IT()函数中接收到的会赋值给rdata数组，所以可以直接取
		//HAL_UART_Transmit(&Uart1_Handler,&rec,1,1000);//接收之后立马发送给上位机
	}
}



