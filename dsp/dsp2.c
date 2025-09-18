#include <system.h>
#include <libFX5000.h>
#include "dspDesigner.h"
#include "custom.h"
#ifndef	_SKIP_DDD_NRPN_CTRL
#include "midictrl.h"
#endif	// _SKIP_DDD_NRPN_CTRL


WORD dsp2pcs[3];

WORD dsp2InitAndRoute(void)
{
	WORD dspId;

	dspId = _FBCancel_Init();

	if ( dspId == -1 ) return 0;

#ifdef _customPreInitFunction2
customPreInitFunction2( dspId );// Do all your custom pre initialization code into this function
#endif

	// Process #1: Feedback canceller
	_FBCancel_SetProcIN( dspId, BIQUAD_SAMPLE_IN, PCS_DSP_IN | 0 );
	_FBCancel_SetProcOUT( dspId, BIQUAD_SAMPLE_OUT, PCS_NODE | 0 );

	// Process #2: Gain
	dsp2pcs[2] = _FBCancel_Gain_Allocate( dspId );
	_FBCancel_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp2pcs[2], PCS_NODE | 0 );
	_FBCancel_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp2pcs[2], PCS_DSP_OUT | 0 );

#ifdef _customPostInitFunction2
customPostInitFunction2( dspId );// Do all your custom post initialization code into this function
#endif

	return( dspId );
}


#ifndef	_SKIP_DDD_NRPN_CTRL

const WORD nrpn2List[NUMBEROFCOMMAND2][2]=
{
	{ 0x0100, 0x0000 }, // _FBCancel_Bypass
	{ 0x0101, 0x0001 }, // _FBCancel_LockFilters
	{ 0x0102, 0x0002 }, // _FBCancel_Reset
	{ 0x0103, 0x0003 }, // _FBCancel_SetMode
	{ 0x0104, 0x0004 }, // _FBCancel_SetNbStaticFilter
	{ 0x0105, 0x0005 }, // _FBCancel_SetNbStaticFilter
	{ 0x0200, 0x000D }, // _FBCancel_Gain_Value
	{ 0x0201, 0x000E } // _FBCancel_Gain_Phase

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
			//Feedback canceller
			case 0x0000: _FBCancel_Bypass( dspId, val8bit ); return 1;
			case 0x0001: _FBCancel_LockFilters( processId, val8bit ); return 1;
			case 0x0002: _FBCancel_Reset( dspId ); return 1;
			case 0x0003: _FBCancel_SetMode( dspId, val8bit ); return 1;
			case 0x0004: _FBCancel_SetNbStaticFilter( dspId, val8bit, 0/* without reset*/ ); return 1;
			case 0x0005: _FBCancel_SetNbStaticFilter( dspId, val8bit, 1/* with reset*/ ); return 1;
			//Gain
			case 0x000D: _FBCancel_Gain_Value( dspId, processId, value ); return 1;
			case 0x000E: _FBCancel_Gain_Phase( dspId, processId, val8bit ); return 1;
		
		}
	
	}
	
	#ifdef _customPostNrpnFunction2
	if ( customPostNrpnFunction2( dspId, nrpn, val8bit, value, dvalue ) )// Do all your custom post NRPN code into this function
		return 1;
	#endif
	return 0;

}
#endif	// _SKIP_DDD_NRPN_CTRL
