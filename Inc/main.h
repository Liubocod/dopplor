/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef enum
{
  FALSE = 0,TRUE = 1
}bool8;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SIGNAL_Pin GPIO_PIN_6
#define SIGNAL_GPIO_Port GPIOA
#define RELAY_LED_Pin GPIO_PIN_3
#define RELAY_LED_GPIO_Port GPIOD
#define POWER_LED_Pin GPIO_PIN_4
#define POWER_LED_GPIO_Port GPIOD
#define SIGNAL_LED_Pin GPIO_PIN_5
#define SIGNAL_LED_GPIO_Port GPIOD
#define WDI_Pin GPIO_PIN_4
#define WDI_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
typedef enum 
{
  FLOW_STATE_LOAD,
  NO_FLOW_STATE_LOAD,
  SENSOR_ABNORMAL,
}SystemStateMachineDef;
#define DMA_BUFF_SIZE                   625
extern uint32_t     SensorResultDMA[DMA_BUFF_SIZE];

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
