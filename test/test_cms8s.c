#include "../sys/sys_i2c_bk9532.h"
#include "../sys/sys_i2c.h"
#include <math.h>
#include <trace.h>

#define cms8s_address 0x24//0x12
extern WORD i2c_send_to_bk9532(i2c_t *obj, WORD addr, WORD reg, DWORD data);
void csm8s_get_chip_id(WORD bus)
{    
    WORD i,j,state;    
    i2c_t *i2c_bus = i2c_get_bus(bus);
    if (i2c_bus == NULL) {
        TRACE("==== I2C NULL ==%d", bus);	
        return ;
    }
    TRACE("==== I2C ADDR TEST ==%d", bus);	
    for(i=0;i<127; i++)
    {
        i2c_start_bit(i2c_bus);
        state = i2c_send_byte(i2c_bus, i);
        //TRACE("==== id: %d\n", i);
        //TRACE("==== state: %d\n", state);
        if(state != I2C_ACK)continue;    
        TRACE("====ok id: %d\n", i);
        for(j=0; j<65535; j++);
    }
   
}

WORD csm8s_reg_write(WORD bus, WORD reg, DWORD reg_val)
{
    i2c_t *i2c_bus = i2c_get_bus(bus);
    if (i2c_bus == NULL) {
        return I2C_ERR_BUS_NOT_EXIST;
    }
    return i2c_send_to_bk9532(i2c_bus, cms8s_address, reg, reg_val);
}
WORD cnt=0;
void csm8s_test(void)
{
   csm8s_get_chip_id(2);
//    WORD state;
//    // DWORD rreg_val = 0;
//     //DWORD wreg_val = 0;
//     state = csm8s_reg_write(2, 0x10, cnt++);
//     TRACE("csm8s_test wr write state = %x\n", state);
//     if(cnt>10)
//         cnt=1;
    //state = bk9532_reg_read(bus, 0x30, &rreg_val);
    //TRACE("bk9532 test wr read back state = %x\n", state);
    //TRACE("bk9532 test wr read back val = %x\n", rreg_val);
}
