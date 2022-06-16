#pragma once
#include "types.h"

#define INT64_MAX_TENTH         10000000000000000000ull
#define INT64_MAX_DIGITS        20
#define INT64_MAX_HEX_DIGITS    16

void setMemory(u8* dest, u64 size, u8 fill) {
    for (u64 i = 0; i < size; i++) {
        dest[i] = fill;
    }
}

void copyMemory(u8* dest, u8* src, u32 size) {
    for (u32 i = 0; i < size; i++) {
        dest[i] = src[i];
    }
}

void uintToString(char16 destination[], u64 num) {
    const u32 charSize = sizeof(destination[0]);

    char16 buffer[INT64_MAX_DIGITS + 1]; // Plus null terminator
    setMemory((u8*)buffer, (INT64_MAX_DIGITS + 1) * charSize, 0);

    s32 firstDigit = -1;

    u64 mod = INT64_MAX_TENTH;
    for (u32 i = 0; i < INT64_MAX_DIGITS; i++) {
        u64 digit = num / mod;

        if (digit != 0 && firstDigit < 0) {
            firstDigit = i;
        }

        buffer[i] = digit + 0x30;

        num %= mod;
        mod /= 10;
    }

    if (firstDigit == -1) {
        firstDigit = INT64_MAX_DIGITS - 1;
    }
    
    u32 digitCount = (INT64_MAX_DIGITS + 1) - firstDigit;
    
    copyMemory((u8*)destination, (u8*)&buffer[firstDigit], digitCount * charSize);
}

void uintToHexString(char16 destination[], u64 num) {
    u32 charSize = sizeof(destination[0]);

    char16 buffer[INT64_MAX_HEX_DIGITS + 1];
    u32 bufferSize = (INT64_MAX_HEX_DIGITS + 1) * sizeof(buffer[0]);
    setMemory((u8*)buffer, bufferSize, 0);

    u32 outIndex = INT64_MAX_HEX_DIGITS - 1;

    if (num == 0) {
        buffer[outIndex] = '0';
        outIndex--;
    } else {
        char hexUnicodeTable[16] = {
        '0', '1', '2', '3',
        '4', '5', '6', '7',
        '8', '9', 'A', 'B',
        'C', 'D', 'E', 'F'
        };

        u64 remainder = num % 16;
        while (num != 0) {
            u64 quotient = num / 16;
            buffer[outIndex] = hexUnicodeTable[remainder];
            num = quotient;
            remainder = num % 16;
            outIndex--;
        }
    }

    u32 stringSizeBytes = bufferSize - (outIndex + 1) * charSize;
    copyMemory((u8*)destination, (u8*)&buffer[outIndex + 1], stringSizeBytes);
}
