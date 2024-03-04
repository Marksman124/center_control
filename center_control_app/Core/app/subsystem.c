/**
******************************************************************************
* @file    		subsystem.c
* @brief   		子系统查询
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "subsystem.h"
#include "usart.h"
#include "metering.h"
#include "dmx512.h"


uint16_t Subsystem_Task_Counter=0;

void Subsystem_Init(void)
{

	Dmx512_Receive_Init();
	
	Metering_Receive_Init();
}


void Subsystem_RxData(uint8_t len)
{
	
	Metering_RxData(len);
}


void Subsystem_Handler_Task(void)
{
	//协议解析 DMX512
	Dmx512_Protocol_Analysis();
	if( Subsystem_Task_Counter > (THREAD_PERIOD_METERING_MODULE_HUART/THREAD_PERIOD_DMX512_HUART))
	{
		Subsystem_Task_Counter = 0;
		//协议解析 计量模块
		Metering_Protocol_Analysis();
	}
	Subsystem_Task_Counter ++;
}
