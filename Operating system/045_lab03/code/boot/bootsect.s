	.code16
	.equ SYSSIZE, 0x3000

	.equ SETUPLEN, 2		# nr of setup-sectors
	.equ BOOTSEG, 0x07c0		# original address of boot-sector
	.equ SETUPSEG, 0x07e0		# setup starts here
	.equ SYSSEG, 0x1000		# system loaded at 0x10000 (65536).
	.equ ENDSEG, SYSSEG + SYSSIZE	# where to stop loading

# ROOT_DEV:	0x000 - same type of floppy as boot.
#		0x301 - first partition on first drive etc
	ljmp    $BOOTSEG, $_start
_start:
	mov	$BOOTSEG, %ax
	mov	%ax, %ds	
	mov	%ax, %es
	mov	$0x03, %ah		# read cursor pos
	xor	%bh, %bh
	int	$0x10
	mov	$26, %cx
	mov	$0x000c, %bx		# page 0, attribute 7 (normal)
	#lea	msg1, %bp
	mov     $msg1, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10
	
load_setup:
	mov	$0x0000, %dx		# drive 0, head 0
	mov	$0x0002, %cx		# sector 2, track 0
	mov	$0x0200, %bx		# address = 512, in INITSEG
	.equ    AX, 0x0200+SETUPLEN
	mov     $AX, %ax		# service 2, nr of sectors
	int	$0x13			# read it
	jnc	ok_load_setup		# ok - continue
	mov	$0x0000, %dx
	mov	$0x0000, %ax		# reset the diskette
	int	$0x13
	jmp	load_setup

ok_load_setup:
	ljmp	$SETUPSEG, $0

msg1:
	.byte 13,10
	.ascii "ZZX os is booting..."
	.byte 13,10,13,10

	.org 510
boot_flag:
	.word 0xAA55

