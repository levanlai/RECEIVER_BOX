#ifndef __BK9532_H__
#define __BK9532_H__

#include <system.h>
#include "../sys/sys_i2c.h"
#include "../sys/sys_i2c_bk9532.h"

#define BK9532_MAX_CHANNELS 2
#define BK9532_IDCODE_RX_LENGTH 7
#define BK9532_PAIRING_TIMEOUT 3000 // ms
#define BK9532_AFC_TIMEOUT 50       // ms
#define BK9532_PAIR_FREQUENCY_MHZ 670
#define BK9532_PAIR_IDCODE 0xFFFFFFFF

#define BK9532_SCAN_FREQUENCY_MIN_MHZ 676
#define BK9532_SCAN_FREQUENCY_MAX_MHZ 684
#define BK9532_SCAN_FREQUENCY_TIMEOUT 150//300 // ms


#define BK9532_FLASH_ID_IDCODE_CHA      0xF001
#define BK9532_FLASH_ID_IDCODE_CHB      0xF002

#define BK9532_FLASH_ID_FREQUENCY_CHA   0xF003
#define BK9532_FLASH_ID_FREQUENCY_CHB   0xF004

#define BK9532_FLASH_FREQUENCY_DEFUALT_CHA 676
#define BK9532_FLASH_FREQUENCY_DEFUALT_CHB 685

#define BK9532_FLASH_IDCODE_DEFUALT_CHA 0x00000000
#define BK9532_FLASH_IDCODE_DEFUALT_CHB 0x00000000

#define BK9532_PASS I2C_OK

#define BK9532_STEP_MS 10 // 10 ms  //  min 10ms // 

#define bk9532_timeout(a, b) ((((DWORD)a) * BK9532_STEP_MS) > (DWORD)b)

typedef enum BK9532_RF_STATE
{
    BK9532_RF_STATE_CHANGE_FREQ = 0,
    BK9532_RF_STATE_SYNC_IDCODE,
    BK9532_RF_STATE_CONNECTED,
    BK9532_RF_STATE_OK,
    BK9532_RF_STATE_TIMEOUT,
} bk9532_rf_state_e;

typedef enum BK9532_FREQUENCY_RANGE
{
    BK9532_FREQUENCY_BELOW_178MHZ = 0,
    BK9532_FREQUENCY_BELOW_270MHZ,
    BK9532_FREQUENCY_BELOW_710MHZ,
    BK9532_FREQUENCY_BELOW_980MHZ,
} bk9532_freqency_range_e;

typedef struct bk9532_rf_afc
{
    WORD count;
    WORD cur_index;
    WORD max_index;
    WORD opened;
    DWORD last_time_tune;
} bk9532_rf_afc_t;

typedef struct bk9532_rf_context
{
    WORD state;
    WORD last_time_freq_change;
    // pairing //
    WORD pair_idc_flag;
    WORD pair_freq;
    DWORD pair_idc;
    WORD pair_ok;
    WORD pair_ok_cnt;
    // rf //
    WORD rf_freq;
    WORD rf_freq_max;
    WORD rf_freq_min;
    DWORD rf_idc;
    WORD is_connected;
   
    WORD iModeSleep;
} bk9532_rf_context_t;

typedef struct bk9532_reg_val
{
    WORD reg_addr;
    DWORD reg_val;
} bk9532_reg_val_t;

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

/////////////////////// BK9532 REG UTILS //////////////////////////
WORD bk9532_get_rf_signal(WORD bus);
WORD bk9532_set_idcode(WORD bus, DWORD idcode);
WORD bk9532_get_idcode(WORD bus, DWORD *idcode);
WORD bk9532_set_xtal_adj_cfg(WORD bus, WORD xtal_adj);
WORD bk9532_set_afc(WORD bus, WORD enable);
WORD bk9532_set_frequency(WORD bus, DWORD frequency);
WORD bk9532_get_frequency(WORD bus, DWORD *freq);
WORD bk9532_set_rf_loop_ldo_vol(WORD bus, WORD enable);
WORD bk9532_set_rf_vco_cb_toggle(WORD bus);
WORD bk9532_set_rf_vco_cb_dig_toggle(WORD bus);
WORD bk9532_set_rf_vco_cb_clk(WORD bus, WORD enable);
WORD bk9532_set_rf_baseband_reset(WORD bus);
WORD bk9532_set_rf_chip_reset(WORD bus);
WORD bk9532_set_rf_plc_reset(WORD bus);
WORD bk9532_set_rf_uhf_band(WORD bus);
WORD bk9532_set_rf_vhf_band(WORD bus);
WORD bk9532_set_rf_frequency_range(WORD bus, WORD freq_range);
WORD bk9532_set_rf_ant_mode(WORD bus, WORD mode);
WORD bk9532_set_rf_ant_pin(WORD bus, WORD pin);
WORD bk9532_set_adpcm_err_mode(WORD bus, WORD mode);
WORD bk9532_set_rf_plc(WORD bus, WORD enable);
WORD bk9532_get_audio_rssi(WORD bus, DWORD *au_rssi);
WORD bk9532_set_audio_volume(WORD bus, WORD vol);
WORD bk9532_set_audio_enable(WORD bus, WORD enable);
WORD bk9532_get_user_data(WORD bus, DWORD *user_data);

WORD bk9532_set_gpio_mode(WORD bus, WORD pin, WORD mode, WORD pupd);
WORD bk9532_set_gpio_func(WORD bus, WORD pin, WORD func);
WORD bk9532_set_gpio_output(WORD bus, WORD pin , WORD level);

WORD bk9532_i2s_config(WORD bus, WORD mode);

//////////////////////////BK9532 TASKS //////////////////////////

void bk9532_register_task(WORD bus);
void bk9532_task_handler(WORD bus);
WORD bk9532_mic_is_connected(WORD bus);
WORD bk9532_rf_indicate_onoff(WORD bus, WORD onoff);
void  bk9532_mic_reset_pair(void);
void BK9532_Enter_SoftSleep(WORD bus) ;
void BK9532_Exit_SoftSleep(WORD bus);
WORD  bk9532_mic_iModeSleep(WORD bus);
#endif //__BK9532_H__