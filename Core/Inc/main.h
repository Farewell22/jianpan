/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "strutil.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define BOOT_MODE_RUN_UPGRADE ((uint64_t)0xaaaaaaaaaaaaaaaa)

#define BOOT_RUN_BOOTLOADER 0
#define BOOT_RUN_APPROM 1

#define PRESSTEST_ON 0
#define PRESSTEST_OFF 1



#define PASSED 1
#define FAILED 0
/****************** ��Ƭ������ģʽ ********************/
#define	MCU_WORK_MODE_GET		0x31
/****************** ��Ƭ���汾��ȡ ********************/
#define MCU_SOFT_VER_GET		0x32
/****************** SN��д ********************/
#define MCU_SN_SET				0x43
/****************** SN��ȡ ********************/
#define MCU_SN_GET				0x44
/****************** �����˳� ********************/
#define MCU_PRESSTEST_EXIT_SET		0x45
/****************** ���������ȡ ********************/

#define MCU_PRESSTEST_RESULT_GET	0x46

#define MCU_PRESSTEST_START_SET		0x61
#define MCU_TEST_RESULT_RECORD		0x62
#define MCU_TEST_RESULT_GET			0x63
#define MCU_MODEL_ID_GET			0xfe

#define MCU_SOFT_VER			0x01
#define MCU_MODEL_ID			0x51

#define APP_FLASH_ERASE			0xbb
#define APP_FLASH_PROGRAM		0xdd
#define APP_FLASH_UPGRADE_OK	0xaa
#define MCU_START_UPGRADER_SET	0xfc
void IAP_ExecuteApp(uint32_t App_Addr);
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
