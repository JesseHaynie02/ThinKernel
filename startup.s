// ARM directive to allow ARM and THUMB based instructions together
.syntax unified

// Make reset_handler globally visible so the linker can find it
.global reset_handler

.global _sdata
.global _edata
.global _ldata
.global _sbss
.global _ebss

reset_handler:
    ldr sp, =_stack_top
    b   copy_data_to_ram
    b   main

copy_data_to_ram:
    ldr r0, =_ldata
    ldr r1, =_sdata
    ldr r2, =_edata
    loop:
        cmp r1, r2
        bge zero_initialize_bss
        ldr r3, [r0]
        str r3, [r1]
        add r0, #0x4
        add r1, #0x4
        b loop

zero_initialize_bss:
    nop

.section .vector, "a"
vector_table:
    .word _stack_top
    .word reset_handler
