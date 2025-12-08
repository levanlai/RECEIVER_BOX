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

#define UI_VALUE_EFFECT_MAX		3

//các giá trị *10 để lấy giá trị x.x, vì kiểu float không dùng được
#define GAIN_MIN        -1160 // dB
#define GAIN_MIN_START    -200 // dB
#define GAIN_OUT_VOLUME_MIN_START    -400 // dB

#define UI_MIC_OUT_VOLUME_MIN    GAIN_OUT_VOLUME_MIN_START //GAIN_MIN

#define UI_MIC_VOLUME_MIN     GAIN_MIN_START
#define UI_MIC_VOLUME_MID      0  
#define UI_MIC_VOLUME_MAX      120  

/*
//in range 0..0x7FFE = 0...100%
->linearValue=value/100*0x7FFF
*///EchoInputLevel
#define UI_MIC_ECHO_MIN         0   // %
#define UI_MIC_ECHO_MID         450
#define UI_MIC_ECHO_MAX         900//1000
//RevLevel
#define UI_MIC_REVERB_MIN       0   
#define UI_MIC_REVERB_MID       500
#define UI_MIC_REVERB_MAX       1000
//EchoFeedback
#define UI_MIC_REPEAT_MIN       0  
#define UI_MIC_REPEAT_MIN_START 400 
#define UI_MIC_REPEAT_MID       700
#define UI_MIC_REPEAT_MAX       1000
/*
//in range 0..0x7FFE = 20ms...650ms
-> linearValue=(value-20)*0x7FFE/(650-20);
*///EchoTime
#define UI_MIC_DELAY_MIN        200
#define UI_MIC_DELAY_MIN_START  1000
#define UI_MIC_DELAY_MID        2000//2210//4420 
#define UI_MIC_DELAY_MAX        2500//6500//2500//6500

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
//#define GAIN_Minus_6_SAM   0x1000

enum {
    cmd_OK,
    cmd_FAIL,
};

enum {
    DSP1_LIVEMIC,
    DSP2_LIVEMIC,
    DSP3_FBC,    
    //DSP4_MIXPAXT,
    DSP4_LIVEMIC
};

enum {
    CHARGE_PLUS,    
    CHARGE_UN_PLUS,
    CHARGE_DONE,   
};

enum {
    MOVE_UP,    
    MOVE_DOWN,    
};
// bypass: 0=normal work, 1=bypass all notch filters
enum {
    FBC_ON,    
    FBC_OFF,    
};
enum {        
    TURN_OFF, 
    TURN_ON,
    TURN_NONE,   
};
enum {        
    EFFECT_OFF,
    EFFECT_MonoEcho_Reverb=12,  
};
// enum {    
//     CMD_MIC_VOL,
//     CMD_ECHO,
//     CMD_DELAY,    
//     CMD_REVERB, 
//     CMD_MIC_BASS,    
//     CMD_MIC_TREB, 
//     //CMD_MIC_MID,   
//     CMD_FILTER_L, 
//     CMD_FILTER_H,
//     //CMD_EFFECT,   
//     CMD_MIC_FBC,
//     CMD_MIC_DEFAULT,  
    
//     CMD_POWER=20,   
//     CMD_PANEL_SYNC,
//     CMD_CHARGE_DET,
//     CMD_BATTERY_VALUE,
//     CMD_DEVICES_CONNECT,
//     CMD_SAVE,	
//     CMD_RESET_FACTORY,
    

//     CMD_MOVE,
//     CMD_SET_CHANGE,	
//     CMD_SELECT_CHANGE,
// };

enum {
    CMD_MIC_1_VOL,   
    //CMD_MIC_1_EFFECT, 
    CMD_MIC_1_ECHO,     
    CMD_MIC_1_DELAY,
    //CMD_MIC_1_REVERB, 
    CMD_MIC_1_REPEAT,         
    CMD_MIC_1_BASS, 
    CMD_MIC_1_MID,
    CMD_MIC_1_TREBLE,

    CMD_MIC_2_VOL,  
    //CMD_MIC_2_EFFECT, 
    CMD_MIC_2_ECHO, 
    CMD_MIC_2_DELAY,
    //CMD_MIC_2_REVERB,
    CMD_MIC_2_REPEAT,  
    CMD_MIC_2_BASS, 
    CMD_MIC_2_MID,
    CMD_MIC_2_TREBLE,    

    CMD_VOL_OUT,
    CMD_MIC_REVERB,
    CMD_MIC_EFFECT,
    CMD_MIC_FBC,
    CMD_CONTROL_LINK,
    CMD_SAVE,
    CMD_RESET,
    CMD_RESET_FORM_MIC,

    CMD_POWER=50,   
    CMD_PANEL_SYNC,
    CMD_CHARGE_DET,
    CMD_BATTERY_VALUE,
    CMD_DEVICES_CONNECT,

    CMD_MOVE,
    CMD_SELECT_CHANGE,
    //CMD_HOT_CHANGE_MIC1,	
    //CMD_HOT_CHANGE_MIC2,

    CMD_TOTAL,
};

typedef struct MyData{
  //WORD init;
  WORD Mic_1_Vol; 
  //WORD Mic_1_Effect; 
  WORD Mic_1_Echo;
  WORD Mic_1_Delay;
  //WORD Mic_1_Reverb;
  WORD Mic_1_Repeat;
  WORD Mic_1_Bass;  
  WORD Mic_1_Mid;  
  WORD Mic_1_Treb;
  
  WORD Mic_2_Vol; 
  //WORD Mic_2_Effect; 
  WORD Mic_2_Echo;
  WORD Mic_2_Delay;
  //WORD Mic_2_Reverb;
  WORD Mic_2_Repeat;
  WORD Mic_2_Bass;  
  WORD Mic_2_Mid;  
  WORD Mic_2_Treb;

  WORD Mic_Vol_Out;
  WORD Mic_Reverb;
  WORD Mic_Effect;
  WORD Mic_FBC; 
  WORD Mic_Control_link;
}MyData_t;
typedef struct format_data
{
    WORD cmd;
    WORD value;   
}format_data_t;

enum {
    ADC_BATERY,
    ADC_BTN,    
};

