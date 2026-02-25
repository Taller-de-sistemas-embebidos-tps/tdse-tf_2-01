/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define LED_KID_Pin GPIO_PIN_1
#define LED_KID_GPIO_Port GPIOB
#define LED_ADULT_Pin GPIO_PIN_2
#define LED_ADULT_GPIO_Port GPIOB
#define LED_ALARM_Pin GPIO_PIN_10
#define LED_ALARM_GPIO_Port GPIOB
#define LED_BL_Pin GPIO_PIN_11
#define LED_BL_GPIO_Port GPIOB
#define DISPLAY_RS_Pin GPIO_PIN_12
#define DISPLAY_RS_GPIO_Port GPIOB
#define DISPLAY_D6_Pin GPIO_PIN_13
#define DISPLAY_D6_GPIO_Port GPIOB
#define DISPLAY_D7_Pin GPIO_PIN_14
#define DISPLAY_D7_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_7
#define BUZZER_GPIO_Port GPIOC
#define DISPLAY_EN_Pin GPIO_PIN_11
#define DISPLAY_EN_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define BTN_MODO_Pin GPIO_PIN_10
#define BTN_MODO_GPIO_Port GPIOC
#define BTN_MODO_EXTI_IRQn EXTI15_10_IRQn
#define BTN_ALARM_Pin GPIO_PIN_11
#define BTN_ALARM_GPIO_Port GPIOC
#define BTN_ALARM_EXTI_IRQn EXTI15_10_IRQn
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define DISPLAY_D5_Pin GPIO_PIN_4
#define DISPLAY_D5_GPIO_Port GPIOB
#define DISPLAY_D4_Pin GPIO_PIN_5
#define DISPLAY_D4_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
