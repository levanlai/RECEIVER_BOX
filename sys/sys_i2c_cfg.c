#include "sys_i2c.h"
#include <trace.h>

// i2c_t i2c_bus_0 = {
//     .sda_port = 0,
//     .sda_pin = 5,
//     .scl_port = 0,
//     .scl_pin = 4,
//     .delay_high_clk = 50,
//     .delay_low_clk = 100,
//     .is_busy = 0
// };

// i2c_t i2c_bus_1 = {
//     .sda_port = 0,
//     .sda_pin = 7,
//     .scl_port = 0,
//     .scl_pin = 6,
//     .delay_high_clk = 50,
//     .delay_low_clk = 100,
//     .is_busy = 0
// };

// struct sys_i2c_str
// {
//     WORD sda_pin;
//     WORD scl_pin;
//     WORD sda_port;
//     WORD scl_port;
//     WORD delay_high_clk;
//     WORD delay_low_clk;
//     WORD is_busy;
// };

// i2c_t sys_i2c_tables[] = {
//     {
//          5,
//          4,
//          0,
//          0,
//          10,
//          20,
//          0,
//     },    
//     {
//          7,
//          6,
//          0,
//          0,
//          10,
//          20,
//          0
//     },
//     {
//          11,
//          12,
//          0,
//          0,
//          10,
//          20,
//          0,
//     }
// };

i2c_t sys_i2c_tables[] = {
    {
         10,
         8,
         0,
         0,
         5, //10,
         10, //20,
         0,
    },
    // {
    //      0,
    //      1,
    //      0,
    //      0,
    //      10,
    //      20,
    //      0,
    // },
    {
         6,
         7,
         0,
         0,
         5, //10,
         10, //20,
         0
    }
};

i2c_t *i2c_get_bus(WORD bus)
{
    if(bus >= sizeof(sys_i2c_tables)/sizeof(i2c_t))
    {
        TRACE("i2c get bus %d over index", bus);
        return NULL;
    }
    return &sys_i2c_tables[bus];
}
