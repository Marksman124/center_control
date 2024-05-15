/**
******************************************************************************
* @file    		modbus.h
* @brief   		modbus协议
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CRC_H__
#define __CRC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "mb.h"
#include "mbport.h"
#include "usart.h"
#include "iap.h"
#include "string.h"
#include "gpio.h"

#include "metering.h" // 消息队列
/* Private defines -----------------------------------------------------------*/

// 03 Holding

#define MB_DATA_ADDR_SLAVE_ADDRESS					( 0x0 )
#define MB_DATA_ADDR_BAUD_RATE             	( 0x1 )
#define MB_DATA_ADDR_GEAR_PUMP_1          	( 0x10 )
#define MB_DATA_ADDR_GEAR_PUMP_2  					( 0x11 )
#define MB_DATA_ADDR_CURRENT_PUMP_1     		( 0x12 )
#define MB_DATA_ADDR_CURRENT_PUMP_2      		( 0x13 )
#define MB_DATA_ADDR_VOLTAGE_PUMP_1        	( 0x14 )
#define MB_DATA_ADDR_VOLTAGE_PUMP_2        	( 0x15 )
//------- 计量模块
#define MB_DATA_ADDR_CONNECTION_MODE        	( 0x20 )
#define MB_DATA_ADDR_METERING_MODULE_CLEAN		( 0x21 )
#define MB_DATA_ADDR_METERING_MODULE_ADDR   	( 0x22 )
#define MB_DATA_ADDR_METERING_MODULE_BAUD   	( 0x23 )
#define MB_DATA_ADDR_METERING_MODULE_FORMAT   ( 0x24 )

#define MB_DATA_ADDR_IO_HARDWARE_CTRL   		( 0x25 )
//------- DMX
#define REG_DATA_ADDR_DMX512_LENTH 					( 0x30 )
#define REG_DATA_ADDR_DMX512_START 					( 0x40 )
#define REG_DATA_ADDR_DMX512_END 						( 0x013F )


// 04 Input
#define MB_DATA_ADDR_SOFTWARE_VERSION_HIGH       	( 0x00 )
#define MB_DATA_ADDR_SOFTWARE_VERSION_LOW        	( 0x01 )
#define MB_DATA_ADDR_WATER_LEVEL_SWITCH_STATE     ( 0x02 )
#define MB_DATA_ADDR_LINE_FREQUENCY_HIGH     			( 0x10 )
#define MB_DATA_ADDR_LINE_FREQUENCY_LOW     			( 0x11 )
#define MB_DATA_ADDR_MODULE_STATE_01     					( 0x12 )
#define MB_DATA_ADDR_MODULE_STATE_02     					( 0x13 )
#define MB_DATA_ADDR_TOTAL_ACTIVE_POWER_HIGH     	( 0x14 )
#define MB_DATA_ADDR_TOTAL_ACTIVE_POWER_LOW     	( 0x15 )


#define MB_DATA_ADDR_TOTAL_ACTIVE_POWER_HIGH     		( 0x14 )

#define REG_DATA_ADDR_IRDOPTO_START 							( 0x0010 )
#define REG_DATA_ADDR_IRDOPTO_END 								( 0x002F )
/* Exported functions prototypes ---------------------------------------------*/

extern void Modbus_Init(void);
extern void Modbus_Handle_Task(void);

// *********  修改内部数据接口  *************************************
extern void Modbus_Buffer_Init(void);
extern void MB_Flash_Buffer_Write(void);
extern uint16_t* Get_DataAddr_Pointer(UCHAR ucFunctionCode, USHORT addr);
extern uint16_t Get_DataAddr_Value(UCHAR ucFunctionCode, USHORT addr);
extern void Set_DataAddr_Value(UCHAR ucFunctionCode, USHORT addr, uint16_t value);
extern void Set_DataValue_U32(UCHAR ucFunctionCode, USHORT addr, uint32_t value);
	
extern void Set_Dmx512_Data(uint8_t* p_buff, uint16_t len);
extern uint8_t Get_Dmx512_Data_Change(void);
extern void Set_Dmx512_Data_Change(uint8_t type);


#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */

