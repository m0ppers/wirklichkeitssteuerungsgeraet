DMACONR		EQU		$dff002
ADKCONR		EQU		$dff010
INTENAR		EQU		$dff01c
INTREQR		EQU		$dff01e

DMACON		EQU		$dff096
ADKCON		EQU		$dff09e
INTENA		EQU		$dff09a
INTREQ		EQU		$dff09c

BPLCON0         EQU             $dff100
BPLCON1         EQU             $dff102
BPL1MOD         EQU             $dff108
BPL2MOD         EQU             $dff10a
DIWSTRT         EQU             $dff08e
DIWSTOP         EQU             $dff090
DDFSTRT         EQU             $dff092
DDFSTOP         EQU             $dff094
VPOSR           EQU             $dff004
COP1LCH         EQU             $dff080

CIAAPRA         EQU             $bfe001

main:
	jsr init
	rts

init:
	; store data in hardwareregisters ORed with $8000
        ;(bit 15 is a write-set bit when values are written back into the system)
	move.w	DMACONR,d0
	or.w #$8000,d0
	move.w d0,olddmareq
	move.w	INTENAR,d0
	or.w #$8000,d0
	move.w d0,oldintena
	move.w	INTREQR,d0
	or.w #$8000,d0
	move.w d0,oldintreq
	move.w	ADKCONR,d0
	or.w #$8000,d0
	move.w d0,oldadkcon

	move.l	$4,a6
	move.l	#gfxname,a1
	moveq	#0,d0
	jsr	-552(a6)
	move.l	d0,gfxbase
	move.l 	d0,a6
	move.l 	34(a6),oldview
	move.l 	38(a6),oldcopper

	move.l #0,a1
	jsr -222(a6)	; LoadView
	jsr -270(a6)	; WaitTOF
	jsr -270(a6)	; WaitTOF
	move.l	$4,a6
	jsr -132(a6)	; Forbid

        ; setup displayhardware to show a 320x200px 3 bitplanes playfield
        ; with zero horizontal scroll and zero modulos
	move.l	a6,olda6
	move.l	d0,oldd0
	move.w	#$4200,BPLCON0			; three bitplanes
	move.w	#$0000,BPLCON1			; horizontal scroll 0
	move.w	#$0078,BPL1MOD			; odd modulo
	move.w	#$0078,BPL2MOD			; even modulo
	move.w	#$2c81,DIWSTRT			; DIWSTRT - topleft corner (2c81)
	move.w	#$2cc1,DIWSTOP			; DIWSTOP - bottomright corner (c8d1)
	move.w	#$0038,DDFSTRT			; DDFSTRT
	move.w	#$00d0,DDFSTOP			; DDFSTOP
	;move.w  #%1000000110000000,DMACON       ; DMA set ON
	;move.w 	#%0000000001111111,DMACON	; DMA set OFF
	;move.w 	#%1100000000000000,INTENA	; IRQ set ON
	;move.w 	#%0011111111111111,INTENA	; IRQ set OFF

mainloop:
	move.l #copper,a6

	; bitplane 0
	move.l #bitplanes,d0
	move.w #$00e2,(a6)+	; LO-bits of start of bitplane
	move.w d0,(a6)+		; go into $dff0e2
	swap d0
	move.w #$00e0,(a6)+	; HI-bits of start of bitplane
	move.w d0,(a6)+		; go into $dff0e0

	; bitplane 1
	move.l #bitplanes+40,d0
	move.w #$00e6,(a6)+	; LO-bits of start of bitplane
	move.w d0,(a6)+		; go into $dff0e6
	swap d0
	move.w #$00e4,(a6)+	; HI-bits of start of bitplane
	move.w d0,(a6)+		; go into $dff0e4

	; bitplane 2
	move.l #bitplanes+80,d0
	move.w #$00ea,(a6)+	; LO-bits of start of bitplane
	move.w d0,(a6)+		; go into $dff0e6
	swap d0
	move.w #$00e8,(a6)+	; HI-bits of start of bitplane
	move.w d0,(a6)+		; go into $dff0e4

    	; bitplane 2
	move.l #bitplanes+120,d0
	move.w #$00ee,(a6)+	; LO-bits of start of bitplane
	move.w d0,(a6)+		; go into $dff0e6
	swap d0
	move.w #$00ec,(a6)+	; HI-bits of start of bitplane
	move.w d0,(a6)+		; go into $dff0e4

	; colors
	move.l #$01800000,(a6)+	; color 0
	move.l #$01820223,(a6)+	; color 1
	move.l #$01840333,(a6)+	; color 2
	move.l #$01860555,(a6)+	; color 3
	move.l #$01880542,(a6)+	; color 4
	move.l #$018a0666,(a6)+	; color 5
	move.l #$018c0462,(a6)+	; color 6
	move.l #$018e0423,(a6)+	; color 7
    move.l #$01900337,(a6)+	; color 7
    move.l #$01920894,(a6)+	; color 7
    move.l #$01940fff,(a6)+	; color 7

    ;dc.w $180,$0
	;dc.w $182,$223
	;dc.w $184,$333
	;dc.w $186,$555
	;dc.w $188,$542
	;dc.w $18a,$666
	;dc.w $18c,$462
	;dc.w $18e,$423
	;dc.w $190,$337
	;dc.w $192,$894
	;dc.w $194,$fff

	; end of copperlist
	move.l #$fffffffe,(a6)+

	; if mousebutton/joystick 1 or 2 pressed then exit
	btst.b #6,CIAAPRA
	beq exit
	btst.b #7,CIAAPRA
	beq exit

; Wait for vertical blanking before taking the copper list into use
waitVB:
	;move.l VPOSR,d0
	;and.l #$1ff00,d0
	;cmp.l #300<<8,d0
	;bne waitVB

	; Take copper list into use
	move.l #copper,a6
	move.l a6,COP1LCH
	bra mainloop

exit:
	move.l olda6,a6
	move.l oldd0,a6
; exit gracefully - reverse everything done in init
	move.w #$7fff,DMACON
	move.w	olddmareq,DMACON
	move.w #$7fff,INTENA
	move.w	oldintena,INTENA
	move.w #$7fff,INTREQ
	move.w	oldintreq,INTREQ
	move.w #$7fff,ADKCON
	move.w	oldadkcon,ADKCON

	move.l	oldcopper,COP1LCH
	move.l 	gfxbase,a6
	move.l 	oldview,a1
	jsr -222(a6)	; LoadView
	jsr -270(a6)	; WaitTOF
	jsr -270(a6)	; WaitTOF
	move.l	$4,a6
	jsr -138(a6)	; Permit

	; end program
	rts

; *******************************************************************************
; *******************************************************************************
; DATA
; *******************************************************************************
; *******************************************************************************

; storage for 32-bit addresses and data
	CNOP 0,4
oldview:	dc.l 0
oldcopper:	dc.l 0
gfxbase:	dc.l 0
frame:          dc.l 0

; storage for 16-bit data
	CNOP 0,4
olddmareq:	dc.w 0
oldintreq:	dc.w 0
oldintena:	dc.w 0
oldadkcon:	dc.w 0

olda6:	dc.l 0
oldd0:	dc.l 0


	CNOP 0,4
gfxname: dc.b 'graphics.library',0

	Section ChipRAM,Data_c

	CNOP 0,4
bitplanes:
  incbin "akronyme_analogiker.bin"
  blk.b 320/8*3*(200-160),0

; datalists aligned to 32-bit
	CNOP 0,4
copper:
  dc.l $ffffffe
  blk.l 1023,0
