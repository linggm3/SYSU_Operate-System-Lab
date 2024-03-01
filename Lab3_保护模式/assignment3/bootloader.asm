%include "boot.inc"
org 0x7e00
[bits 16]

mov ax, 0xb800
mov gs, ax
mov ah, 0x03 ;青色
mov ecx, bootloader_tag_end - bootloader_tag
xor ebx, ebx
mov esi, bootloader_tag
output_bootloader_tag:
    mov al, [esi]
    mov word[gs:bx], ax
    inc esi
    add ebx,2
    loop output_bootloader_tag

bootloader_tag db 'run bootloader'
bootloader_tag_end:


;空描述符
mov dword [GDT_START_ADDRESS+0x00],0x00
mov dword [GDT_START_ADDRESS+0x04],0x00  

;创建描述符，这是一个数据段，对应0~4GB的线性地址空间
mov dword [GDT_START_ADDRESS+0x08],0x0000ffff    ; 基地址为0，段界限为0xFFFFF
mov dword [GDT_START_ADDRESS+0x0c],0x00cf9200    ; 粒度为4KB，存储器段描述符 

;建立保护模式下的堆栈段描述符      
mov dword [GDT_START_ADDRESS+0x10],0x00000000    ; 基地址为0x00000000，界限0x0 
mov dword [GDT_START_ADDRESS+0x14],0x00409600    ; 粒度为1个字节

;建立保护模式下的显存描述符   
mov dword [GDT_START_ADDRESS+0x18],0x80007fff    ; 基地址为0x000B8000，界限0x07FFF 
mov dword [GDT_START_ADDRESS+0x1c],0x0040920b    ; 粒度为字节

;创建保护模式下平坦模式代码段描述符
mov dword [GDT_START_ADDRESS+0x20],0x0000ffff    ; 基地址为0，段界限为0xFFFFF
mov dword [GDT_START_ADDRESS+0x24],0x00cf9800    ; 粒度为4kb，代码段描述符 

pgdt dw 0 
    dd GDT_START_ADDRESS

;初始化描述符表寄存器GDTR
mov word [pgdt], 39      ;描述符表的界限   
lgdt [pgdt]

; _____________Selector_____________
;平坦模式数据段选择子
DATA_SELECTOR equ 0x8
;平坦模式栈段选择子
STACK_SELECTOR equ 0x10
;平坦模式视频段选择子
VIDEO_SELECTOR equ 0x18
VIDEO_NUM equ 0x18
;平坦模式代码段选择子
CODE_SELECTOR equ 0x20

in al,0x92                         ;南桥芯片内的端口 
or al,0000_0010B
out 0x92,al                        ;打开A20

cli                                ;中断机制尚未工作
mov eax,cr0
or eax,1
mov cr0,eax                        ;设置PE位

jmp dword CODE_SELECTOR:protect_mode_begin

;16位的描述符选择子：32位偏移
;清流水线并串行化处理器
[bits 32]           
protect_mode_begin:                              

mov eax, DATA_SELECTOR                     ;加载数据段(0..4GB)选择子
mov ds, eax
mov es, eax
mov eax, STACK_SELECTOR
mov ss, eax
mov eax, VIDEO_SELECTOR
mov gs, eax
mov ecx, protect_mode_tag_end - protect_mode_tag
mov ebx, 80 * 2
mov esi, protect_mode_tag
mov ah, 0x3
output_protect_mode_tag:
    mov al, [esi]
    mov word[gs:ebx], ax
    add ebx, 2
    inc esi
    loop output_protect_mode_tag

protect_mode_tag db 'enter protect mode'
protect_mode_tag_end:


mov ecx, 0
mov dh, 2
mov dl, 0
mov ah, 0x07
mov al, 48

loop:
	call get_random
	call print_al
	call sleep
	jmp loop


; 函数功能：取随机数，打印数字的随机数，颜色的随机数
get_random: 
	inc ah
	cmp al, 57
	jge set_0

	inc al
	ret

	set_0:
		mov al, 48
	ret ; 

; 函数功能：睡眠一定时间
sleep:
	pushad
	mov ebx, 12000000 ; 设置需要延迟的时钟周期数
	delay:
	dec ebx ; 浪费1个时钟周期
	jnz delay ; 如果还有剩余时钟周期，则继续浪费
	popad
	ret ; 返回调用者

print_al:
	pushad
	mov ebx, 0
	add bl, dh
	imul ebx, ebx, 80
	and edx, 0x000000FF
	add ebx, edx
	imul ebx, ebx, 2
	mov [gs:ebx], ax
	popad
	call cursor_inc
	ret


cursor_inc:
	branch1: cmp ecx, 0 ;右下
		jne branch2
		
		b11: cmp dh, 24  ;行号24
			jne b12
			sub dh, 1
			add dl, 1
			mov ecx, 1 ;右上
			jmp end_1

		b12: cmp dl, 79 ;列号79
			jne b13
			sub dl, 1
			add dh, 1
			mov ecx, 3 ;左下
			jmp end_1
	
		b13: ;正常
			add dl, 1
			add dh, 1

		end_1:

		jmp end_if

	branch2: cmp ecx, 1 ;右上
		jne branch3

		b21: cmp dh, 0  ;行号0
			jne b22
			add dh, 1
			add dl, 1
			mov ecx, 0 ;右下
			jmp end_2

		b22: cmp dl, 79 ;列号79
			jne b23
			sub dl, 1
			sub dh, 1
			mov ecx, 2 ;左上
			jmp end_2
	
		b23: ;正常
			add dl, 1
			sub dh, 1

		end_2:

		jmp end_if

	branch3: cmp ecx, 2 ;左上
		jne branch4

		b31: cmp dh, 0  ;行号0
			jne b32
			add dh, 1
			sub dl, 1
			mov ecx, 3 ;左下
			jmp end_3

		b32: cmp dl, 0 ;列号0
			jne b33
			add dl, 1
			sub dh, 1
			mov ecx, 1 ;右上
			jmp end_3
	
		b33: ;正常
			sub dl, 1
			sub dh, 1

		end_3:		

		jmp end_if

	branch4: cmp ecx, 3 ;左下
	
		b41: cmp dh, 24  ;行号24
			jne b42
			sub dh, 1
			sub dl, 1
			mov ecx, 2 ;左上
			jmp end_4

		b42: cmp dl, 0 ;列号0
			jne b43
			add dl, 1
			add dh, 1
			mov ecx, 0 ;右下
			jmp end_4
	
		b43: ;正常
			sub dl, 1
			add dh, 1

		end_4:
	end_if:

	ret





