

milkaddr	equ		0x00100000

cyls	equ		0x0ff0
leds	equ		0x0ff1
vmode	equ		0x0ff2
scrnx	equ		0x0ff4
scrny	equ		0x0ff6
vram	equ		0x0ff8

		org		0xbe00

		mov		al,0x03
		mov		ah,0x00
		int		0x10

		mov		al,0xff
		out		0x21,al
		nop
		out		0xa1,al

		cli

		call wait_keyboard
        mov	al,0xd1
        out	0x64,al
        call wait_keyboard
        mov	al,0xdf			; enable a20
        out	0x60,al
        call wait_keyboard

		lgdt [gdtr0]

        mov eax,cr0
        and eax,0x7fffffff
        or	eax,1
        mov	cr0,eax
		jmp	pipelineflush
pipelineflush:
		mov		ax,1*8
		mov		ds,ax
		mov		es,ax
		mov		fs,ax
		mov		gs,ax
		mov		ss,ax

		mov		esi,milk
		add		esi,0x1000
		mov		edi,milkaddr
		mov		ecx,512*1024/4
		call	memcpy
skip:
		jmp		dword 2*8:0x00101b30

wait_keyboard:
		in		 al,0x64
		and		 al,0x02
		jnz		wait_keyboard
		ret

memcpy:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		sub		ecx,1
		jnz		memcpy
		ret

		align	16
gdt0:
		resb	8
		dw		0xffff,0x0000,0x9200,0x00cf
		dw		0xffff,0x0000,0x9a00,0x00cf

		dw		0
gdtr0:
		dw		8*3-1
		dd		gdt0
        resb 512 - ($ - $$)
		align	16
milk:
