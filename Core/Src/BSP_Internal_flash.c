#include "BSP_Internal_flash.h"
user_data_t user_data ={0};


int InternalFlash_Test(void)
{

    uint32_t add = 0x08010000;        //定义写入数据的地址
	uint32_t error = 0;
	uint64_t dat = 0x0123453337643210;//要写入的数据，必须得是双字64bit
	uint64_t read_dat = 0 ;
	FLASH_EraseInitTypeDef flash_dat;          //定义一个结构体变量，里面有擦除操作需要定义的变量

	HAL_FLASH_Unlock();                                    //第二步：解锁                      
	flash_dat.TypeErase = FLASH_TYPEERASE_PAGES;         //擦除类型是“Page Erase” 仅删除页面 另外一个参数是全部删除
	flash_dat.Page = (uint32_t)((add-0x08000000)/2048);            //擦除地址对应的页
	flash_dat.NbPages = 1;                               //一次性擦除1页,可以是任意页


	HAL_FLASHEx_Erase(&flash_dat,&error);            //第三步：参数写好后调用擦除函数
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, add, dat);//第四步：写入数据
	HAL_FLASH_Lock();                                      //第五步：上锁

	read_dat = *(__I uint64_t *)add;	   //读出flash中的数据
	uint32_t read_dat1=read_dat>>32;
	uint32_t read_dat2=read_dat&0x00000000FFFFFFFF;
	printf("[INFO] Flash_Test:0x%08x 0x%08x\n",read_dat1,read_dat2);

}



static uint32_t flash_get_page(uint32_t Addr)
{
	return (Addr-FLASH_START_ADDR) / FLASH_PAGE_SIZE;
}


static uint32_t flash_pages_count(uint32_t start_address,uint32_t end_address)
{
	uint32_t pages = (end_address - start_address) / FLASH_PAGE_SIZE == 0;
	if(pages ==0)
	{
		pages =1;
	}
	return pages;
}


uint8_t InternalFlash_erase(uint32_t start_address,uint32_t end_address)
{
	uint32_t error =0;
	FLASH_EraseInitTypeDef erase ={0};
	erase.TypeErase = FLASH_TYPEERASE_PAGES;
	erase.Page = flash_get_page(start_address);
	erase.NbPages = flash_pages_count(start_address,end_address);
    erase.Banks =FLASH_BANK_1;

	HAL_FLASH_Unlock();
	if(HAL_FLASHEx_Erase(&erase,&error)!=HAL_OK)
	{
		return FAILED;
	}
	HAL_FLASH_Lock();
	return PASSED;
}





uint8_t InternalFlash_program_row(uint32_t start_address,uint32_t end_address,uint64_t datas)
{
	uint32_t current_address = start_address;
	if(InternalFlash_erase(start_address,end_address)==FAILED)
	{
		return FAILED;
	}
	HAL_FLASH_Unlock();
	while(current_address < end_address)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, current_address, datas);
		current_address += 8;
    }
	HAL_FLASH_Lock();
	return PASSED;
}


uint8_t mcu_aprom_erase(void)
{
	if(InternalFlash_erase(APP_ADDR,BANK1_END_ADDRESS)==FAILED)
	{
		return FAILED;
	}
	else
	{
		return PASSED;
	}
}


uint8_t mcu_aprom_program(uint32_t offset, uint8_t *buff,uint8_t count)//这个时候buff的大小应该是128
{
	uint32_t data[2] = {0};
    uint64_t sum_data = 0;
	HAL_FLASH_Unlock();
	for(uint8_t i = 0; i < (count / 8); i++)
	{
		data[0] =(buff[i*8+0]<<0)+(buff[i*8+1]<<8)
            +(buff[i*8+2]<<16)+(buff[i*8+3]<<24);
        data[1] = (buff[i*8+4]<<0)+(buff[i*8+5]<<8)
            +(buff[i*8+6]<<16)+(buff[i*8+7]<<24);
        sum_data = data[1];
        sum_data = sum_data <<32;
        sum_data+= data[0];
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, APP_ADDR+ offset + 8 * i, sum_data);
	}

	HAL_FLASH_Lock();

	return PASSED;
}

uint8_t boot_mode_read(void)
{	
	uint64_t val = 0;
	val = *(__IO uint64_t *)(BOOT_MODE_ADDR);//读取对应区域存放的模式
	if(val == BOOT_MODE_RUN_UPGRADE)
		return BOOT_RUN_BOOTLOADER;
	else
		return BOOT_RUN_APPROM;
}

void Mcu_Upgrader_Flag_Clear(void)
{	
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef EraseInitStruct = {0};
    uint32_t PageError = 0;
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page        = flash_get_page(BOOT_MODE_ADDR);
    EraseInitStruct.NbPages     = 1;
    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
	HAL_FLASH_Lock();
}

/// @brief Write the upgrade flag bit to the corresponding position of FLASH
/// @param  
void Mcu_Upgrader_Func(void)
{
	HAL_FLASH_Unlock();
	uint32_t PageError = 0;
	FLASH_EraseInitTypeDef EraseInitStruct = {0};
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page        = flash_get_page(BOOT_MODE_ADDR);
    EraseInitStruct.NbPages     = 1;
	EraseInitStruct.Banks = FLASH_BANK_1;
	HAL_FLASHEx_Erase(&EraseInitStruct,&PageError);

	HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,BOOT_MODE_ADDR,BOOT_MODE_RUN_UPGRADE);
	HAL_FLASH_Unlock();
	HAL_NVIC_SystemReset();//software_reset
}


void Mcu_User_Data_Get(void)
{
	uint8_t rbuf[20] = {0} , i=0;
	uint32_t val = 0;
	for(i = 0;i < sizeof(user_data_t);i+=4)
	{
		val = *(__IO uint32_t*)(USER_DATA_ADDR + i);//每次读取4字节数据复制到缓冲区
		rt_memcpy(&rbuf[i],(uint8_t*)&val,sizeof(uint32_t));
	}
	rt_memcpy(&user_data.product_sn[0],&rbuf,sizeof(user_data_t));//再将获取到的用户数据从缓冲区拷给存储区变量
	if(user_data.st_flag != 0xaa)
	{
		user_data.st_result = 0x00;
	}
}


uint64_t addr = 0;
#define CEIL_DIV(a, b) (((a) + (b) - 1) / (b)) 
void Mcu_User_Data_Set(void)
{
    uint64_t wbuf[4] = { 0 };
    rt_memcpy(wbuf, &user_data.product_sn[0], sizeof(user_data_t));
    
    uint32_t PageError = 0;
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef EraseInitStruct = {0};
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page        = GetPage(USER_DATA_ADDR);
    EraseInitStruct.NbPages     = 1;
    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
    for (uint8_t i = 0; i < CEIL_DIV(sizeof(user_data),8); i++) {
        addr = USER_DATA_ADDR + (8*i);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, USER_DATA_ADDR + (8*i), wbuf[i]);
    }
    HAL_FLASH_Lock();
}


