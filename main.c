// arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -nostdlib -nostartfiles -ffreestanding -Os -Wl,--gc-sections,-Map=output.map -T stm32f303k8.ld startup.s main.c -o main.elf

void main()
{
    while(1) {};
}