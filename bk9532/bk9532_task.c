#include <system.h>
#include <sys5000.h>
#include <trace.h>
#include "bk9532.h"
#include "../config.h"
#include "../lcd/uart.h"
#include "../nvs/pms.h"
#include "../lib.h"
#define BK9532_AFC_MAX_INDEX 5

/**********************************************************************
 *                              prototype                             *
 **********************************************************************/

static void bk9532_task_handler_channel_0(void);
static void bk9532_task_handler_channel_1(void);


static WORD bk9532_rf_param_init(WORD bus);
static WORD bk9532_rf_chip_init(WORD bus);


static void bk9532_task_handler(WORD bus);

/**********************************************************************
 *                              variables                             *
 **********************************************************************/
// WORD idCode5bit[BK9532_MAX_CHANNELS][BK9532_IDCODE_RX_LENGTH];
// WORD idCodeRecvFlag[BK9532_MAX_CHANNELS];
// WORD last_time_ReceiveData[BK9532_MAX_CHANNELS];

static  struct bk9532_rf_context g_bk9532_rf_ctx[BK9532_MAX_CHANNELS];

static struct bk9532_rf_afc g_rf_afc[BK9532_MAX_CHANNELS];

static _SYS_TASK bk9532_task[BK9532_MAX_CHANNELS];

static _SYS_TASK_CALLBACK bk9532_task_handler_channel[BK9532_MAX_CHANNELS] =
{
    bk9532_task_handler_channel_0,
    bk9532_task_handler_channel_1
};

static const struct bk9532_reg_val g_bk9532_rom[] = {
    {0x00, 0xDFFFFFF8}, // 00 REG0
    {0x01, 0x04D28057}, // 02 REG1,0x52 to 0xD2,140515
    {0x02, 0x8990E028}, // 04 REG2,
    {0x03, 0x2452069F}, // 06 REG3,
    {0x04, 0x52880044}, // 08 REG4,
    {0x05, 0x00280380}, // 0A REG5,
    {0x06, 0x5BEDFB00}, // 0C REG6,
    {0x07, 0x1C2EC5AA}, // 0E REG7, UBAND,150917;
    {0x08, 0xEFF1194C}, // 10 REG8, UBAND,150917
    {0x09, 0x885113A2}, // 12 REG9, update REG9[7]=0-->1,140414;
    {0x0A, 0x006F006F}, // 14 REGA
    {0x0B, 0x1BD25863}, // 16 REGB
    {0x0C, 0x00000008}, // 18 REGC,
    {0x0D, 0x3A9B69D0}, // 1A REGD,          13
    {0x2C, 0x00000000}, // 58 REG2C, ??    14
    {0x2D, 0x000000ff}, // 5A REG2D, ??
    {0x2E, 0xf128a000}, // 5C REG2E, ??
    {0x2F, 0x00002e91}, // 5E REF2F, ??    17
    {0x30, 0x004D4D4D}, // 60 REG30, ??GPIO3,GPIO2,GPIO1,GPIO0????
    {0x31, 0xC1080071}, // 62 REG31, ?GPIO3????? GPIO0,GPIO1,GPIO2?I2S????
    {0x32, 0x20FF0F09}, // 64 REG32, 2dB
    {0x33, 0x00900080}, // 66 REG33, ??RSSI?????
    {0x34, 0xFFFF010E}, // 68 REG34, ????
    {0x35, 0x09000000}, // 6A REG35, ????
    {0x36, 0x0C6060D5}, // 6C REG36, ??? I2S??,MSB??,????
    {0x37, 0x3e009800}, // 6E REG37,
    {0x38, 0x40D7D5F7}, // 70 REG38,
    {0x39, 0x00000000}, // 72 REG39,
    {0x3A, 0x28020564}, // 74 REG3A
    {0x3B, 0x6D000800}, // 76 REG3B, ?PLC
    {0x3C, 0x0040FFDC}, // 78 REG3C,
    {0x3D, 0x00006629}, // 7A REG3D,
    {0x3E, 0x1F554FEE}, // 7C REG3E,
    {0x3F, 0x8D7A002F}, // 7E REG3F,         33
    {0x59, 0x43000000}, // B2 REG59, ??    34
    {0x5A, 0x00000000}, // B4 REG5A,
    {0x5B, 0x00000000}, // B6 REG5B,
    {0x5C, 0x2cd50000}, // B8 REG5C, ??
    {0x5D, 0x1FFF3FFF}, // BA REG5D,
    {0x5E, 0x00000F00}, // BC REG5E,         39
    {0x70, 0x00089532}, // E0 REG70,         40
    {0x71, 0x18A40810}, // E2 REG71,
    {0x72, 0x00000000}, // E4 REG72,
    {0x73, 0x00000008}, // E6 REG73,
    {0x74, 0x00000000}, // E8 REG74,
    {0x75, 0x00000629}, // EA REG75,
    {0x76, 0x0000FB06}, // EC REG76,
    {0x77, 0x00000000}, // EE REG77,
    {0x78, 0x00000000}, // F0 REG78,
    {0x79, 0x00000000}, // F2 REG79,
    {0x7A, 0x00010001}, // F4 REG7A,
    {0x7B, 0x3BE40796}, // F6 REG7B,
    {0x7C, 0x0F860074}, // F8 REG7C,
    {0x7D, 0x0032A8FF}, // FA REG7D,         53
};

static const WORD XTAL_ADJ_TAB[BK9532_AFC_MAX_INDEX] = {0x31, 0x43, 0x55, 0x67, 0x79};
static WORD state_pair[BK9532_MAX_CHANNELS] = {0, 0};
static state_machine[BK9532_MAX_CHANNELS] = {0,0};
/**********************************************************************
 *                              implement                             *
 **********************************************************************/

/**
 * @brief led initialization function
 * 
 * @param bus channel
 * @return WORD 
 */
static WORD bk9532_rf_indicate_init(WORD bus)
{
    bk9532_set_gpio_mode(bus, BK9532_GPIO_PIN_4, BK9532_GPIO_MODE_OUTPUT, BK9532_GPIO_PULLDOWN);
    bk9532_set_gpio_func(bus, BK9532_GPIO_PIN_4, 0);
    return 0;
}
/**
 * @brief led switch state on off
 * 
 * @param bus channel
 * @param onoff state
 * @return WORD 
 */
WORD bk9532_rf_indicate_onoff(WORD bus, WORD onoff)
{
   // TRACE("bk9532_rf_indicate_onoff  onoff=%x", onoff);
    return bk9532_set_gpio_output(bus, BK9532_GPIO_PIN_4, onoff);
}

static WORD bk9532_rf_indicate_period(WORD bus, WORD time, WORD period)
{
    static WORD indicate[BK9532_MAX_CHANNELS] = {1, 1};
    if (((DWORD)time * BK9532_STEP_MS) % (period) == 0)
    {
        bk9532_rf_indicate_onoff(bus, indicate[bus]);
        //indicate[bus] = ~indicate[bus];
        indicate[bus] = (indicate[bus]==1)?0:1;
    }
    return 0;
}

static WORD bk9532_rf_idcode_freq_set(WORD bus, DWORD idcode, DWORD freq)
{
    WORD state =  bk9532_set_idcode(bus, idcode); 

    if(state != BK9532_PASS)
    {
        return state;
    }        
    
    return bk9532_set_frequency(bus, freq);
}
/**
 * @brief bk9532 calib rf module when change frequency
 * 
 * @param bus channel
 * @param reset reset machine state
 * @return WORD 0: successful 1: waiting next transition 
 */
static WORD bk9532_rf_calib_process(WORD bus, WORD reset)
{
    WORD cal_result = 1;
    static WORD calib_step[BK9532_MAX_CHANNELS] = {0,0};
    if(reset)
    {
        calib_step[bus] = 0;

        return cal_result;
    }
#if 0
    switch (calib_step[bus])
    {
    case 0:
        if (bk9532_set_rf_loop_ldo_vol(bus, FALSE) == BK9532_PASS)
        {
            calib_step[bus] = 1;
        }
        break;
    case 1:
        if (bk9532_set_rf_vco_cb_clk(bus, TRUE) == BK9532_PASS 
        && bk9532_set_rf_vco_cb_toggle(bus) == BK9532_PASS)
        {
            calib_step[bus] = 2;
        }
        else
        {
            calib_step[bus] = 0;
        }
        break;
    case 2:
        if (bk9532_set_rf_vco_cb_dig_toggle(bus) == BK9532_PASS 
        && bk9532_set_rf_vco_cb_clk(bus, FALSE) == BK9532_PASS)
        {
            calib_step[bus] = 3;
        }
        else
        {
            calib_step[bus] = 0;
        }
        break;
    case 3:
        if (bk9532_set_rf_loop_ldo_vol(bus, TRUE) == BK9532_PASS 
        && bk9532_set_rf_baseband_reset(bus) == BK9532_PASS 
        && bk9532_set_rf_plc_reset(bus) == BK9532_PASS)
        {
            cal_result = 0;
        }
        calib_step[bus] = 0;
        break;
    default:
        calib_step[bus] = 0;
        break;
    }
#endif    
    bk9532_set_rf_loop_ldo_vol(bus, FALSE);
    bk9532_set_rf_vco_cb_clk(bus, TRUE);
    bk9532_set_rf_vco_cb_toggle(bus);
    bk9532_set_rf_vco_cb_dig_toggle(bus);
    bk9532_set_rf_vco_cb_clk(bus, FALSE);
    bk9532_set_rf_loop_ldo_vol(bus, TRUE);
    bk9532_set_rf_baseband_reset(bus);
    bk9532_set_rf_plc_reset(bus);
    cal_result = 0;
    return cal_result;
}
/**
 * @brief bk9532 rf module detect tx signal 
 * 
 * @param bus channel
 * @return WORD 0: tx signal receive 1: waiting next transition
 */
static WORD bk9532_rf_detect_signal(WORD bus)
{
    WORD state = 1;
    if (bk9532_get_rf_signal(bus) == 0)
    {
        //TRACE("bk9532 rf signal detect %d", g_rf_afc[bus].opened);
        if(!g_rf_afc[bus].opened)
        {
            if (bk9532_set_afc(bus, TRUE) == BK9532_PASS)
            {
                g_rf_afc[bus].opened = TRUE;
            }
        }
        state = 0;
    }
    else
    {
        g_rf_afc[bus].last_time_tune++;
        //TRACE("bk9532 rf signal no detect %d", g_rf_afc[bus].last_time_tune);
        if(bk9532_timeout(g_rf_afc[bus].last_time_tune, BK9532_AFC_TIMEOUT))
        {
            //TRACE("cur_index %d", g_rf_afc[bus].cur_index);
            if (++g_rf_afc[bus].cur_index >= BK9532_AFC_MAX_INDEX)
            {
                    g_rf_afc[bus].cur_index = 0;
            }
            bk9532_set_xtal_adj_cfg(bus, XTAL_ADJ_TAB[g_rf_afc[bus].cur_index]);
            g_rf_afc[bus].last_time_tune = 0;
        }
        // bk9532_reg_read(bus, BK9532_REG_XTAL, &reg_val);
         //TRACE("pairing BK9532_REG_XTAL ", reg_val);
    }
    return state;
}

/**
 * @brief bk9532 rf monitor tx signal
 * 
 * @param bus  channel
 * @return WORD  1: have tx signal -1: lost tx signal
 */
static WORD bk9532_rf_monitor(WORD bus)
{
    WORD state = 1;
    if (bk9532_get_rf_signal(bus) != 0)// no signal
    {
        //TRACE(" bk9532_rf_monitor lost signal detect %d", g_rf_afc[bus].last_time_tune);
        if(g_rf_afc[bus].opened)
        {
            if (bk9532_set_afc(bus, FALSE) == BK9532_PASS)
            {
                g_rf_afc[bus].opened = FALSE;
            }
            g_bk9532_rf_ctx[bus].last_time_freq_change = 0;
        }
        g_rf_afc[bus].last_time_tune++;
        if(bk9532_timeout(g_rf_afc[bus].last_time_tune, BK9532_AFC_TIMEOUT))
        {
            if (++g_rf_afc[bus].cur_index >= BK9532_AFC_MAX_INDEX)
            {
                    g_rf_afc[bus].cur_index = 0;
            }
            bk9532_set_xtal_adj_cfg(bus, XTAL_ADJ_TAB[g_rf_afc[bus].cur_index]);
            g_rf_afc[bus].last_time_tune = 0;
        }
        // bk9532_reg_read(bus, BK9532_REG_XTAL, &reg_val);
        // TRACE("pairing BK9532_REG_XTAL ", reg_val);
        // if time out //
        if(bk9532_timeout(g_bk9532_rf_ctx[bus].last_time_freq_change, BK9532_SCAN_FREQUENCY_TIMEOUT))
        {
            state = -1;
        }
    }
    else
    {
        if(!g_rf_afc[bus].opened)
        {
            if (bk9532_set_afc(bus, TRUE) == BK9532_PASS)
            {
                g_rf_afc[bus].opened = TRUE;
            }
        }

    }
    return state;
}
/**
 * @brief bk9532 get idcode from tx device
 * 
 * @param bus channel
 * @param idcode idcode
 * @param flag   count index for idcode
 * @return WORD 0 : idcode new 1: wait next
 */
static WORD bk9532_rf_query_idcode(WORD bus, DWORD *idcode, WORD *flag)
{
    WORD tmp;
    WORD state = 1;
    DWORD reg_usrdata;
    tmp=bk9532_get_user_data(bus, &reg_usrdata);
    TRACE("bk9532 query tmp= %x", tmp);
    if (tmp == BK9532_PASS)
    //if (bk9532_get_user_data(bus, &reg_usrdata) == BK9532_PASS)
    {
        DWORD udata = reg_usrdata & 0xFF;
        WORD ud_idx = (udata >> 5);
        WORD ud_dat = (udata & 0x1f);
        TRACE("bk9532 query idcode %x", udata);
        TRACE("bk9532 query idcode at %d", ud_idx);
        TRACE("bk9532 query idcode val %x", ud_dat);
        *idcode &= ~(((DWORD)0x1f) << (30 - 5*((udata >> 5) & 0x07)));
        *idcode |= ((udata & 0x1f) << (30 - 5*((udata >> 5) & 0x07)));
        *flag |= 1 << ((udata >> 5) & 0x07);
        udata = *idcode;
        TRACE("bk9532 query idcode flag %x", *flag);
        TRACE("bk9532 query idcode data %x", udata);
    }

    if ((*flag & 0x7f) == 0x7f)
    {
        state = 0;
    }

    return state;
}
/**
 * @brief bk9532 process change idcode and new frequency to rf module 
 * 
 * @param bus channel
 * @param idcode 
 * @param frequency 
 * @param reset reset state machine
 * @return WORD 1: wait next cycle 0: got tx signal at this idcode and frequency
 */
static WORD bk9532_rf_change_idcode_and_frequency_process(WORD bus, DWORD idcode, DWORD frequency, WORD reset)
{
    WORD state = 1;

    //static state_machine[BK9532_MAX_CHANNELS] = {0,0};

    if(reset)
    {
        state_machine[bus] = 0;
        bk9532_rf_calib_process(bus, TRUE);
        return state;
    }

    switch (state_machine[bus])
    {
    case 0:
        if(bk9532_rf_idcode_freq_set(bus, idcode, frequency) == BK9532_PASS)
        {
            state_machine[bus] = 1;
            bk9532_rf_calib_process(bus, TRUE);
            //TRACE("case0 %d ", state);
        }
        break;
    case 1:
        if(bk9532_rf_calib_process(bus, FALSE) == 0)
        {
            if ((bk9532_set_xtal_adj_cfg(bus, XTAL_ADJ_TAB[0]) == BK9532_PASS) // set xtal adj configuration //
                && (bk9532_set_afc(bus, FALSE) == BK9532_PASS)                 // turn off afc //
            )
            {
                //TRACE("case1 here0 %d ", state);
                state_machine[bus] = 2;
                _memset(&g_rf_afc[bus], 0, sizeof(struct bk9532_rf_afc));
            }
            else
            {
                state_machine[bus] = 0;
                //TRACE("case1 here1 %d ", state);
            }
        }
        break;
    case 2:
        if(bk9532_rf_detect_signal(bus) == 0)
        {            
            state = 0;
            state_machine[bus] = 0;
            //TRACE("case2 %d ", state);
        }
        break;
    case 3:
        break;    
    default:
        break;
    }
    return state;
}


/**
 * @brief bk9532 task function get idcode from new tx devices
 * 
 * @param bus channel
 * @param idcode idcode addr
 * @return WORD 0: new idcode 1: next cycle -1: no tx device detect
 */
WORD bk9532_rf_sync_idcode_handle(WORD bus, DWORD *idcode)
{
    WORD ret = 1;

    DWORD tmp;

    if (bus >= BK9532_MAX_CHANNELS)
    {
        TRACE("channel over range", bus);
        return ret;
    }

    g_bk9532_rf_ctx[bus].last_time_freq_change += 1;
    // tmp = g_bk9532_rf_ctx[bus].last_time_freq_change;
    // TRACE("bk9532 pairing last_time_pairing %d ", tmp);

    if (bk9532_timeout(g_bk9532_rf_ctx[bus].last_time_freq_change, BK9532_PAIRING_TIMEOUT))
    {
        g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_TIMEOUT;
        //TRACE("bk9532 pairing timeout = %d", g_bk9532_rf_ctx[bus].last_time_freq_change);
    }

    bk9532_rf_indicate_period(bus, g_bk9532_rf_ctx[bus].last_time_freq_change, 200);
    //TRACE("bk9532_rf_sync_idcode_handle %d ", bus);
    //TRACE("state %d ", g_bk9532_rf_ctx[bus].state);
    switch (g_bk9532_rf_ctx[bus].state)
    {
    case BK9532_RF_STATE_CHANGE_FREQ:
        if(bk9532_rf_change_idcode_and_frequency_process(bus, BK9532_PAIR_IDCODE, g_bk9532_rf_ctx[bus].pair_freq, FALSE) == 0)
        {
            g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_SYNC_IDCODE;
        }
        break;
    case BK9532_RF_STATE_SYNC_IDCODE:
        if(bk9532_rf_query_idcode(bus, idcode, &g_bk9532_rf_ctx[bus].pair_idc_flag) == 0)
        {
            g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_OK;
        }
        break;
    case BK9532_RF_STATE_TIMEOUT:
        ret = -1;
       TRACE("bk9532 pairing timeout bus=%d", bus);
        bk9532_rf_change_idcode_and_frequency_process(bus, BK9532_PAIR_IDCODE, g_bk9532_rf_ctx[bus].pair_freq, TRUE);
        g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_CHANGE_FREQ;
        g_bk9532_rf_ctx[bus].last_time_freq_change = 0;
        break;
    case BK9532_RF_STATE_OK:
        ret = 0;
        tmp =*idcode;
        TRACE("bk9532 pairing ok, with bus %d", bus);
        TRACE("bk9532 pairing ok, with idcode %x", tmp);
        g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_CHANGE_FREQ;
        //g_bk9532_rf_ctx[bus].last_time_freq_change = 0;
        bk9532_rf_indicate_onoff(bus, TRUE);
        break;
    default:
        g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_CHANGE_FREQ;
        g_bk9532_rf_ctx[bus].last_time_freq_change = 0;
        break;
    }
    return ret;
}
/**
 * @brief bk9532 task function scan tx device from 670MhZ to 693Mhz 
 * 
 * @param bus channel
 * @return WORD 
 */
static WORD bk9532_rf_scan_frequency_handle(WORD bus)
{
    WORD ret = 1;
    static WORD cnt_indicate[BK9532_MAX_CHANNELS] = {0,0};
    static WORD sw_indicate[BK9532_MAX_CHANNELS] = {1,1};
    DWORD aud_rssi;

    if (bus >= BK9532_MAX_CHANNELS)
    {
        //TRACE("channel over range", bus);
        return ret;
    }
    if(g_bk9532_rf_ctx[bus].pair_ok)
    {
        g_bk9532_rf_ctx[bus].pair_ok_cnt++;
        if(g_bk9532_rf_ctx[bus].pair_ok_cnt>4000)//check timeout 40s after pair ok
        {
           TRACE("clear time pair ok, bus=%d", bus);
           g_bk9532_rf_ctx[bus].pair_ok=FALSE;
        }
    }

    g_bk9532_rf_ctx[bus].last_time_freq_change += 1;
    //bk9532_rf_indicate_period(bus, g_bk9532_rf_ctx[bus].last_time_freq_change, 750);
    // TRACE("rf_scan %d", bus);
     //TRACE("state %d", g_bk9532_rf_ctx[bus].state);
    // id=g_bk9532_rf_ctx[bus].rf_idc;
    // TRACE("rf_idc %x",id);
    switch (g_bk9532_rf_ctx[bus].state)
    {
    case BK9532_RF_STATE_CHANGE_FREQ:
        if(bk9532_rf_change_idcode_and_frequency_process(bus, g_bk9532_rf_ctx[bus].rf_idc, g_bk9532_rf_ctx[bus].rf_freq, FALSE) == 0)
        {
            TRACE("bk9532 rf connected at  %d", g_bk9532_rf_ctx[bus].rf_freq);
            g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_CONNECTED;
            bk9532_rf_indicate_onoff(bus, TRUE);
            g_bk9532_rf_ctx[bus].is_connected = TRUE;
            check_mics_connect(FALSE);
            // bk9532_flash_save_freq(bus, g_bk9532_rf_ctx[bus].rf_freq);
            // callback connected to tx device//
        }
        else 
        {
            if (bk9532_timeout(g_bk9532_rf_ctx[bus].last_time_freq_change, BK9532_SCAN_FREQUENCY_TIMEOUT))
                g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_TIMEOUT;
            else
            {
                bk9532_rf_indicate_period(bus, g_bk9532_rf_ctx[bus].last_time_freq_change, 200);
            }    
        }
        break;
    case BK9532_RF_STATE_CONNECTED:        
        if(bk9532_rf_monitor(bus) == -1)
        {
            //TRACE("bk9532 rf lost connected at  ", g_bk9532_rf_ctx[bus].rf_freq);
            bk9532_rf_change_idcode_and_frequency_process(bus, g_bk9532_rf_ctx[bus].rf_idc, g_bk9532_rf_ctx[bus].rf_freq, TRUE);
            g_bk9532_rf_ctx[bus].last_time_freq_change = 0;
            g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_CHANGE_FREQ;
            bk9532_rf_indicate_onoff(bus, FALSE);           
            // callback lost connected to tx device //
        }
        else 
        {
            // // get user data //
            // // callback indicate new userdata //     
            // WORD tmp;       
            // DWORD reg_usrdata;
            // tmp = bk9532_get_user_data(bus, &reg_usrdata);
            // last_time_ReceiveData[bus]++;
            // if (tmp == BK9532_PASS)
            // {                
            //     if (bk9532_timeout(last_time_ReceiveData[bus], 2000))
            //     {
            //         idCodeRecvFlag[bus]=0;
            //         last_time_ReceiveData[bus]=0;
            //         //TRACE("timeout ReceiveData %x", bus);
            //     }else
            //     {
            //         WORD index, datatmp;  
            //         DWORD i;  
            //         datatmp = reg_usrdata & 0xFF;
            //         index = datatmp >> 5;
            //         idCode5bit[bus][index] = datatmp & 0x1F;
            //         idCodeRecvFlag[bus] |= 1 << index;
            //         //TRACE("reg_usrdata %x", reg_usrdata);
            //         //TRACE("Flag %x", idCodeRecvFlag[bus]);
            //         if (idCodeRecvFlag[bus] == 0x7f)
            //         {
            //             reg_usrdata=0;
            //             for (i = 0; i < BK9532_IDCODE_RX_LENGTH; i++)
            //             {
            //                 reg_usrdata <<= 5;
            //                 reg_usrdata |= idCode5bit[bus][i];
            //             }
            //             //TRACE("here reg_usrdata %x", reg_usrdata);
            //             //i=cpu_swap_edian(reg_usrdata);
            //             //TRACE("swap %x", i);
            //             //i=CRC8_Array((WORD *)&i,3);
            //             //i=CRC8_Array((WORD *)&reg_usrdata,3);
            //             i=CRC8_Array(reg_usrdata,3);
                       
            //             //TRACE("i %x", i);
            //             if(i==((reg_usrdata>>24)&0x0ff))
            //                 parseDataFromMic(reg_usrdata);
            //             idCodeRecvFlag[bus]=0;  
            //             last_time_ReceiveData[bus]=0;    
            //         }                                
            //     }               
            // }
            
            // audio rssi //
            bk9532_get_audio_rssi(bus, &aud_rssi);
            if (aud_rssi > 80)
			{
				bk9532_rf_indicate_onoff(bus, TRUE);
			}
			else
			{
				bk9532_rf_indicate_onoff(bus, FALSE);
			}
            
        }

        break;
    case BK9532_RF_STATE_TIMEOUT: // scan next frequency //
        ret = -1;
        
        
        if(++g_bk9532_rf_ctx[bus].rf_freq > g_bk9532_rf_ctx[bus].rf_freq_max)
        {
            g_bk9532_rf_ctx[bus].rf_freq = g_bk9532_rf_ctx[bus].rf_freq_min;
        }
        bk9532_rf_change_idcode_and_frequency_process(bus, g_bk9532_rf_ctx[bus].rf_idc, g_bk9532_rf_ctx[bus].rf_freq, TRUE);
        g_bk9532_rf_ctx[bus].last_time_freq_change = 0;
        g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_CHANGE_FREQ;

        if(++cnt_indicate[bus] >= 5)
        {
            //bk9532_rf_indicate_onoff(bus, sw_indicate[bus]);
            //sw_indicate[bus] = ~sw_indicate[bus];
            cnt_indicate[bus] = 0;
        }
       // TRACE("bk9532 rf channel %d ", bus);
        //TRACE("bk9532 rf next freq %d", g_bk9532_rf_ctx[bus].rf_freq);
        aud_rssi = g_bk9532_rf_ctx[bus].rf_idc;
        //TRACE("bk9532 rf idc ", aud_rssi);
        if(g_bk9532_rf_ctx[bus].is_connected && !g_bk9532_rf_ctx[bus].pair_ok)
        {
            TRACE("BK9532_RF_STATE_TIMEOUT  %d", bus);
            g_bk9532_rf_ctx[bus].is_connected = FALSE;
            check_mics_connect(FALSE);
        }
        break;
    default:
        g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_CHANGE_FREQ;
        g_bk9532_rf_ctx[bus].last_time_freq_change = 0;
        break;
    }

    return ret;
}


 void bk9532_task_handler(WORD bus)
{
    //static state_machine[BK9532_MAX_CHANNELS] = {0, 0};
int state;
    switch (state_pair[bus])
    {
    case 0: // get idcode //
        /*if(bk9532_rf_sync_idcode_handle(bus, &g_bk9532_rf_ctx[bus].pair_idc) == 0)
        {
            state_pair[bus] = 1;
            g_bk9532_rf_ctx[bus].rf_idc =  g_bk9532_rf_ctx[bus].pair_idc;
            uart_send_ID_mic_pair(bus,g_bk9532_rf_ctx[bus].rf_idc);
            //save idcode to rom //
        }*/
        state = bk9532_rf_sync_idcode_handle(bus, &g_bk9532_rf_ctx[bus].pair_idc);
        if(state != 1)
        {
			DWORD tmp;
            //TRACE("bk9532_task_handler 0 state: %d",state);
            if(state == 0)
            {            
                //save idcode to rom //
                g_bk9532_rf_ctx[bus].rf_idc =  g_bk9532_rf_ctx[bus].pair_idc;
                bk9532_flash_save_idcode(bus, g_bk9532_rf_ctx[bus].rf_idc);                
                tmp=g_bk9532_rf_ctx[bus].rf_idc;
                TRACE("pair ok, bus: %x", bus);
                TRACE("save idcode: %x", tmp);
                bk9532_rf_indicate_onoff(bus, TRUE);
                g_bk9532_rf_ctx[bus].is_connected = TRUE;
                check_mics_connect(FALSE);
                g_bk9532_rf_ctx[bus].pair_ok=TRUE;
                g_bk9532_rf_ctx[bus].pair_ok_cnt=0;
                state_pair[bus] = 1;
            }
            else
            {
                //load idcode from rom //

                bk9532_flash_load_idcode(bus, &g_bk9532_rf_ctx[bus].rf_idc);
                tmp=g_bk9532_rf_ctx[bus].rf_idc;
                TRACE("pair fail,bus: %x", bus);
                TRACE("load idcode: %x", tmp);
                state_pair[bus] = 1;
            }
                //load freq from rom //
           // bk9532_flash_load_freq(bus, &g_bk9532_rf_ctx[bus].rf_freq);
           // TRACE("bk9532 flash load freq: %d", g_bk9532_rf_ctx[bus].rf_freq);
            //state_pair[bus] = 1;
        }
        break;
    case 1: // scan frequency //
        bk9532_rf_scan_frequency_handle(bus);
        break;
    case 2:
        g_bk9532_rf_ctx[bus].last_time_freq_change += 1;    
        if (bk9532_timeout(g_bk9532_rf_ctx[bus].last_time_freq_change, BK9532_PAIRING_TIMEOUT))
        {
             g_bk9532_rf_ctx[bus].last_time_freq_change =0; 
             state_pair[bus] = 1;   
        }
        break;    
    default:
        break;
    }
}

void  bk9532_mic_reset_pair(void)
{
    state_pair[0] = 0;
    state_pair[1] = 0;
    state_machine[0] = 0;
    state_machine[1] = 0;
    bk9532_TurnLED(FALSE);
    //bk9532_rf_param_init(0);
    //bk9532_rf_param_init(1);    
}

DWORD bk9532_get_ID_pair(WORD bus)
{
    DWORD id=0;
    if(g_bk9532_rf_ctx[bus].is_connected)
	   id= g_bk9532_rf_ctx[bus].rf_idc;
    return id;   
} 
WORD  bk9532_mic_is_connected(WORD bus)
{
	return g_bk9532_rf_ctx[bus].is_connected;
} 
static void bk9532_task_handler_channel_0(void)
{
    bk9532_task_handler(0);
}

static void bk9532_task_handler_channel_1(void)
{
    bk9532_task_handler(1);
}



static WORD bk9532_rf_param_init(WORD bus)
{
    _memset(&g_bk9532_rf_ctx[bus], 0, sizeof(struct bk9532_rf_context));
    g_bk9532_rf_ctx[bus].pair_freq = BK9532_PAIR_FREQUENCY_MHZ + bus;
    g_bk9532_rf_ctx[bus].rf_freq_min = BK9532_SCAN_FREQUENCY_MIN_MHZ + (BK9532_SCAN_FREQUENCY_MAX_MHZ - BK9532_SCAN_FREQUENCY_MIN_MHZ + 1)*bus;
    g_bk9532_rf_ctx[bus].rf_freq_max = BK9532_SCAN_FREQUENCY_MAX_MHZ + (BK9532_SCAN_FREQUENCY_MAX_MHZ - BK9532_SCAN_FREQUENCY_MIN_MHZ + 1)*bus;
    g_bk9532_rf_ctx[bus].rf_freq = g_bk9532_rf_ctx[bus].rf_freq_min;
    
    return 0;
}

static WORD bk9532_rf_chip_init(WORD bus)
{
    // default rom //
    WORD i;
    DWORD reg_val;
     WORD ret;
    for (i = 0; i < sizeof(g_bk9532_rom) / sizeof(struct bk9532_reg_val); i++)
    {
        ret=bk9532_reg_write(bus, g_bk9532_rom[i].reg_addr, g_bk9532_rom[i].reg_val);
        //TRACE("ret=%x", ret);
    }

    bk9532_set_rf_chip_reset(bus);
    bk9532_set_rf_uhf_band(bus);
    bk9532_set_rf_frequency_range(bus, BK9532_FREQUENCY_BELOW_710MHZ);
    bk9532_set_rf_ant_mode(bus, 0);
    bk9532_set_rf_ant_pin(bus, 1);
    bk9532_set_adpcm_err_mode(bus, 1);
    bk9532_set_rf_plc(bus, TRUE);
    bk9532_set_afc(bus, FALSE);

    // // bk9532_dump_regs(bus);
    // for (i = 0; i < sizeof(g_bk9532_rom) / sizeof(struct bk9532_reg_val); i++)
    // {
    //     bk9532_reg_read(bus, g_bk9532_rom[i].reg_addr, &reg_val);
    //     TRACE("bk9532 reg addr %x", g_bk9532_rom[i].reg_addr);
    //     TRACE("bk9532 reg val %x", reg_val);
    // }
    bk9532_rf_indicate_init(bus);
    //bk9532_set_audio_volume(bus, 24);
    bk9532_set_audio_volume(bus, 18);//note
	//bk9532_set_audio_volume(bus, 12);//note
    
#if 0   // test i2s //

    bk9532_i2s_config(bus, 0); // 0: slave; 1: master
    bk9532_set_audio_enable(bus, TRUE);

    bk9532_reg_read(bus, 0x30, &reg_val);
    TRACE("bk9532 reg addr [0x30]=%x", reg_val);
    bk9532_reg_read(bus, 0x31, &reg_val);
    TRACE("bk9532 reg addr [0x31]=%x", reg_val);
    bk9532_reg_read(bus, 0x36, &reg_val);
    TRACE("bk9532 reg addr [0x36]=%x", reg_val);
    // bk9532_i2s_config(bus, 0); // 0: slave; 1: master
    // bk9532_set_audio_enable(bus, TRUE);
#else
    bk9532_i2s_config(bus, 0); // 0: slave; 1: master
    bk9532_set_audio_enable(bus, FALSE);
#endif
    return 0;
}

void bk9532_register_task(WORD bus)
{
    bk9532_rf_param_init(bus);
    bk9532_rf_chip_init(bus);
    //_Sys_SchedRegister(&bk9532_task[bus], bk9532_task_handler_channel[bus], BK9532_STEP_MS);
}

int bk9532_flash_save_freq(WORD bus, WORD frequency)
{
    int rc ;
    WORD ffrequency = 0;

    if(bus == 0)
    {
        rc = pms_get_word(BK9532_FLASH_ID_FREQUENCY_CHA, &ffrequency);
    }
    else
    {
        rc = pms_get_word(BK9532_FLASH_ID_FREQUENCY_CHB, &ffrequency);
    }

    if (rc == sizeof(WORD) && frequency == ffrequency)
        return rc;

    if(bus == 0)
    {
        return pms_set_word(BK9532_FLASH_ID_FREQUENCY_CHA, frequency);
    }
    else
    {
        return pms_set_word(BK9532_FLASH_ID_FREQUENCY_CHB, frequency);
    }
}

int bk9532_flash_load_freq(WORD bus, PWORD frequency)
{
    int rc ;

    if(bus == 0)
    {
        rc = pms_get_word(BK9532_FLASH_ID_FREQUENCY_CHA, frequency);
    }
    else {
        rc = pms_get_word(BK9532_FLASH_ID_FREQUENCY_CHB, frequency);
    }

    if(rc != sizeof(WORD))
    {
        if(bus == 0)
        {
            *frequency = BK9532_FLASH_FREQUENCY_DEFUALT_CHA;
        }else {
            *frequency = BK9532_FLASH_FREQUENCY_DEFUALT_CHB;
        }
    }

    return rc;
}

int bk9532_flash_save_idcode(WORD bus, DWORD idcode)
{
    int rc ;
    DWORD fidcode = 0;

    if(bus == 0)
    {
        rc = pms_get_dword(BK9532_FLASH_ID_IDCODE_CHA, &fidcode);
    }
    else
    {
        rc = pms_get_dword(BK9532_FLASH_ID_IDCODE_CHB, &fidcode);
    }

    if (rc == sizeof(DWORD) && idcode == fidcode)
        return rc;

    if(bus == 0)
    {
        return pms_set_dword(BK9532_FLASH_ID_IDCODE_CHA, idcode);
    }
    else
    {
        return pms_set_dword(BK9532_FLASH_ID_IDCODE_CHB, idcode);
    }
}

int bk9532_flash_load_idcode(WORD bus, PDWORD idcode)
{
    int rc ;

    if(bus == 0)
    {
        rc = pms_get_dword(BK9532_FLASH_ID_IDCODE_CHA, idcode);
    }
    else {
        rc = pms_get_dword(BK9532_FLASH_ID_IDCODE_CHB, idcode);
    }

    if(rc != sizeof(DWORD))
    {
        if(bus == 0)
        {
            *idcode = BK9532_FLASH_IDCODE_DEFUALT_CHA;
        }else {
            *idcode = BK9532_FLASH_IDCODE_DEFUALT_CHB;
        }
    }

    return rc;
}
