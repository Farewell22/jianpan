#ifndef _BSP_INTERNAL_FLASH
#define _BSP_INTERNAL_FLASH
#include "main.h"
//写入的起始地址与结束地址
#define BANK1_END_ADDRESS ((uint32_t)0x08040000)
#define FLASH_START_ADDR ((uint32_t)0x08000000)

#define BOOTLOADER_SIZE ((uint32_t)13 * FLASH_PAGE_SIZE)
#define BOOT_MODE_ADDR ((uint32_t)(FLASH_START_ADDR + BOOTLOADER_SIZE))//将Bootloader模式放在bootloader区的末尾

#define ENV_SIZE ((uint32_t)7 * FLASH_PAGE_SIZE)
#define APP_ADDR ((uint32_t)(FLASH_START_ADDR + BOOTLOADER_SIZE + ENV_SIZE))

#define USER_DATA_ADDR      FLASH_START_ADDR + BOOTLOADER_SIZE + (1* FLASH_PAGE_SIZE)
#define RLY_STATE_STORE_START      USER_DATA_ADDR + (2* FLASH_PAGE_SIZE)
#define RLY_STATE_STORE_END         USER_DATA_ADDR + (4*FLASH_PAGE_SIZE);

typedef struct __user_data_t
{
    uint8_t product_sn[13];
    uint8_t st_flag;
    uint8_t press_flag;
    uint8_t presstest_result;
    int st_result;
}user_data_t;

extern user_data_t user_data;
extern uint16_t g_rly_store_status;
extern uint64_t g_rly_store_flag;



int InternalFlash_Test(void);
uint8_t InternalFlash_program_row(uint32_t start_address,uint32_t end_address,uint64_t datas);
uint8_t InternalFlash_erase(uint32_t start_address,uint32_t end_address);
uint8_t mcu_aprom_erase(void);
uint8_t mcu_aprom_program(uint32_t offset, uint8_t *buff,uint8_t count);
void Mcu_Upgrader_Flag_Clear(void);
uint8_t boot_mode_read(void);
#endif // !_BSP_INTERNAL_FLASH
