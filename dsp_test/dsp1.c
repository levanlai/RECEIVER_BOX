#include <system.h>
#include <libFX5000.h>
#include "dspDesigner.h"
#include "custom.h"
#ifndef	_SKIP_DDD_NRPN_CTRL
#include "midictrl.h"
#endif	// _SKIP_DDD_NRPN_CTRL


WORD dsp1pcs[5];

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
	_MixPaXT_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp1pcs[2], PCS_DSP_IN | 0 );
	_MixPaXT_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp1pcs[2], PCS_DSP_OUT | 0 );

	// Process #4: Gain
	dsp1pcs[4] = _MixPaXT_Gain_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp1pcs[4], PCS_DSP_IN | 1 );
	_MixPaXT_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp1pcs[4], PCS_DSP_OUT | 1 );

#ifdef _customPostInitFunction1
customPostInitFunction1( dspId );// Do all your custom post initialization code into this function
#endif

	return( dspId );
}


#ifndef	_SKIP_DDD_NRPN_CTRL

const WORD nrpn1List[NUMBEROFCOMMAND1][2]=
{
	{ 0x0200, 0x002F }, // _MixPaXT_Gain_Value
	{ 0x0201, 0x0030 }, // _MixPaXT_Gain_Phase
	{ 0x0400, 0x002F }, // _MixPaXT_Gain_Value
	{ 0x0401, 0x0030 } // _MixPaXT_Gain_Phase

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
			//Gain
			case 0x002F: _MixPaXT_Gain_Value( dspId, processId, value ); return 1;
			case 0x0030: _MixPaXT_Gain_Phase( dspId, processId, val8bit ); return 1;
		
		}
	
	}
	
	#ifdef _customPostNrpnFunction1
	if ( customPostNrpnFunction1( dspId, nrpn, val8bit, value, dvalue ) )// Do all your custom post NRPN code into this function
		return 1;
	#endif
	return 0;

}
#endif	// _SKIP_DDD_NRPN_CTRL
