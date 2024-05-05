		.export 	_fuji_cbm_open

		.import 	_cbm_k_open
		.import 	_cbm_k_setlfs
		.import 	popa
		.import     ___oserror

        .include    "cbm.inc"
        .include    "macros.inc"
        .include    "zp.inc"

; A replacement for cbm_open that takes parameters for location of the data to send to SETNAM and the size
; instead of computing it, which allows us to send binary data as part of the NAME field to pass parameters to FN.

; uint8_t fuji_cbm_open(uint8_t lfn,
;                       uint8_t device,
;                       uint8_t sec_addr,
;                       const uint8_t  len
;                       const uint8_t* name,
;                       );

_fuji_cbm_open:
		setax 	ptr1			; name in ptr1
		jsr 	popa			; length of data (can include 0x00 values in it now)
        ldx     ptr1
        ldy     ptr1+1
        jsr     SETNAM

		jsr 	popa			; sec_addr
		jsr 	_cbm_k_setlfs	; Call SETLFS, pop all args

		jsr     _cbm_k_open
		sta 	___oserror
		rts
