/**
******************************************************************************
* @file    		debug_protocol.c
* @brief			调试协议
*
*
* @author			WQG
* @versions   v2.0.2
* @date   		2024-4-10
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "debug_protocol.h"	///////////////////////	串口调试


/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#if DEBUG_HUART == 3
UART_HandleTypeDef* p_huart_debug = &huart3;		 //调试串口 UART句柄
#elif DEBUG_HUART == 5
UART_HandleTypeDef* p_huart_debug = &huart5;
#endif


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t Debug_Protocol_Mode	=	0;
uint8_t Debug_Send_Buffer[256];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/



void Set_Debug_Protocol_Mode(uint8_t mode)
{
	if( mode == 2)
		Debug_Protocol_Mode = 2;
	else if( mode == 1)
		Debug_Protocol_Mode = 1;
	else
		Debug_Protocol_Mode = 0;
	
	Debug_Send_Buffer[0] = 0xAA;
	Debug_Send_Buffer[1] = 0xAA;
	Debug_Send_Buffer[2] = 0x60;
	Debug_Send_Buffer[3] = 0xDE;
	Debug_Send_Buffer[4] = Debug_Protocol_Mode;
	HAL_UART_Transmit(p_huart_debug, Debug_Send_Buffer, 5,0xFFFF); //将收到的信息发送出去
}


void UART_Send_Debug(uint8_t * p_buff, uint8_t len, uint8_t cmd)
{
#ifdef DEBUG_HUART
	if(Debug_Protocol_Mode != 1)
		return;
	
	
	Debug_Send_Buffer[0] = cmd;
	memcpy(&Debug_Send_Buffer[1], p_buff, len);
	// 转发至串口5  用于调试
	HAL_UART_Transmit(p_huart_debug, Debug_Send_Buffer, len+1,0xFFFF); //将收到的信息发送出去
	//HAL_UART_Transmit_IT(p_huart_debug, (uint8_t *)Debug_Send_Buffer, len+1);//使用DMA发送数据

#else
	return;
#endif
}



// od oa 结尾
void To_Debug_Protocol_Analysis(uint8_t* p_buf, uint8_t len)
{
#ifdef DEBUG_HUART
	uint16_t addr = 0;
	uint16_t value = 0;
	uint16_t num = 0;
	uint8_t i=0;
#define DEBUG_READ_BUFF_OFFSET		3
	
	// DE B8 60 DE 
	if((p_buf[0] == 0xDE)&&(p_buf[1] == 0xB8)&&(p_buf[2] == 0x60)&&(p_buf[3] == 0xDE))
	{
		Set_Debug_Protocol_Mode(2);
	}
	// 7E AD 60 DE
	else if((p_buf[0] == 0x7E)&&(p_buf[1] == 0xAD)&&(p_buf[2] == 0x60)&&(p_buf[3] == 0xDE))
	{
		Set_Debug_Protocol_Mode(1);
	}
	// 60 6A 60 DE
	else if((p_buf[0] == 0x60)&&(p_buf[1] == 0x6A)&&(p_buf[2] == 0x60)&&(p_buf[3] == 0xDE))
	{
		Set_Debug_Protocol_Mode(0);
	}
	else
	{
		if(Debug_Protocol_Mode == 2)
		{
			// 60 DB 87 MODBUS
			if((p_buf[0] == 0x60)&&(p_buf[1] == 0xDB)&&(p_buf[2] == 0x87))
			{
				if(( p_buf[DEBUG_READ_BUFF_OFFSET] == MB_FUNC_READ_HOLDING_REGISTER ) || ( p_buf[DEBUG_READ_BUFF_OFFSET] == MB_FUNC_READ_INPUT_REGISTER ))
				{
					addr = p_buf[DEBUG_READ_BUFF_OFFSET+1]<<8 | p_buf[DEBUG_READ_BUFF_OFFSET+2];
					
					memset(Debug_Send_Buffer,0,sizeof(Debug_Send_Buffer));
					for(i=0,len=0;i<p_buf[DEBUG_READ_BUFF_OFFSET+4];i++)
					{
						value = Get_DataAddr_Value( p_buf[DEBUG_READ_BUFF_OFFSET],  addr+i);
						Debug_Send_Buffer[len++] = value>>8;
						Debug_Send_Buffer[len++] = value;
					}
					HAL_UART_Transmit(p_huart_debug, Debug_Send_Buffer, len,0xFFFF); //将收到的信息发送出去
				}
				else if( p_buf[DEBUG_READ_BUFF_OFFSET] == MB_FUNC_WRITE_REGISTER ) //6 
				{
					addr = p_buf[DEBUG_READ_BUFF_OFFSET+1]<<8 | p_buf[DEBUG_READ_BUFF_OFFSET+2];
					value = p_buf[DEBUG_READ_BUFF_OFFSET+3]<<8 | p_buf[DEBUG_READ_BUFF_OFFSET+4];
					
					Set_DataAddr_Value( MB_FUNC_READ_HOLDING_REGISTER,  addr, value);
					
					memset(Debug_Send_Buffer,0,sizeof(Debug_Send_Buffer));
					Debug_Send_Buffer[0] = 0xAA;
					Debug_Send_Buffer[1] = 0xAA;
					HAL_UART_Transmit(p_huart_debug, Debug_Send_Buffer, 2,0xFFFF); //将收到的信息发送出去
				}
				else if( p_buf[DEBUG_READ_BUFF_OFFSET] == MB_FUNC_WRITE_MULTIPLE_REGISTERS ) // 16
				{
					addr = p_buf[DEBUG_READ_BUFF_OFFSET+1]<<8 | p_buf[DEBUG_READ_BUFF_OFFSET+2];
					num = p_buf[DEBUG_READ_BUFF_OFFSET+3]<<8 | p_buf[DEBUG_READ_BUFF_OFFSET+4];
					
					for(i=0;i<num;i++)
					{
						value = p_buf[DEBUG_READ_BUFF_OFFSET+6+(i*2)]<<8 | p_buf[DEBUG_READ_BUFF_OFFSET+7+(i*2)];
					
						Set_DataAddr_Value( MB_FUNC_READ_HOLDING_REGISTER,  addr+i, value);
					}
					memset(Debug_Send_Buffer,0,sizeof(Debug_Send_Buffer));
					Debug_Send_Buffer[0] = 0xAA;
					Debug_Send_Buffer[1] = 0xAA;
					HAL_UART_Transmit(p_huart_debug, Debug_Send_Buffer, 2,0xFFFF); //将收到的信息发送出去
				}
			}
		}
	}
#else
	return;
#endif
}




