FIRMWARE_SIGNATURE EQU 0x4452; Our special Firmware Signature
	segment code
	PUBLIC __lowlevel_init
__lowlevel_init:
	li r0, 0x0B ; Enable GP CS
	orio 0x0030 ; SPICS0/ bit 5, SPI bit 4
	li r5, 0xFF03 ; Re-Initialize QSPI Controller
	wrio r5, 0x68
	li r5, 00006h
	wrio r5, 0x69
	li r5, 0x0090
	wrio r5, 0x6A
	li r5, 6 ; Clock Divider (25MHz)
	wrio r5, 0x6B
	dli dr6, 0xC0000000 ; GP QSPI (mapped to DataFlash)
	dli dr7, 0x80000000 ; (SD)RAM Shadow Memory
	lxi r0, dr6
	cpi r0, FIRMWARE_SIGNATURE ; Verify proper Signature
	jnz $
	dlxi dr3, dr6+1 ; Get Firmware Size
lliCopyToSdram: ; Copy Firmware into RAM
	lxi r0, dr6
	stxi r0, dr7
	dinc dr6
	dinc dr7
	ddec dr3
	jnz lliCopyToSdram
	ret