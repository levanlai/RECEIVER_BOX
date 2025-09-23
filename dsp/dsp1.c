#include <system.h>
#include <libFX5000.h>
#include "dspDesigner.h"
#include "custom.h"
#ifndef	_SKIP_DDD_NRPN_CTRL
#include "midictrl.h"
#endif	// _SKIP_DDD_NRPN_CTRL


WORD dsp1pcs[14];

WORD dsp1InitAndRoute(void)
{
	WORD dspId;

	dspId = _MixPaXT_Init();

	if ( dspId == -1 ) return 0;

#ifdef _customPreInitFunction1
customPreInitFunction1( dspId );// Do all your custom pre initialization code into this function
#endif

	// Process #3: MixN
	dsp1pcs[3] = _MixPaXT_MixN_Allocate( dspId, 2 );
	_MixPaXT_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp1pcs[3], PCS_DSP_IN | 0 );
	_MixPaXT_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp1pcs[3], PCS_NODE | 0 );

	// Process #4: Gain
	dsp1pcs[4] = _MixPaXT_Gain_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp1pcs[4], PCS_NODE | 0 );
	_MixPaXT_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp1pcs[4], PCS_NODE | 1 );

	// Process #13: NoiseGate
	dsp1pcs[13] = _MixPaXT_NoiseGate_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, NOISEGATE_SAMPLE_IN|dsp1pcs[13], PCS_NODE | 1 );
	_MixPaXT_SetProcOUT( dspId, NOISEGATE_SAMPLE_OUT|dsp1pcs[13], PCS_NODE | 2 );

	// Process #10: FreqShifter
	dsp1pcs[10] = _MixPaXT_FreqShifter_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, FREQSHIFTER_SAMPLE_IN|dsp1pcs[10], PCS_NODE | 2 );
	_MixPaXT_SetProcOUT( dspId, FREQSHIFTER_SAMPLE_OUT|dsp1pcs[10], PCS_NODE | 3 );

	// Process #9: Gain
	dsp1pcs[9] = _MixPaXT_Gain_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp1pcs[9], PCS_NODE | 3 );
	_MixPaXT_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp1pcs[9], PCS_DSP_OUT | 0 );

#ifdef _customPostInitFunction1
customPostInitFunction1( dspId );// Do all your custom post initialization code into this function
#endif

	return( dspId );
}


#ifndef	_SKIP_DDD_NRPN_CTRL

const WORD nrpn1List[NUMBEROFCOMMAND1][2]=
{
	{ 0x0300, 0x403A }, // _MixPaXT_MixN_GainPhase
	{ 0x031F, 0x403B }, // _MixPaXT_MixN_GainValue
	{ 0x0400, 0x002F }, // _MixPaXT_Gain_Value
	{ 0x0401, 0x0030 }, // _MixPaXT_Gain_Phase
	{ 0x0900, 0x002F }, // _MixPaXT_Gain_Value
	{ 0x0901, 0x0030 }, // _MixPaXT_Gain_Phase
	{ 0x0A00, 0x0029 }, // _MixPaXT_FreqShifter_OnOff
	{ 0x0A01, 0x002A }, // _MixPaXT_FreqShifter_PostHP
	{ 0x0A02, 0x002B }, // _MixPaXT_FreqShifter_Amount
	{ 0x0A03, 0x002C }, // _MixPaXT_FreqShifter_SetMaxScale
	{ 0x0A04, 0x002D }, // _MixPaXT_FreqShifter_InGainValue
	{ 0x0A05, 0x002E }, // _MixPaXT_FreqShifter_InGainPhase
	{ 0x0D00, 0x003C }, // _MixPaXT_NoiseGate_OnOff
	{ 0x0D01, 0x003D }, // _MixPaXT_NoiseGate_Threshold
	{ 0x0D02, 0x003E }, // _MixPaXT_NoiseGate_Attack
	{ 0x0D03, 0x003F }, // _MixPaXT_NoiseGate_Release
	{ 0x0D04, 0x0040 }, // _MixPaXT_NoiseGate_InGainValue
	{ 0x0D05, 0x0041 } // _MixPaXT_NoiseGate_InGainPhase

};

WORD dsp1NrpnHandler( WORD nrpn, WORD dspId, WORD processId, DWORD value, WORD format )
{
	
	WORD i, functionId, index = 0, val8bit;
	DWORD dvalue;
	#ifdef _customPreNrpnFunction1
	if ( customPreNrpnFunction1( dspId, nrpn, &value, format ) )// Do all your custom pre NRPN code into this function
		return 1;
	#endif
	
	val8bit = value>>8;
	if ( format == FORMAT_14BIT_PRECISION )
		dvalue=(DWORD)value<<(DWORD)13;
	else
		dvalue=value;
	
	i = dichotomicSearch( _cptr32(nrpn1List), 2, 0, NUMBEROFCOMMAND1, nrpn );
	
	if ((i&(1<<15)))
	{
		i &= 0x7FFF;
		if ( i == NUMBEROFCOMMAND1 ) // higher bound test
			i--;
		else
		{
			if ( nrpn < nrpn1List[i][0] )
				i--;		
		}
		if ( !(nrpn1List[i][1] & (1<<14)) )
			i=-1;	
	}
	
	if ( i != -1 )
	{
		index = nrpn - nrpn1List[i][0];
		functionId = nrpn1List[i][1];
		processId = dsp1pcs[processId];
	}
	else
		return 0;	
	if (i != -1)
	{
		switch (functionId)
		{
			//MixN
			case 0x403A: _MixPaXT_MixN_GainPhase( dspId, processId, index, val8bit ); return 1;
			case 0x403B: _MixPaXT_MixN_GainValue( dspId, processId, index, value ); return 1;
			//Gain
			case 0x002F: _MixPaXT_Gain_Value( dspId, processId, value ); return 1;
			case 0x0030: _MixPaXT_Gain_Phase( dspId, processId, val8bit ); return 1;
			//FreqShifter
			case 0x0029: _MixPaXT_FreqShifter_OnOff( dspId, processId, val8bit ); return 1;
			case 0x002A: _MixPaXT_FreqShifter_PostHP( dspId, processId, val8bit ); return 1;
			case 0x002B: _MixPaXT_FreqShifter_Amount( dspId, processId, value ); return 1;
			case 0x002C: _MixPaXT_FreqShifter_SetMaxScale( dspId, processId, value ); return 1;
			case 0x002D: _MixPaXT_FreqShifter_InGainValue( dspId, processId, value ); return 1;
			case 0x002E: _MixPaXT_FreqShifter_InGainPhase( dspId, processId, val8bit ); return 1;
			//NoiseGate
			case 0x003C: _MixPaXT_NoiseGate_OnOff( dspId, processId, val8bit ); return 1;
			case 0x003D: _MixPaXT_NoiseGate_Threshold( dspId, processId, value ); return 1;
			case 0x003E: _MixPaXT_NoiseGate_Attack( dspId, processId, value ); return 1;
			case 0x003F: _MixPaXT_NoiseGate_Release( dspId, processId, value ); return 1;
			case 0x0040: _MixPaXT_NoiseGate_InGainValue( dspId, processId, value ); return 1;
			case 0x0041: _MixPaXT_NoiseGate_InGainPhase( dspId, processId, val8bit ); return 1;
		
		}
	
	}
	
	#ifdef _customPostNrpnFunction1
	if ( customPostNrpnFunction1( dspId, nrpn, val8bit, value, dvalue ) )// Do all your custom post NRPN code into this function
		return 1;
	#endif
	return 0;

}
#endif	// _SKIP_DDD_NRPN_CTRL
