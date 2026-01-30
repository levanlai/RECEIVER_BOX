#include "../sys/sys_i2c_bk9532.h"
#include "../bk9532/bk9532.h"
#include <math.h>
#include <trace.h>


DWORD bk9532_get_chip_id(WORD bus)
{
    DWORD reg_val;
    WORD state;

    state = bk9532_reg_read(bus, 0x70, &reg_val);

    if (state == I2C_OK)
    {
        TRACE("bk9532 bus %d", bus);
        TRACE("bk9532 chipID %x", reg_val);
    }
    else
    {
        TRACE("bk9532 bus %d", bus);
        TRACE("bk9532 i2c get chipID error %d ", state);
    }
    if ((reg_val & 0xffff) == (DWORD)0x9532)
    {
        TRACE("bk9532 chip id bus %d pass\n", bus);
    }
    else
    {
        TRACE("bk9532 chip id bus %d falied\n", bus);
    }
    return reg_val;
}

void bk9532_test_chip_id(void)
{
    DWORD chipid0 = bk9532_get_chip_id(I2C_PORT_MIC1);
    DWORD chipid1 = bk9532_get_chip_id(I2C_PORT_MIC2);
}

// void bk9532_test_write_and_read_bus(WORD bus)
// {
//     WORD state;
//     DWORD rreg_val = 0;
//     DWORD wreg_val = 0;
//     TRACE("bk9532 test wr bus %d ", bus);
//     state = bk9532_reg_read(bus, 0x30, &rreg_val);
//     TRACE("bk9532 test wr read state = %x\n", state);
//     TRACE("bk9532 test wr read val = %x\n", rreg_val);
//     wreg_val = rreg_val + 5;
//     state = bk9532_reg_write(bus, 0x30, wreg_val);
//     TRACE("bk9532 test wr write state = %x\n", state);
//     state = bk9532_reg_read(bus, 0x30, &rreg_val);
//     TRACE("bk9532 test wr read back state = %x\n", state);
//     TRACE("bk9532 test wr read back val = %x\n", rreg_val);

//     if (wreg_val == rreg_val)
//     {
//         TRACE("bk9532 test rwr %d pass\n", bus);
//     }
//     else
//     {
//         TRACE("bk9532 test rwr %d failed\n", bus);
//     }
// }

// void bk9532_test_write_and_read(void)
// {
//     bk9532_test_write_and_read_bus(0);
//     bk9532_test_write_and_read_bus(1);
// }

#if 0
// bk9532 test pairing //

#define BK9532_MAX_CHANNELS 2
#define BK9532_PAIRING_TIMEOUT 3000   // ms
#define BK9532_AFC_TIMEOUT 50 // ms
#define BK9532_PAIR_FREQUENCY_MHZ 670
#define BK9532_PAIR_IDCODE 0xFFFFFFFF

#define BK9532_SCAN_FREQUENCY_MIN_MHZ 676
#define BK9532_SCAN_FREQUENCY_MAX_MHZ 684
#define BK9532_SCAN_FREQUENCY_TIMEOUT 150   // ms

#define BK9532_PASS I2C_OK

enum BK9532_RF_STATE
{
    BK9532_RF_STATE_CHANGE_FREQ = 0,
    BK9532_RF_STATE_SYNC_IDCODE,
    BK9532_RF_STATE_CONNECTED,
    BK9532_RF_STATE_OK,
    BK9532_RF_STATE_TIMEOUT,
};

enum BK9532_FREQUENCY_RANGE
{
    BK9532_FREQUENCY_BELOW_178MHZ = 0,
    BK9532_FREQUENCY_BELOW_270MHZ,
    BK9532_FREQUENCY_BELOW_710MHZ,
    BK9532_FREQUENCY_BELOW_980MHZ
};

struct bk9532_rf_afc
{
    WORD count;
    WORD cur_index;
    WORD max_index;
    WORD opened;
    DWORD last_time_tune;
};

struct bk9532_rf_context
{
    WORD state;
    WORD last_time_freq_change;
    // pairing //
    WORD pair_idc_flag;
    WORD pair_freq;
    DWORD pair_idc;
    // rf //
    WORD rf_freq;
    WORD rf_freq_max;
    WORD rf_freq_min;
    DWORD rf_idc;
};


struct bk9532_reg_val
{
    WORD reg_addr;
    DWORD reg_val;
};

const struct bk9532_reg_val g_bk9532_rom[] = {
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
    {0x30, 0x4D4D4D4D}, // 60 REG30, ??GPIO3,GPIO2,GPIO1,GPIO0????
    {0x31, 0xC108007D}, // 62 REG31, ?GPIO3????? GPIO0,GPIO1,GPIO2?I2S????
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


struct bk9532_rf_context g_bk9532_rf_ctx[BK9532_MAX_CHANNELS];
struct bk9532_rf_afc g_rf_afc[BK9532_MAX_CHANNELS];

#define BK9532_STEP_MS 10 //ms  min 10ms // 

#define bk9532_timeout(a, b) ((a) * BK9532_STEP_MS > b)

#define BK9532_REG_TX_SIGNAL 0x7C
#define BK9532_REG_TX_SIGNAL_MASK (((DWORD)1) << 27)
WORD bk9532_get_rf_signal(WORD bus)
{
    DWORD reg_rf_signal;
    if (bk9532_reg_read(bus, BK9532_REG_TX_SIGNAL, &reg_rf_signal) == I2C_OK)
    {
        if ((reg_rf_signal & BK9532_REG_TX_SIGNAL_MASK) == BK9532_REG_TX_SIGNAL_MASK)
            return 1; // no signal
    }
    //TRACE("bk9532_rf_get_signal %x", reg_rf_signal);
    return 0;
}

#define BK9532_REG_IDCODE 0x39
WORD bk9532_set_idcode(WORD bus, DWORD idcode)
{
    return bk9532_reg_write(bus, BK9532_REG_IDCODE, idcode);
}

WORD bk9532_get_idcode(WORD bus, DWORD *idcode)
{
    return bk9532_reg_read(bus, BK9532_REG_IDCODE, idcode);
}

#define BK9532_AFC_MAX_INDEX 5

const WORD XTAL_ADJ_TAB[BK9532_AFC_MAX_INDEX] = {0x31, 0x43, 0x55, 0x67, 0x79};

#define BK9532_REG_XTAL 0x38
#define BK9532_REG_XTAL_SHIFT 24
#define BK9532_REG_XTAL_MASK 0x7F
WORD bk9532_set_xtal_adj_cfg(WORD bus, WORD xtal_adj)
{
    return bk9532_reg_and_mask_or_flag(bus, BK9532_REG_XTAL, ~((DWORD)BK9532_REG_XTAL_MASK << BK9532_REG_XTAL_SHIFT), (DWORD)xtal_adj << BK9532_REG_XTAL_SHIFT);
}

#define BK9532_REG_AFC 0x3F
#define BK9532_REG_AFC_EN ((DWORD)1 << 26)
WORD bk9532_set_afc(WORD bus, WORD enable)
{
    if (enable)
        return bk9532_reg_or_mask(bus, BK9532_REG_AFC, BK9532_REG_AFC_EN);
    else
        return bk9532_reg_and_mask(bus, BK9532_REG_AFC, ~BK9532_REG_AFC_EN);
}

// (freq + 0.16384) * 6 * 2^23 / 24.576 // for range 500-700 MHz
// (freq + 0.16384) * 4 * 2^23 / 24.576 // for range 700-900 MHz
#define FLOAT_0_16384 0x3E27C5AC
#define FLOAT_SIX_2POW23 0x4C400000  // 50331648.000000 = 2^23 * 6
#define FLOAT_FOUR_2POW23 0x4C000000 // 33554432.000000 = 2^23 * 4
#define FLOAT_24_576 0x41C49BA6      // 24.576000 = 24.576

#define BK9532_REG_FREQUENCY 0x0D
WORD bk9532_set_frequency(WORD bus, DWORD frequency)
{
    FLOAT freq_cal;
    DWORD frequency_bk9532;
    if (500 <= frequency && frequency <= 710)
    {
        freq_cal = _fdiv(_fmul(_fadd(_float(frequency), FLOAT_0_16384), FLOAT_SIX_2POW23), FLOAT_24_576);
        frequency_bk9532 = _ftol(freq_cal) + 56 ;// error converted;
    }
    else if (710 < frequency && frequency < 980)
    {
        freq_cal = _fdiv(_fmul(_fadd(_float(frequency), FLOAT_0_16384), FLOAT_FOUR_2POW23), FLOAT_24_576);
        frequency_bk9532 = _ftol(freq_cal)+ 56 ;// error converted;
    }
    TRACE("bk9532_set_frequency freq %d", frequency);
    TRACE("bk9532_set_frequency freq cal", freq_cal);
    TRACE("bk9532_set_frequency freq convert %x", frequency_bk9532);
    return bk9532_reg_write(bus, BK9532_REG_FREQUENCY, frequency_bk9532);
}

WORD bk9532_get_frequency(WORD bus, DWORD *freq)
{
    return bk9532_reg_read(bus, BK9532_REG_FREQUENCY, freq);
}

#define BK9532_REG_RF_LOOP_LDO_VOL 0x06
#define BK9532_REG_RF_LOOP_LDO_VOL_SHIFT 29
WORD bk9532_set_rf_loop_ldo_vol(WORD bus, WORD enable)
{
    if (enable)
        return bk9532_reg_or_mask(bus, BK9532_REG_RF_LOOP_LDO_VOL, (DWORD)0x02 << BK9532_REG_RF_LOOP_LDO_VOL_SHIFT);
    else
        return bk9532_reg_and_mask(bus, BK9532_REG_RF_LOOP_LDO_VOL, ~((DWORD)0x07 << BK9532_REG_RF_LOOP_LDO_VOL_SHIFT));
}

#define BK9532_REG_RF_VCO_CB 0x03
#define BK9532_REG_RF_VCO_CB_SHIFT 22
WORD bk9532_set_rf_vco_cb_toggle(WORD bus)
{
    WORD state = bk9532_reg_and_mask(bus, BK9532_REG_RF_VCO_CB, ~((DWORD)0x1 << BK9532_REG_RF_VCO_CB_SHIFT));
    if (state == BK9532_PASS)
    {
        return bk9532_reg_or_mask(bus, BK9532_REG_RF_VCO_CB, (DWORD)0x1 << BK9532_REG_RF_VCO_CB_SHIFT);
    }
    return state;
}

#define BK9532_REG_RF_VCO_CB_DIG 0x04
#define BK9532_REG_RF_VCO_CB_DIG_SHIFT 25
WORD bk9532_set_rf_vco_cb_dig_toggle(WORD bus)
{
    WORD state = bk9532_reg_and_mask(bus, BK9532_REG_RF_VCO_CB_DIG, ~((DWORD)0x1 << BK9532_REG_RF_VCO_CB_DIG_SHIFT));
    if (state == BK9532_PASS)
    {
        return bk9532_reg_or_mask(bus, BK9532_REG_RF_VCO_CB_DIG, (DWORD)0x1 << BK9532_REG_RF_VCO_CB_DIG_SHIFT);
    }
    return state;
}

#define BK9532_REG_RF_VCO_CB_CLK 0x07
#define BK9532_REG_RF_VCO_CB_CLK_SHIFT 25
WORD bk9532_set_rf_vco_cb_clk(WORD bus, WORD enable)
{
    if (enable)
        return bk9532_reg_or_mask(bus, BK9532_REG_RF_VCO_CB_CLK, (DWORD)0x1 << BK9532_REG_RF_VCO_CB_CLK_SHIFT);
    else
        return bk9532_reg_and_mask(bus, BK9532_REG_RF_VCO_CB_CLK, ~((DWORD)0x1 << BK9532_REG_RF_VCO_CB_CLK_SHIFT));
}

#define BK9532_REG_RF_BASEBAND 0x3F
#define BK9532_REG_RF_BASEBAND_SHIFT 31
WORD bk9532_set_rf_baseband_reset(WORD bus)
{
    WORD state = bk9532_reg_and_mask(bus, BK9532_REG_RF_BASEBAND, ~((DWORD)0x1 << BK9532_REG_RF_BASEBAND_SHIFT));
    if (state == BK9532_PASS)
    {
        return bk9532_reg_or_mask(bus, BK9532_REG_RF_BASEBAND, (DWORD)0x1 << BK9532_REG_RF_BASEBAND_SHIFT);
    }
    return state;
}

#define BK9532_REG_RF_CHIP 0x3F
#define BK9532_REG_RF_CHIP_SHIFT 5
WORD bk9532_set_rf_chip_reset(WORD bus)
{
    WORD state = bk9532_reg_and_mask(bus, BK9532_REG_RF_CHIP, ~((DWORD)0x1 << BK9532_REG_RF_CHIP_SHIFT));
    if (state == BK9532_PASS)
    {
        return bk9532_reg_or_mask(bus, BK9532_REG_RF_CHIP, (DWORD)0x1 << BK9532_REG_RF_CHIP_SHIFT);
    }
    return state;
}

#define BK9532_REG_PLC 0x3F
#define BK9532_REG_PLC_SHIFT 28
WORD bk9532_set_rf_plc_reset(WORD bus)
{
    WORD state = bk9532_reg_or_mask(bus, BK9532_REG_PLC, (DWORD)0x1 << BK9532_REG_PLC_SHIFT);
    if (state == BK9532_PASS)
    {
        return bk9532_reg_and_mask(bus, BK9532_REG_PLC, ~((DWORD)0x1 << BK9532_REG_PLC_SHIFT));
    }
    return state;
}

#define BK9532_REG_USER_DATA 0x7C
WORD bk9532_get_user_data(WORD bus, DWORD *user_data)
{
    return bk9532_reg_read(bus, BK9532_REG_USER_DATA, user_data);
}

WORD bk9532_set_rf_uhf_band(WORD bus)
{
    bk9532_reg_and_mask_or_flag(bus, 0x03, ~((DWORD)0x3 << 20), ((DWORD)0x1 << 20));
    bk9532_reg_and_mask_or_flag(bus, 0x07, ~((DWORD)0x7 << 17), ((DWORD)0x7 << 17));
    bk9532_reg_and_mask_or_flag(bus, 0x3E, ~((DWORD)0xF << 24), ((DWORD)0xF << 24));
    return bk9532_reg_write(bus, 0x08, 0xEFF1194C);
}

WORD bk9532_set_rf_vhf_band(WORD bus)
{
    bk9532_reg_and_mask_or_flag(bus, 0x03, ~((DWORD)0x3 << 20), ((DWORD)0x2 << 20));
    bk9532_reg_and_mask_or_flag(bus, 0x07, ~((DWORD)0x7 << 17), ((DWORD)0x2 << 17));
    bk9532_reg_and_mask_or_flag(bus, 0x3E, ~((DWORD)0xF << 24), ((DWORD)0xD << 24));
    return bk9532_reg_write(bus, 0x08, 0xEDCD874C);
}

WORD bk9532_set_rf_frequency_range(WORD bus, WORD freq_range)
{
    if (freq_range == BK9532_FREQUENCY_BELOW_178MHZ)
    {
        return bk9532_reg_and_mask_or_flag(bus, 0x03, ~((DWORD)0x7 << 13), ((DWORD)0x2 << 13));
    }
    else if (freq_range == BK9532_FREQUENCY_BELOW_270MHZ || freq_range == BK9532_FREQUENCY_BELOW_710MHZ)
    {
        return bk9532_reg_and_mask_or_flag(bus, 0x03, ~((DWORD)0x7 << 13), ((DWORD)0x1 << 13));
    }
    else if (freq_range == BK9532_FREQUENCY_BELOW_980MHZ)
    {
        return bk9532_reg_and_mask_or_flag(bus, 0x03, ~((DWORD)0x7 << 13), ((DWORD)0x0 << 13));
    }

    return ~BK9532_PASS;
}

#define BK9532_REG_ANT 0x3F
#define BK9532_REG_ANT_SHIFT 17
WORD bk9532_set_rf_ant_mode(WORD bus, WORD mode)
{
    if (mode == 1)
    {
        return bk9532_reg_or_mask(bus, BK9532_REG_ANT, ((DWORD)1 << BK9532_REG_ANT_SHIFT));
    }
    else
    {
        return bk9532_reg_and_mask(bus, BK9532_REG_ANT, ~((DWORD)1 << BK9532_REG_ANT_SHIFT));
    }
}

#define BK9532_REG_ANT_PIN 0x3F
#define BK9532_REG_ANT_PIN_SHIFT 16
WORD bk9532_set_rf_ant_pin(WORD bus, WORD pin)
{
    if (pin == 1)
    {
        return bk9532_reg_or_mask(bus, BK9532_REG_ANT_PIN, ((DWORD)1 << BK9532_REG_ANT_PIN_SHIFT));
    }
    else
    {
        return bk9532_reg_and_mask(bus, BK9532_REG_ANT_PIN, ~((DWORD)1 << BK9532_REG_ANT_PIN_SHIFT));
    }
}

#define BK9532_REG_ADPCM_ERR_MODE 0x3d
#define BK9532_REG_ADPCM_ERR_MODE_SHIFT 14
WORD bk9532_set_adpcm_err_mode(WORD bus, WORD mode)
{

    if (mode == 1)
    {
        return bk9532_reg_or_mask(bus, BK9532_REG_ADPCM_ERR_MODE, ((DWORD)1 << BK9532_REG_ADPCM_ERR_MODE_SHIFT));
    }
    else
    {
        return bk9532_reg_and_mask(bus, BK9532_REG_ADPCM_ERR_MODE, ~((DWORD)1 << BK9532_REG_ADPCM_ERR_MODE_SHIFT));
    }
}

#define BK9532_REG_PLC_CTR 0x3B
#define BK9532_REG_PLC_CTR_SHIFT 11
WORD bk9532_set_rf_plc(WORD bus, WORD enable)
{
    WORD state;
    if (enable)
    {
        state = bk9532_reg_or_mask(bus, BK9532_REG_PLC_CTR, ((DWORD)1 << BK9532_REG_PLC_CTR_SHIFT));
    }
    else
    {
        state = bk9532_reg_and_mask(bus, BK9532_REG_PLC_CTR, ~((DWORD)1 << BK9532_REG_PLC_CTR_SHIFT));
    }

    if (state != BK9532_PASS)
        return state;
    return bk9532_set_rf_plc_reset(bus);
}


#define BK9532_REG_AUDIO_RSSI 0x79

WORD bk9532_get_audio_rssi(WORD bus, DWORD *au_rssi)
{
    WORD state =  bk9532_reg_read(bus, BK9532_REG_AUDIO_RSSI, au_rssi);
    *au_rssi = *au_rssi & 0xFFFF;
    return state;
}

#define BK9532_REG_VOL_GAIN 0x32
WORD bk9532_set_audio_volume(WORD bus, WORD vol)
{
    DWORD vol_duf ;
    DWORD vol_eq ;
    DWORD vol_all;

#if 0
	vol_duf = ((DWORD)vol & 0x1F) << 0);
	vol_eq =  ((DWORD)18  & 0x1F) << 5);;//18:-6dB;24:0dB
#else  
	vol_duf = ((DWORD)vol & 0x1F) << 0;
	vol_eq =  ((DWORD)24  & 0x1F) << 5;//18:-6dB;24:0dB
#endif
    vol_all = vol_duf | vol_eq;

    return bk9532_reg_and_mask_or_flag(bus, BK9532_REG_VOL_GAIN, ~((DWORD)0x3FF) , vol_all);
}

#define BK9532_REG_VOL_MUTE 0x36
WORD bk9532_set_audio_enable(WORD bus, WORD enable)
{
    return bk9532_reg_and_mask_or_flag(bus, BK9532_REG_VOL_MUTE, ~((DWORD)1 << 13), (((DWORD)enable&0x01) << 13));
}

enum BK9532_GPIO_PIN
{
	BK9532_GPIO_PIN_0 = 0,
	BK9532_GPIO_PIN_1,
	BK9532_GPIO_PIN_2,
	BK9532_GPIO_PIN_3,
	BK9532_GPIO_PIN_4
};

enum BK9532_GPIO_MODE
{
	BK9532_GPIO_MODE_INPUT = 0,
	BK9532_GPIO_MODE_OUTPUT,
	BK9532_GPIO_MODE_SND_FUNC_INPUT,
	BK9532_GPIO_MODE_SND_FUNC_OUTPUT
};

enum BK9532_GPIO_PULLUPDOWN
{
	BK9532_GPIO_PULLDOWN = 0,
	BK9532_GPIO_PULLUP,
	BK9532_GPIO_NOPULL,
};

#define BK9532_REG_GPIO_CFG_R1 0x30
#define BK9532_REG_GPIO_CFG_R2 0x31

#define BK9532_REG_GPIO_CFG_GPIO_IP             ((DWORD)1 << 0)
#define BK9532_REG_GPIO_CFG_GPIO_OP             ((DWORD)1 << 1)
#define BK9532_REG_GPIO_CFG_GPIO_IP_EN          ((DWORD)1 << 2)
#define BK9532_REG_GPIO_CFG_GPIO_OP_EN          ((DWORD)1 << 3)
#define BK9532_REG_GPIO_CFG_GPIO_PUPD           ((DWORD)1 << 4)
#define BK9532_REG_GPIO_CFG_GPIO_PUPD_EN        ((DWORD)1 << 5)
#define BK9532_REG_GPIO_CFG_GPIO_2ND_FUNC_EN    ((DWORD)1 << 6)
#define BK9532_REG_GPIO_CFG_GPIO_MASK 0xFF


WORD bk9532_set_gpio_output(WORD bus, WORD pin , WORD level)
{

    if(pin == BK9532_GPIO_PIN_4)
    {
        return bk9532_reg_and_mask_or_flag(bus, BK9532_REG_GPIO_CFG_R2, ~((DWORD)1 << 1), (((DWORD)level&0x01) << 1));
    }
    else
    {
        return bk9532_reg_and_mask_or_flag(bus, BK9532_REG_GPIO_CFG_R1, ~(((DWORD)1 << 1) << (pin << 3)), ((((DWORD)level&0x01) << 1) << (pin << 3)));
    }
}

WORD bk9532_set_gpio_mode(WORD bus, WORD pin, WORD mode, WORD pupd)
{
    DWORD reg_cfg = 0x7F;

if (mode == BK9532_GPIO_MODE_INPUT)
	{
		reg_cfg &= ~BK9532_REG_GPIO_CFG_GPIO_2ND_FUNC_EN;
		reg_cfg |= BK9532_REG_GPIO_CFG_GPIO_OP_EN;
		reg_cfg |= BK9532_REG_GPIO_CFG_GPIO_IP_EN;
		reg_cfg &= ~BK9532_REG_GPIO_CFG_GPIO_OP;
	}
	else if (mode == BK9532_GPIO_MODE_OUTPUT)
	{
		reg_cfg &= ~BK9532_REG_GPIO_CFG_GPIO_2ND_FUNC_EN;
		reg_cfg &= ~BK9532_REG_GPIO_CFG_GPIO_OP_EN;
		reg_cfg &= ~BK9532_REG_GPIO_CFG_GPIO_IP_EN;
		reg_cfg &= ~BK9532_REG_GPIO_CFG_GPIO_OP;
		reg_cfg &= ~BK9532_REG_GPIO_CFG_GPIO_IP;
	}
	else if (mode == BK9532_GPIO_MODE_SND_FUNC_INPUT)
	{
		reg_cfg |= BK9532_REG_GPIO_CFG_GPIO_OP_EN;
		reg_cfg |= BK9532_REG_GPIO_CFG_GPIO_IP_EN;
		reg_cfg |= BK9532_REG_GPIO_CFG_GPIO_2ND_FUNC_EN;
		reg_cfg &= ~BK9532_REG_GPIO_CFG_GPIO_OP;
	}
	else if (mode == BK9532_GPIO_MODE_SND_FUNC_OUTPUT)
	{
		reg_cfg &= ~BK9532_REG_GPIO_CFG_GPIO_OP_EN;
		reg_cfg &= ~BK9532_REG_GPIO_CFG_GPIO_IP_EN;
		reg_cfg |= BK9532_REG_GPIO_CFG_GPIO_2ND_FUNC_EN;
		reg_cfg &= ~BK9532_REG_GPIO_CFG_GPIO_OP;
		reg_cfg &= ~BK9532_REG_GPIO_CFG_GPIO_IP;
	}

	if (pupd == BK9532_GPIO_NOPULL)
	{
		reg_cfg &= ~BK9532_REG_GPIO_CFG_GPIO_PUPD_EN;
		reg_cfg &= ~BK9532_REG_GPIO_CFG_GPIO_PUPD;
	}
	else
	{
		reg_cfg |= BK9532_REG_GPIO_CFG_GPIO_PUPD_EN;
		if (pupd == BK9532_GPIO_PULLUP)
		{
			reg_cfg |= BK9532_REG_GPIO_CFG_GPIO_PUPD;
		}
		else
		{
			reg_cfg &= ~BK9532_REG_GPIO_CFG_GPIO_PUPD;
		}
	}

    if(pin == BK9532_GPIO_PIN_4)
    {
        return bk9532_reg_and_mask_or_flag(bus, BK9532_REG_GPIO_CFG_R2, ~((DWORD)BK9532_REG_GPIO_CFG_GPIO_MASK), reg_cfg & BK9532_REG_GPIO_CFG_GPIO_MASK);
    }
    else
    {
        return bk9532_reg_and_mask_or_flag(bus, BK9532_REG_GPIO_CFG_R1, ~((DWORD)BK9532_REG_GPIO_CFG_GPIO_MASK << (pin << 3)), ((reg_cfg & BK9532_REG_GPIO_CFG_GPIO_MASK) << (pin << 3)));
    }
}

WORD bk9532_set_gpio_func(WORD bus, WORD pin, WORD func)
{
    WORD mask = ( 8 + 3 * pin);
    return bk9532_reg_and_mask_or_flag(bus, BK9532_REG_GPIO_CFG_R2, ~((DWORD)0x07 << mask), (((DWORD)func & 0x07) << mask));
}

WORD bk9532_i2s_config(WORD bus, WORD mode)
{

    if(mode) // master 
    {
	    bk9532_set_gpio_mode(bus, BK9532_GPIO_PIN_0, BK9532_GPIO_MODE_SND_FUNC_OUTPUT, BK9532_GPIO_NOPULL);
	    bk9532_set_gpio_mode(bus, BK9532_GPIO_PIN_1, BK9532_GPIO_MODE_SND_FUNC_OUTPUT, BK9532_GPIO_NOPULL);
        bk9532_set_gpio_func(bus, BK9532_GPIO_PIN_0, 0); // pcm_lrck_o;
	    bk9532_set_gpio_func(bus, BK9532_GPIO_PIN_1, 0); // pcm_sck_o;
        bk9532_reg_or_mask(bus, 0x36, (DWORD)1 << 27);
    }
    else
    {
        bk9532_set_gpio_mode(bus, BK9532_GPIO_PIN_0, BK9532_GPIO_MODE_SND_FUNC_INPUT, BK9532_GPIO_PULLUP);
	    bk9532_set_gpio_mode(bus, BK9532_GPIO_PIN_1, BK9532_GPIO_MODE_SND_FUNC_INPUT, BK9532_GPIO_PULLUP);
        bk9532_set_gpio_func(bus, BK9532_GPIO_PIN_0, 0); // pcm_lrck_i;
	    bk9532_set_gpio_func(bus, BK9532_GPIO_PIN_1, 0); // pcm_sck_i;
        bk9532_reg_and_mask(bus, 0x36, ~((DWORD)1 << 27));
    }
    // i2s mode //
    bk9532_reg_and_mask_or_flag(bus, 0x36, ~((DWORD)0x07 << 28), ~(((DWORD)0x0 & 0x07) << 28));
    // i2s enable //
    bk9532_reg_or_mask(bus, 0x36, (DWORD)1 << 26);
   //Default 24.576 MHz crystal, 48K sample rate, 24bits data length.
    bk9532_reg_write(bus, 0x37, 0x3E009800);
    // i2s outz //
    if(bus)
    {
        bk9532_reg_and_mask_or_flag(bus, 0x3F, ~((DWORD)0x03 << 3), ((DWORD)0x03 << 3));
    }
    else
    {
        bk9532_reg_and_mask_or_flag(bus, 0x3F, ~((DWORD)0x03 << 3), ((DWORD)0x01 << 3));
    }

    return 0;
}   


////////////////////////////////////////////////////////////////

#endif //

#if 0

WORD bk9532_rf_indicate_init(WORD bus)
{
    bk9532_set_gpio_mode(bus, BK9532_GPIO_PIN_4, BK9532_GPIO_MODE_OUTPUT, BK9532_GPIO_PULLDOWN);
    bk9532_set_gpio_func(bus, BK9532_GPIO_PIN_4, 0);
    return 0;
}

WORD bk9532_rf_indicate_onoff(WORD bus, WORD onoff)
{
    return bk9532_set_gpio_output(bus, BK9532_GPIO_PIN_4, onoff);
}

WORD bk9532_rf_indicate_period(WORD bus, WORD time, WORD period)
{
    static WORD indicate = 0;
    if (((DWORD)time * BK9532_STEP_MS) % (period) == 0)
    {
        bk9532_rf_indicate_onoff(bus, indicate);
        indicate = ~indicate;
    }
    return 0;
}


WORD bk9532_rf_idcode_freq_set(WORD bus, DWORD idcode, DWORD freq)
{
    WORD state =  bk9532_set_idcode(bus, idcode); 

    if(state != BK9532_PASS)
    {
        return state;
    }        
    
    return bk9532_set_frequency(bus, freq);
}

WORD bk9532_rf_calib_process(WORD bus, WORD reset)
{
    WORD cal_result = 1;
    static WORD calib_step[BK9532_MAX_CHANNELS] = {0,0};
    if(reset)
    {
        calib_step[bus] = 0;

        return cal_result;
    }

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
    return cal_result;
}

WORD bk9532_rf_detect_signal(WORD bus)
{
    WORD state = 1;
    if (bk9532_get_rf_signal(bus) == 0)
    {
        TRACE("bk9532 rf signal detect", g_rf_afc[bus].last_time_tune);
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
    }
    return state;
}

WORD bk9532_rf_change_idcode_and_frequency_process(WORD bus, DWORD idcode, DWORD frequency, WORD reset)
{
    WORD state = 1;

    static state_machine[BK9532_MAX_CHANNELS] = {0,0};

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
        }
        break;
    case 1:
        if(bk9532_rf_calib_process(bus, FALSE) == 0)
        {
            if ((bk9532_set_xtal_adj_cfg(bus, XTAL_ADJ_TAB[0]) == BK9532_PASS) // set xtal adj configuration //
                && (bk9532_set_afc(bus, FALSE) == BK9532_PASS)                 // turn off afc //
            )
            {
                state_machine[bus] = 2;
                _memset(&g_rf_afc[bus], 0, sizeof(struct bk9532_rf_afc));
            }
            else
            {
                state_machine[bus] = 0;
            }
        }
        break;
    case 2:
        if(bk9532_rf_detect_signal(bus) == 0)
        {
            state = 0;
            state_machine[bus] = 0;
        }
        break;
    case 3:
        break;    
    default:
        break;
    }
    return state;
}

WORD bk9532_rf_query_idcode(WORD bus, DWORD *idcode, WORD *flag)
{

    WORD state = 1;
    DWORD reg_usrdata;
    if (bk9532_get_user_data(bus, &reg_usrdata) == BK9532_PASS)
    {
        DWORD udata = reg_usrdata & 0xFF;
        WORD ud_idx = (udata >> 5);
        WORD ud_dat = (udata & 0x1f);
        TRACE("bk9532 query idcode %x", udata);
        TRACE("bk9532 query idcode at %d", ud_idx);
        TRACE("bk9532 query idcode val %x", ud_dat);

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

WORD bk9532_rf_monitor(WORD bus)
{
    WORD state = 1;
    if (bk9532_get_rf_signal(bus) != 0)
    {
        TRACE(" bk9532_rf_monitor lost signal detect", g_rf_afc[bus].last_time_tune);
        if(g_rf_afc[bus].opened)
        {
            if (bk9532_set_afc(bus, FALSE) == BK9532_PASS)
            {
                g_rf_afc[bus].opened = FALSE;
            }
            g_bk9532_rf_ctx[bus].last_time_freq_change = 0;
        }
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

WORD bk9532_rf_param_init(WORD bus)
{
    _memset(&g_bk9532_rf_ctx[bus], 0, sizeof(struct bk9532_rf_context));
    g_bk9532_rf_ctx[bus].pair_freq = BK9532_PAIR_FREQUENCY_MHZ + bus;
    g_bk9532_rf_ctx[bus].rf_freq_min = BK9532_SCAN_FREQUENCY_MIN_MHZ + (BK9532_SCAN_FREQUENCY_MAX_MHZ - BK9532_SCAN_FREQUENCY_MIN_MHZ + 1)*bus;
    g_bk9532_rf_ctx[bus].rf_freq_max = BK9532_SCAN_FREQUENCY_MAX_MHZ + (BK9532_SCAN_FREQUENCY_MAX_MHZ - BK9532_SCAN_FREQUENCY_MIN_MHZ + 1)*bus;
    g_bk9532_rf_ctx[bus].rf_freq = g_bk9532_rf_ctx[bus].rf_freq_min;
    
    return 0;
}


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
        TRACE("bk9532 pairing timeout = %d", g_bk9532_rf_ctx[bus].last_time_freq_change);
    }

    bk9532_rf_indicate_period(bus, g_bk9532_rf_ctx[bus].last_time_freq_change, 200);

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
        TRACE("bk9532 pairing timeout ", g_bk9532_rf_ctx[bus].last_time_freq_change);
        bk9532_rf_change_idcode_and_frequency_process(bus, BK9532_PAIR_IDCODE, g_bk9532_rf_ctx[bus].pair_freq, TRUE);
        g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_CHANGE_FREQ;
        g_bk9532_rf_ctx[bus].last_time_freq_change = 0;
        break;
    case BK9532_RF_STATE_OK:
        ret = 0;
        tmp =*idcode;
        TRACE("bk9532 pairing ok, with idcode %x", tmp);
        g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_CHANGE_FREQ;
        g_bk9532_rf_ctx[bus].last_time_freq_change = 0;
        bk9532_rf_indicate_onoff(bus, TRUE);
        break;
    default:
        g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_CHANGE_FREQ;
        g_bk9532_rf_ctx[bus].last_time_freq_change = 0;
        break;
    }
    return ret;
}

WORD bk9532_rf_scan_frequency_handle(WORD bus)
{
    WORD ret = 1;
    static WORD cnt_indicate = 0;
    static WORD sw_indicate = 0;
    DWORD aud_rssi;

    if (bus >= BK9532_MAX_CHANNELS)
    {
        TRACE("channel over range", bus);
        return ret;
    }

    g_bk9532_rf_ctx[bus].last_time_freq_change += 1;
    //bk9532_rf_indicate_period(bus, g_bk9532_rf_ctx[bus].last_time_freq_change, 750);

    switch (g_bk9532_rf_ctx[bus].state)
    {
    case BK9532_RF_STATE_CHANGE_FREQ:
        if(bk9532_rf_change_idcode_and_frequency_process(bus, g_bk9532_rf_ctx[bus].rf_idc, g_bk9532_rf_ctx[bus].rf_freq, FALSE) == 0)
        {
            TRACE("bk9532 rf connected at  ", g_bk9532_rf_ctx[bus].rf_freq);
            g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_CONNECTED;
            bk9532_rf_indicate_onoff(bus, TRUE);
        }
        else if (bk9532_timeout(g_bk9532_rf_ctx[bus].last_time_freq_change, BK9532_SCAN_FREQUENCY_TIMEOUT))
        {
            g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_TIMEOUT;
        }
        break;
    case BK9532_RF_STATE_CONNECTED:
        if(bk9532_rf_monitor(bus) == -1)
        {
            TRACE("bk9532 rf lost connected at  ", g_bk9532_rf_ctx[bus].rf_freq);
            bk9532_rf_change_idcode_and_frequency_process(bus, g_bk9532_rf_ctx[bus].rf_idc, g_bk9532_rf_ctx[bus].rf_freq, TRUE);
            g_bk9532_rf_ctx[bus].last_time_freq_change = 0;
            g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_CHANGE_FREQ;
            bk9532_rf_indicate_onoff(bus, FALSE);
        }
        else 
        {
            // get user data //


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
    case BK9532_RF_STATE_TIMEOUT:
        ret = -1;
        TRACE("bk9532 rf timeout ", g_bk9532_rf_ctx[bus].last_time_freq_change);
        
        if(++g_bk9532_rf_ctx[bus].rf_freq > g_bk9532_rf_ctx[bus].rf_freq_max)
        {
            g_bk9532_rf_ctx[bus].rf_freq = g_bk9532_rf_ctx[bus].rf_freq_min;
        }
        bk9532_rf_change_idcode_and_frequency_process(bus, g_bk9532_rf_ctx[bus].rf_idc, g_bk9532_rf_ctx[bus].rf_freq, TRUE);
        g_bk9532_rf_ctx[bus].last_time_freq_change = 0;
        g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_CHANGE_FREQ;

        if(++cnt_indicate >= 5)
        {
            bk9532_rf_indicate_onoff(bus, sw_indicate);
            sw_indicate = ~ sw_indicate;
            cnt_indicate = 0;
        }

        break;
    default:
        g_bk9532_rf_ctx[bus].state = BK9532_RF_STATE_CHANGE_FREQ;
        g_bk9532_rf_ctx[bus].last_time_freq_change = 0;
        break;
    }

    return ret;
}

WORD bk9532_rf_chip_init(WORD bus)
{
    // default rom //
    WORD i;
    DWORD reg_val;
    for (i = 0; i < sizeof(g_bk9532_rom) / sizeof(struct bk9532_reg_val); i++)
    {
        bk9532_reg_write(bus, g_bk9532_rom[i].reg_addr, g_bk9532_rom[i].reg_val);
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
    bk9532_set_audio_volume(bus, 12);
    
    bk9532_i2s_config(bus, 0); // 0: slave; 1: master
    bk9532_set_audio_enable(bus, FALSE);
    return 0;
}

void bk9532_task_handler(WORD bus)
{
    static state_machine[BK9532_MAX_CHANNELS] = {0, 0};

    switch (state_machine[bus])
    {
    case 0: // get idcode //
        if(bk9532_rf_sync_idcode_handle(0, &g_bk9532_rf_ctx[bus].pair_idc) == 0)
        {
            state_machine[bus] = 1;
            g_bk9532_rf_ctx[bus].rf_idc =  g_bk9532_rf_ctx[bus].pair_idc;
            //save idcode to rom //
        }
        // if != 1 ; state_machine[bus] = 1; if == 0 update idcode ;
        // state = bk9532_rf_sync_idcode_handle(0, &g_bk9532_rf_ctx[bus].pair_idc);
        // if(state != 1)
        // {
        //     if(state == 0)
        //     {
        //         //save idcode to rom //
        //         g_bk9532_rf_ctx[bus].rf_idc =  g_bk9532_rf_ctx[bus].pair_idc;
        //     }
        //     else
        //     {
        //         //load idcode from rom //
        //         g_bk9532_rf_ctx[bus].rf_idc =  bk9532_flash_get_idcode(bus);
        //     }
        //         //load idcode from rom //
        //     g_bk9532_rf_ctx[bus].rf_freq =  bk9532_flash_get_freq(bus);
        //     state_machine[bus] = 1;
        // }

        break;
    case 1: // scan frequency //
        bk9532_rf_scan_frequency_handle(bus);
        break;
    default:
        break;
    }
}

void bk9532_task_handler_channel_0(void)
{
    bk9532_task_handler(0);
}

void bk9532_task_handler_channel_1(void)
{
    bk9532_task_handler(1);
}

_SYS_TASK_CALLBACK bk9532_task_handler_channel[BK9532_MAX_CHANNELS] =
{
    bk9532_task_handler_channel_0,
    bk9532_task_handler_channel_1
};

_SYS_TASK bk9532_task[BK9532_MAX_CHANNELS];

void bk9532_register_task(WORD bus)
{
    bk9532_rf_param_init(bus);
    bk9532_rf_chip_init(bus);
    _Sys_SchedRegister(&bk9532_task[bus], bk9532_task_handler_channel[bus], BK9532_STEP_MS);
}

#endif 

void bk9532_test_pairing(WORD bus)
{
    bk9532_register_task(bus);
}   

// void bk9532_scanid(WORD bus)
// {    
//     WORD i,j,state;    
//     i2c_t *i2c_bus = i2c_get_bus(bus);
//     if (i2c_bus == NULL) {
//         TRACE("==== I2C NULL ==%d", bus);	
//         return ;
//     }
//     TRACE("==== I2C ADDR TEST ==%d", bus);	
//     for(i=0;i<127; i++)
//     {
//         i2c_start_bit(i2c_bus);
//         state = i2c_send_byte(i2c_bus, i);
//         //TRACE("==== id: %d\n", i);
//         //TRACE("==== state: %d\n", state);
//         if(state != I2C_ACK)continue;    
//         TRACE("====ok id: %d\n", i);
//         for(j=0; j<65535; j++);
//     }   
// }
// void ADC_test(void)
// {    
//     bk9532_scanid(I2C_PORT_ADC);
// }
void bk9532_test(void)
{
    //ADC_test();    
    //bk9532_scanid(0);
    //bk9532_scanid(1);

    //bk9532_test_chip_id();
    //bk9532_test_write_and_read();
    
    bk9532_test_pairing(I2C_PORT_MIC1);
    bk9532_test_pairing(I2C_PORT_MIC2);
    bk9532_TurnLED(TRUE);
}
void bk9532_callHander(WORD bus)
{
    bk9532_task_handler(bus);
    //bk9532_task_handler(I2C_PORT_MIC1);
    //bk9532_task_handler(I2C_PORT_MIC2);
}
void bk9532_TurnLED(WORD value)
{
    bk9532_rf_indicate_onoff(I2C_PORT_MIC1,value);
    bk9532_rf_indicate_onoff(I2C_PORT_MIC2,value);
}