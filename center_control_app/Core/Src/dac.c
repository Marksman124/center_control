/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dac.c
  * @brief   This file provides code for the configuration
  *          of the DAC instances.
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "dac.h"

/* USER CODE BEGIN 0 */
#include "tim.h"
#include "Math.h"
/* USER CODE END 0 */

DAC_HandleTypeDef hdac;

/* DAC init function */
void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */

  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT2 config
  */
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

void HAL_DAC_MspInit(DAC_HandleTypeDef* dacHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(dacHandle->Instance==DAC)
  {
  /* USER CODE BEGIN DAC_MspInit 0 */

  /* USER CODE END DAC_MspInit 0 */
    /* DAC clock enable */
    __HAL_RCC_DAC_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**DAC GPIO Configuration
    PA4     ------> DAC_OUT1
    PA5     ------> DAC_OUT2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN DAC_MspInit 1 */
		HAL_DAC_Start(&hdac,DAC1_CHANNEL_1);
		HAL_DAC_Start(&hdac,DAC1_CHANNEL_2);
  /* USER CODE END DAC_MspInit 1 */
  }
}

void HAL_DAC_MspDeInit(DAC_HandleTypeDef* dacHandle)
{

  if(dacHandle->Instance==DAC)
  {
  /* USER CODE BEGIN DAC_MspDeInit 0 */

  /* USER CODE END DAC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DAC_CLK_DISABLE();

    /**DAC GPIO Configuration
    PA4     ------> DAC_OUT1
    PA5     ------> DAC_OUT2
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_5);

  /* USER CODE BEGIN DAC_MspDeInit 1 */
		
  /* USER CODE END DAC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
uint16_t Dat[100];

// ÕýÏÒ
//void SineWave()
//{
//	uint16_t i;
//	for(i=0; i<100; i++)
//	{
//		Dat[i] = (uint16_t)((2047*sin(i*2*3.1416/100))+2048);
//	}
//}
//	
//void StartUp_DAC1(uint16_t para)
//{
//	SineWave();
//	HAL_TIM_Base_Start(&htim4);
//	HAL_DAC_Start_DMA(&hdac,DAC1_CHANNEL_1,(uint32_t*)Dat,100,DAC_ALIGN_12B_R);
//}

//void StartUp_DAC2(uint16_t para)
//{
//	SineWave();
//	HAL_TIM_Base_Start(&htim4);
//	HAL_DAC_Start_DMA(&hdac,DAC1_CHANNEL_2,(uint32_t*)Dat,100,DAC_ALIGN_12B_R);
//}

void StartUp_DAC1(uint16_t vol)
{
//	uint16_t data;
//	uint16_t ad_vol;
	
	//ad_vol = vol/4095*100;
	//data = (uint16_t) ((ad_vol/33) * 4095);
	HAL_DAC_SetValue(&hdac,DAC1_CHANNEL_1,DAC_ALIGN_12B_R,vol);
}

void StartUp_DAC2(uint16_t vol)
{
	//uint16_t data;
	//data = (uint16_t)((vol/3.3) *4095);
	HAL_DAC_SetValue(&hdac,DAC1_CHANNEL_2,DAC_ALIGN_12B_R,vol);
	
}


/* USER CODE END 1 */
