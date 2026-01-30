#include <system.h>
#include <sys5000.h>
#include <math.h>
#include <trace.h>

#include "bk9532.h"

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


//IEEE-754 Floating Point Standard
//https://gregstoll.com/~gregstoll/floattohex/
//https://resource.heltec.cn/utils/hf

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
    // TRACE("bk9532_set_frequency freq %d", frequency);
    // TRACE("bk9532_set_frequency freq cal", freq_cal);
    // TRACE("bk9532_set_frequency freq convert %x", frequency_bk9532);
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
WORD bk9532_set_rf_baseband_enable(WORD bus, WORD enable)
{
    return bk9532_reg_and_mask_or_flag(bus, BK9532_REG_RF_BASEBAND, ~((DWORD)1 << BK9532_REG_RF_BASEBAND_SHIFT), (((DWORD)enable&0x01) << BK9532_REG_RF_BASEBAND_SHIFT));
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
	//vol_eq =  ((DWORD)24  & 0x1F) << 5;//18:-6dB;24:0dB
    vol_eq =  ((DWORD)18  & 0x1F) << 5;//18:-6dB;24:0dB
   //vol_eq =  ((DWORD)12  & 0x1F) << 5;//18:-6dB;24:0dB
#endif
    vol_all = vol_duf | vol_eq;

    return bk9532_reg_and_mask_or_flag(bus, BK9532_REG_VOL_GAIN, ~((DWORD)0x3FF) , vol_all);
}

#define BK9532_REG_VOL_MUTE 0x36
WORD bk9532_set_audio_enable(WORD bus, WORD enable)
{
    //TRACE("set_audio_enable bus=",bus);
    // TRACE("enable=",enable);
   // return bk9532_reg_and_mask_or_flag(bus, BK9532_REG_VOL_MUTE, ~((DWORD)1 << 13), (((DWORD)enable&0x01) << 13));
    return bk9532_reg_and_mask_or_flag(bus, BK9532_REG_VOL_MUTE, ~((DWORD)1 << 13), (DWORD)enable);
}

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

    if(mode == 1) // master 
    {
	    bk9532_set_gpio_mode(bus, BK9532_GPIO_PIN_0, BK9532_GPIO_MODE_SND_FUNC_OUTPUT, BK9532_GPIO_NOPULL);
	    bk9532_set_gpio_mode(bus, BK9532_GPIO_PIN_1, BK9532_GPIO_MODE_SND_FUNC_OUTPUT, BK9532_GPIO_NOPULL);
        bk9532_set_gpio_mode(bus, BK9532_GPIO_PIN_2, BK9532_GPIO_MODE_SND_FUNC_OUTPUT, BK9532_GPIO_NOPULL);
        bk9532_set_gpio_func(bus, BK9532_GPIO_PIN_0, 0); // pcm_lrck_o;
	    bk9532_set_gpio_func(bus, BK9532_GPIO_PIN_1, 0); // pcm_sck_o;
        bk9532_set_gpio_func(bus, BK9532_GPIO_PIN_2, 0); // pcm_sda_o;
        bk9532_reg_or_mask(bus, 0x36, (DWORD)1 << 27);
    }
    else
    {
        bk9532_set_gpio_mode(bus, BK9532_GPIO_PIN_0, BK9532_GPIO_MODE_SND_FUNC_INPUT, BK9532_GPIO_NOPULL);
	    bk9532_set_gpio_mode(bus, BK9532_GPIO_PIN_1, BK9532_GPIO_MODE_SND_FUNC_INPUT, BK9532_GPIO_NOPULL);
        bk9532_set_gpio_mode(bus, BK9532_GPIO_PIN_2, BK9532_GPIO_MODE_SND_FUNC_OUTPUT, BK9532_GPIO_PULLDOWN);
        bk9532_set_gpio_func(bus, BK9532_GPIO_PIN_0, 4); // pcm_lrck_i;
	    bk9532_set_gpio_func(bus, BK9532_GPIO_PIN_1, 4); // pcm_sck_i;
        bk9532_set_gpio_func(bus, BK9532_GPIO_PIN_2, 0); // pcm_sda_o;
        bk9532_reg_and_mask(bus, 0x36, ~((DWORD)1 << 27));
    }
    // i2s mode //
    bk9532_reg_and_mask_or_flag(bus, 0x36, ~((DWORD)0x07 << 28), (((DWORD)0 & 0x07) << 28));
    // i2s enable //
    bk9532_reg_or_mask(bus, 0x36, (DWORD)1 << 26);
   //Default 24.576 MHz crystal, 48K sample rate, 24bits data length.
    //bk9532_reg_write(bus, 0x37, 0x3E005800); 96k
    bk9532_reg_write(bus, 0x37, 0x3E009800);
    // i2s outz //
    if(bus)
    {
        bk9532_reg_and_mask_or_flag(bus, 0x3F, ~((DWORD)0x03 << 3), ((DWORD)0x03 << 3));
    }
    else
    {
        bk9532_reg_and_mask_or_flag(bus, 0x3F, ~((DWORD)0x03 << 3), ((DWORD)0x02 << 3));
    }

    return 0;
}   
