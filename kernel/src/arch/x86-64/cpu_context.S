.intel_syntax noprefix
.global save_cpu_context
.type   save_cpu_context, @function
save_cpu_context:
    # Save callee-saved GPRs (ABI requires caller to save the others)
    mov [rdi +  0], r15
    mov [rdi +  8], r14
    mov [rdi + 16], r13
    mov [rdi + 24], r12
    mov [rdi + 32], rbx
    mov [rdi + 40], rbp

    # Save stack and the "resume RIP" for the double-return
    mov [rdi + 48], rsp
    lea rax, [rip + .Lresume_inside_save]   # address inside this function
    mov [rdi + 56], rax

    # Save flags
    pushfq
    pop  qword ptr [rdi + 64]

    # First return (the save path)
    xor eax, eax    # Set return value in the rax register as 0
                    # (rax is the 64-bit extension of the eax register, writing
                    # to eax automatically zeroes out the upper 32-bits) 
.Lresume_inside_save:
    ret
.size save_cpu_context, .-save_cpu_context

.global restore_cpu_context
.type   restore_cpu_context, @function
restore_cpu_context:              # rdi = &CpuContext
    mov     rdx, rdi              # keep ctx base in rdx; don't clobber it

    # Restore callee-saved first (as the caller expects)
    mov r15, [rdx +  0]
    mov r14, [rdx +  8]
    mov r13, [rdx + 16]
    mov r12, [rdx + 24]
    mov rbx, [rdx + 32]
    mov rbp, [rdx + 40]

    # Restore stack
    mov rsp, [rdx + 48]

    # Restore flags
    mov rax, [rdx + 64]
    push rax
    popfq

    # Set the "second return" value
    mov eax, 1

    # Jump to the saved resume RIP (which is *inside* save_cpu_context)
    jmp qword ptr [rdx + 56]
.size restore_cpu_context, .-restore_cpu_context
