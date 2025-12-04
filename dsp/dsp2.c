#include <system.h>
#include <libFX5000.h>
#include "dspDesigner.h"
#include "custom.h"
#ifndef	_SKIP_DDD_NRPN_CTRL
#include "midictrl.h"
#endif	// _SKIP_DDD_NRPN_CTRL
#include "memorymap.h"
#include <trace.h>

WORD dsp2pcs[9];

WORD dsp2InitAndRoute(void)
{
	WORD dspId;

	dspId = _LiveMic_Init( MEMADDR_DSPINIT02_PCS00PAR00 );

	if ( dspId == -1 ) return 0;

#ifdef _customPreInitFunction2
customPreInitFunction2( dspId );// Do all your custom pre initialization code into this function
#endif

	// Process #1: Gain
	dsp2pcs[1] = _LiveMic_Gain_Allocate( dspId );
	_LiveMic_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp2pcs[1], PCS_DSP_IN | 0 );
	_LiveMic_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp2pcs[1], PCS_NODE | 7 );

	// Process #2: NoiseGate
	dsp2pcs[2] = _LiveMic_NoiseGate_Allocate( dspId );
	_LiveMic_SetProcIN( dspId, NOISEGATE_SAMPLE_IN|dsp2pcs[2], PCS_NODE | 7 );
	_LiveMic_SetProcOUT( dspId, NOISEGATE_SAMPLE_OUT|dsp2pcs[2], PCS_NODE | 8 );

	// Process #3: FreqShifter
	dsp2pcs[3] = _LiveMic_FreqShifter_Allocate( dspId );
	_LiveMic_SetProcIN( dspId, FREQSHIFTER_SAMPLE_IN|dsp2pcs[3], PCS_NODE | 8 );
	_LiveMic_SetProcOUT( dspId, FREQSHIFTER_SAMPLE_OUT|dsp2pcs[3], PCS_NODE | 9 );

	// Process #4: Gain
	dsp2pcs[4] = _LiveMic_Gain_Allocate( dspId );
	_LiveMic_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp2pcs[4], PCS_NODE | 9 );
	_LiveMic_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp2pcs[4], PCS_NODE | 6 );

	// Process #5: (s)Reverb/Echo
	_LiveMic_SetProcIN( dspId, LIVEMIC_REVERB_SAMPLE_IN, PCS_NODE | 6 );
	_LiveMic_SetProcIN( dspId, LIVEMIC_ECHO_SAMPLE_IN, PCS_NODE | 6 );
	_LiveMic_SetProcOUT( dspId, LIVEMIC_REVERB_SAMPLE_OUTL, PCS_NODE | 2 );
	_LiveMic_SetProcOUT( dspId, LIVEMIC_REVERB_SAMPLE_OUTR, PCS_NODE | 3 );
	_LiveMic_SetProcOUT( dspId, LIVEMIC_ECHO_SAMPLE_OUTL, PCS_NODE | 0 );
	_LiveMic_SetProcOUT( dspId, LIVEMIC_ECHO_SAMPLE_OUTR, PCS_NODE | 1 );

	// Process #6: MixN
	dsp2pcs[6] = _LiveMic_MixN_Allocate( dspId, 2 );
	_LiveMic_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp2pcs[6], PCS_NODE | 0 );
	_LiveMic_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp2pcs[6], PCS_NODE | 5 );

	// Process #7: MixN
	dsp2pcs[7] = _LiveMic_MixN_Allocate( dspId, 2 );
	_LiveMic_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp2pcs[7], PCS_NODE | 2 );
	_LiveMic_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp2pcs[7], PCS_NODE | 4 );

	// Process #8: MixN
	dsp2pcs[8] = _LiveMic_MixN_Allocate( dspId, 3 );
	_LiveMic_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp2pcs[8], PCS_NODE | 4 );
	_LiveMic_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp2pcs[8], PCS_DSP_OUT | 0 );

#ifdef _customPostInitFunction2
customPostInitFunction2( dspId );// Do all your custom post initialization code into this function
#endif

	return( dspId );
}


#ifndef	_SKIP_DDD_NRPN_CTRL

const WORD nrpn2List[NUMBEROFCOMMAND2][2]=
{
	{ 0x0100, 0x0030 }, // _LiveMic_Gain_Value
	{ 0x0101, 0x0031 }, // _LiveMic_Gain_Phase
	{ 0x0200, 0x0038 }, // _LiveMic_NoiseGate_OnOff
	{ 0x0201, 0x0039 }, // _LiveMic_NoiseGate_Threshold
	{ 0x0202, 0x003A }, // _LiveMic_NoiseGate_Attack
	{ 0x0203, 0x003B }, // _LiveMic_NoiseGate_Release
	{ 0x0204, 0x003C }, // _LiveMic_NoiseGate_InGainValue
	{ 0x0205, 0x003D }, // _LiveMic_NoiseGate_InGainPhase
	{ 0x0300, 0x002A }, // _LiveMic_FreqShifter_OnOff
	{ 0x0301, 0x002B }, // _LiveMic_FreqShifter_PostHP
	{ 0x0302, 0x002C }, // _LiveMic_FreqShifter_Amount
	{ 0x0303, 0x002D }, // _LiveMic_FreqShifter_SetMaxScale
	{ 0x0304, 0x002E }, // _LiveMic_FreqShifter_InGainValue
	{ 0x0305, 0x002F }, // _LiveMic_FreqShifter_InGainPhase
	{ 0x0400, 0x0030 }, // _LiveMic_Gain_Value
	{ 0x0401, 0x0031 }, // _LiveMic_Gain_Phase
	{ 0x0500, 0x0000 }, // _LiveMic_Effect_LoadProgram
	{ 0x0501, 0x0001 }, // _LiveMic_Effect_RevInputLevel
	{ 0x0502, 0x0002 }, // _LiveMic_Effect_RevLevel
	{ 0x0503, 0x0003 }, // _LiveMic_Effect_RevPreHP
	{ 0x0504, 0x0004 }, // _LiveMic_Effect_RevHDamp
	{ 0x0505, 0x0005 }, // _LiveMic_Effect_RevTime
	{ 0x0506, 0x0006 }, // _LiveMic_Effect_RevToneGain
	{ 0x0507, 0x0007 }, // _LiveMic_Effect_RevToneFreq
	{ 0x0508, 0x0008 }, // _LiveMic_Effect_EchoInputLevel
	{ 0x0509, 0x0009 }, // _LiveMic_Effect_EchoTime
	{ 0x050A, 0x000A }, // _LiveMic_Effect_LongEchoMode
	{ 0x050B, 0x000B }, // _LiveMic_Effect_EchoLDamp
	{ 0x050C, 0x000C }, // _LiveMic_Effect_EchoHDamp
	{ 0x050D, 0x000D }, // _LiveMic_Effect_EchoFeedback
	{ 0x050E, 0x000E }, // _LiveMic_Effect_EchoOutputLevel
	{ 0x050F, 0x000F }, // _LiveMic_Effect_EchoOutputPhase
	{ 0x0510, 0x0010 }, // _LiveMic_Effect_EchoOutputLevel
	{ 0x0511, 0x0011 }, // _LiveMic_Effect_EchoOutputPhase
	{ 0x0600, 0x4036 }, // _LiveMic_MixN_GainPhase
	{ 0x061F, 0x4037 }, // _LiveMic_MixN_GainValue
	{ 0x0700, 0x4036 }, // _LiveMic_MixN_GainPhase
	{ 0x071F, 0x4037 }, // _LiveMic_MixN_GainValue
	{ 0x0800, 0x4036 }, // _LiveMic_MixN_GainPhase
	{ 0x081F, 0x4037 } // _LiveMic_MixN_GainValue

};

WORD dsp2NrpnHandler( WORD nrpn, WORD dspId, WORD processId, DWORD value, WORD format )
{
	
	WORD i, functionId, index = 0, val8bit;
	DWORD dvalue;
	#ifdef _customPreNrpnFunction2
	if ( customPreNrpnFunction2( dspId, nrpn, &value, format ) )// Do all your custom pre NRPN code into this function
		return 1;
	#endif
	
	val8bit = value>>8;
	if ( format == FORMAT_14BIT_PRECISION )
		dvalue=(DWORD)value<<(DWORD)13;
	else
		dvalue=value;
	
	i = dichotomicSearch( _cptr32(nrpn2List), 2, 0, NUMBEROFCOMMAND2, nrpn );
	
	if ((i&(1<<15)))
	{
		i &= 0x7FFF;
		if ( i == NUMBEROFCOMMAND2 ) // higher bound test
			i--;
		else
		{
			if ( nrpn < nrpn2List[i][0] )
				i--;		
		}
		if ( !(nrpn2List[i][1] & (1<<14)) )
			i=-1;	
	}
	
	if ( i != -1 )
	{
		index = nrpn - nrpn2List[i][0];
		functionId = nrpn2List[i][1];
		processId = dsp2pcs[processId];
	}
	else
		return 0;	
	if (i != -1)
	{
		switch (functionId)
		{
			//Gain
			case 0x0030: _LiveMic_Gain_Value( dspId, processId, value ); return 1;
			case 0x0031: _LiveMic_Gain_Phase( dspId, processId, val8bit ); return 1;
			//NoiseGate
			case 0x0038: _LiveMic_NoiseGate_OnOff( dspId, processId, val8bit ); return 1;
			case 0x0039: _LiveMic_NoiseGate_Threshold( dspId, processId, value ); return 1;
			case 0x003A: _LiveMic_NoiseGate_Attack( dspId, processId, value ); return 1;
			case 0x003B: _LiveMic_NoiseGate_Release( dspId, processId, value ); return 1;
			case 0x003C: _LiveMic_NoiseGate_InGainValue( dspId, processId, value ); return 1;
			case 0x003D: _LiveMic_NoiseGate_InGainPhase( dspId, processId, val8bit ); return 1;
			//FreqShifter
			case 0x002A: _LiveMic_FreqShifter_OnOff( dspId, processId, val8bit ); return 1;
			case 0x002B: _LiveMic_FreqShifter_PostHP( dspId, processId, val8bit ); return 1;
			case 0x002C: _LiveMic_FreqShifter_Amount( dspId, processId, value ); return 1;
			case 0x002D: _LiveMic_FreqShifter_SetMaxScale( dspId, processId, value ); return 1;
			case 0x002E: _LiveMic_FreqShifter_InGainValue( dspId, processId, value ); return 1;
			case 0x002F: _LiveMic_FreqShifter_InGainPhase( dspId, processId, val8bit ); return 1;
			//(s)Reverb/Echo
			case 0x0000: _LiveMic_Effect_LoadProgram( dspId, val8bit ); return 1;
			case 0x0001: _LiveMic_Effect_RevInputLevel( dspId, value ); return 1;
			case 0x0002: _LiveMic_Effect_RevLevel( dspId, value ); return 1;
			case 0x0003: _LiveMic_Effect_RevPreHP( dspId, value ); return 1;
			case 0x0004: _LiveMic_Effect_RevHDamp( dspId, value ); return 1;
			case 0x0005: _LiveMic_Effect_RevTime( dspId, value ); return 1;
			case 0x0006: _LiveMic_Effect_RevToneGain( dspId, value ); return 1;
			case 0x0007: _LiveMic_Effect_RevToneFreq( dspId, value ); return 1;
			case 0x0008: _LiveMic_Effect_EchoInputLevel( dspId, value ); return 1;
			case 0x0009: 
			TRACE("EchoTime %x",value);
			_LiveMic_Effect_EchoTime( dspId, value ); return 1;
			case 0x000A: _LiveMic_Effect_LongEchoMode( dspId, val8bit ); return 1;
			case 0x000B: _LiveMic_Effect_EchoLDamp( dspId, value ); return 1;
			case 0x000C: _LiveMic_Effect_EchoHDamp( dspId, value ); return 1;
			case 0x000D: _LiveMic_Effect_EchoFeedback( dspId, value ); return 1;
			case 0x000E: _LiveMic_Effect_EchoOutputLevel( dspId, 0, value ); return 1;
			case 0x000F: _LiveMic_Effect_EchoOutputPhase( dspId, 0, val8bit ); return 1;
			case 0x0010: _LiveMic_Effect_EchoOutputLevel( dspId, 1, value ); return 1;
			case 0x0011: _LiveMic_Effect_EchoOutputPhase( dspId, 1, val8bit ); return 1;
			//MixN
			case 0x4036: _LiveMic_MixN_GainPhase( dspId, processId, index, val8bit ); return 1;
			case 0x4037: _LiveMic_MixN_GainValue( dspId, processId, index, value ); return 1;
		
		}
	
	}
	
	#ifdef _customPostNrpnFunction2
	if ( customPostNrpnFunction2( dspId, nrpn, val8bit, value, dvalue ) )// Do all your custom post NRPN code into this function
		return 1;
	#endif
	return 0;

}
#endif	// _SKIP_DDD_NRPN_CTRL
