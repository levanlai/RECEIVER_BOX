//#define _TARGET_CLASS_5xxx
#define _TARGET_IC      5504

#define AUTO_POWER_ON 

#define ENABLE_USB         1

#define _USE_GLOBAL_PRESET 1

#define UART_BAUDRATE                           115200//(57600) //supported are 31250, 38400 or 57600
#define UART1					(0x00)			
#define UART2					(0x01)	
#define UART_ALL				(0x02)	
//Timer0 reload value for 9,87ms, used for GM2Synth (env & lfo), do not change!
#define TIMER0V		(0x10000-(0.00987*UART_BAUDRATE*16))

/**************************************
Vol : Gain (-116(->-20db)-> 12dB)
EQ Bass,Treble:  Biquad (-24 -> 15 dB)
Echo: EchoInputLevel (0->100%)
Reverb: RevLevel (0->100%)
Delay: EchoTime (20->650 ms)
/***************************************/
/*
Converts linearValue parameter[0..0x7FFF] to 20*log10(linearValue/0x7FFF)+12. 
linearValue is the gain value, 0x7FFF=+12dB .. 0x5A9D=+9dB .. 0x4026=+6dB  0x2D6A=+3db .. 0x2000=0dB .. 0x16C3=-3dB ..0x1332=-4dB .. 0x1000=-6dB, .. 0=-Inf 
vd: 20*log10(linearValue/0x7FFF)+12=-3
 ->log10(linearValue/0x7FFF)=(-3-12)/20
 ->linearValue/0x7FFF=10^((-3-12)/20)
 ->linearValue=0x7FFF*10^((-3-12)/20)
log⁡(a)=b có nghĩa là 10^b=a
0x7FFF=32767 
-> % = (linearValue / 0x7FFF) × 100
->linearValue=%/100*0x7FFF
-> db=20*log(linearValue:0x7FFF)+12
-> linearValue=0x7FFF*10^((db-12)/20)

12=100%=0x7FFF    
9=70%=0x5A9D
8=60%=0x4ccc    
6=50%=0x4000
3=35%=0x2D6A
0=25%=0x2000
-3=18%=0x16C3
-4=15%=0x1332
-6=12%=0x1000
*/
#define UI_VALUE_MIN		0
#define UI_VALUE_MAX		16//254//100
#define UI_VALUE_MID		(UI_VALUE_MAX - UI_VALUE_MIN)/2

//các giá trị *10 để lấy giá trị x.x, vì kiểu float không dùng được
#define GAIN_MIN        -1160 // dB
#define GAIN_MIN_START    -200 // dB

#define UI_MASTER_VOLUME_MIN    GAIN_MIN_START// dB
#define UI_MASTER_VOLUME_MID    0//-35  
#define UI_MASTER_VOLUME_MAX    60//50   

#define UI_MIC_VOLUME_MIN     GAIN_MIN_START
#define UI_MIC_VOLUME_MID      0  
#define UI_MIC_VOLUME_MAX      60  

/*
//in range 0..0x7FFE = 0...100%
->linearValue=value/100*0x7FFF
*/
#define UI_MIC_ECHO_MIN         0   // %
#define UI_MIC_ECHO_MID         500
#define UI_MIC_ECHO_MAX         700 

#define UI_MIC_REVERB_MIN       0   // %
#define UI_MIC_REVERB_MID       150
#define UI_MIC_REVERB_MAX       500
/*
//in range 0..0x7FFE = 20ms...650ms
-> linearValue=(value-20)*0x7FFE/(650-20);
*/
#define UI_MIC_DELAY_MIN        3000 // ms    
#define UI_MIC_DELAY_MID        5000    
#define UI_MIC_DELAY_MAX        6500

// #define UI_MIC_BASS_MIN       -120 
// #define UI_MIC_BASS_MID       0 
// #define UI_MIC_BASS_MAX       120  

// #define UI_MIC_TREB_MIN       -120 
// #define UI_MIC_TREB_MID       0 
// #define UI_MIC_TREB_MAX       120 

//range EQ_gain: [ 01000h .. 04000h .. 05E00h ] -> [ -24dB .. 0dB .. +15dB ], step 0.5=0x100, 1=0x200
//cách tính: max=15dB=0x5Exx, giảm 1 là 15-1=0x5Exx - (1x2)=0x5Cxx //lấy giá trị x2
#define EQ_GAIN_MIN   -120
#define EQ_GAIN_MID   0
#define EQ_GAIN_MAX   120

#define EQ_GAIN_MID_SAM   0x40//0dB

#define GAIN_0_SAM   0x2000
#define GAIN_Minus_6_SAM   0x1000

enum {
    cmd_OK,
    cmd_FAIL,
};

enum {
    DSP1_MIXPAXT,
    DSP2_FBC,    
    DSP3_MIXPAXT,
    DSP4_LIVEMIC,
};
enum {
    Turn_OFF,
    Turn_ON,
};

enum {
    Detect_unPlug,
    Detect_Plug,
};
enum {
    MOVE_UP,    
    MOVE_DOWN,    
};
enum {    
    CMD_MIC_VOL,    
    CMD_MIC_BASS,    
    CMD_MIC_TREB, 
    CMD_MIC_MID,   
    CMD_ECHO,
    CMD_DELAY,    
    CMD_REVERB, 
    CMD_FILTER_L, 
    CMD_FILTER_H, 
    CMD_MIC_FBC, 
    
    CMD_POWER=20,   
    CMD_PANEL_SYNC,
    CMD_CHARGE_DET,
    CMD_BATTERY_VALUE,
    CMD_DEVICES_CONNECT,
    CMD_SAVE,	
    CMD_RESET_FACTORY,

    CMD_MOVE,
    CMD_SET_CHANGE,	
    CMD_SELECT_CHANGE,
};
typedef struct MyData{
  //WORD init;
  WORD Mic_Vol;
  WORD Mic_Bass;
  WORD Mic_Mid;
  WORD Mic_Treb;
  WORD Echo_Vol;
  WORD Delay;
  WORD Reverb;
  WORD Mic_FBC;  
}MyData_t;
typedef struct format_data
{
    WORD cmd;
    WORD value;   
}format_data_t;