#include <system.h>
#include <libFX5000.h>
#include "dspDesigner.h"
#include "custom.h"
#ifndef	_SKIP_DDD_NRPN_CTRL
#include "midictrl.h"
#endif	// _SKIP_DDD_NRPN_CTRL
#include "BiquadCtrl.h"

// Biquad(s) - define, variable, ... 
#define BIQUAD_ITEMCOUNT 2


#define NUMBER_OF_BIQUAD_EXTRAFUNCTION 4

#define	BIQUAD6BANDCOUNT	5
#define	BIQUAD12BANDCOUNT	5

#ifndef	_SKIP_DDD_NRPN_CTRL


_FILTER_PARAM biquad1Parameters6[BIQUAD6BANDCOUNT], biquad1Parameters12[BIQUAD12BANDCOUNT];
WORD biquad1Type6[BIQUAD6BANDCOUNT], biquad1Type12[BIQUAD12BANDCOUNT];
WORD biquad1XoverType6[BIQUAD6BANDCOUNT], biquad1XoverType12[BIQUAD12BANDCOUNT];
DWORD biquad1RawFrequency6[BIQUAD6BANDCOUNT], biquad1RawFrequency12[BIQUAD12BANDCOUNT];

BiquadParameters biquad1ParamAddr6 = { biquad1Parameters6, biquad1Type6, biquad1XoverType6, biquad1RawFrequency6, BIQUAD6BANDCOUNT };
BiquadParameters biquad1ParamAddr12 = { biquad1Parameters12, biquad1Type12, biquad1XoverType12, biquad1RawFrequency12, BIQUAD12BANDCOUNT };
#endif	// _SKIP_DDD_NRPN_CTRL

WORD dsp1pcs[18];

WORD dsp1InitAndRoute(void)
{
	WORD dspId;

	dspId = _MixPaXT_Init();

	if ( dspId == -1 ) return 0;

#ifdef _customPreInitFunction1
customPreInitFunction1( dspId );// Do all your custom pre initialization code into this function
#endif

	// Process #2: Gain
	dsp1pcs[2] = _MixPaXT_Gain_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp1pcs[2], PCS_DSP_IN | 4 );
	_MixPaXT_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp1pcs[2], PCS_NODE | 1 );

	// Process #8: Gain
	dsp1pcs[8] = _MixPaXT_Gain_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp1pcs[8], PCS_DSP_IN | 5 );
	_MixPaXT_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp1pcs[8], PCS_NODE | 3 );

	// Process #3: MixN
	dsp1pcs[3] = _MixPaXT_MixN_Allocate( dspId, 3 );
	_MixPaXT_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp1pcs[3], PCS_DSP_IN | 0 );
	_MixPaXT_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp1pcs[3], PCS_NODE | 4 );

	// Process #16: Gain
	dsp1pcs[16] = _MixPaXT_Gain_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp1pcs[16], PCS_DSP_IN | 3 );
	_MixPaXT_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp1pcs[16], PCS_NODE | 0 );

	// Process #17: Gain
	dsp1pcs[17] = _MixPaXT_Gain_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp1pcs[17], PCS_DSP_IN | 3 );
	_MixPaXT_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp1pcs[17], PCS_NODE | 2 );

	// Process #14: MixN
	dsp1pcs[14] = _MixPaXT_MixN_Allocate( dspId, 2 );
	_MixPaXT_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp1pcs[14], PCS_NODE | 0 );
	_MixPaXT_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp1pcs[14], PCS_NODE | 5 );

	// Process #15: MixN
	dsp1pcs[15] = _MixPaXT_MixN_Allocate( dspId, 2 );
	_MixPaXT_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp1pcs[15], PCS_NODE | 2 );
	_MixPaXT_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp1pcs[15], PCS_NODE | 6 );

	// Process #4: Gain
	dsp1pcs[4] = _MixPaXT_Gain_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp1pcs[4], PCS_NODE | 4 );
	_MixPaXT_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp1pcs[4], PCS_NODE | 7 );

	// Process #1: Exciter
	dsp1pcs[1] = _MixPaXT_Exciter_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, EXCITER_SAMPLE_IN|dsp1pcs[1], PCS_NODE | 5 );
	_MixPaXT_SetProcOUT( dspId, EXCITER_SAMPLE_OUT|dsp1pcs[1], PCS_NODE | 8 );

	// Process #7: Exciter
	dsp1pcs[7] = _MixPaXT_Exciter_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, EXCITER_SAMPLE_IN|dsp1pcs[7], PCS_NODE | 6 );
	_MixPaXT_SetProcOUT( dspId, EXCITER_SAMPLE_OUT|dsp1pcs[7], PCS_NODE | 9 );

	// Process #13: NoiseGate
	dsp1pcs[13] = _MixPaXT_NoiseGate_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, NOISEGATE_SAMPLE_IN|dsp1pcs[13], PCS_NODE | 7 );
	_MixPaXT_SetProcOUT( dspId, NOISEGATE_SAMPLE_OUT|dsp1pcs[13], PCS_NODE | 10 );

	// Process #6: Biquad
	dsp1pcs[6] = _MixPaXT_Biquad_Allocate( dspId, BIQUAD6BANDCOUNT );
	_MixPaXT_SetProcIN( dspId, BIQUAD_SAMPLE_IN|dsp1pcs[6], PCS_NODE | 8 );
	_MixPaXT_SetProcOUT( dspId, BIQUAD_SAMPLE_OUT|dsp1pcs[6], PCS_NODE | 11 );

	// Process #12: Biquad
	dsp1pcs[12] = _MixPaXT_Biquad_Allocate( dspId, BIQUAD12BANDCOUNT );
	_MixPaXT_SetProcIN( dspId, BIQUAD_SAMPLE_IN|dsp1pcs[12], PCS_NODE | 9 );
	_MixPaXT_SetProcOUT( dspId, BIQUAD_SAMPLE_OUT|dsp1pcs[12], PCS_NODE | 12 );

	// Process #10: FreqShifter
	dsp1pcs[10] = _MixPaXT_FreqShifter_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, FREQSHIFTER_SAMPLE_IN|dsp1pcs[10], PCS_NODE | 10 );
	_MixPaXT_SetProcOUT( dspId, FREQSHIFTER_SAMPLE_OUT|dsp1pcs[10], PCS_NODE | 13 );

	// Process #5: Gain
	dsp1pcs[5] = _MixPaXT_Gain_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp1pcs[5], PCS_NODE | 11 );
	_MixPaXT_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp1pcs[5], PCS_DSP_OUT | 1 );

	// Process #11: Gain
	dsp1pcs[11] = _MixPaXT_Gain_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp1pcs[11], PCS_NODE | 12 );
	_MixPaXT_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp1pcs[11], PCS_DSP_OUT | 2 );

	// Process #9: Gain
	dsp1pcs[9] = _MixPaXT_Gain_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp1pcs[9], PCS_NODE | 13 );
	_MixPaXT_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp1pcs[9], PCS_DSP_OUT | 0 );

#ifdef _customPostInitFunction1
customPostInitFunction1( dspId );// Do all your custom post initialization code into this function
#endif

	return( dspId );
}


#ifndef	_SKIP_DDD_NRPN_CTRL

const WORD nrpn1List[NUMBEROFCOMMAND1][2]=
{
	{ 0x0100, 0x0023 }, // _MixPaXT_Exciter_OnOff
	{ 0x0101, 0x0024 }, // _MixPaXT_Exciter_InGainValue
	{ 0x0102, 0x0025 }, // _MixPaXT_Exciter_ThresholdLow
	{ 0x0103, 0x0026 }, // _MixPaXT_Exciter_ThresholdHigh
	{ 0x0104, 0x0027 }, // _MixPaXT_Exciter_LoContourAmount
	{ 0x0105, 0x0028 }, // _MixPaXT_Exciter_Intensity
	{ 0x0200, 0x002F }, // _MixPaXT_Gain_Value
	{ 0x0201, 0x0030 }, // _MixPaXT_Gain_Phase
	{ 0x0300, 0x403A }, // _MixPaXT_MixN_GainPhase
	{ 0x031F, 0x403B }, // _MixPaXT_MixN_GainValue
	{ 0x0400, 0x002F }, // _MixPaXT_Gain_Value
	{ 0x0401, 0x0030 }, // _MixPaXT_Gain_Phase
	{ 0x0500, 0x002F }, // _MixPaXT_Gain_Value
	{ 0x0501, 0x0030 }, // _MixPaXT_Gain_Phase
	{ 0x0600, 0x0000 }, // _MixPaXT_Biquad_OnOff
	{ 0x0601, 0x0001 }, // _MixPaXT_Biquad_InGainPhase
	{ 0x0602, 0x0002 }, // _MixPaXT_Biquad_InGainValue
	{ 0x0700, 0x0023 }, // _MixPaXT_Exciter_OnOff
	{ 0x0701, 0x0024 }, // _MixPaXT_Exciter_InGainValue
	{ 0x0702, 0x0025 }, // _MixPaXT_Exciter_ThresholdLow
	{ 0x0703, 0x0026 }, // _MixPaXT_Exciter_ThresholdHigh
	{ 0x0704, 0x0027 }, // _MixPaXT_Exciter_LoContourAmount
	{ 0x0705, 0x0028 }, // _MixPaXT_Exciter_Intensity
	{ 0x0800, 0x002F }, // _MixPaXT_Gain_Value
	{ 0x0801, 0x0030 }, // _MixPaXT_Gain_Phase
	{ 0x0900, 0x002F }, // _MixPaXT_Gain_Value
	{ 0x0901, 0x0030 }, // _MixPaXT_Gain_Phase
	{ 0x0A00, 0x0029 }, // _MixPaXT_FreqShifter_OnOff
	{ 0x0A01, 0x002A }, // _MixPaXT_FreqShifter_PostHP
	{ 0x0A02, 0x002B }, // _MixPaXT_FreqShifter_Amount
	{ 0x0A03, 0x002C }, // _MixPaXT_FreqShifter_SetMaxScale
	{ 0x0A04, 0x002D }, // _MixPaXT_FreqShifter_InGainValue
	{ 0x0A05, 0x002E }, // _MixPaXT_FreqShifter_InGainPhase
	{ 0x0B00, 0x002F }, // _MixPaXT_Gain_Value
	{ 0x0B01, 0x0030 }, // _MixPaXT_Gain_Phase
	{ 0x0C00, 0x0000 }, // _MixPaXT_Biquad_OnOff
	{ 0x0C01, 0x0001 }, // _MixPaXT_Biquad_InGainPhase
	{ 0x0C02, 0x0002 }, // _MixPaXT_Biquad_InGainValue
	{ 0x0D00, 0x003C }, // _MixPaXT_NoiseGate_OnOff
	{ 0x0D01, 0x003D }, // _MixPaXT_NoiseGate_Threshold
	{ 0x0D02, 0x003E }, // _MixPaXT_NoiseGate_Attack
	{ 0x0D03, 0x003F }, // _MixPaXT_NoiseGate_Release
	{ 0x0D04, 0x0040 }, // _MixPaXT_NoiseGate_InGainValue
	{ 0x0D05, 0x0041 }, // _MixPaXT_NoiseGate_InGainPhase
	{ 0x0E00, 0x403A }, // _MixPaXT_MixN_GainPhase
	{ 0x0E1F, 0x403B }, // _MixPaXT_MixN_GainValue
	{ 0x0F00, 0x403A }, // _MixPaXT_MixN_GainPhase
	{ 0x0F1F, 0x403B }, // _MixPaXT_MixN_GainValue
	{ 0x1000, 0x002F }, // _MixPaXT_Gain_Value
	{ 0x1001, 0x0030 }, // _MixPaXT_Gain_Phase
	{ 0x1100, 0x002F }, // _MixPaXT_Gain_Value
	{ 0x1101, 0x0030 } // _MixPaXT_Gain_Phase

};

#define NB_BIQUAD_COMMAND 8
const BiquadParamsTable nrpn1BiquadTable[NB_BIQUAD_COMMAND] = 
{
	{ 0x0603, 0x4003, &biquad1ParamAddr6 },
	{ 0x0622, 0x4004, &biquad1ParamAddr6 },
	{ 0x0641, 0x4005, &biquad1ParamAddr6 },
	{ 0x0660, 0x4006, &biquad1ParamAddr6 },
	{ 0x0C03, 0x4003, &biquad1ParamAddr12 },
	{ 0x0C22, 0x4004, &biquad1ParamAddr12 },
	{ 0x0C41, 0x4005, &biquad1ParamAddr12 },
	{ 0x0C60, 0x4006, &biquad1ParamAddr12 }
};

WORD dsp1NrpnHandler( WORD nrpn, WORD dspId, WORD processId, DWORD value, WORD format )
{
	
	WORD i, functionId, index = 0, val8bit;
	DWORD dvalue;
	BiquadParameters *theBiquad;
	UpdateCoeffCallback updateCoeffFunc;
	
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
	{
		i = dichotomicSearch( _cptr32(nrpn1BiquadTable), 3, 0, NB_BIQUAD_COMMAND, nrpn );
		if (i!=-1)
		{
			if ((i&(1<<15)))
			{
				i &= 0x7FFF;
				if ( i >= NB_BIQUAD_COMMAND ) // higher bound test
					i--;
				else
				{
					if ( nrpn < nrpn1BiquadTable[i].nrpn )
						i--;				
				}
				if ( !(nrpn1BiquadTable[i].functionId & (1<<14)) )
					i=-1;			
			}
			
			if ( i != -1 )
			{
				index = nrpn - nrpn1BiquadTable[i].nrpn;
				functionId = nrpn1BiquadTable[i].functionId;
				processId = dsp1pcs[processId];
				theBiquad = (BiquadParameters *)nrpn1BiquadTable[i].parametersTable;
				updateCoeffFunc.BIQUAD_UpdateCoeffFuncPtr = _cptr32( &_MixPaXT_Biquad_UpdateCoeff );
				updateCoeffFunc.BIQUAD_FlatFuncPtr = _cptr32( &_MixPaXT_Biquad_Flat );
			}
		
		}
	
	}
	
	if (i != -1)
	{
		switch (functionId)
		{
			//Gain
			case 0x002F: _MixPaXT_Gain_Value( dspId, processId, value ); return 1;
			case 0x0030: _MixPaXT_Gain_Phase( dspId, processId, val8bit ); return 1;
			//Biquad
			case 0x0000: _MixPaXT_Biquad_OnOff( dspId, processId, val8bit ); return 1;
			case 0x0001: _MixPaXT_Biquad_InGainPhase( dspId, processId, val8bit ); return 1;
			case 0x0002: _MixPaXT_Biquad_InGainValue( dspId, processId, value ); return 1;
			case 0x4003: SetFilterType( &updateCoeffFunc, theBiquad, dspId, processId, index, val8bit ); return 1;
			case 0x4004: SetFilterQ( &updateCoeffFunc, theBiquad, dspId, processId, index, value ); return 1;
			case 0x4005: SetFilterFreq( &updateCoeffFunc, theBiquad, dspId, processId, index, dvalue ); return 1;
			case 0x4006: SetFilterGain( &updateCoeffFunc, theBiquad, dspId, processId, index, value ); return 1;
			//MixN
			case 0x403A: _MixPaXT_MixN_GainPhase( dspId, processId, index, val8bit ); return 1;
			case 0x403B: _MixPaXT_MixN_GainValue( dspId, processId, index, value ); return 1;
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
			//Exciter
			case 0x0023: _MixPaXT_Exciter_OnOff( dspId, processId, val8bit ); return 1;
			case 0x0024: _MixPaXT_Exciter_InGainValue( dspId, processId, value ); return 1;
			case 0x0025: _MixPaXT_Exciter_ThresholdLow( dspId, processId, value ); return 1;
			case 0x0026: _MixPaXT_Exciter_ThresholdHigh( dspId, processId, value ); return 1;
			case 0x0027: _MixPaXT_Exciter_LoContourAmount( dspId, processId, value ); return 1;
			case 0x0028: _MixPaXT_Exciter_Intensity( dspId, processId, value ); return 1;
		
		}
	
	}
	
	#ifdef _customPostNrpnFunction1
	if ( customPostNrpnFunction1( dspId, nrpn, val8bit, value, dvalue ) )// Do all your custom post NRPN code into this function
		return 1;
	#endif
	return 0;

}
#endif	// _SKIP_DDD_NRPN_CTRL
