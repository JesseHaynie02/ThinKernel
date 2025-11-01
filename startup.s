// ARM directive to allow ARM and THUMB based instructions together
.syntax unified

// Make reset_handler globally visible so the linker can find it
.global reset_handler

reset_handler:
    ldr sp, =_stack_top

    b   main

.vector
vector_table:
    .word _stack_top
    .word reset_handler
