#ifndef __BK9532_I2C_TRANSFER_H__
#define __BK9532_I2C_TRANSFER_H__

#include <system.h>

#define BK9532_I2C_ADDR 0x26

WORD bk9532_reg_read(WORD bus, WORD reg, DWORD *reg_val);
WORD bk9532_reg_write(WORD bus, WORD reg, DWORD reg_val);
WORD bk9532_reg_or_mask(WORD bus, WORD reg, DWORD mask);
WORD bk9532_reg_and_mask(WORD bus, WORD reg, DWORD mask);
WORD bk9532_reg_and_mask_or_flag(WORD bus, WORD reg, DWORD mask, DWORD flag);

#endif