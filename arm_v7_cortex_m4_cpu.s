
// Interrupt handler for system level service used for context switching.
// pendsv should be set to the lowest priority interrupt.

pendsv_handler:
    // Disable all interrupts except non-maskable interrupt (NMI) and hardfault
    CPSID   i

    // Load program stack pointer into (PSP)

    // Check to see if PSP is 0, if so we don't need to save off the current context
    // because there is no context to save and we can branch to loading the next
    // highest priority task ready to run, in this case the first task to run.

    // Current stack pointer in PSP needs to grow to make room for registers
    // Entry into an exception pushes an exception frame onto the main stack pointer (MSP)
    // containing R0–R3, R12, LR, PC, xPSR we only need to save off the rest of the registers
    // that didn't get saved in the excpetion frame.
    // R4-11 + LR(EXEC_RETURN) = 9 registers * 32-bit = 0x24 (36) bytes

    // Push R4-R11 onto the current stack

    // Push LR (R14) onto stack

