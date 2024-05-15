/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "iap.h"
#include "iwdg.h"
#include "gpio.h"
#include "pump.h"
#include "dev.h"
#include "modbus.h"
#include "subsystem.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId Subsystem_TaskHandle;
osThreadId SOC_Monitor_TasHandle;
osThreadId Main_TaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Subsystem_Handler(void const * argument);
void SOC_Monitor_Handler(void const * argument);
void Main_Handler(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationTickHook(void);

/* USER CODE BEGIN 3 */
__weak void vApplicationTickHook( void )
{
   /* This function will be called by each tick interrupt if
   configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
   added here, but the tick hook is called from an interrupt context, so
   code must not attempt to block, and only the interrupt safe FreeRTOS API
   functions can be used (those that end in FromISR()). */
}
/* USER CODE END 3 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Subsystem_Task */
  osThreadDef(Subsystem_Task, Subsystem_Handler, osPriorityNormal, 0, 128);
  Subsystem_TaskHandle = osThreadCreate(osThread(Subsystem_Task), NULL);

  /* definition and creation of SOC_Monitor_Tas */
  osThreadDef(SOC_Monitor_Tas, SOC_Monitor_Handler, osPriorityAboveNormal, 0, 256);
  SOC_Monitor_TasHandle = osThreadCreate(osThread(SOC_Monitor_Tas), NULL);

  /* definition and creation of Main_Task */
  osThreadDef(Main_Task, Main_Handler, osPriorityBelowNormal, 0, 128);
  Main_TaskHandle = osThreadCreate(osThread(Main_Task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Subsystem_Handler */
/**
  * @brief  Function implementing the Subsystem_Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Subsystem_Handler */
void Subsystem_Handler(void const * argument)
{
  /* USER CODE BEGIN Subsystem_Handler */
	Subsystem_Init();
  /* Infinite loop */
  while(1)
  {
		Subsystem_Handler_Task();
    osDelay(THREAD_PERIOD_DMX512_HUART);
  }
  /* USER CODE END Subsystem_Handler */
}

/* USER CODE BEGIN Header_SOC_Monitor_Handler */
/**
* @brief Function implementing the SOC_Monitor_Tas thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SOC_Monitor_Handler */
void SOC_Monitor_Handler(void const * argument)
{
  /* USER CODE BEGIN SOC_Monitor_Handler */
	Modbus_Init();
	
  /* Infinite loop */
  while(1)
  {
		Modbus_Handle_Task();
		
    osDelay(THREAD_PERIOD_MODBUS_USART);
#ifndef SYSTEM_HARDWARE_DEBUG
            led_off();
#endif
  }
  /* USER CODE END SOC_Monitor_Handler */
}

/* USER CODE BEGIN Header_Main_Handler */
/**
* @brief Function implementing the Main_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Main_Handler */
void Main_Handler(void const * argument)
{
  /* USER CODE BEGIN Main_Handler */
	
	Call_PUMP_Handle_Init();
	Set_Software_Version(SOFTWARE_VERSION_UINT32);
	
  /* Infinite loop */
  while(1)
  {
		HAL_IWDG_Refresh(&hiwdg);
#ifdef SYSTEM_HARDWARE_DEBUG
		PUMP_Hardware_Debug();
#else
		Call_PUMP_Handle_Task();
		StartUp_Pump(0, 1);
#endif
		osDelay(THREAD_PERIOD_MAIN_PUMP_TASK);
  }
	
  /* USER CODE END Main_Handler */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

