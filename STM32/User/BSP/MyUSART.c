#include "MyUSART.h"
#include "stdio.h"

uint8_t SerialRxFlag 	= 0;
uint8_t SerialRxData 	= 0;
uint8_t SerialRxIndex 	= 0;
uint8_t SerialRxOverflow = 0;
char USART1_Buffer[USART1_BufferSize];

char USART2_Buffer[USART2_BufferSize];
uint8_t BufferFlag = 0;
static uint8_t BufferIndex = 0;
static uint8_t FrameReceiving = 0;

//USAR1初始化
void MyUSART1_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART1, ENABLE);
}

//UART1中断接收初始化
void NVIC_USART1_Init(void) {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_InitStructure);

}

//USAR2初始化
void MyUSART2_Init(void) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStructure);
	
	USART_Cmd(USART2, ENABLE);
}

//UART1发送字符
void MyUSART1_SendChar(char c) {
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	
	USART_SendData(USART1, c);
}

//UART1发送字符串
void MyUSART1_SendStr(char* str) {
	while (*str != '\0') {
		MyUSART1_SendChar(*str++);
	}
}

//重定向输出至UART1 Send
int fputc(int ch, FILE* f) {
	MyUSART1_SendChar(ch);
	
	return ch;
}

//UART中断处理
void USART1_IRQHandler(void) {
	uint8_t RxData;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
		RxData = USART_ReceiveData(USART1);
		if (SerialRxFlag == 1) return ;  //存在未处理完成数据
		//检查是否为结束标志
		if (RxData == '\r' || RxData == '\n') {
			if (SerialRxIndex > 0) {
				USART1_Buffer[SerialRxIndex] = '\0';
				SerialRxFlag = 1;
			}
			SerialRxOverflow = 0;
			SerialRxIndex 	 = 0;
		}else if (SerialRxIndex < USART1_BufferSize - 1) {
			USART1_Buffer[SerialRxIndex++] = RxData;
		}else {  //获取到数据过长，不符合标准
			SerialRxOverflow = 1;
		}
		
	}
}

//UART2接收时间数据&校准数据
void MyUSART2_ReceiveBuff(void) {
	uint8_t Data;
	
	if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET) {
		Data = USART_ReceiveData(USART2);
		
		if (Data == '#') {  //检测到帧头，开始接收数据
			BufferIndex = 0;
			FrameReceiving = 1;
			return ;
		}
		
		if (FrameReceiving == 0) return ;  //为接收到帧头
		
		if (Data != '@' ) {  //未检测到帧尾，一直接收
			USART2_Buffer[BufferIndex] = (char)Data;
			BufferIndex++;
		}else if (Data == '@' ) {  //帧尾 结束标志
			USART2_Buffer[BufferIndex] = '\0';
			BufferFlag = 1;
			BufferIndex = 0;
			FrameReceiving = 0;
		}
		if (BufferIndex >= USART2_BufferSize - 1) {
			BufferIndex = 0;
			FrameReceiving = 0;
			return ;
		}
	}
}


