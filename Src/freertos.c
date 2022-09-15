/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "doppler.h"
#include "adc.h"
#include "sensitivity.h"
#include "limits.h"
#include "EEPROM.h"
#include "i2c.h"
#include "Lcd.h"
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
osThreadId defaultTaskHandle;
osThreadId EnergyCalculateHandle;
osThreadId OutputControlHandle;
osThreadId LCD_CommunicatiHandle;
osMessageQId myQueue01Handle;
osSemaphoreId ADCReceivecpltHandle;
osSemaphoreId I2c2WaitHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void EnergyCalculateFunction(void const * argument);
void OutputControlFunction(void const * argument);
void LCD_Handler(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

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

  /* Create the semaphores(s) */
  /* definition and creation of ADCReceivecplt */
  osSemaphoreDef(ADCReceivecplt);
  ADCReceivecpltHandle = osSemaphoreCreate(osSemaphore(ADCReceivecplt), 1);

  /* definition and creation of I2c2Wait */
  osSemaphoreDef(I2c2Wait);
  I2c2WaitHandle = osSemaphoreCreate(osSemaphore(I2c2Wait), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of myQueue01 */
  osMessageQDef(myQueue01, 1, uint32_t);
  myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityLow, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of EnergyCalculate */
  osThreadDef(EnergyCalculate, EnergyCalculateFunction, osPriorityRealtime, 0, 128);
  EnergyCalculateHandle = osThreadCreate(osThread(EnergyCalculate), NULL);

  /* definition and creation of OutputControl */
  osThreadDef(OutputControl, OutputControlFunction, osPriorityBelowNormal, 0, 128);
  OutputControlHandle = osThreadCreate(osThread(OutputControl), NULL);

  /* definition and creation of LCD_Communicati */
  osThreadDef(LCD_Communicati, LCD_Handler, osPriorityNormal, 0, 128);
  LCD_CommunicatiHandle = osThreadCreate(osThread(LCD_Communicati), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  //系统默认信号量初始为释放状态
  osSemaphoreWait(I2c2WaitHandle, osWaitForever);
  EEP_ParamInit();
  HAL_ADC_Start_IT(&hadc2);
  if(HAL_I2C_Master_Receive_DMA(&hi2c1, (uint16_t)LCD_ADDRESS, (uint8_t *)G_LcdDataBuffer.LCDBufferC, LCDBUFFERSIZE) != HAL_OK)
  {

  }

  uint8_t FedDog = 0;
  /* Infinite loop */
  for(;;)
  {
    //硬件看门狗喂狗
    HardFedDog((uint8_t*)FedDog);
    osDelay(10);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_EnergyCalculateFunction */
/**
* @brief Function implementing the EnergyCalculate thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_EnergyCalculateFunction */
void EnergyCalculateFunction(void const * argument)
{
  /* USER CODE BEGIN EnergyCalculateFunction */
  int32_t result = os_status_reserved;
  float EnergyValue = 0.0;
  HAL_ADC_Start_DMA(&hadc1, SensorResultDMA, DMA_BUFF_SIZE);
  //系统默认信号量初始为释放状态
  osSemaphoreWait(ADCReceivecpltHandle, osWaitForever);
  /* Infinite loop */
  for(;;)
  {
    result = osSemaphoreWait(ADCReceivecpltHandle, osWaitForever);
    if(result == osOK)
    {
      EnergyValue = SensorDataHandle();
      result = -1;
      G_SystemStateMachine = SensorSignalEnergyAnalysis(EnergyValue);
      xTaskNotify(OutputControlHandle, (uint32_t)EnergyValue, eSetValueWithOverwrite );
      HAL_ADC_Start_DMA(&hadc1, SensorResultDMA, DMA_BUFF_SIZE);
    }
    osDelay(10);
  }
  /* USER CODE END EnergyCalculateFunction */
}

/* USER CODE BEGIN Header_OutputControlFunction */
/**
* @brief Function implementing the OutputControl thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_OutputControlFunction */
void OutputControlFunction(void const * argument)
{
  /* USER CODE BEGIN OutputControlFunction */
  uint32_t value = 0;
  uint32_t EnergyValue = 0;
  BaseType_t result;
  /* Infinite loop */
  for(;;)
  {
    result = xTaskNotifyWait( 0, ULONG_MAX, &EnergyValue,1);
    if(result == pdPASS)
    {
      value = LedFlashingFrequencyGet(EnergyValue);
      OutputSignalHandle(G_SystemStateMachine);
    }
    DopplerEnergyFlashing(value);
  }
  /* USER CODE END OutputControlFunction */
}

/* USER CODE BEGIN Header_LCD_Handler */
/**
* @brief Function implementing the LCD_Communicati thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LCD_Handler */
void LCD_Handler(void const * argument)
{
  /* USER CODE BEGIN LCD_Handler */
  /* Infinite loop */
  for(;;)
  {

    osDelay(300);
  }
  /* USER CODE END LCD_Handler */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
