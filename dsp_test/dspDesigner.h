#ifndef __DSPDESIGNER_H__
#define __DSPDESIGNER_H__

#define NBDSPINITIALIZED 1

typedef struct biquadTag
{	struct _tagFILTER_PARAM *filterParameters;
	WORD *type;
	WORD *xoverType;
	DWORD *frequency; // 20.12 fixedpoint format
	int numberOfBand;
	int channel; // command channel attributed to the biquad
} BiquadParameters;

typedef struct biquadParamsTabTag
{	WORD nrpn;
	WORD functionId;
	BiquadParameters *parametersTable;
} BiquadParamsTable;

typedef struct biquadCommandParamsTabTag
{	WORD command;
	BiquadParameters *parametersTable;
} BiquadCommandParamsTable;

typedef struct UpdateCoeffCallback_tag
{
	PTR32 BIQUAD_FlatFuncPtr;
	PTR32 BIQUAD_UpdateCoeffFuncPtr;
} UpdateCoeffCallback;

void dspDesigner_InitAndRoute (void);
void dspDesigner_InitNrpnFunction (void);
WORD dichotomicSearch( PTR32 table, WORD rowSize, WORD itemIndex, WORD tableLen, WORD valueToSearch );
WORD dspNrpnHandlerCallback ( PTR32 FunctionPointer32, WORD nrpn, WORD dspId, WORD processId, DWORD value, WORD format );
extern WORD dsp[NBDSPINITIALIZED];

WORD dsp1InitAndRoute(void);
extern WORD dsp1pcs[5];
#define	NUMBEROFPROCESS1	0x0002
#define	NUMBEROFCOMMAND1	0x0004
extern const WORD nrpn1List[NUMBEROFCOMMAND1][2];
extern WORD dsp1NrpnHandler( WORD nrpn, WORD dspId, WORD processId, DWORD value, WORD format );

#endif //__DSPDESIGNER_H__