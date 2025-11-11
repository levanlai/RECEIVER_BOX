#ifndef __GLOBALPRESETS_H__
#define __GLOBALPRESETS_H__

#include <system.h>


/* Global preset header file */

void dspDesigner_LoadGlobalPreset( PTR32 global_preset_table );

// Gain preset structure.
typedef struct 
{
	WORD parameter1; // Gain
	WORD parameter2; // 180deg
} globalGainPreset;


// Gain preset control structure.
typedef struct 
{
	WORD numberOfControls; // number of controls
	WORD sizeOfStruct; // size of this Structure
	WORD control[2]; // controls array
} globalGainControl;

#define	GLOBAL_GAIN_PRESET_NB	1
extern const globalGainPreset global_gain_presets[GLOBAL_GAIN_PRESET_NB];
#define	NUMBER_OF_PRESET_TABLE	1
extern const WORD_FAR_P global_preset_table_address[NUMBER_OF_PRESET_TABLE];
extern const globalGainControl global_gain_controls;
extern const WORD_FAR_P global_preset_control_table_address[NUMBER_OF_PRESET_TABLE];
#define	GLOBAL_NUMBER_OF_DSP	1
extern const DWORD globalpreset[1][3];

#endif //__GLOBALPRESETS_H__