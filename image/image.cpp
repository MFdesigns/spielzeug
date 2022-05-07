#include <cstdint>
#include <cstdio>
#include <Windows.h>

const uint32_t IMAGE_SIZE = 1024 * 1000;

void LpoverlappedCompletionRoutine(
    uint32_t dwErrorCode, 
    uint32_t dwNumberOfBytesTransfered, 
    OVERLAPPED* lpOverlapped) {
    printf("Completed\n");
}

int main() {
    HANDLE handle = GetCurrentProcess();
    uint8_t* image = (uint8_t*)VirtualAllocEx(handle, nullptr, IMAGE_SIZE, MEM_COMMIT, PAGE_READWRITE);
    if (!image) {
        printf("Could not allocate image %lu\n", GetLastError());
        return 1;
    }

    HANDLE file = CreateFileW(
        L"spielzeug.iso",
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_FLAG_OVERLAPPED,
        nullptr
    );

    OVERLAPPED overlapped{};
    overlapped.hEvent = CreateEventExW(
        nullptr,
        nullptr,
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
