#include <system.h>
#include <sys5000.h>
#include "sys_i2c.h"
#include <trace.h>
#include "sys_io.h"

//0: P0.[i] is INPUT (default at power-up or reset).
//1: P0.[i] is OUTPUT.
#define SDA_DIR_INPUT(obj)      _andio(IO_PORT_DIR(obj->sda_port), ~(1 << obj->sda_pin))
#define SDA_DIR_OUTPUT(obj)     _orio (IO_PORT_DIR(obj->sda_port),  (1 << obj->sda_pin))

#define SDA_INPUT_READ(obj)     ((_rdio(IO_PORT_VAL(obj->sda_port)) >> obj->sda_pin) & 0x01)

#define SDA_HIGH_BIT(obj)       _orio (IO_PORT_VAL(obj->sda_port),  (1 << obj->sda_pin))
#define SDA_LOW_BIT(obj)        _andio(IO_PORT_VAL(obj->sda_port), ~(1 << obj->sda_pin))


#define SCL_DIR_INPUT(obj)      _andio(IO_PORT_DIR(obj->scl_port), ~(1 << obj->scl_pin))
#define SCL_DIR_OUTPUT(obj)     _orio (IO_PORT_DIR(obj->scl_port),  (1 << obj->scl_pin))

#define SCL_HIGH_BIT(obj)       _orio (IO_PORT_VAL(obj->scl_port),  (1 << obj->scl_pin))
#define SCL_LOW_BIT(obj)        _andio(IO_PORT_VAL(obj->scl_port), ~(1 << obj->scl_pin))


void i2c_init(i2c_t *obj, WORD sda, WORD scl);


void i2c_delay_high_clk(i2c_t *obj)
{
    WORD cnt = obj->delay_high_clk;
    for(; cnt > 0; cnt--);
}

void i2c_delay_low_clk(i2c_t *obj)
{
    WORD cnt = obj->delay_low_clk;
    for(; cnt > 0; cnt--);
}

/**
 * @brief START CONDITION
 *   As mentioned earlier the slaves know that the communication 
 *   is being initiated when the SDA line changes from HIGH to LOW
 *   while the clock is HIGH. So as a first thing we leave both lines 
 *   pulled-up by the resistors, then we bring the SDA LOW and we also 
 *   set the clock LOW for the next phase.
 * @param obj 
 * @return none
 */

void i2c_start_bit(i2c_t *obj)
{
    // make sure SDA is set as an output
    SDA_DIR_OUTPUT(obj);
    SCL_DIR_OUTPUT(obj);
    // set SCL high and SDA high
    SDA_HIGH_BIT(obj);
    SCL_HIGH_BIT(obj);
    i2c_delay_high_clk(obj);
    // set SCL high and SDA low
    SDA_LOW_BIT(obj);
    i2c_delay_high_clk(obj);
    // set SCL low and SDA low
    SCL_LOW_BIT(obj);
    i2c_delay_low_clk(obj);
}

/**
 * @brief STOP CONDITION
 *  Similar as before the stop condition is as follows: set SDA low,
 *  let SCL go high, then let SDA go high as well.
 * 
 * @param obj 
 * @return none
 */

void i2c_stop_bit(i2c_t *obj)
{
    // set SCL low, let SDA go low
    SDA_LOW_BIT(obj);
    SCL_LOW_BIT(obj);
    i2c_delay_low_clk(obj);
    // set SCL high and SDA low
    SCL_HIGH_BIT(obj);
    i2c_delay_high_clk(obj);
    // set SCL high and SDA high
    SDA_HIGH_BIT(obj);
    i2c_delay_high_clk(obj);
}

/**
 * @brief i2c send byte
 * 
 * @param obj i2c context
 * @param data byte send 
 * @return WORD ack 
 */
WORD i2c_send_byte(i2c_t *obj, WORD data)
{
    WORD i;
    WORD ack;
    for (i = 0; i < 8; i++)
    {
        
        if (data & 0x80)
        {
            SDA_HIGH_BIT(obj);
        }
        else
        {
            SDA_LOW_BIT(obj);
        }
        SCL_HIGH_BIT(obj);
        i2c_delay_high_clk(obj);
        SCL_LOW_BIT(obj);
        i2c_delay_low_clk(obj);
        data <<= 1;
    }
    
    // read ack //
    // set SDA to an input 
    SDA_DIR_INPUT(obj);
    // set SCL low and SDA high
    SDA_HIGH_BIT(obj);
    i2c_delay_low_clk(obj);
    // set SCL high
    SCL_HIGH_BIT(obj);
    i2c_delay_high_clk(obj);
    // read ack //
    ack = SDA_INPUT_READ(obj);
    // set SCL low 
    SCL_LOW_BIT(obj);
    i2c_delay_low_clk(obj);
    // set SDA to an output
    SDA_DIR_OUTPUT(obj);
    // set SCL low and SDA high
    SDA_HIGH_BIT(obj);

    return ack;
}

/**
 * @brief i2c receive byte 
 * 
 * @param obj i2c context
 * @param ack 
 * @return WORD byte receive
 */
WORD i2c_receive_byte(i2c_t *obj, WORD ack)
{
    WORD i;
    WORD data = 0;
    // set SDA to an input 
    SDA_DIR_INPUT(obj);
    // set  SDA high
    SDA_HIGH_BIT(obj);

    for (i = 0; i < 8; i++)
    {
        // set SCL high
        SCL_HIGH_BIT(obj);
        i2c_delay_high_clk(obj);
        // read SDA bit
        data <<= 1;
        data |= SDA_INPUT_READ(obj);
        //i2c_delay_high_clk(obj);
        // set SCL low
        SCL_LOW_BIT(obj);
        i2c_delay_low_clk(obj);

    }

    // set SDA to an output 
    SDA_DIR_OUTPUT(obj);
    if (ack)
        SDA_LOW_BIT(obj);
    else
        SDA_HIGH_BIT(obj);
    i2c_delay_low_clk(obj);

    // set SCL high //
    SCL_HIGH_BIT(obj);
    i2c_delay_high_clk(obj);
    // set SCL low
    SCL_LOW_BIT(obj);

    return data & 0x00ff;
}

