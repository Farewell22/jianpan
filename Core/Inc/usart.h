/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */
#define UART_LEN  (uint8_t)148
#define UPG_ONE_PACK_MAX_SIZE   128
extern uint8_t rx_buf[UART_LEN];
extern uint8_t rx_len;
extern uint8_t rx_data;

void receiving_data();
void uart_datadeal(void);
void uart_unpack(uint8_t *szbuf,uint8_t len);
void read_id_action(uint8_t* szData,uint8_t real_size);
void uart_send(uint8_t* data,uint8_t lens);
void fw_reply(void);
int response_msg_processing(uint8_t *buf,uint8_t len);
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

