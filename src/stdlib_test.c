#include "stdio.h"
// stdlib headers
#include "print.h"
#include "types.h"

int main() {
    u64 num1 = 1;
    u64 num2 = 1234567890;
    u64 num3 = 0xFFFFFFFFFFFFFFFF;

    char16 output[256] = {};
    uintToString(output, num1);
    printf("Num %llu: '%ls'\n", num1, output);

    uintToString(output, num2);
    printf("Num %llu: '%ls'\n", num2, output);

    uintToString(output, num3);
    printf("Num %llu: '%ls'\n", num3, output);
}
