/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    debug_protocol.h
  * @brief   This file contains all the function prototypes for
  *          the debug_protocol.c file
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEBUG_PROTOCOL_H__
#define __DEBUG_PROTOCOL_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "modbus.h"


void UART_Send_Debug(uint8_t * p_buff, uint8_t len, uint8_t cmd);
void To_Debug_Protocol_Analysis(uint8_t* p_buf, uint8_t len);


#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_PROTOCOL_H__ */

