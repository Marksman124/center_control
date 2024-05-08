/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

//#define SYSTEM_HARDWARE_DEBUG
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
// 串口1 --> 计量模块(485)
// 串口2 --> 核心板(485)
// 串口3 --> 预留(232)
// 串口4 --> DMX512(485)
// 串口5 --> 预留(232)
#define MODBUS_USART								2		//2  //(A3)  3备用
#define	METERING_MODULE_HUART				4		//(A1) 	原来 1 
#define	DMX512_HUART								1 	//(A2)	原来 4

#if MODBUS_USART == 2
#define	DEBUG_HUART									3 	//
#endif

#define	SYSTEM_USER_USART_MAX				5

#define THREAD_PERIOD_MODBUS_USART								10
#define THREAD_PERIOD_MAIN_PUMP_TASK							10
#define	THREAD_PERIOD_METERING_MODULE_HUART				500
#define	THREAD_PERIOD_DMX512_HUART								50

// 软件版本
#define	SOFTWARE_VERSION_UINT32								0x00022505		//2.37.5

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RS48501_RE_Pin GPIO_PIN_8
#define RS48501_RE_GPIO_Port GPIOA
#define RS48504_RE_Pin GPIO_PIN_12
#define RS48504_RE_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
