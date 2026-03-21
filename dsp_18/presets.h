#ifndef __PRESETS_H__
#define __PRESETS_H__

#include <system.h>


/* Preset header file */

/*-----------------------------------------------------------------------------
 Preset loader function
 Input:	channel = DSP# -1
		process = Process ID
		*presetControls = pointer to preset controls table
		*presetTable = pointer to preset table
		presetNumber = preset# to load (0..PRESET_NB-1)
------------------------------------------------------------------------------*/
void dspDesigner_LoadPreset( WORD channel, WORD process, PTR32 presetControls, PTR32 presetTable, WORD presetNumber );


#endif //__PRESETS_H__