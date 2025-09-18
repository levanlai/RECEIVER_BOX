#ifndef __SYS_I2C_H__
#define __SYS_I2C_H__

#include <system.h>

struct sys_i2c_str
{
    WORD sda_pin;
    WORD scl_pin;
    WORD sda_port;
    WORD scl_port;
    WORD delay_high_clk;
    WORD delay_low_clk;
    WORD is_busy;
};

typedef struct sys_i2c_str  i2c_t;

typedef enum I2C_ERROR
{
    I2C_OK = 0,
    I2C_ERR_BUS_BUSY = -255,
    I2C_ERR_BUS_NOT_EXIST ,
    I2C_ERR_ACK,
    I2C_ERR_SDA_PORT,
    I2C_ERR_SCL_PORT,
    I2C_ERR_DELAY_HIGH_CLK,
    I2C_ERR_DELAY_LOW_CLK,
    I2C_ERR_IS_BUSY,
    I2C_ERR_UNKNOWN
    
};

typedef enum I2C_STATUS
{
    I2C_ACK  = 0,
    I2C_NACK = 1
};

typedef enum I2C_ACTION_BIT
{
    I2C_WRITE_BIT = 0,
    I2C_READ_BIT  = 1,
};

enum {
        I2C_PORT_MIC1,
        I2C_PORT_MIC2,
        I2C_PORT_ADC, 
    };
    
void i2c_start_bit(i2c_t *obj);
void i2c_stop_bit(i2c_t *obj);
WORD i2c_send_byte(i2c_t *obj, WORD data);
WORD i2c_receive_byte(i2c_t *obj, WORD ack);

i2c_t *i2c_get_bus(WORD bus);


#endif /* __SYS_I2C_H__ */