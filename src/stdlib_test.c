// stdlib headers
#include "print.h"
#include "types.h"
#include "stdio.h"

void putChar(void* instance, char c) {
    printf("%c", c);
}

int main() {
    formatInit(0, putChar);

    format("Num 1: {}\n", fmt((u32)1));
    format("Num 0xFF: 0x{X}\n", fmt((u8)255));
    format("Num 0xff: 0x{x}\n", fmt((u8)255));
}
