#include <system.h>
#include <sys5000.h>
#include <DreamBoards.h>
#include <trace.h>
#include "../config.h"
#include "sys_io.h"

extern void delayMsec(WORD ms);

#define SET_DIR_INPUT(port,pin)      _andio(IO_PORT_DIR(port), ~(1 << pin))
#define SET_DIR_OUTPUT(port,pin)     _orio (IO_PORT_DIR(port),  (1 << pin))

#define IO_INPUT_READ(port,pin)     ((_rdio(IO_PORT_VAL(port)) >> pin) & 0x01)

#define IO_HIGH_BIT(port,pin)       _orio (IO_PORT_VAL(port),  (1 << pin))
#define IO_LOW_BIT(port,pin)        _andio(IO_PORT_VAL(port), ~(1 << pin))

//xem config trong file ProgRef5000
// void sys_io_i2c_init(void)
// {
//     /* Configure I2C pins */
//     // P0.7 SDA bus 1
//     // P0.6 SCL bss 1
//     // P0.5 SDA bus 0
//     // P0.4 SCL bss 0
//     //GPIO P0.[5:4] Reg. 0x08 PRM0[0]=0b Reg. 0x0F SEC2[9:8]≠10b
//     //GPIO P0.[7:6] Reg. 0x08 PRM0[0]=0b Reg. 0x0F SEC2[7:6]≠10b
//     _andio(PRM0PORT, ~(1 << 0));
//     // _andio(SEC2PORT, ~(0x03 << 6));
//     // _andio(SEC2PORT, ~(0x03 << 8));
//     _andio(SEC2PORT, ~(0x000F << 6));

//     // P0.11 SDA3
//     // P0.12 SCL3
//     //GPIO P0.[12:10] Reg. 0x08 PRM0[0]=0b Reg. 0x0D SEC0[15]=0b
//     _andio(SEC0PORT, ~(1 << 15));
// }

// void sys_io_gpio_init(void)
// {
//     /*config P0.10= LINE1-KC, input
//             P0.8= LINE2-KC, input   */
//     //GPIO P0.[12:10]   Reg. 0x08 PRM0[0]=0b Reg. 0x0D SEC0[15]=0b 
//     //GPIO P0.8         Reg. 0x08 PRM0[1]=0b Reg. 0x0D SEC0[15]=0b 
//     _andio(PRM0PORT, ~(3 << 0));     
//     _andio(SEC0PORT, ~(1 << 15));
//     SET_DIR_INPUT(DETECT_MIC_WIRE_PORT,DETECT_MIC_WIRE_PIN);
//     SET_DIR_INPUT(DETECT_GUITAR_PORT,DETECT_GUITAR_PIN);
    
//     /*config P0.2= EN-48V, output*/
//      _andio(PRM0PORT, ~(1 << 0));
//     //GPIO P0.2 Reg. 0x08 PRM0[0]=0b Reg. 0x0E SEC1[14:13]≠10b
//     _andio(SEC1PORT, ~(0x03 << 13));
//     //GPIO P0.0 Reg. 0x08 PRM0[0]=0b Reg. 0x0E SEC1[3:2]≠10b
//     //_andio(SEC1PORT, ~(0x03 << 2));
     
//     SET_DIR_OUTPUT(Power48v_PORT,Power48v_PIN);
//     //set_Power48v_value(Turn_OFF);
//     /* Configure I2S pins */
//     // P0.3: SAM-DAAD0-SELECT: src audio i2s 0 select
//     // Reg. 0x08 PRM0[0]=0b Reg. 0x0E SEC1[12]=0b
//     _andio(PRM0PORT, ~(1 << 0));
//     _andio(SEC1PORT, ~(1 << 12));

// }

// void sys_io_audio_init(void)
// {
//     //Register 0x0A – PRM2: Primary Function Select for P2
//     //(enable DAAD0/DAAD1/DAAD2/DABD1/DABD2)
//     //DAAD0:
//     _orio(PRM2PORT, (((1<<9)|(1<<8))|((1<<2)|(1<<1)|(1<<0))));
       
//     //_orio(DIGITAL_AUDIO_CONFIG_PORT, 1 << 6);
//     // _andio(DIGITAL_AUDIO_CONFIG_PORT, ~(1 << 11));
//     // _andio(DIGITAL_AUDIO_CONFIG_PORT, ~(1 << 8));
//     //_orio(DIGITAL_AUDIO_CONFIG_PORT, 1 << 5);

//     //_wrio(CLOCK_AND_RESET_CONTROL0_PORT, 0xC988);
//     //_wrio(CLOCK_AND_RESET_CONTROL0_PORT, 0xE126);

//     // i2s //
//     // master mode //
//     _andio(DIGITAL_AUDIO_IN_CONFIG_PORT, ~(0x3F));
//     _andio(DIGITAL_AUDIO_IN_CONFIG_PORT, 0);
//     _andio(DIGITAL_AUDIO_CONFIG_PORT, ~(1 << 0));
//     // slave mode // in1
//     // _andio(SEC2PORT, ~(1 << 8));
//     // _orio(SEC2PORT, (1 << 9));
//     // _andio(PRM0PORT, ~(1 << 0));
//     // _orio(DIGITAL_AUDIO_IN_CONFIG_PORT, 0xffff);
//     // _andio(DIGITAL_AUDIO_CONFIG_PORT, ~(1 << 0));
//     // _andio(DIGITAL_AUDIO_CONFIG_PORT, ~(1 << 2));
//     // _orio(DIGITAL_AUDIO_CONFIG_PORT, 1 << 6);
// }

// void sys_io_init(void)
// {
//     /* Initialize the system I/O pins */
//     sys_io_gpio_init();
//     sys_io_i2c_init();
//     sys_io_audio_init();
// }
void sys_io_i2c_init(void)
{
    /* Configure I2C pins */
    // P0.7 SCL1
    // P0.6 SDA1
    // P0.8 SCL2
    // P0.10 SDA2

    _andio(PRM0PORT, ~(1 << 0));
    // _andio(SEC2PORT, ~(0x03 << 6));
    // _andio(SEC2PORT, ~(0x03 << 8));
    _andio(SEC2PORT, ~(0x000F << 6));
}

void sys_io_gpio_init(void)
{
    /* Configure I2C pins */
    // P0.3 src audio i2s 0 select
    // Reg. 0x08 PRM0[0]=0b Reg. 0x0E SEC1[12]=0b
    _andio(PRM0PORT, ~(1 << 0));
    _andio(SEC1PORT, ~(1 << 12));

}

void sys_io_audio_init(void)
{
    //Register 0x0A – PRM2: Primary Function Select for P2
    //(enable DAAD0/DABD0)    
    _orio(PRM2PORT, (((1<<15)|(1<<0))));
    
    // i2s //
    // master mode //
    // _andio(DIGITAL_AUDIO_IN_CONFIG_PORT, ~(0x3F));
    _andio(DIGITAL_AUDIO_IN_CONFIG_PORT, 0);
    _andio(DIGITAL_AUDIO_CONFIG_PORT, ~(1 << 0));
    // slave mode // in1
    // _andio(SEC2PORT, ~(1 << 8));
    // _orio(SEC2PORT, (1 << 9));
    // _andio(PRM0PORT, ~(1 << 0));
    // _orio(DIGITAL_AUDIO_IN_CONFIG_PORT, 0xffff);
    // _andio(DIGITAL_AUDIO_CONFIG_PORT, ~(1 << 0));
    // _andio(DIGITAL_AUDIO_CONFIG_PORT, ~(1 << 2));
    // _orio(DIGITAL_AUDIO_CONFIG_PORT, 1 << 6);
    
    // slave mode // in1
    // _andio(SEC2PORT, ~(1 << 8));
    // _orio(SEC2PORT, (1 << 9));
    // _andio(PRM0PORT, ~(1 << 0)); // select GPIO or Secondary digital audio function on P0[7:0]
    // _andio(DIGITAL_AUDIO_IN_CONFIG_PORT, ~(0x0F));
    // _orio(DIGITAL_AUDIO_IN_CONFIG_PORT, 0x30);
    // _andio(DIGITAL_AUDIO_CONFIG_PORT, ~(1 << 0));
	// _andio(DIGITAL_AUDIO_CONFIG_PORT, ~(1 << 1));
    // _andio(DIGITAL_AUDIO_CONFIG_PORT, ~(1 << 2));
    //_orio(DIGITAL_AUDIO_CONFIG_PORT, 1 << 6);
}

void sys_io_init(void)
{
    /* Initialize the system I/O pins */
    sys_io_gpio_init();
    sys_io_i2c_init();
    sys_io_audio_init();
    //_orio(PRM2PORT, (((1<<15)|(1<<8))|((1<<2)|(1<<1)|(1<<0))));
}
void sys_timer0_init(void)
{
    // #ifdef _USE_SERIAL_MIDI
    //     _wrio(TIMER0PORT, TIMER0V);  //init timer 0  //Timer 0 reload value for ~10ms (base level timer)
	//     _orio(CONTROLPORT, ENA_TIMER0);
    // #else
    //     _wrio( TIMER2PORT, _Sys_CalcTimerReload(_BOARD_MIDIBAUDRATE));	//init Timer 2 reload value
    //     _orio( CONTROLPORT, ENA_TIMER2 );	//enable Timer 2
    //     _wrio( TIMER0PORT, 0x10000-(0.01*_BOARD_MIDIBAUDRATE*16) );	//Timer 0 reload value for ~10ms (base level timer)
    //     _orio( CONTROLPORT, ENA_TIMER0 );	//enable Timer 0
    // #endif 
    //uart ko cần timer2
    #if ENABLE_USB
        _wrio( TIMER2PORT, _Sys_CalcTimerReload(UART_BAUDRATE));	//init Timer 2 reload value
        _orio( CONTROLPORT, ENA_TIMER2 );	//enable Timer 2
    #endif
    _wrio(TIMER0PORT, TIMER0V);  //init timer 0  //Timer 0 reload value for ~10ms (base level timer)
	_orio(CONTROLPORT, ENA_TIMER0);    
    
}
