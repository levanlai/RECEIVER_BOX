/* This file contains all dsp routing and initialization */
#include <system.h>
#include "dspDesigner.h"

WORD OutBusOf( WORD DSPid, WORD out ){ return( 0x80+((DSPid-0x30)<<3)+out ); }

WORD dsp[NBDSPINITIALIZED];

const WORD dspRouting_In[4][8] = 
{
	{DAAD0L,0,0,0,0,0,0,0},
	{DAAD0R,0,0,0,0,0,0,0},
	{32816,0,0,0,0,0,0,0},
	{32769,32785,32768,32784,0,0,0,0}
};

const WORD dspRouting_Out[4][8] = 
{
	{-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1},
	{DABD0L,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1}
};

WORD getBus( WORD bus )
{
	if ( bus == -1 ) return -1;
	if ( bus&(1<<15) )
		return OutBusOf( dsp[(bus>>4)&0xF], bus&0x7 );
	else
		return bus;
}

void dspDesigner_InitAndRoute (void)
{

	WORD rout[8], i, j;
	WORD dspIndex[4] = { 0,1,2,3 };
	dsp[0] = dsp1InitAndRoute();
	dsp[1] = dsp2InitAndRoute();
	dsp[2] = dsp3InitAndRoute();
	dsp[3] = dsp4InitAndRoute();

	// Start DSP
	for (i = 0; i<4; i++)
	{
		for (j=0; j<8; j++)
			rout[j] = getBus( dspRouting_In[i][j] );
		_DSProutInEx( dsp[dspIndex[i]], rout );

		for (j=0; j<8; j++)
			rout[j] = getBus( dspRouting_Out[i][j] );
		_DSProutEx( dsp[dspIndex[i]], rout );

		_StartDSP( dsp[dspIndex[i]] );
	}

}
