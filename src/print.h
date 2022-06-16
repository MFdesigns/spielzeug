#pragma once
#include "types.h"

#define INT64_MAX_TENTH     10000000000000000000ull
#define INT64_MAX_DIGITS    20

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

    char16 buffer[INT64_MAX_DIGITS + 3]; // Plus null terminator and new line
    setMemory((u8*)buffer, INT64_MAX_DIGITS * charSize, 0);

    buffer[INT64_MAX_DIGITS] = '\r';
    buffer[INT64_MAX_DIGITS + 1] = '\n';

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
    u32 digitCount = INT64_MAX_DIGITS - firstDigit;
    
    copyMemory((u8*)destination, (u8*)&buffer[firstDigit], digitCount * charSize);
}
