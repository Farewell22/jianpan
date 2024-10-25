/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "BSP_Internal_flash.h"
#define text
#ifdef text
typedef unsigned char uint8_t;
#endif


uint8_t rx_data; //接收数杮存储坘針
uint8_t rx_len =0;//接块数杮下标
uint8_t rx_buf[UART_LEN] = {0};//接收数杮缓存
unsigned char tx_buf[50] = {0};
uint8_t cur_pos =0;
uint8_t last_pos =0;

#define MD_HEAD 0
#define MD_ID 1
#define MD_DATA 2
#define MD_TAIL 3

/*以下定义事件ID*/
#define FW_READ 0xA5
#define TEMP_READ 0xA0

#define ANS_HEAD 0xAA

#define ANS_ACK 0xfb
#define NANS_ACK 0xfc


#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1, (uint8_t*)&ch,1,HAL_MAX_DELAY);
    return ch;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    if(UartHandle->Instance == USART3){
        Usart3_receiving_data();
        HAL_UART_Receive_IT(&huart3, &rx_data, sizeof(rx_data));
    
    }
}



void Usart3_receiving_data(void)
{
	if(rx_len>=UART_LEN)
	{
		rx_len = 0;
	}
	rx_buf[rx_len] = rx_data;
	rx_len++;
}


void uart_datadeal(void)
{
	uint8_t tmp_len;
	cur_pos = rx_len;
  // printf("run deal\r\n");
	if(cur_pos >= last_pos)
	{
		tmp_len = cur_pos - last_pos;
		if(tmp_len > 0)
		{
			uart_unpack(&rx_buf[cur_pos-tmp_len],tmp_len);
		}
	}
	else
	{
		tmp_len = UART_LEN - last_pos;
		uart_unpack(&rx_buf[last_pos],tmp_len);
		tmp_len = cur_pos;
		uart_unpack(&rx_buf[0],tmp_len);

	}
	last_pos = cur_pos;
}

static unsigned char cStatus = MD_HEAD;
void uart_unpack(uint8_t *szbuf,uint8_t len)
{
  // printf("unpack\r\n");
	int i = 0;
	int j = 0;
	int k = 0;
	static uint8_t szID[2];//用数组存放帧长度-事件
	static uint8_t szData[20];
	static int iReadLen = 0;
	unsigned char c;
	unsigned char sum_crc;//存放校验�????
	unsigned char event_id;
	unsigned char tmp_buf[6] = {0};

	for(i=0;i<len;i++)
	{
		c = szbuf[i];
		switch (cStatus)
		{
		case MD_HEAD:			
    if(c==0xAA)
			{
				cStatus++;
				iReadLen =0;
			}
			break;
		case MD_ID:
			szID[iReadLen++]=c;//读一�???ID坳坯
			if(iReadLen == 1)
			{
				cStatus++;
				iReadLen =0;
			}
			break;
		case MD_DATA:
			szData[iReadLen++] =c;
			if(iReadLen == szID[0]-1)//读至长度坎结�????
			{
				cStatus++;
				iReadLen =0;
			}
			break;
		case MD_TAIL:
			if(c==0xbb)//接收到帧尾时�????验数�???校验�???,如果戝功，则处睆数杮
			{
				sum_crc =0;
				for(j=0;j<(szID[0]-2);j++)
				{
					sum_crc += szData[j];//����У���??
				}
				if(sum_crc == szData[szID[0]-2])
				{
					printf("success\r\n");
					// event_id = szData[0];
					read_id_action(&szData[0],szID[0]);
				}
				  else
				  {
					printf("crc error\r\n");
			  	}
				
			  }
			  else 
			  {
			   	printf("tail error\r\n");
			  }
				cStatus = MD_HEAD;
				break;
		default:
				cStatus = MD_HEAD;
				break;
		}
	}

}


/// @brief 接收戝功坎的事件判断和从机回�????
static uint8_t check_sum(uint8_t *dat,uint8_t len)
{
     int i = 0;
     unsigned char check_sum = 0;

      for (i = 0; i < len;i++) 
      {
        check_sum += dat[i];
      }

    return check_sum;
}
/// @param eventid 
// #define DEBUG
#ifdef DEBUG
// void read_id_action(uint8_t* szData)
// {
// 	if(szData[0] == FW_READ)
// 	{
// 		fw_reply();
// 	}
// }


// unsigned char fw_ver = 1;
// void fw_reply(void)
// {
// 	tx_buf[0] = ANS_HEAD;
// 	tx_buf[1] = 4;
// 	tx_buf[2] = ANS_ACK;
// 	tx_buf[3] = fw_ver;
// 	tx_buf[4] = 0x00;//CRC
// 	for(int i =0;i<4;i++)
// 	{
// 		tx_buf[4] +=tx_buf[i];
// 	}
// 	tx_buf[5] = 0xbb;
// 	uart_send(tx_buf,6);
//   printf("done\n");

// }
#endif

uint8_t g_mcu_start_upgrader_flag  = 0;
void read_id_action(uint8_t* szData,uint8_t real_size)
{
  uint8_t tbuf[20] = { 0 };
  uint8_t tlen = 0;
  uint8_t mcu_id = 0;
  uint8_t pack_num = 0;
	switch (szData[0])
  {
  case MCU_MODEL_ID_GET:
    tbuf[0]  = MCU_MODEL_ID_GET;
    tbuf[1] =  MCU_MODEL_ID;
    break;
  case MCU_WORK_MODE_GET:
    tbuf[0] = MCU_WORK_MODE_GET;
    tbuf[1] = BOOT_RUN_BOOTLOADER;
    break;
  case MCU_SOFT_VER_GET:
    tbuf[0] = MCU_SOFT_VER_GET;
    tbuf[1] = MCU_SOFT_VER;
    break;
  case MCU_START_UPGRADER_SET://����MCU������־λ
    mcu_id = szData[1];
    tbuf[0] = MCU_START_UPGRADER_SET;
     if(mcu_id == MCU_MODEL_ID)
     {
        g_mcu_start_upgrader_flag = 1;
        tbuf[1] = 0x01;
      }else
      {
        tbuf[1] = 0x00;
      }
     break;
   case APP_FLASH_ERASE: //��λ�����Ͳ���ָ��
      if(g_mcu_start_upgrader_flag == 0)
      {
        return;
      }
      tbuf[0] = APP_FLASH_ERASE;
      tbuf[1] = mcu_aprom_erase();
      break;
    case APP_FLASH_PROGRAM:
      pack_num = szData[1];
      tbuf[0] = APP_FLASH_PROGRAM;
      tbuf[1] = mcu_aprom_program(pack_num * UPG_ONE_PACK_MAX_SIZE,
                                &szData[2],real_size);
      break;
    case APP_FLASH_UPGRADE_OK:
      Mcu_Upgrader_Flag_Clear();//�����־λ
      IAP_ExecuteApp(APP_ADDR);
      break; 
    default :
       break; 
  }

  tlen = response_msg_processing(&tbuf[0],2);
  uart_send(tx_buf,tlen);//�ظ�����
}




int response_msg_processing(uint8_t *buf,uint8_t len)
{
	int txlen = 0;

   tx_buf[0] = ANS_HEAD;
   tx_buf[1] = 3 + len;
   tx_buf[2] = ANS_ACK;
    rt_memcpy(&tx_buf[3], buf, len);
   tx_buf[3 + len] = check_sum(&tx_buf[2], len + 1);
   tx_buf[3 + len + 1] = 0xbb;

   txlen = len + 5;

    return txlen;
}

void uart_send(uint8_t* data,uint8_t lens)
{
	if(data==NULL||lens==0)
	{
		return;
	}
	for(int i =0;i<lens;i++)
	{
		HAL_UART_Transmit(&huart3,&data[i],1,HAL_MAX_DELAY);
	}
	return;
}





/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PC4     ------> USART1_TX
    PC5     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3;
    PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB0     ------> USART3_RX
    PB2     ------> USART3_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_4_5_6_LPUART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_4_5_6_LPUART1_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PC4     ------> USART1_TX
    PC5     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_4|GPIO_PIN_5);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB0     ------> USART3_RX
    PB2     ------> USART3_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_2);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_4_5_6_LPUART1_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
