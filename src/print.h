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

typedef void (*PutCharFunc) (void* instance, char c);

void* globalPrintInstance = 0;
PutCharFunc globalPutChar = 0;

void formatInit(void* instance, PutCharFunc func) {
    globalPrintInstance = instance;
    globalPutChar = func;
}

void fmtUintToString(u64 num) {
    char buffer[INT64_MAX_DIGITS + 1]; // Plus null terminator
    setMemory((u8*)buffer, (INT64_MAX_DIGITS + 1), 0);

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
    for (u32 i = 0; i < digitCount; i++) {
        globalPutChar(globalPrintInstance, (char)buffer[firstDigit + i]);
    }
}

void fmtUintToHexString(u64 num, bool upperCase) {
    char hexTableUpper[16] = {
        '0', '1', '2', '3',
        '4', '5', '6', '7',
        '8', '9', 'A', 'B',
        'C', 'D', 'E', 'F'
    };

    char hexTableLower[16] = {
        '0', '1', '2', '3',
        '4', '5', '6', '7',
        '8', '9', 'a', 'b',
        'c', 'd', 'e', 'f'
    };

    char* hexTable = hexTableLower;
    if (upperCase) {
        hexTable = hexTableUpper;
    }

    if (num == 0) {
        globalPutChar(globalPrintInstance, '0');
    } else {

        u64 remainder = num % 16;
        while (num != 0) {
            u64 quotient = num / 16;
            globalPutChar(globalPrintInstance, hexTable[remainder]);
            num = quotient;
            remainder = num % 16;
        }
    }
}

enum FormatArgType {
    ARG_U8,
    ARG_U16,
    ARG_U32,
    ARG_U64
};

struct FormatArg {
    u64 type;
    u64 data;
};

// NOTE: not really sure why we need to do this but if we dont then
// __COUNTER__ will not get replaced...
#define internalFormatMacro2(str, counter, ...) \
    const char tmp_fmt_str_ ## counter[] = str; \
    struct FormatArg tmp_fmt_args_ ## counter [] = { __VA_ARGS__ }; \
    internalPrintFormat(tmp_fmt_str_ ## counter, sizeof(tmp_fmt_str_ ## counter), tmp_fmt_args_ ## counter, sizeof(tmp_fmt_args_ ## counter) / sizeof(tmp_fmt_args_ ## counter[0]))

#define internalFormatMacro(str, counter, ...) \
    internalFormatMacro2(str, counter, __VA_ARGS__)

#define format(str, ...) \
    internalFormatMacro(str, __COUNTER__, __VA_ARGS__)

#define fmt(var) { _Generic(var, u8: ARG_U8, u16: ARG_U16, u32: ARG_U32, u64: ARG_U64), var }

struct FormatStringParser {
    const char* string;
    u32 stringSize;
    u32 cursor;
};

static char fmtPeek(struct FormatStringParser* parser) {
    if (parser->cursor >= parser->stringSize) {
        return 0;
    }
    return parser->string[parser->cursor];
}

static char fmtConsume(struct FormatStringParser* parser) {
    if (parser->cursor + 1 >= parser->stringSize) {
        return 0;
    }

    char c = parser->string[parser->cursor];
    parser->cursor += 1;
    return c;
}

#define FMT_SPEC_DECIMAL        0
#define FMT_SPEC_HEX_UPPERCASE  1
#define FMT_SPEC_HEX_LOWERCASE  2

struct FormatSpecifier {
    u32 flags;
};

void fmtParseSpecifier(
    struct FormatStringParser* parser, 
    struct FormatSpecifier* outSpecifier
) {
    char c = fmtConsume(parser);
    while (c != 0 && c != '}') {
        switch (c) {
        case 'X': outSpecifier->flags |= FMT_SPEC_HEX_UPPERCASE; break;
        case 'x': outSpecifier->flags |= FMT_SPEC_HEX_LOWERCASE; break;
        default: break;
        }
        c = fmtConsume(parser);
    }
}

void internalPrintFormat(const char* format, u32 formatSize, struct FormatArg* args, u32 argCount) {
    struct FormatStringParser parser = {
        .string = format,
        .stringSize = formatSize,
        .cursor = 0,
    };

    u32 argIndex = 0;

    char c = fmtConsume(&parser);
    while (c) {
        if (c == '{') {
            struct FormatSpecifier specifier = {
                .flags = 0
            };
            fmtParseSpecifier(&parser, &specifier);

            if (argIndex < argCount) {
                struct FormatArg* arg = &args[argIndex];
                u64 data = 0;
                switch (specifier.flags) {
                case FMT_SPEC_DECIMAL:
                    fmtUintToString(arg->data);
                    break;
                case FMT_SPEC_HEX_UPPERCASE:
                    fmtUintToHexString(arg->data, true);
                    break;
                case FMT_SPEC_HEX_LOWERCASE:
                    fmtUintToHexString(arg->data, false);
                    break;
                }

                argIndex++;
            }
        } else {
            globalPutChar(globalPrintInstance, c);
        }

        c = fmtConsume(&parser);
    }
}
