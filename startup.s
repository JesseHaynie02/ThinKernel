// ARM directive to allow ARM and THUMB based instructions together
.syntax unified

// Make reset_handler globally visible so the linker can find it
.global reset_handler

.global _sdata
.global _edata
.global _ldata
.global _sbss
.global _ebss

.section    .text.reset_handler, "ax", %progbits
.type       reset_handler, %function

reset_handler:
    ldr sp, =_stack_top
    b   copy_data_to_ram

copy_data_to_ram:
    ldr r0, =_ldata
    ldr r1, =_sdata
    ldr r2, =_edata

copy_data_to_ram_loop:
    cmp r1, r2
    bge zero_initialize_bss
    ldr r3, [r0]
    str r3, [r1]
    add r0, #0x4
    add r1, #0x4
    b copy_data_to_ram_loop

zero_initialize_bss:
    ldr r0, =_sbss
    ldr r1, =_ebss
    mov r2, #0x0

zero_initialize_bss_loop:
    cmp r0, r1
    bge main
    str r2, [r0]
    add r0, #0x4
    b zero_initialize_bss_loop

.section .vector, "a"
vector_table:
    .word _stack_top
    .word reset_handler
    .word nmi_handler
    .word hardfault_manager
    .word memmanage_handler
    .word busfault_handler
    .word usagefault_handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word svc_handler
    .word debugmon_handler
    .word 0
    .word pendsv_handler
    .word systick_handler
    .word irq_handler

.section .text.default_handlers, "ax", %progbits

.weak systick_handler

nmi_handler:
hardfault_manager:
memmanage_handler:
busfault_handler:
usagefault_handler:
svc_handler:
debugmon_handler:
pendsv_handler:
systick_handler:
irq_handler:
default_handler:
    b default_handler
