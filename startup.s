
reset_handler:
    ldr sp, =reset_location // Load stack pointer register with reset address

.data
reset_location: .word 0x08000000