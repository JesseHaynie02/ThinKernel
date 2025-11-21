.syntax unified
.thumb

.global pendsv_handler
.type pendsv_handler, %function

// Interrupt handler for system level service used for context switching
// pendsv should be set to the lowest priority interrupt

pendsv_handler:
    // Disable all interrupts except non-maskable interrupt (NMI) and hardfault
    cpsid   i

    // Load program stack pointer into (PSP)
    mrs     r0, psp

    // Check to see if PSP is 0, if so we don't need to save off the current context
    // because there is no context to save and we can branch to loading the next
    // highest priority task ready to run, in this case the first task to run
    cmp     r0, #0x0
    beq     load_next_task

save_curr_task:
    // Current stack pointer in PSP needs to grow to make room for registers
    // Entry into an exception pushes an exception frame onto the main stack pointer (MSP)
    // containing R0–R3, R12, LR, PC, xPSR we only need to save off the rest of the registers
    // that didn't get saved in the excpetion frame
    // R4-11 + LR(EXEC_RETURN) = 9 registers * 32-bit = 0x24 (36) bytes
    subs    r0, #0x24

    // Save off stack pointer into task control block where the first address
    // of the TCB should be the stack pointer
    ldr     r1, =curr_task_ptr
    ldr     r1, [r1]
    str     r0, [r1]

    // Push R4-R11 onto the current stack
    stmia   r0!, {r4-r7}
    mov     r4, r8
    mov     r5, r9
    mov     r6, r10
    mov     r7, r11
    stmia   r0!, {r4-r7}

    // Push LR (R14) onto stack
    mov     r3, r14
    stmia   r0!, {r3}

load_next_task:
    // Get the next highest priority task from linked list of ready to run tasks
    ldr     r0, =curr_task_ptr
    ldr     r1, =highest_prio_task_ptr
    ldr     r2, [r1]
    str     r2, [r0]

    ldr     r0, [r2]

    // Reload R8-R11 and LR from the stack pointer into their respective registers
    adds    r0, #0x10
    ldmfd   r0!, {r4-r7}
    mov     r8, r4
    mov     r9, r5
    mov     r10, r6
    mov     r11, r7

    ldmfd   r0!, {r3}
    mov     r14, r3

    // Store stack pointer back into PSP
    msr     psp, r0

    // Decrease stack size by 0x24 (36) bytes
    subs    r0, #0x24

    // Reload R4-R7 from the stack pointer into their respective registers
    ldmfd   r0!, {r4-r7}

    // Reenable interrupts and return
    cpsie   i
    bx      lr
