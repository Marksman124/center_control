/**
******************************************************************************
* @file    		pump.c
* @brief   		ˮ�ÿ���
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "pump.h"
#include "dac.h"
#include "dma.h"
#include "gpio.h"
#include "tim.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "modbus.h"

uint16_t* p_Gear_Pump_1;
uint16_t* p_Gear_Pump_2;

uint16_t* p_Current_Pump_1;
uint16_t* p_Current_Pump_2;

uint16_t* p_Voltage_Pump_1;
uint16_t* p_Voltage_Pump_2;

uint16_t Gear_Pump_1;
uint16_t Gear_Pump_2;

uint16_t Current_Pump_1;
uint16_t Current_Pump_2;

uint16_t Voltage_Pump_1;
uint16_t Voltage_Pump_2;

void Test_pump_output(void)
{
	*p_Gear_Pump_1 = 2;	// K1 0~4
	*p_Gear_Pump_2 = 4;	// K2

	*p_Current_Pump_1 = 100;	// 0~100
	*p_Current_Pump_2 = 50;

	*p_Voltage_Pump_1 = 4095;	// 0~4095
	*p_Voltage_Pump_2 = 409;
	
}


void Call_PUMP_Handle_Init(void)
{
	p_Gear_Pump_1 = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_GEAR_PUMP_1);
	p_Gear_Pump_2 = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_GEAR_PUMP_2);
	
	p_Current_Pump_1 = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_CURRENT_PUMP_1);
	p_Current_Pump_2 = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_CURRENT_PUMP_2);
	
	p_Voltage_Pump_1 = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_VOLTAGE_PUMP_1);
	p_Voltage_Pump_2 = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_VOLTAGE_PUMP_2);
	
	Test_pump_output();
}


void Call_Current_Pump(void)
{
	if(Current_Pump_1 !=  *p_Current_Pump_1)
	{
		if(*p_Current_Pump_1 > AGREEMENT_CURRENT_PUMP_MAX)
		{
			*p_Current_Pump_1 = Current_Pump_1;
		}
		else
		{
			Current_Pump_1 = *p_Current_Pump_1;
		}
		StartUp_PWM1(Current_Pump_1);
	}
	if(Current_Pump_2 !=  *p_Current_Pump_2)
	{
		if(*p_Current_Pump_2 > AGREEMENT_CURRENT_PUMP_MAX)
		{
			*p_Current_Pump_2 = Current_Pump_2;
		}
		else
		{
			Current_Pump_2 = *p_Current_Pump_2;
		}
		StartUp_PWM2(Current_Pump_2);
	}
}


void Call_Voltage_Pump(void)
{
	if(Voltage_Pump_1 !=  *p_Voltage_Pump_1)
	{
		if(*p_Voltage_Pump_1 > AGREEMENT_VOLTAGE_PUMP_MAX)
		{
			*p_Voltage_Pump_1 = Voltage_Pump_1;
		}
		else
		{
			Voltage_Pump_1 = *p_Voltage_Pump_1;
		}
		StartUp_DAC1(Voltage_Pump_1);
	}
	if(Voltage_Pump_2 !=  *p_Voltage_Pump_2)
	{
		if(*p_Voltage_Pump_2 > AGREEMENT_VOLTAGE_PUMP_MAX)
		{
			*p_Voltage_Pump_2 = Voltage_Pump_2;
		}
		else
		{
			Voltage_Pump_2 = *p_Voltage_Pump_2;
		}
		StartUp_DAC2(Voltage_Pump_2);
	}
}

void Call_Gear_Pump(void)
{
	if(Gear_Pump_1 !=  *p_Gear_Pump_1)
	{
		if(*p_Gear_Pump_1 > AGREEMENT_GEAR_PUMP_MAX)
		{
			*p_Gear_Pump_1 = Gear_Pump_1;
		}
		else
		{
			Gear_Pump_1 = *p_Gear_Pump_1;
			//StartUp_KA1(Gear_Pump_1);
		}
		StartUp_KA1(Gear_Pump_1);
	}
	//StartUp_KA1(Gear_Pump_1);
	if(Gear_Pump_2 !=  *p_Gear_Pump_2)
	{
		if(*p_Gear_Pump_2 > AGREEMENT_GEAR_PUMP_MAX)
		{
			*p_Gear_Pump_2 = Gear_Pump_2;
		}
		else
		{
			Gear_Pump_2 = *p_Gear_Pump_2;
			//StartUp_KA2(Gear_Pump_2);
		}
		StartUp_KA2(Gear_Pump_2);
	}
	//StartUp_KA2(Gear_Pump_2);
}

void Call_PUMP_Handle_Task(void)
{
	Call_Current_Pump();
	
	Call_Voltage_Pump();
	
	Call_Gear_Pump();
}
