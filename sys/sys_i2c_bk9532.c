#include <system.h>
#include "sys_i2c.h"
#include "sys_i2c_bk9532.h"
#include <trace.h>
/**
 * @brief i2c send command to bk9532 device
 * 
 * @param obj i2c bus get from function i2c_get_bus(index)
 * @param addr bk9532 address
 * @param reg  bk9532 register
 * @param data data to send
 * @return WORD status
 */
WORD i2c_send_to_bk9532(i2c_t *obj, WORD addr, WORD reg, DWORD data)
{
    WORD state = I2C_ERR_BUS_BUSY;
    WORD i;
    if(obj->is_busy)    return I2C_ERR_BUS_BUSY;
    obj->is_busy = 1;
    i2c_start_bit(obj);

    // send address 
    state = i2c_send_byte(obj, addr);

    if(state != I2C_ACK)
    {   
        state = I2C_ERR_ACK;
        goto end;
    }
    
    // send register << 1 | write bit 
    state = i2c_send_byte(obj, reg << 1 | I2C_WRITE_BIT);

    if(state != I2C_ACK)
    {   
        state = I2C_ERR_ACK;
        goto end;
    }

    for( i = 0; i < 4; i++)
    {
        state = i2c_send_byte(obj, (WORD)(data >> (24 - i*8)));
        if(state!= I2C_ACK)
        {   
            state = I2C_ERR_ACK;
            goto end;
        }
    }

    state = I2C_OK;


end: 
    i2c_stop_bit(obj);
    obj->is_busy = 0;
    return state;
}

/**
 * @brief i2c receive command from bk9532 device
 * 
 * @param obj i2c bus get from fundtion i2c_get_bus(index)
 * @param addr bk9532 address
 * @param reg  bk9532 register
 * @param data data to receive
 * @return WORD status
 */
static WORD i2c_receive_from_bk9532(i2c_t *obj, WORD addr, WORD reg, DWORD *data)
{
    WORD state = I2C_ERR_BUS_BUSY;
    WORD rbyte = 0;
    WORD i;
    if(obj->is_busy)    return state;
    obj->is_busy = 1;
    i2c_start_bit(obj);

    // send address 
    state = i2c_send_byte(obj, addr);

    if(state != I2C_ACK)
    {   
        state = I2C_ERR_ACK;
        goto end;
    }
    
    // send register << 1 | write bit 
    state = i2c_send_byte(obj, reg << 1 | I2C_READ_BIT);

    if(state != I2C_ACK)
    {   
        state = I2C_ERR_ACK;
        goto end;
    }

    *data = 0;

    for( i = 0; i < 4; i++)
    {
        rbyte = i2c_receive_byte(obj, i != 3 ? 1 : 0);
        *data |= (((DWORD)rbyte) << ( 24 - i * 8));
    }

    state = I2C_OK;

end: 
    i2c_stop_bit(obj);
    obj->is_busy = 0;
    return state;
}

/**
 * @brief read register from bk9532 
 * 
 * @param bus i2c bus 
 * @param reg register of bk9532
 * @param reg_val value of bk9532's register 
 * @return WORD state bus i2c 
 */
WORD bk9532_reg_read(WORD bus, WORD reg, DWORD *reg_val)
{
    i2c_t *i2c_bus = i2c_get_bus(bus);
    if (i2c_bus == NULL) {
        return I2C_ERR_BUS_NOT_EXIST;
    }
    return i2c_receive_from_bk9532(i2c_bus, BK9532_I2C_ADDR, reg, reg_val);
}

/**
 * @brief write data to register of bk9532 
 * 
 * @param bus i2c bus 
 * @param reg register of bk9532
 * @param reg_val value of bk9532's register 
 * @return WORD state bus i2c 
 */
WORD bk9532_reg_write(WORD bus, WORD reg, DWORD reg_val)
{
    i2c_t *i2c_bus = i2c_get_bus(bus);
    if (i2c_bus == NULL) {
        return I2C_ERR_BUS_NOT_EXIST;
    }
    return i2c_send_to_bk9532(i2c_bus, BK9532_I2C_ADDR, reg, reg_val);
}

/**
 * @brief read reg value or with mask and write back to reg 
 * 
 * @param bus i2c bus
 * @param reg 
 * @param mask register of bk9532
 * @return WORD state bus i2c
 */
WORD bk9532_reg_or_mask(WORD bus, WORD reg, DWORD mask)
{
    WORD state;
    DWORD reg_val;

    state = bk9532_reg_read(bus, reg, &reg_val);
    if(state != I2C_OK)
        return state;
    
    reg_val |= mask;

    state = bk9532_reg_write(bus, reg, reg_val);
    return state;
}

/**
 * @brief read reg value and with mask and write back to reg 
 * 
 * @param bus i2c bus
 * @param reg 
 * @param mask register of bk9532
 * @return WORD state bus i2c
 */
WORD bk9532_reg_and_mask(WORD bus, WORD reg, DWORD mask)
{
    WORD state;
    DWORD reg_val;

    state = bk9532_reg_read(bus, reg, &reg_val);
    if(state != I2C_OK)
        return state;
    
    reg_val &= mask;

    state = bk9532_reg_write(bus, reg, reg_val);
    return state;
}

/**
 * @brief read reg value and with mask then or with flash finally write back to reg 
 * 
 * @param bus i2c bus
 * @param reg 
 * @param mask register of bk9532
 * @return WORD state bus i2c
 */
WORD bk9532_reg_and_mask_or_flag(WORD bus, WORD reg, DWORD mask, DWORD flag)
{
    WORD state;
    DWORD reg_val;

    state = bk9532_reg_read(bus, reg, &reg_val);
    // if(reg==0x36)
    // {
    //     TRACE("read 0x36 state=%x",state);
    //      TRACE("read 0x36 reg_val=%x",reg_val);
    // }
    if(state != I2C_OK)
        return state;
    
    reg_val &= mask;
    reg_val |= flag;

    state = bk9532_reg_write(bus, reg, reg_val);
    // if(reg==0x36)
    // {
    //     TRACE("write 0x36 state=%x",state);
    //      TRACE("write 0x36 reg_val=%x",reg_val);
    // }
    return state;
}