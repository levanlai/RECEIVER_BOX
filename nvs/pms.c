#include <system.h>
#include <trace.h>
#include "nvs.h"
#include "../sys/sys_flash.h"


#define FLASH_SIZE_WORDS   		(DWORD)(((DWORD)2 << 20) / 16) 	   				// W25X20CLSNIG(2Mbit)
#define FLASH_SIZE_SECTORS      (WORD)(FLASH_SIZE_WORDS / FLASH_SECTOR_SIZE) 	//
#define FLASH_USED_SECTORS		3

static struct nvs_fs g_nvs_fs = {
	(FLASH_SIZE_SECTORS - FLASH_USED_SECTORS ) << FLASH_ADDR_SECT_SHIFT ,//offset 
	0							,//ate_wra 
	0							,//data_wra 
	FLASH_SECTOR_SIZE			,//sector_size 
	FLASH_USED_SECTORS			,//sector_count 
	FALSE						,//ready 
	FALSE						,//lock 
};


int pms_init(void)
{
    mpspi_init(0);
    return nvs_init(&g_nvs_fs);
}

int pms_get_word(WORD key, WORD *val)
{
    return nvs_read(&g_nvs_fs, key, val, sizeof(WORD));
}

int pms_set_word(WORD key, WORD val)
{
    return nvs_write(&g_nvs_fs, key, (void *)&val, sizeof(WORD));
}

int pms_get_dword(WORD key, DWORD *val)
{
    return nvs_read(&g_nvs_fs, key, val, sizeof(DWORD));
}

int pms_set_dword(WORD key, DWORD val)
{
    return nvs_write(&g_nvs_fs, key, (void *)&val, sizeof(DWORD));
}

int pms_get_bufs(WORD key, WORD *val, WORD nwords)
{
    return nvs_read(&g_nvs_fs, key, val, nwords);
}

int pms_set_bufs(WORD key, WORD *val, WORD nwords)
{
    return nvs_write(&g_nvs_fs, key, (void *)val, nwords);
}