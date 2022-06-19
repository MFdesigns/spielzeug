#define UNICODE

#include "image.h"
#include "fat.h"
#include "gpt.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <wchar.h>
#include <stdbool.h>

#define LOGICAL_BLOCK_SIZE          512
#define IMAGE_SIZE                  51200000

void fatFillDateTime(struct FatDirectoryEntry* dir) {
    SYSTEMTIME time;
    GetSystemTime(&time);

    const uint32_t MS_DOS_EPOCH     = 1980;
    const uint32_t DATE_MONTH_MASK  = 0b0000000111100000;
    const uint32_t DATE_DAY_MASK    = 0b0000000000011111;

    uint16_t msDosYear = (time.wYear - MS_DOS_EPOCH) << 9;
    uint16_t msDosMonth = (time.wMonth << 5) & DATE_MONTH_MASK;
    uint16_t msDosDay = time.wDay & DATE_DAY_MASK;
    uint16_t msDosDate = msDosDay | msDosMonth | msDosYear;

    const uint32_t TIME_MINUTE_MASK = 0b0000011111100000;
    const uint32_t TIME_SECOND_MASK = 0b0000000000011111;

    uint16_t msDosHour = time.wHour << 11;
    uint16_t msDosMinute = (time.wMinute << 5) & TIME_MINUTE_MASK;
    uint16_t msDosSecond = (time.wSecond / 2) & TIME_SECOND_MASK;
    uint16_t msDosTime = msDosSecond | msDosMinute | msDosHour;

    dir->creationTimeTenth = time.wSecond % 200;
    dir->creationTime = msDosTime;
    dir->creationDate = msDosDate;
    dir->lastAccessDate = msDosDate;
    dir->writeDate = msDosDate;
    dir->writeTime = msDosTime;
}

void LpoverlappedCompletionRoutine(
    uint32_t dwErrorCode, 
    uint32_t dwNumberOfBytesTransfered, 
    OVERLAPPED* lpOverlapped) {
    printf("Completed\n");
}

void toChar16(char* string, char16_t* out) {
    uint32_t strSize = strlen(string);
    for (uint32_t i = 0; i < strSize; i++) {
        out[i] = string[i];
    }
}

int main(int32_t argc, char** argv) {
    if (argc < 2) {
        printf("usage: <working directory>");
        return 1;
    }

    // TODO: not very safe...
    uint32_t workingDirSize = strlen(argv[1]);
    char16_t workingDir[64] = {};
    toChar16(argv[1], workingDir);

    char16_t efiAppPath[64] = {};
    char16_t efiFileName[] = u"BOOTX64.EFI";
    char16_t efiFileNameSize = wcslen(efiFileName) * sizeof(char16_t);

    memcpy(efiAppPath, workingDir, workingDirSize * sizeof(char16_t));
    memcpy(&efiAppPath[workingDirSize], efiFileName, efiFileNameSize);

    char16_t isoFilePath[64] = {};
    char16_t isoFileName[] = u"spielzeug.iso";
    char16_t isoFileNameSize = wcslen(isoFileName) * sizeof(char16_t);

    memcpy(isoFilePath, workingDir, workingDirSize * sizeof(char16_t));
    memcpy(&isoFilePath[workingDirSize], isoFileName, isoFileNameSize);

    HANDLE handle = GetCurrentProcess();
    uint8_t* image = (uint8_t*)VirtualAllocEx(handle, NULL, IMAGE_SIZE, MEM_COMMIT, PAGE_READWRITE);
    if (!image) {
        printf("Could not allocate image %lu\n", GetLastError());
        return 1;
    }

    struct GptVolume gptVolume = {};
    createGpt(&gptVolume, image, IMAGE_SIZE, LOGICAL_BLOCK_SIZE);

    // Minimal size for a valid FAT32 volume
    // TODO: move this check into partition creation
    uint32_t partitionSize = 66562 * 512;
    char16_t partitionName[] = u"Boot Partition";
    uint32_t partitionNameSize = sizeof(partitionName);

    uint32_t actualSize = 0;
    uint8_t* efiPartition = NULL;
    gptAllocateEfiPartition(
        &gptVolume, 
        partitionSize, 
        partitionName,
        partitionNameSize,
        &efiPartition,
        &actualSize
    );

    struct FatVolume fatVolume = {};
    createFat32(&fatVolume, efiPartition, LOGICAL_BLOCK_SIZE);

    fatCreateDirectory(&fatVolume, "EFI");
    fatCreateDirectory(&fatVolume, "/EFI/BOOT/");

    HANDLE efiFile = CreateFileW(
        efiAppPath,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    uint32_t efiFileSize = GetFileSize(efiFile, NULL);

    uint8_t* efiFileBuffer = VirtualAllocEx(
        GetCurrentProcess(),
        NULL,
        efiFileSize,
        MEM_COMMIT,
        PAGE_READWRITE
    );

    BOOL readRes = ReadFile(
        efiFile,
        efiFileBuffer,
        efiFileSize,
        NULL,
        NULL
    );

    fatCreateFile(&fatVolume, efiFileBuffer, efiFileSize, "/EFI/BOOT/BOOTX64.EFI");

    VirtualFree(efiFileBuffer, 0, MEM_RELEASE);
    CloseHandle(efiFile);

    HANDLE file = CreateFileW(
        isoFilePath,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_FLAG_OVERLAPPED,
        NULL
    );

    OVERLAPPED overlapped = {};
    overlapped.hEvent = CreateEventExW(
        NULL,
        NULL,
        0,
        SYNCHRONIZE
    );

    LPOVERLAPPED_COMPLETION_ROUTINE writeCallback = 
        (LPOVERLAPPED_COMPLETION_ROUTINE)&LpoverlappedCompletionRoutine;
    int32_t writeRes = WriteFileEx(
        file,
        image,
        IMAGE_SIZE,
        &overlapped,
        writeCallback
    );

    if (!writeRes) {
        printf("Error async write file %lu\n", GetLastError());
    }

    uint32_t waitRes = WaitForSingleObjectEx(overlapped.hEvent, 5000, true);
    if (waitRes == WAIT_FAILED) {
        printf("Wait failed %x %lu\n", waitRes, GetLastError());
    }

    CloseHandle(file);
    VirtualFreeEx(handle, image, 0, MEM_RELEASE);
}
