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
extern void HardFedDog(uint8_t* FedDog);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RELAY_CP_Pin GPIO_PIN_2
#define RELAY_CP_GPIO_Port GPIOE
#define RELAY_Q_Pin GPIO_PIN_3
#define RELAY_Q_GPIO_Port GPIOE
#define SIGNAL_Pin GPIO_PIN_6
#define SIGNAL_GPIO_Port GPIOA
#define SENSV_Pin GPIO_PIN_4
#define SENSV_GPIO_Port GPIOC
#define DELAY_Pin GPIO_PIN_5
#define DELAY_GPIO_Port GPIOC
#define SVTY_Pin GPIO_PIN_0
#define SVTY_GPIO_Port GPIOB
#define EEPROM_SCL_Pin GPIO_PIN_13
#define EEPROM_SCL_GPIO_Port GPIOB
#define EEPROM_SDA_Pin GPIO_PIN_14
#define EEPROM_SDA_GPIO_Port GPIOB
#define RELAY_CTRL_Pin GPIO_PIN_15
#define RELAY_CTRL_GPIO_Port GPIOA
#define RELAY_LED_Pin GPIO_PIN_3
#define RELAY_LED_GPIO_Port GPIOD
#define POWER_LED_Pin GPIO_PIN_4
#define POWER_LED_GPIO_Port GPIOD
#define SIGNAL_LED_Pin GPIO_PIN_5
#define SIGNAL_LED_GPIO_Port GPIOD
#define WDI_Pin GPIO_PIN_4
#define WDI_GPIO_Port GPIOB
#define LCD_SCL_Pin GPIO_PIN_6
#define LCD_SCL_GPIO_Port GPIOB
#define LCD_SDA_Pin GPIO_PIN_7
#define LCD_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

#define DMA_BUFF_SIZE                   625
extern uint32_t     SensorResultDMA[DMA_BUFF_SIZE];

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
