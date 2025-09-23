#include "GlobalPresets.h"
#include "midictrl.h"

// Gain preset list. Table ID: 0x0
const globalGainPreset global_gain_presets[GLOBAL_GAIN_PRESET_NB] = 
{
	// GlobalPresetA_DSP01-PCS02
	{ 14847, 0 }
};
// Biquad[5] preset list. Table ID: 0x1
const globalBiquad5Preset global_biquad5_presets[GLOBAL_BIQUAD5_PRESET_NB] = 
{
	// GlobalPresetA_DSP01-PCS06
	{ 1, 0, 14847, 7, 955, 1228800, 64, 1, 955, 6144000, 64, 6, 955, 20480000, 64, 106, 955, 122880, 64, 1, 955, 81968, 64 },
	// GlobalPresetA_DSP03-PCS03
	{ 1, 0, 14847, 6, 955, 34816000, 70, 1, 955, 6144000, 58, 1, 955, 20480000, 58, 1, 955, 1024000, 61, 1, 955, 184320, 70 }
};
// Biquad[3] preset list. Table ID: 0xa
const globalBiquad3Preset global_biquad3_presets[GLOBAL_BIQUAD3_PRESET_NB] = 
{
	// GlobalPresetA_DSP03-PCS02
	{ 0, 0, 14847, 1, 955, 614400, 64, 5, 955, 819200, 64, 1, 955, 81944, 64 },
	// GlobalPresetA_DSP04-PCS03
	{ 1, 0, 14847, 7, 955, 1228800, 64, 1, 955, 6144000, 64, 6, 955, 20480000, 64 }
};
// MixN[3] preset list. Table ID: 0x2
const globalMixN3Preset global_mixn3_presets[GLOBAL_MIXN3_PRESET_NB] = 
{
	// GlobalPresetA_DSP01-PCS03
	{ 0, 0, 0, 14847, 14847, 14847 }
};
// MixN[2] preset list. Table ID: 0x6
const globalMixN2Preset global_mixn2_presets[GLOBAL_MIXN2_PRESET_NB] = 
{
	// GlobalPresetA_DSP01-PCS0E
	{ 0, 0, 14847, 14847 }
};
// FreqShifter preset list. Table ID: 0x3
const globalFreqShifterPreset global_freqshifter_presets[GLOBAL_FREQSHIFTER_PRESET_NB] = 
{
	// GlobalPresetA_DSP01-PCS0A
	{ 1, 0, 0, 2983, 16383, 0 }
};
// NoiseGate preset list. Table ID: 0x4
const globalNoiseGatePreset global_noisegate_presets[GLOBAL_NOISEGATE_PRESET_NB] = 
{
	// GlobalPresetA_DSP01-PCS0D
	{ 0, 10623, 8192, 8192, 14847, 0 }
};
// Exciter preset list. Table ID: 0x5
const globalExciterPreset global_exciter_presets[GLOBAL_EXCITER_PRESET_NB] = 
{
	// GlobalPresetA_DSP01-PCS01
	{ 0, 14847, 8192, 8192, 3768, 4915 }
};
// Feedback canceller preset list. Table ID: 0x7
const globalFeedback_cancellerPreset global_feedback_canceller_presets[GLOBAL_FEEDBACK_CANCELLER_PRESET_NB] = 
{
	// GlobalPresetA_DSP02-PCS01
	{ 0, 0, 0 }
};
// Compressor preset list. Table ID: 0x8
const globalCompressorPreset global_compressor_presets[GLOBAL_COMPRESSOR_PRESET_NB] = 
{
	// GlobalPresetA_DSP03-PCS09
	{ 0, 16383, 8192, 0, 0 },
	// GlobalPresetA_DSP03-PCS0A
	{ 0, 16383, 8147, 0, 0 }
};
// LevelDetect preset list. Table ID: 0x9
const globalLevelDetectPreset global_leveldetect_presets[GLOBAL_LEVELDETECT_PRESET_NB] = 
{
	// GlobalPresetA_DSP03-PCS0C
	{ 8192, 8192 }
};
// (s)Reverb/Echo preset list. Table ID: 0xb
const globalS_Reverb_EchoPreset global_s_reverb_echo_presets[GLOBAL_S_REVERB_ECHO_PRESET_NB] = 
{
	// GlobalPresetA_DSP04-PCS01
	{ 12, 8192, 4915, 410, 8192, 9830, 16383, 16383, 8192, 12508, 0, 8192, 8192, 10764, 8192, 127, 8192, 127 }
};
const WORD_FAR_P global_preset_table_address[NUMBER_OF_PRESET_TABLE] = 
{
	FARPTR((const WORD*)&global_gain_presets), 
	FARPTR((const WORD*)&global_biquad5_presets), FARPTR((const WORD*)&global_mixn3_presets), FARPTR((const WORD*)&global_freqshifter_presets), FARPTR((const WORD*)&global_noisegate_presets), 
	FARPTR((const WORD*)&global_exciter_presets), FARPTR((const WORD*)&global_mixn2_presets), FARPTR((const WORD*)&global_feedback_canceller_presets), FARPTR((const WORD*)&global_compressor_presets), 
	FARPTR((const WORD*)&global_leveldetect_presets), FARPTR((const WORD*)&global_biquad3_presets), FARPTR((const WORD*)&global_s_reverb_echo_presets)
};
// Gain preset control list.
const globalGainControl global_gain_controls = 
{
	2, 2, 0x4000, 0x1
};
// Biquad[5] preset control list.
const globalBiquad5Control global_biquad5_controls = 
{
	0x8017, 28, 0x0, 0x1, 0x4002, 0x3, 0x4022, 0x8041, 0x60, 0x4, 0x4023, 0x8042, 0x61, 0x5, 0x4024, 0x8043, 0x62, 0x6, 0x4025, 0x8044, 0x63, 0x7, 0x4026, 0x8045, 0x64
};
// Biquad[3] preset control list.
const globalBiquad3Control global_biquad3_controls = 
{
	0x800f, 18, 0x0, 0x1, 0x4002, 0x3, 0x4022, 0x8041, 0x60, 0x4, 0x4023, 0x8042, 0x61, 0x5, 0x4024, 0x8043, 0x62
};
// MixN[3] preset control list.
const globalMixN3Control global_mixn3_controls = 
{
	6, 6, 0x0, 0x1, 0x2, 0x401f, 0x4020, 0x4021
};
// MixN[2] preset control list.
const globalMixN2Control global_mixn2_controls = 
{
	4, 4, 0x0, 0x1, 0x401f, 0x4020
};
// FreqShifter preset control list.
const globalFreqShifterControl global_freqshifter_controls = 
{
	6, 6, 0x0, 0x1, 0x4002, 0x4003, 0x4004, 0x5
};
// NoiseGate preset control list.
const globalNoiseGateControl global_noisegate_controls = 
{
	6, 6, 0x0, 0x4001, 0x4002, 0x4003, 0x4004, 0x5
};
// Exciter preset control list.
const globalExciterControl global_exciter_controls = 
{
	6, 6, 0x0, 0x4001, 0x4002, 0x4003, 0x4004, 0x4005
};
// Feedback canceller preset control list.
const globalFeedback_cancellerControl global_feedback_canceller_controls = 
{
	3, 3, 0x0, 0x1, 0x3
};
// Compressor preset control list.
const globalCompressorControl global_compressor_controls = 
{
	5, 5, 0x1, 0x4002, 0x4003, 0x4004, 0x5
};
// LevelDetect preset control list.
const globalLevelDetectControl global_leveldetect_controls = 
{
	2, 2, 0x4000, 0x4001
};
// (s)Reverb/Echo preset control list.
const globalS_Reverb_EchoControl global_s_reverb_echo_controls = 
{
	18, 18, 0x0, 0x4001, 0x4002, 0x4003, 0x4004, 0x4005, 0x4006, 0x4007, 0x4008, 0x4009, 0xa, 0x400b, 0x400c, 0x400d, 0x400e, 0xf, 0x4010, 0x11
};
const WORD_FAR_P global_preset_control_table_address[NUMBER_OF_PRESET_TABLE] = 
{
	FARPTR((const WORD*)&global_gain_controls), 
	FARPTR((const WORD*)&global_biquad5_controls), FARPTR((const WORD*)&global_mixn3_controls), FARPTR((const WORD*)&global_freqshifter_controls), FARPTR((const WORD*)&global_noisegate_controls), 
	FARPTR((const WORD*)&global_exciter_controls), FARPTR((const WORD*)&global_mixn2_controls), FARPTR((const WORD*)&global_feedback_canceller_controls), FARPTR((const WORD*)&global_compressor_controls), 
	FARPTR((const WORD*)&global_leveldetect_controls), FARPTR((const WORD*)&global_biquad3_controls), FARPTR((const WORD*)&global_s_reverb_echo_controls)
};
const DWORD globalpreset[1][54] = 
{
	// GlobalPresetA
	{ 
	// DSP#1
	0x10011, 0x2, 0x5, 0x100006, 0x8, 0xb, 0x10000c, 0x200003, 0x4, 0x9, 0x30000a, 0x40000d, 0x500001, 0x500007, 0x10, 0x60000e, 0x60000f, 0x11,
	// DSP#2
	0x20002, 0x700001, 0x2,
	// DSP#3
	0x30019, 0x6, 0x7, 0x8, 0x800009, 0x80010a, 0x80000b, 0x90000c, 0x90000d, 0x90000e, 0x200016, 0x200017, 0x100103, 0x10010f, 0x11, 0x19, 0x600001, 0xa00002, 0x4, 0x600005, 0xa00010, 0x12, 0x18, 0x1a, 0x15, 0x1b,
	// DSP#4
	0x40006, 0xb00001, 0x2, 0x200005, 0x600006, 0x600007, 0xa00103 }
};
#define GLOBAL_PROCESS_ID_NBIT 8
#define GLOBAL_PRESET_ID_NBIT 12
#define GLOBAL_TABLE_ID_NBIT 12

/*-----------------------------------------------------------------------------
Global preset loader function
 Input: *global_preset_table = pointer to global preset table

-----------------------------------------------------------------------------*/
void dspDesigner_LoadGlobalPreset( PTR32 global_preset_table )
{
	WORD numberOfProcess;
	WORD processIndex, dspIndex;
	DWORD data;
	WORD processId, dspId, tableId, presetNumber;
	
	PTR32 control_table_address =  _cptr32( &global_preset_control_table_address );
	PTR32 preset_table_address = _cptr32( &global_preset_table_address );
	
	for ( dspIndex = 0; dspIndex < GLOBAL_NUMBER_OF_DSP; dspIndex++ )
	{
		data = readExtMemDword( global_preset_table );
		global_preset_table+=sizeof(DWORD);
		numberOfProcess = data & 0xFFFF;
		dspId = (data >> 16) & 0xFFFF;
		for ( processIndex = 0; processIndex < numberOfProcess; processIndex++ )
		{
			data = readExtMemDword( global_preset_table );
			global_preset_table+=sizeof(DWORD);
			processId = (data & ((1<<GLOBAL_PROCESS_ID_NBIT)-1));
			data >>= GLOBAL_PROCESS_ID_NBIT;
			presetNumber = (data & ((1<<GLOBAL_PRESET_ID_NBIT)-1));
			data >>= GLOBAL_PRESET_ID_NBIT;
			tableId = (data & ((1<<GLOBAL_TABLE_ID_NBIT)-1));

			dspDesigner_LoadPreset( dspId-1, processId, 
				_rdxtmem32Ex( control_table_address + (tableId<<1) ),
				_rdxtmem32Ex( preset_table_address + (tableId<<1) ),
				presetNumber );

		}
	}
}

