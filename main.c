// arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -g -nostdlib -nostartfiles -ffreestanding -Os -Wl,-Map=output.map -T stm32f303k8.ld startup.s main.c

void main()
{
    while(1) {};
}
