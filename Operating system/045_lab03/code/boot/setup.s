	.code16
	.equ INITSEG, 0x9000	# we move boot here - out of the way
	.equ SYSSEG, 0x1000	# system loaded at 0x10000 (65536).
	.equ SETUPSEG, 0x07e0		# setup starts here
	
	ljmp $SETUPSEG, $_start	
	
_start:
	mov	$SETUPSEG, %ax
	mov	%ax, %ds	
	mov	%ax, %es
	mov	$0x03, %ah		# read cursor pos
	xor	%bh, %bh
	int	$0x10
	mov	$25, %cx
	mov	$0x000c, %bx		# page 0, attribute 7 (normal)
	#lea	msg2, %bp
	mov     $msg2, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10

# ok, the read went well so we get current cursor position and save it for
# posterity.

	mov	$INITSEG, %ax	# this is done in bootsect already, but...
	mov	%ax, %ds
	mov	$0x03, %ah	# read cursor pos
	xor	%bh, %bh
	int	$0x10		# save it in known place, con_init fetches
	mov	%dx, %ds:0	# it from 0x90000.
	
# Get memory size (extended mem, kB)

	mov	$0x88, %ah 
	int	$0x15
	mov	%ax, %ds:2
	
# Get video-card data
	mov	$0x12, %ah
  	mov	$0x10, %bl 
	int	$0x10
  	mov 	%ax, %ds:8
	mov	%bx, %ds:10
 	mov	%cx, %ds:12
	
# Get hd0 data

	mov	$0x0000, %ax
	mov	%ax, %ds
	lds	%ds:4*0x41, %si
	mov	$INITSEG, %ax
	mov	%ax, %es
	mov	$0x0080, %di
	mov	$0x10, %cx
	rep
	movsb
	
info_print:
	# 打印硬件参数-cursor
	mov 	$INITSEG, %ax
	mov	%ax, %ds
	mov 	$SETUPSEG, %ax
	mov	%ax, %es
	mov    	$0x03, %ah
	xor	%bh, %bh
	int  	$0x10
	mov  	$11, %cx
	mov	$0x0007, %bx		# page 0, attribute 7 (normal)
	mov     $Cursor, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10
	
	mov 	%ds:0, %ax
	call	print_hex
	call	print_nl
	
	# 打印硬件参数-memory
	mov 	$INITSEG, %ax
	mov	%ax, %ds
	mov 	$SETUPSEG, %ax
	mov	%ax, %es
	mov    	$0x03, %ah
	xor	%bh, %bh
	int  	$0x10
	mov  	$12, %cx
	mov	$0x0007, %bx		# page 0, attribute 7 (normal)
	mov     $Memory, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10

	mov 	%ds:2, %ax
	call	print_hex

	mov 	$INITSEG, %ax
	mov	%ax, %ds
	mov 	$SETUPSEG, %ax
	mov	%ax, %es
	mov    	$0x03, %ah
	xor	%bh, %bh
	int  	$0x10
	mov  	$2, %cx
	mov	$0x0007, %bx		# page 0, attribute 7 (normal)
	mov     $kb, %bp
	mov	$0x1301, %ax		# write string, move cursor	
	int	$0x10

	call	print_nl
	
	# 打印硬件参数-video-card
	mov 	$INITSEG, %ax
	mov	%ax, %ds
	mov 	$SETUPSEG, %ax
	mov	%ax, %es
	mov    	$0x03, %ah
	xor	%bh, %bh
	int  	$0x10
	mov  	$24, %cx
	mov	$0x0007, %bx		# page 0, attribute 7 (normal)
	mov     $VIDEO, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10
	
	mov 	%ds:10, %ax
	call	print_hex
	call	print_nl
		
	# 打印硬件参数-HD0
	mov 	$INITSEG, %ax
	mov	%ax, %ds
	mov 	$SETUPSEG, %ax
	mov	%ax, %es
	mov    	$0x03, %ah
	xor	%bh, %bh
	int  	$0x10
	mov  	$21,%cx
	mov	$0x0007, %bx		# page 0, attribute 7 (normal)
	mov     $HD_info, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10

	mov 	%ds:128, %ax
	call	print_hex
	call	print_nl

	mov 	$INITSEG, %ax
	mov	%ax, %ds
	mov 	$SETUPSEG, %ax
	mov	%ax, %es
	mov    	$0x03, %ah
	xor	%bh, %bh
	int  	$0x10
	mov  	$8,%cx
	mov	$0x0007, %bx		# page 0, attribute 7 (normal)
	mov     $HD_info1, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10

	mov 	%ds:130, %ax
	call	print_hex
	call	print_nl

	mov 	$INITSEG,%ax
	mov	%ax, %ds
	mov 	$SETUPSEG, %ax
	mov	%ax, %es
	mov    	$0x03, %ah
	xor	%bh, %bh
	int  	$0x10
	mov  	$8,%cx
	mov	$0x0007, %bx		# page 0, attribute 7 (normal)
	mov     $HD_info2, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10

	mov 	%ds:142, %ax
	call	print_hex
	call	print_nl
		
inf_loop:
	jmp 	inf_loop

# 以16进制方式打印栈顶的16位数	
print_hex:
    	mov    $4, %cx         # 4个十六进制数字
    	mov    %ax, %dx        # 将(bp)所指的值放入dx中，如果bp是指向栈顶的话
    
print_digit:
    	rol    $4, %dx         # 循环以使低4比特用上 !! 取dx的高4比特移到低4比特处。
    	mov    $0xe0f, %ax     # ah = 请求的功能值，al = 半字节(4个比特)掩码。
    	and    %dl, %al        # 取dl的低4比特值。
    	add    $0x30, %al      # 给al数字加上十六进制0x30
    	cmp    $0x3a, %al
    	jl     outp            # 是一个不大于十的数字
    	add    $0x07, %al      # 是a～f，要多加7
    
outp:
	int    $0x10
	loop   print_digit
	ret

# 打印回车换行   
print_nl:
	mov    $0xe0d, %ax   # CR
	int    $0x10
	mov    $0xa, %al     # LF
	int    $0x10
	ret

msg2:
	.byte 13,10
	.ascii "Now we are in SETUP"
	.byte 13,10,13,10
	
Cursor:
        .ascii "Cursor POS:"

Memory:
        .ascii "Memory Size:"
        
kb:
        .ascii "KB"
        
VIDEO:
	.ascii "Video Card display mode:"  
	
HD_info:
	.byte 13,10
	.ascii "HD Info"
	.byte 13,10
	.ascii "Cylinders:"
	
HD_info1:
    .ascii "Headers:"
    
HD_info2:
    .ascii "Secotrs:"

