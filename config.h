//#define _TARGET_CLASS_5xxx
#define _TARGET_IC      5504

#define AUTO_POWER_ON 

#define _USE_SERIAL_MIDI 
#define ENABLE_USB         1
#define USING_LCD           1
#define USING_PANEL         0
//dùng board AXIGN, 1: setting mono (2 ngõ ra nối vào MIX và bật EQ bass/treble ngõ ra), 0: setting stereo (left/right độc lập (tắt G khi nối qua MIX) và tắt EQ bass/treble ngõ ra)
//#define USING_BOARD_AXIGN   Output_STEREO
//#define USING_BOARD_AXIGN

#define _USE_GLOBAL_PRESET 1

#define UART_BAUDRATE                           115200//(57600) //supported are 31250, 38400 or 57600
//Timer0 reload value for 9,87ms, used for GM2Synth (env & lfo), do not change!
#define TIMER0V		(0x10000-(0.00987*UART_BAUDRATE*16))
//#define PACK_STOP_CODE             0xBF

#define time_sync_panel 100//1s vì timer0 là 10ms
#define time_sync_func 200//2s vì timer0 là 10ms
#define time_pair_mic 300
#define time_toggle_Led 10
#define POWER_TOGGLE_CNT 9



// #define GAIN_MAX_12dB   12
// #define GAIN_0dB        0
// #define GAIN_MIN_minus80dB   -80
// #define GAIN_MIN_minus116dB   -116

// //range gain: 12->-116dB (0x7FFE ->0) , 0db=0x73FE
// //cách tính: max=12dB=0x7Fxx, giảm 1 là 12-1=0x7Fxx- 0x01xx=11=0x7Exx
// #define GAIN_MAX_12dB_SAM   0x7F//12dB
// #define GAIN_0dB_SAM   0x73//-80dB//0x67FE//-12dB//0:-116dB
// #define GAIN_MIN_minus80dB_SAM   0x23//-80dB//0x67FE//-12dB//0:-116dB
// #define GAIN_MIN_minus116dB_SAM   0

// #define GAIN_LSB   0xFE//12dB

// //range EQ_gain: 15->-24dB (0x5E00 ->0x1000), 0db=0x4000 
// //cách tính: max=15dB=0x5Exx, giảm 1 là 15-1=0x5Exx - (1x2)=0x5Cxx //lấy giá trị x2
// #define EQ_GAIN_MAX_15dB   15
// //#define EQ_GAIN_MAX_12dB   12
// #define EQ_GAIN_MIN_minus12dB   -12
// #define EQ_GAIN_MIN_minus12dB_abs   12
// #define EQ_GAIN_MIN_minus24dB   -24
// #define EQ_GAIN_MIN_minus24dB_abs   24
// #define EQ_GAIN_0dB        0

// #define EQ_GAIN_MAX_15dB_SAM   0x5E//15dB
// //#define EQ_GAIN_MAX_12dB_SAM   0x58//12dB//0x4C00=6dB
// #define EQ_GAIN_MIN_minus12dB_SAM   0x28//-12dB //0x3400=-6dB
// #define EQ_GAIN_MIN_minus24dB_SAM   0x10//-24dB
// #define EQ_GAIN_0dB_SAM   0x40
#define CIRCLE_NO_MID_POS   0
#define CIRCLE_MID_POS      50
#define CIRCLE_MAX_POS      100

//MASTER vol: -116db(->-20db)->-3db->5db 
//MUSIC vol: -116db(->-20db)->-3db->0db 
//MIC VOL: -116db(->-20db)->0db->6db
//EQ Bass,Treble:  -12db->0db->12db
//Echo: 0->50->70  (-116db->6db->9db) 
//Delay: 45->75->100 (300ms->500ms->650ms)
//Reverb: 0->30->60 (-116db->1.5db->7.5db -> -116db->2db->8db) 
#define GAIN_Minus_6_SAM   0x1000
#define GAIN_0_SAM   0x2000

#define GAIN_MASTER_MAX   5
#define GAIN_MAX   0
#define GAIN_MID       -3
#define GAIN_MIN   -116
//#define GAIN_MIN_start   -20
//range gain: 12->-116dB (0x7FFE ->0) , 0db=0x73FE
//cách tính: max=12dB=0x7Fxx, giảm 1 là 12-1=0x7Fxx- 0x01xx=11=0x7Exx
// #define GAIN_MASTER_MAX_SAM   0x78//5dB //0x76//3dB
// #define GAIN_MAX_SAM   0x73//0dB //0x76//3dB
// #define GAIN_MID_SAM   0x70//-3dB
// #define GAIN_MIN_SAM   0//-116dB
// #define GAIN_MIN_start_SAM   0x5F//-20dB

#define GAIN_MIC_MAX   6
#define GAIN_MIC_MID       0
#define GAIN_MIC_MIN   -116
//#define GAIN_MIC_MIN_start   -20

// #define GAIN_MIC_MAX_SAM   0x79//6dB
// #define GAIN_MIC_MID_SAM   0x73//0dB
// #define GAIN_MIC_MIN_SAM   0//-116dB
// #define GAIN_MIC_MIN_start_SAM   0x5F//-20dB

#define GAIN_LSB   0xFE

//range EQ_gain: 15->-24dB (0x5E00 ->0x1000), 0db=0x4000 
//cách tính: max=15dB=0x5Exx, giảm 1 là 15-1=0x5Exx - (1x2)=0x5Cxx //lấy giá trị x2

#define EQ_GAIN_MAX   12
#define EQ_GAIN_MID   0
#define EQ_GAIN_MIN   -12

#define EQ_GAIN_MAX_SAM   0x58//12dB//0x4C00=6dB
#define EQ_GAIN_MID_SAM   0x40//0dB
#define EQ_GAIN_MIN_SAM   0x28//-12dB //0x3400=-6dB

#define GAIN_ECHO_MAX   9 //70%
#define GAIN_ECHO_MID   6 //50%
#define GAIN_ECHO_MIN   -116 //0%

// #define GAIN_ECHO_MAX   70 
// #define GAIN_ECHO_MID   50 
// #define GAIN_ECHO_MIN   0

// #define GAIN_ECHO_MAX_SAM   0x59//70%
// #define GAIN_ECHO_MID_SAM   0x40//50%
// #define GAIN_ECHO_MIN_SAM   0 //0%

#define GAIN_DELAY_MAX   100
#define GAIN_DELAY_MID    75
#define GAIN_DELAY_MIN   45

#define GAIN_DELAY_MAX_SAM   0x7F//650ms
#define GAIN_DELAY_MID_SAM   0x61//500ms
#define GAIN_DELAY_MIN_SAM   0x38//300ms

//#define GAIN_REV_MAX   8//60%=0x4ccc (log từ DSP)=7.5db=20*log(0x4ccc:0x8000)+12
//#define GAIN_REV_MID    2//30%=0x2666=1.5db=20*log(0x2666:0x8000)+12
#define GAIN_REV_MAX   6//50%=0x4000 (log từ DSP)=7.5db=20*log(0x4000:0x8000)+12
#define GAIN_REV_MID    -4//15%=0x1332=-4.5db=20*log(0x1332:0x8000)+12
#define GAIN_REV_MIN   -116 //0%
// #define GAIN_REV_MAX   60
// #define GAIN_REV_MID    30
// #define GAIN_REV_MIN   0

// #define GAIN_REV_MAX_SAM   0x4C //60%
// #define GAIN_REV_MID_SAM   0x26 //30%
// #define GAIN_REV_MIN_SAM   0 //0%

#define SYSTEM_FUNC_FBX_STATE_OFFS          0
#define SYSTEM_FUNC_BASSB_STATE_OFFS        1
#define SYSTEM_FUNC_ENHANCER_STATE_OFFS     2
#define SYSTEM_FUNC_TWS_STATE_OFFS          3
#define SYSTEM_FUNC_BT_STATE_OFFS           4
#define SYSTEM_FUNC_MIC_STATE_OFFS          5
#define SYSTEM_FUNC_Power48v_OFFS           6
#define SYSTEM_FUNC_OUTPUT_AUDIO_MODE       7

// #define SYSTEM_FUNC_AVAIL_BT_OFFS           0
// #define SYSTEM_FUNC_AVAIL_USB_OFFS          1
// #define SYSTEM_FUNC_AVAIL_HDMI_OFFS         2
// #define SYSTEM_FUNC_AVAIL_OPT_OFFS          3
// //#define SYSTEM_FUNC_AVAIL_OPT_ANDROID_OFFS  4
// #define SYSTEM_FUNC_AVAIL_AUX_OFFS          4
// #define SYSTEM_FUNC_AVAIL_FBX_OFFS          5
// #define SYSTEM_FUNC_AVAIL_BB_OFFS           6
// #define SYSTEM_FUNC_AVAIL_ENH_OFFS          7
// #define SYSTEM_FUNC_AVAIL_TWS_OFFS          8

// #define SYSTEM_FUNC_AVAIL_BT            (WORD)(1 << SYSTEM_FUNC_AVAIL_BT_OFFS)
// #define SYSTEM_FUNC_AVAIL_USB           (WORD)(1 << SYSTEM_FUNC_AVAIL_USB_OFFS)
// #define SYSTEM_FUNC_AVAIL_HDMI          (WORD)(1 << SYSTEM_FUNC_AVAIL_HDMI_OFFS)
// #define SYSTEM_FUNC_AVAIL_OPT           (WORD)(1 << SYSTEM_FUNC_AVAIL_OPT_OFFS)
// //#define SYSTEM_FUNC_AVAIL_OPT_ANDROID   (WORD)(1 << SYSTEM_FUNC_AVAIL_OPT_ANDROID_OFFS)
// #define SYSTEM_FUNC_AVAIL_AUX           (WORD)(1 << SYSTEM_FUNC_AVAIL_AUX_OFFS)
// #define SYSTEM_FUNC_AVAIL_FBX           (WORD)(1 << SYSTEM_FUNC_AVAIL_FBX_OFFS)
// #define SYSTEM_FUNC_AVAIL_BB            (WORD)(1 << SYSTEM_FUNC_AVAIL_BB_OFFS)
// #define SYSTEM_FUNC_AVAIL_ENH           (WORD)(1 << SYSTEM_FUNC_AVAIL_ENH_OFFS)
// #ifdef BT_TWS_SUPPORT
//     #define SYSTEM_FUNC_AVAIL_TWS           (WORD)(1 << SYSTEM_FUNC_AVAIL_TWS_OFFS)
// #else    
//     #define SYSTEM_FUNC_AVAIL_TWS           (WORD)(0 << SYSTEM_FUNC_AVAIL_TWS_OFFS)
// #endif

// #define SYSTEM_FUNC_AVAILS      (WORD)(SYSTEM_FUNC_AVAIL_BT| SYSTEM_FUNC_AVAIL_USB| SYSTEM_FUNC_AVAIL_HDMI| SYSTEM_FUNC_AVAIL_OPT| SYSTEM_FUNC_AVAIL_AUX| SYSTEM_FUNC_AVAIL_FBX| SYSTEM_FUNC_AVAIL_BB | SYSTEM_FUNC_AVAIL_ENH| SYSTEM_FUNC_AVAIL_TWS)


enum {
    cmd_OK,
    cmd_FAIL,
};
enum {    
    Type_MASTER_VOL,
    Type_MUSIC_VOL,
    Type_MIC_VOL,
    Type_EQ,
    Type_Echo,
    Type_Delay,
    Type_Reverb,
};

#define LED_CMD_START           0x30
enum {
    LED_OFF,
    LED_ON,
};

#if USING_PANEL    
    //#define PANEL_MODEL_FP_P11_K03L04_V10 //11 nút,4 led
    //#ifdef PANEL_MODEL_FP_P11_K03L04_V10
    enum {
        CMD_MIC1_VOL,
        CMD_MIC2_VOL,
        CMD_MIC_BASS,    
        CMD_MIC_TREB,             
        CMD_ECHO_VOL,
        CMD_DELAY,    
        CMD_REVERB,    
        CMD_MUSIC_BASS,
        CMD_MUSIC_TREB,
        CMD_MUSIC_VOL,    
        CMD_MASTER_VOL,  
        CMD_MIC_WIRE_VOL,   
        CMD_GUITAR_VOL,    
        CMD_MIC_MID,  //ko dung
        CMD_BASS_BOOST, 
        LED_1= LED_CMD_START,
        LED_2,
        LED_3,
        LED_4,
    };  
#else
    enum {
    CMD_MIC1_VOL,
    CMD_MIC2_VOL,
    CMD_MIC_BASS,    
    CMD_MIC_TREB, 
    CMD_MIC_MID,   
    CMD_ECHO_VOL,
    CMD_DELAY,    
    CMD_REVERB,    
    CMD_MUSIC_BASS,
    CMD_MUSIC_TREB,
    CMD_MUSIC_VOL,    
    CMD_MASTER_VOL, 
    CMD_MIC_WIRE_VOL,   
    CMD_GUITAR_VOL, 
    
    CMD_POWER=20,
    CMD_FUNC_SYNC, //đồng bộ dữ liệu
	CMD_FUNC_ENABLE, //enable/disable chức năng
	CMD_FUNC_VALUE, //giá trị
	CMD_SWITCH_INPUT,//thay đổi input
    CMD_BATTERY_VALUE,
	CMD_MEDIA_INFO,//tên bài hát,ca sĩ,...
	CMD_MEDIA_CONTROL,
	CMD_MEDIA_GAIN,   
    CMD_INFO, //get info fw
    CMD_RESET_FACTORY,
    CMD_PANEL_SYNC, 
    CMD_CHARGE_DET,
    // CMD_CHARGE_DONE,
    CMD_PAIR_MIC_ID_1_0,
    CMD_PAIR_MIC_ID_1_1,
    CMD_PAIR_MIC_ID_1_2,    
    CMD_PAIR_MIC_ID_2_0,
    CMD_PAIR_MIC_ID_2_1,
    CMD_PAIR_MIC_ID_2_2,    
    CMD_BASS_BOOST, 
};
#endif
enum {
    DSP1,
    DSP2,
    DSP3,
    DSP4,
};

enum {
    Turn_OFF,
    Turn_ON,
};

enum {
    Detect_unPlug,
    Detect_Plug,
};

//dùng board AXIGN
//mono : tat tieng 1 kenh (kenh right), có mix (gain music 2 kênh=0, qua mix thì gain mỗi kênh giảm -6)
//stereo : 2 ngõ ra, không mix (qua mix gain music từng kênh =0 và kênh kia =-116)
enum {
    Output_MONO,
    Output_STEREO,  
};

enum {
    Input_Bluetooth       ,   
    Input_USB             ,  
    Input_Optical         ,  
    //Input_HDMI            ,
    //Input_OpticalAndroid     , 
    Input_Line_In         , 
    Input_total           ,     
};
