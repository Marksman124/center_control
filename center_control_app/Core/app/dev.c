/**
******************************************************************************
* @file    		dev.c
* @brief   		本机信息
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "dev.h"
#include "iap.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "modbus.h"

//FLASH_ProcessTypeDef p_Flash; 
uint16_t* p_Local_Address;			//	本地地址
uint32_t* p_Software_Version;		//	软件版本

//与上位机modbus
uint16_t* p_Baud_Rate;							//	波特率
uint32_t Modbus_BaudRate_Table[] = 	{0,4800,9600,19200,115200};
#define MODBUS_BAUDRATE_TABLE_LEN		(sizeof(Modbus_BaudRate_Table)/sizeof(Modbus_BaudRate_Table[0]))
#define MODBUS_BAUDRATE_DEFAULT			4

//电表 modbus
uint16_t* p_Metering_Baud_Rate;				//	波特率
uint32_t Metering_BaudRate_Table[] = 	{1200,2400,4800,9600};
#define METERING_BAUDRATE_TABLE_LEN		(sizeof(Metering_BaudRate_Table)/sizeof(Metering_BaudRate_Table[0]))
#define METERING_BAUDRATE_DEFAULT			3


uint32_t Dev_BaudRate_Get(uint8_t usart_num)
{
	uint32_t all_usart_baudrate_table[SYSTEM_USER_USART_MAX] = {9600,115200,9600,250000,9600};//U1 ~ U5 默认波特率
	
	if(usart_num == MODBUS_USART)	// modbus
	{
		if( (*p_Baud_Rate >= MODBUS_BAUDRATE_TABLE_LEN) || (*p_Baud_Rate == 0) )
		{
			*p_Baud_Rate = MODBUS_BAUDRATE_DEFAULT;	// 默认 115200
		}
		return Modbus_BaudRate_Table[*p_Baud_Rate];
	}
	else if(usart_num == METERING_MODULE_HUART)	// metering
	{
		if( (*p_Metering_Baud_Rate >= METERING_BAUDRATE_TABLE_LEN) || (*p_Metering_Baud_Rate == 0) )
		{
			*p_Metering_Baud_Rate = METERING_BAUDRATE_DEFAULT;	// 默认值
		}
		return Metering_BaudRate_Table[*p_Metering_Baud_Rate];
	}
	else if(usart_num == DMX512_HUART)	// DMX512
	{
		return 250000;
	}
	else if( (usart_num <= SYSTEM_USER_USART_MAX) && (usart_num > 0) )
	{
		return all_usart_baudrate_table[usart_num -1];
	}
	else
		return 0;
}


void Dev_Information_Init(void)
{
	p_Local_Address = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_SLAVE_ADDRESS);
	
	p_Baud_Rate = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_BAUD_RATE);
	Dev_BaudRate_Get(MODBUS_USART);
	
	p_Software_Version = (uint32_t*)Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_DATA_ADDR_SOFTWARE_VERSION_HIGH);
	
	p_Metering_Baud_Rate = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_METERING_MODULE_BAUD);
	Dev_BaudRate_Get(METERING_MODULE_HUART);
}


uint16_t Read_Local_Address(void)
{
	return *p_Local_Address;
}

uint16_t Read_Baud_Rate(void)
{
	return *p_Baud_Rate;
}

uint32_t Read_Software_Version(void)
{
	return *p_Software_Version;
}

void Set_Local_Address(uint16_t addr)
{
	*p_Local_Address = addr;
	// 写flash
	//STMFLASH_Write(USER_FLASH_ADDR_LOCAL_ADDR, p_Local_Address, 1);
	MB_Flash_Buffer_Write();
}

void Set_Baud_Rate(uint16_t rate)
{
	*p_Baud_Rate = rate;
	// 写flash
	//STMFLASH_Write(USER_FLASH_ADDR_BAUD_RATE, p_Baud_Rate, 1);
	MB_Flash_Buffer_Write();
}

void Set_Software_Version(uint32_t version)
{
	*p_Software_Version = version;
	// 写flash
	//STMFLASH_Write(USER_FLASH_ADDR_SOFTWARE_VERSION, (uint16_t*)p_Software_Version, 2);
	MB_Flash_Buffer_Write();
}
 

