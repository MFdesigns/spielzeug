#include <cstdint>
#include <cstdio>
#include <Windows.h>
#include <cwchar>

const uint32_t IMAGE_SIZE = 1024 * 1000;
const uint64_t EFI_PART_MAGIC = 0x5452415020494645; // "EFI PART"
const uint32_t GPT_HEADER_VERSION = 0x00010000; // Version 1.0
const uint32_t GPT_PARTITION_ENTRY_SIZE = 128;

static uint32_t CRC32_LOOKUP[256] = {
    0, 0x77073096, 0xEE0E612C, 0x990951BA,
    0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
    0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
    0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
    0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
    0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
    0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
    0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
    0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
    0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
    0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
    0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
    0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
    0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
    0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
    0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
    0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
    0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
    0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
    0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
    0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
    0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

struct MbrPartitionRecord {
    uint8_t bootIndicator;
    uint8_t startingChs[3];
    uint8_t osType;
    uint8_t endingChs[3];
    uint32_t startingLba;
    uint32_t sizeInLba;
};

struct Guid {
    uint32_t data1;
    uint16_t data2;
    uint16_t data3;
    uint8_t data4[8];
};

struct GptHeader {
    uint64_t signature;
    uint32_t revision;
    uint32_t headerSize;
    uint32_t headerCrc32;
    uint32_t reserved;
    uint64_t myLba;
    uint64_t alternateLba;
    uint64_t firstUsableLba;
    uint64_t lastUsableLba;
    Guid diskGuid;
    uint64_t partitionEntryLba;
    uint32_t numberOfPartitionEntries;
    uint32_t sizeOfPartitionEntry;
    uint32_t partitionEntryArrayCrc32;
};

struct GptPartitionEntry {
    Guid partitionTypeGuid;
    Guid uniquePartitionGuid;
    uint64_t startingLba;
    uint64_t endingLba;
    uint64_t attributes;
    // Followed by a null-terminated char16_t string up to 
    // 72 bytes long
};

uint32_t crc32(uint8_t* data, uint32_t size) {
    uint32_t crc = 0xFFFFFFFF;

    for (uint32_t i = 0; i < size; i++) {
        crc = CRC32_LOOKUP[(uint8_t)crc ^ data[i]] ^ (crc >> 8);
    }
    return ~crc;
}

void LpoverlappedCompletionRoutine(
    uint32_t dwErrorCode, 
    uint32_t dwNumberOfBytesTransfered, 
    OVERLAPPED* lpOverlapped) {
    printf("Completed\n");
}

void createGptPartitionTable(uint8_t* dest, 
                             uint32_t lba, 
                             uint32_t alternateLba,
                             uint32_t firstLba,
                             uint32_t lastLba,
                             uint32_t partitionArrayLba,
                             uint32_t partitionArrayCrc) {

    GptHeader* gptHeader = (GptHeader*)dest;
    gptHeader->signature = EFI_PART_MAGIC;
    gptHeader->revision = GPT_HEADER_VERSION;
    gptHeader->headerSize = sizeof(GptHeader);
    gptHeader->headerCrc32 = 0;
    gptHeader->reserved = 0;
    gptHeader->myLba = lba;
    gptHeader->alternateLba = alternateLba;
    gptHeader->firstUsableLba = firstLba;
    gptHeader->lastUsableLba = lastLba;
    gptHeader->diskGuid.data1 = 0xAAAAAAAA;
    gptHeader->diskGuid.data2 = 0xBBBB;
    gptHeader->diskGuid.data3 = 0xCCCC;
    gptHeader->diskGuid.data4[0] = 0xEE;
    gptHeader->diskGuid.data4[1] = 0xEE;
    gptHeader->diskGuid.data4[2] = 0xEE;
    gptHeader->diskGuid.data4[3] = 0xEE;
    gptHeader->diskGuid.data4[4] = 0xEE;
    gptHeader->diskGuid.data4[5] = 0xEE;
    gptHeader->diskGuid.data4[6] = 0xEE;
    gptHeader->diskGuid.data4[7] = 0xEE;
    gptHeader->partitionEntryLba = partitionArrayLba;
    gptHeader->numberOfPartitionEntries = 1;
    gptHeader->sizeOfPartitionEntry = GPT_PARTITION_ENTRY_SIZE;
    gptHeader->partitionEntryArrayCrc32 = partitionArrayCrc;

    gptHeader->headerCrc32 = crc32(dest, gptHeader->headerSize);
}

uint32_t createGptPartitionArray(uint8_t* dest) {
    GptPartitionEntry* partEntry = (GptPartitionEntry*)dest;
    // GUID EFI System Partition
    partEntry->partitionTypeGuid.data1 = 0xC12A7328;
    partEntry->partitionTypeGuid.data2 = 0xF81F;
    partEntry->partitionTypeGuid.data3 = 0x11D2;
    partEntry->partitionTypeGuid.data4[0] = 0xBA;
    partEntry->partitionTypeGuid.data4[1] = 0x4B;
    partEntry->partitionTypeGuid.data4[2] = 0x00;
    partEntry->partitionTypeGuid.data4[3] = 0xA0;
    partEntry->partitionTypeGuid.data4[4] = 0xC9;
    partEntry->partitionTypeGuid.data4[5] = 0x3E;
    partEntry->partitionTypeGuid.data4[6] = 0xC9;
    partEntry->partitionTypeGuid.data4[7] = 0x3B;

    partEntry->uniquePartitionGuid.data1 = 0xAAAAAAAA;
    partEntry->uniquePartitionGuid.data2 = 0xF81F;
    partEntry->uniquePartitionGuid.data3 = 0x11D2;
    partEntry->uniquePartitionGuid.data4[0] = 0xBA;
    partEntry->uniquePartitionGuid.data4[1] = 0x4B;
    partEntry->uniquePartitionGuid.data4[2] = 0x00;
    partEntry->uniquePartitionGuid.data4[3] = 0xA0;
    partEntry->uniquePartitionGuid.data4[4] = 0xC9;
    partEntry->uniquePartitionGuid.data4[5] = 0x3E;
    partEntry->uniquePartitionGuid.data4[6] = 0xC9;
    partEntry->uniquePartitionGuid.data4[7] = 0x3B;
    // Minimum lba 32 x 512 + lba 0 and lba 1; Maybe this should be 2048?
    partEntry->startingLba = 34;
    partEntry->endingLba = 35;
    partEntry->attributes = 0;

    // Partition name located at the end of GPT Partition Entry
    const wchar_t* PARTITION_NAME = L"Boot Partition";
    wchar_t* partitionName = (wchar_t*)((uint8_t*)partEntry + sizeof(GptPartitionEntry));
    // Get partition name string in bytes plus null-terminator
    uint32_t partitionNameSize = wcsnlen_s(PARTITION_NAME, 32) * sizeof(wchar_t) + sizeof(wchar_t);
    memcpy(partitionName, PARTITION_NAME, partitionNameSize);

    const uint32_t ENTRY_COUNT = 1;
    uint32_t crc = crc32(dest, ENTRY_COUNT * GPT_PARTITION_ENTRY_SIZE);
    return crc;
}

void createGpt(uint8_t* image) {
    const uint32_t LOGICAL_BLOCK_SIZE = 512;
    const uint32_t PARTITION_RECORD_OFFSET = 446;
    const uint32_t MBR_SIGNATURE_OFFSET = 510;
    const uint8_t OS_TYPE_GPT_PROTECTIVE = 0xEE;
    const uint64_t LBA_1 = 1;
    const uint16_t MBR_SIGNATURE = 0xAA55;

    // Disk layout:
    // lba 0:   Protective MBR
    // lba 1:   Primary GPT Partition Table
    // lba 2:   GPT Partition Array (must be atleast 32x512 big; which means 32 lba blocks)
    // lba 34:  First Usable LBA
    // ...      Actuall Partition Content
    // lba x:   Last Usable LBA
    // lba n-2: Alternate GPT Partition Array (32x512)
    // lba n-1: Alternate GPT Partition Table
    
    const uint32_t GPT_PARTITION_ARRAY_SIZE = 32;
    const uint32_t PARTITION_SIZE_LBA = 2;
    
    const uint32_t LBA_PRIMARY_GPT = 1;
    const uint32_t LBA_GPT_PARTITION_ARRAY = 2;
    const uint32_t LBA_FIRST_USABLE = LBA_GPT_PARTITION_ARRAY + GPT_PARTITION_ARRAY_SIZE;
    const uint32_t LBA_LAST_USABLE = LBA_FIRST_USABLE + PARTITION_SIZE_LBA;
    const uint32_t LBA_ALTERNATE_GPT_PARTITION_ARRAY = LBA_LAST_USABLE + 1;
    const uint32_t LBA_ALTERNATE_GPT = LBA_ALTERNATE_GPT_PARTITION_ARRAY + GPT_PARTITION_ARRAY_SIZE;
    

    // Protective MBR

    MbrPartitionRecord* partRecord = (MbrPartitionRecord*)&image[PARTITION_RECORD_OFFSET];
    // Set to non-bootable disk; Ignored on UEFI systems
    partRecord->bootIndicator = 0;
    partRecord->startingChs[0] = 0;
    partRecord->startingChs[1] = 2;
    partRecord->startingChs[2] = 0;
    partRecord->osType = OS_TYPE_GPT_PROTECTIVE;
    // TODO: fill this with the index of the last logical block
    partRecord->endingChs[0] = 0xFF;
    partRecord->endingChs[1] = 0xFF;
    partRecord->endingChs[2] = 0xFF;
    // Set to LBA of GPT partition header
    partRecord->startingLba = 1;
    // Size of disk minus one
    partRecord->sizeInLba = IMAGE_SIZE - 1;

    uint16_t* mbrSignature = (uint16_t*)&image[MBR_SIGNATURE_OFFSET];
    *mbrSignature = MBR_SIGNATURE;

    uint8_t* lbaPrimaryPartitionArray = 
        (uint8_t*)(image + LOGICAL_BLOCK_SIZE * LBA_GPT_PARTITION_ARRAY);
    uint8_t* lbaAlternatePartitionArray = 
        (uint8_t*)(image + LOGICAL_BLOCK_SIZE * LBA_ALTERNATE_GPT_PARTITION_ARRAY);

    uint32_t primaryPartitionArrayCrc = createGptPartitionArray(lbaPrimaryPartitionArray);
    uint32_t alternatePartitionArrayCrc = createGptPartitionArray(lbaAlternatePartitionArray);

    // GUID Partition Table

    uint8_t* lba1 = (uint8_t*)(image + LOGICAL_BLOCK_SIZE);
    createGptPartitionTable(
        lba1, 
        LBA_PRIMARY_GPT, 
        LBA_ALTERNATE_GPT, 
        LBA_FIRST_USABLE, 
        LBA_LAST_USABLE,
        LBA_GPT_PARTITION_ARRAY,
        primaryPartitionArrayCrc
    );

    uint8_t* lbaAlternateGpt = (uint8_t*)(image + LOGICAL_BLOCK_SIZE * LBA_ALTERNATE_GPT);
    createGptPartitionTable(
        lbaAlternateGpt, 
        LBA_ALTERNATE_GPT, 
        LBA_PRIMARY_GPT, 
        LBA_FIRST_USABLE, 
        LBA_LAST_USABLE,
        LBA_ALTERNATE_GPT_PARTITION_ARRAY,
        alternatePartitionArrayCrc
    );
}

int main() {
    HANDLE handle = GetCurrentProcess();
    uint8_t* image = (uint8_t*)VirtualAllocEx(handle, nullptr, IMAGE_SIZE, MEM_COMMIT, PAGE_READWRITE);
    if (!image) {
        printf("Could not allocate image %lu\n", GetLastError());
        return 1;
    }

    createGpt(image);

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
