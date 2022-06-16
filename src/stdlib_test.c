#include "stdio.h"
// stdlib headers
#include "print.h"
#include "types.h"

int main() {
    u64 num1 = 2;
    u64 num2 = 1234567890;
    u64 num3 = 0xFFFFFFFFFFFFFFFF;

    char16 output[256];

    // Decimal numbers
    uintToString(output, 0);
    printf("Num %llu: '%ls'\n", 0ULL, output);

    uintToString(output, num1);
    printf("Num %llu: '%ls'\n", num1, output);

    uintToString(output, num2);
    printf("Num %llu: '%ls'\n", num2, output);

    uintToString(output, num3);
    printf("Num %llu: '%ls'\n", num3, output);

    // Hex numbers
    uintToHexString(output, 0);
    printf("Num %llx: '%ls'\n", 0ULL, output);

    uintToHexString(output, num1);
    printf("Num %llx: '%ls'\n", num1, output);

    uintToHexString(output, num2);
    printf("Num %llx: '%ls'\n", num2, output);

    uintToHexString(output, num3);
    printf("Num %llx: '%ls'\n", num3, output);
}
