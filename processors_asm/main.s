# gcc -no-pie -fno-PIE main.s
.intel_syntax noprefix
.global main
.text

check:
	sub rsp, 8	
	mov rsi, rdi
	mov rdi, offset arr

cycle:
	mov al, [rdi]
	cmp al, 0
	jz cycle_exit
	xor al, 37
	mov [rdi], al
	inc rdi
	jmp cycle
	
cycle_exit:
	mov rdi, rsi
	mov rsi, offset arr
	call strcmp
	add rsp, 8
	ret

main:
	sub rsp, 8
	
	mov rdi, offset welcome
	call printf
	
	mov rdi, offset buffer
	call gets
		
	mov rdi, offset buffer
	call check
	cmp rax, 0
	
	jnz fail
	mov rdi, offset youwon
	call puts
	jmp finish

fail:
	mov rdi, offset youlose
	call puts
	jmp finish

finish:
	add rsp, 8
	ret

.data

.lcomm buffer, 4096

welcome:
	.asciz "Hello, please enter key: "

arr:
	.asciz "kvp^ktsDPIQ\vDVHX"

youwon:
	.asciz "Correct key, congratulations!"

youlose:
	.asciz "Wrong key"
