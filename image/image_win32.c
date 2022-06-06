#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <wchar.h>
#include <stdbool.h>

#define IMAGE_SIZE                  50000000
#define LOGICAL_BLOCK_SIZE          512
#define EFI_PART_MAGIC              0x5452415020494645 // "EFI PART"
#define GPT_HEADER_VERSION          0x00010000 // Version 1.0
#define GPT_PARTITION_ENTRY_SIZE    128
#define FAT_DIR_NAME_SIZE           11

// FAT Entry
#define FAT_ENTRY_MASK      0x0FFFFFFF
#define FAT_BAD_CLUSTER     0x0FFFFFF7
#define FAT_END_OF_CLUSTER  0x0FFFFFF8

#define FAT_ATTR_READ_ONLY  0x01
#define FAT_ATTR_HIDDEN     0x02
#define FAT_ATTR_SYSTEM     0x04
#define FAT_ATTR_VOLUME_ID  0x08
#define FAT_ATTR_DIRECTORY  0x10
#define FAT_ATTR_ARCHIVE    0x20
#define FAT_ATTR_LONG_NAME  FAT_ATTR_READ_ONLY | \
                            FAT_ATTR_HIDDEN | \
                            FAT_ATTR_SYSTEM | \
                            FAT_ATTR_VOLUME_ID


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

struct __attribute__((__packed__)) GptHeader {
    uint64_t signature;
    uint32_t revision;
    uint32_t headerSize;
    uint32_t headerCrc32;
    uint32_t reserved;
    uint64_t myLba;
    uint64_t alternateLba;
    uint64_t firstUsableLba;
    uint64_t lastUsableLba;
    struct Guid diskGuid;
    uint64_t partitionEntryLba;
    uint32_t numberOfPartitionEntries;
    uint32_t sizeOfPartitionEntry;
    uint32_t partitionEntryArrayCrc32;
};

struct GptPartitionEntry {
    struct Guid partitionTypeGuid;
    struct Guid uniquePartitionGuid;
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

    struct GptHeader* gptHeader = (struct GptHeader*)dest;
    gptHeader->signature = EFI_PART_MAGIC;
    gptHeader->revision = GPT_HEADER_VERSION;
    gptHeader->headerSize = sizeof(struct GptHeader);
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
    struct GptPartitionEntry* partEntry = (struct GptPartitionEntry*)dest;
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
    partEntry->endingLba = 34 + 66562;
    partEntry->attributes = 0;

    // Partition name located at the end of GPT Partition Entry
    const wchar_t* PARTITION_NAME = L"Boot Partition";
    wchar_t* partitionName = (wchar_t*)((uint8_t*)partEntry + sizeof(struct GptPartitionEntry));
    // Get partition name string in bytes plus null-terminator
    uint32_t partitionNameSize = wcsnlen_s(PARTITION_NAME, 32) * sizeof(wchar_t) + sizeof(wchar_t);
    memcpy(partitionName, PARTITION_NAME, partitionNameSize);

    const uint32_t ENTRY_COUNT = 1;
    uint32_t crc = crc32(dest, ENTRY_COUNT * GPT_PARTITION_ENTRY_SIZE);
    return crc;
}

// FAT32 Memory Layout
// Sector   What                        Region
// ------------------------------------------------------
// 0        BIOS Parameter Block (BPB)  Reserved
// 1        FSInfo                      Reserved
// 3        Emtpy sector with sign.     ???
// 4        FAT Table                   FAT
// ...      Root Directory              Data 
// ...      Data Region                 Data                         

#define FAT_SECTOR_SIZE             512
#define FAT_SECTOR_PER_CLUSTER      1
#define FAT_CLUSTER_SIZE            (FAT_SECTOR_PER_CLUSTER * FAT_SECTOR_SIZE)
#define FAT_RESERVED_SECTOR_COUNT   3
#define FAT_NUM_OF_FAT              2 // Primary and backup FAT
#define FAT_TOTAL_SECTOR_COUNT      (FAT_SECTOR_SIZE * 2 + FAT_RESERVED_SECTOR_COUNT \
                                    + FAT_DATA_SECTOR_COUNT)
#define FAT_FSINFO_SECTOR           1
#define FAT_ROOT_CLUSTER            2 // Relative to the data region

#define FAT_EXTENDED_BOOT_SIGNATURE 0x29

struct __attribute__((__packed__)) Fat32BootSector {
    uint8_t jmpBoot[3];
    uint64_t oemName;
    uint16_t bytesPerSector;
    uint8_t sectorsPerCluster;
    uint16_t reservedSectorCount;
    uint8_t numberOfFats;
    uint16_t rootEntryCount;
    uint16_t totalSector16;
    uint8_t media;
    uint16_t fatSize16;
    uint16_t sectorPerTrack;
    uint16_t numberOfHeads;
    uint32_t hiddenSector;
    uint32_t totalSector32;
    uint32_t fatSize32;
    uint16_t extFlags;
    uint16_t fileSystemVersion;
    uint32_t rootCluster;
    uint16_t fileSystemInfo;
    uint16_t backupBootSector;
    uint64_t reserved1;
    uint32_t reserved2;
    uint8_t driverNumber;
    uint8_t reserved3;
    uint8_t bootSignature;
    uint32_t volumeId;
    uint8_t volumeLabel[11];
    uint64_t fileSystemType;
};

struct __attribute__((__packed__)) FatFileSystemInfo {
    uint32_t leadSignature;
    uint8_t reserved1[480];
    uint32_t structSignature;
    uint32_t freeCount;
    uint32_t nextFree;
    uint8_t reserved2[12];
    uint32_t trailSignature;
};

struct __attribute__((__packed__)) FatDirectoryEntry {
    char name[FAT_DIR_NAME_SIZE];
    uint8_t attribute;
    uint8_t ntReserved;
    uint8_t creationTimeTenth;
    uint16_t creationTime;
    uint16_t creationDate;
    uint16_t lastAccessDate;
    uint16_t firstClusterHigh;
    uint16_t writeTime;
    uint16_t writeDate;
    uint16_t firstClusterLow;
    uint32_t fileSize;
};

struct FatVolume {
    struct Fat32BootSector* bootSector;
    struct FatFileSystemInfo* fsInfo;
    uint32_t* fat;
    uint32_t dataClusterCount;
    uint8_t* dataSector;
};

uint8_t* fatGetDataCluster(struct FatVolume* volume, uint32_t clusterNum) {
    // TODO: check upper bounds
    if (clusterNum < 2) {
        printf("Invalid data cluster number '%u'\n", clusterNum);
        return NULL;
    }

    const uint32_t CLUSTER_SIZE = FAT_SECTOR_SIZE * FAT_SECTOR_PER_CLUSTER;
    return volume->dataSector + (clusterNum - 2) * CLUSTER_SIZE;
}


static uint8_t FAT_VALID_CHARS[256] = {
//  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
    1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, // 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, // 3
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, // 5
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, // 7
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // F
};

bool fatCreateDirectoryName(char name[], uint32_t size, char* destination) {
    const uint32_t MAX_INPUT_SIZE = 12;
    if (size > MAX_INPUT_SIZE) {
        printf("Directory name '%s' is too long (> 12)\n", name);
        return false;
    }

    // A directory name is only allowed to have at most an
    // 8 byte name followed by an optional dot '.' and a 
    // 3 byte long extension
    int32_t separatorIndex = -1;
    for (uint32_t i = 0; i < size; i++) {
        if (name[i] == '.') {
            if (separatorIndex > -1) {
                printf("Found multiple '.' in file name\n");
                return false;
            }
            separatorIndex = i;
        }
    }

    if (separatorIndex == 0) {
        printf("File name cannot start with '.' '%s'\n", name);
        return false;
    }

    const uint32_t MAX_NAME_SIZE = 8;
    const uint32_t MAX_EXT_SIZE = 3;

    uint32_t nameSize = size;
    uint32_t extSize = 0;
    if (separatorIndex > -1) {
        nameSize = size - (size - separatorIndex);
        extSize = size - (separatorIndex + 1);
    }

    if (nameSize > MAX_NAME_SIZE) {
        printf("File name too long '%s'\n", name);
        return false;
    }
    if (extSize > MAX_EXT_SIZE) {
        printf("File extension too long '%s'\n", name);
        return false;
    }

    memset(destination, 0x20, FAT_DIR_NAME_SIZE);
    uint32_t extensionSize = 0;

    char* out = destination;
    for (uint32_t i = 0; i < size; i++) {
        uint8_t c = name[i];

        if (c == '.') {
            out = &destination[MAX_NAME_SIZE];
            continue;
        }

        // To upper case
        if (c >= 'a' && c <= 'z') {
            c -= 32;
        }

        if (!FAT_VALID_CHARS[c]) {
            printf("Invalid char '%c' in directory name '%s'\n", c, name);
            return false;
        }

        *out = c;
        out++;
    }

    return true;
}

bool fatDirNameEqual(char* a, char* b) {
    for (uint32_t i = 0; i < FAT_DIR_NAME_SIZE; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

struct FatDirectoryEntry* fatFindDirectory(struct FatVolume* volume, uint32_t startClusterIndex, char* dirName) {
    const uint32_t MAX_DIR_PER_CLUSTER = 
        (FAT_SECTOR_SIZE * FAT_SECTOR_PER_CLUSTER) / sizeof(struct FatDirectoryEntry);

    uint32_t nextCluster = volume->fat[startClusterIndex];

    uint32_t dirIndex = 0;
    struct FatDirectoryEntry* dir = 
        (struct FatDirectoryEntry*)fatGetDataCluster(volume, startClusterIndex);


    while (dir->name[0] != 0) {
        if (fatDirNameEqual(dir->name, dirName)) {
            return dir;
        }

        if (dirIndex + 1 > MAX_DIR_PER_CLUSTER) {
            if (nextCluster == FAT_END_OF_CLUSTER) {
                break;
            }

            dirIndex = 0;
            dir = (struct FatDirectoryEntry*)fatGetDataCluster(volume, nextCluster);
            nextCluster = volume->fat[nextCluster];
            continue;
        }
        dirIndex++;
        dir++;
    }

    return NULL;
}

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

uint32_t fatAllocateCluster(struct FatVolume* volume) {
    uint32_t cluster = 0;
    for (uint32_t i = 0; i < volume->dataClusterCount; i++) {
        if (volume->fat[i] == 0) {
            volume->fat[i] = FAT_END_OF_CLUSTER;
            cluster = i;
            break;
        }
    }

    return cluster;
}

struct FatDirectoryEntry* fatAllocateDirectory(struct FatVolume* volume, uint32_t parentClusterNumber) {
    uint32_t clusterIndex = parentClusterNumber;
    uint32_t cluster = volume->fat[clusterIndex];
    uint8_t* clusterData = fatGetDataCluster(volume, parentClusterNumber);

    uint32_t entryIndex = 0;
    struct FatDirectoryEntry* dir = (struct FatDirectoryEntry*)clusterData;

    const uint32_t MAX_DIR_PER_CLUSTER = 
        (FAT_SECTOR_SIZE * FAT_SECTOR_PER_CLUSTER) / sizeof(struct FatDirectoryEntry);

    while (dir->name[0] != 0) {

        if (entryIndex + 1 >= MAX_DIR_PER_CLUSTER) {
            if (cluster == FAT_END_OF_CLUSTER) {
                uint32_t nextCluster = fatAllocateCluster(volume);
                volume->fat[clusterIndex] = nextCluster;
                dir = (struct FatDirectoryEntry*)fatGetDataCluster(volume, nextCluster);
                break; 
            }

            clusterIndex = cluster;
            cluster = volume->fat[clusterIndex];
            clusterData = fatGetDataCluster(volume, clusterIndex);
            entryIndex = 0;
            dir = (struct FatDirectoryEntry*)clusterData;
            continue;
        }

        entryIndex++;
        dir++;
    }
    

    return dir;
}

#define MAX_SUB_DIRS        32
#define MAX_PATH_SIZE       64
#define MAX_DIR_NAME        13

void changeToLastDir(
    struct FatVolume* volume, 
    char* inputPath, 
    char pathElements[MAX_SUB_DIRS][FAT_DIR_NAME_SIZE],
    uint32_t* pathElementCount,
    uint32_t* parentClusterIndex
) {
    *pathElementCount = 0;
    char* path = inputPath;
    uint32_t pathSize = strnlen_s(path, MAX_PATH_SIZE);

    bool skipRootSlash = path[0] == '/';
    if (skipRootSlash) {
        pathSize--;
        path++;
    }

#define KEY_FROM    0
#define KEY_SIZE    1

    uint32_t pathSpans[MAX_SUB_DIRS][2] = {};
    uint32_t pathSpanCount = 1;

    pathSpans[0][KEY_FROM] = 0;
    pathSpans[0][KEY_SIZE] = 0;

    for (uint32_t pathIndex = 0; pathIndex < pathSize; pathIndex++) {
        uint32_t currentSpan = pathSpanCount - 1;
        bool isLast = pathIndex + 1 == pathSize;
    
        if (path[pathIndex] == '/') {
            if (pathSpanCount + 1 > MAX_SUB_DIRS) {
                printf("Too many subdirectories in path '%s'\n", inputPath);
                return;
            }
            pathSpans[currentSpan][KEY_SIZE] = pathIndex - pathSpans[currentSpan][KEY_FROM];

            if (!isLast) {
                pathSpans[pathSpanCount][KEY_FROM] = pathIndex + 1;
                pathSpanCount++;
            }
        } else if (isLast) {
            pathSpans[currentSpan][KEY_SIZE] = (pathIndex + 1) - pathSpans[currentSpan][KEY_FROM];
        }
    }

    for (uint32_t pathSpanIdx = 0; pathSpanIdx < pathSpanCount; pathSpanIdx++) {
        uint32_t from = pathSpans[pathSpanIdx][KEY_FROM];
        uint32_t size = pathSpans[pathSpanIdx][KEY_SIZE];
        fatCreateDirectoryName(&path[from], size, pathElements[*pathElementCount]);
        *pathElementCount += 1;
    }

#undef KEY_FROM
#undef KEY_SIZE

    *parentClusterIndex = 2;
    uint32_t cluster = volume->fat[*parentClusterIndex];

    int32_t parentDirCount = *pathElementCount - 1;
    for (uint32_t dirIndex = 0; (int32_t)dirIndex < parentDirCount; dirIndex++) {
        struct FatDirectoryEntry* entry = fatFindDirectory(volume, *parentClusterIndex, pathElements[dirIndex]);
        if (!entry) {
            printf("Directory '%*.s' does not exist in '%s'\n", 
                    FAT_DIR_NAME_SIZE, pathElements[dirIndex], inputPath);
        }

        *parentClusterIndex = ((uint16_t)entry->firstClusterHigh << 16) | entry->firstClusterLow;
    }
}

void fatCreateDirectory(struct FatVolume* volume, char* inputPath) {

    char pathElements[MAX_SUB_DIRS][FAT_DIR_NAME_SIZE] = {};
    uint32_t parentClusterIndex = 0;
    uint32_t pathElementCount = 0;
    changeToLastDir(volume, inputPath, pathElements, &pathElementCount, &parentClusterIndex);
    
    char* newPathElement = pathElements[pathElementCount - 1];
    struct FatDirectoryEntry* existingEntry = fatFindDirectory(volume, parentClusterIndex, newPathElement);
    if (existingEntry) {
        printf("Directory '%*.s' already exists in path %s\n", FAT_DIR_NAME_SIZE, newPathElement, inputPath);
        return;
    }

    struct FatDirectoryEntry* newDir = fatAllocateDirectory(volume, parentClusterIndex);
    newDir->attribute = FAT_ATTR_DIRECTORY;
    memcpy(newDir->name, newPathElement, FAT_DIR_NAME_SIZE); 
    fatFillDateTime(newDir);

    uint32_t dirContentCluster = fatAllocateCluster(volume);
    newDir->firstClusterHigh = dirContentCluster >> 16;
    newDir->firstClusterLow = dirContentCluster & 0xFF;

    uint8_t* contentCluster = fatGetDataCluster(volume, dirContentCluster);
    struct FatDirectoryEntry* dotDir = (struct FatDirectoryEntry*)contentCluster;
    struct FatDirectoryEntry* dotDotDir = (struct FatDirectoryEntry*)(contentCluster + sizeof(struct FatDirectoryEntry));

    char dotDirName[FAT_DIR_NAME_SIZE] = ".          ";
    char dotDotDirName[FAT_DIR_NAME_SIZE] = "..         ";

    memcpy(dotDir->name, dotDirName, FAT_DIR_NAME_SIZE);
    memcpy(dotDotDir->name, dotDotDirName, FAT_DIR_NAME_SIZE);

    dotDir->attribute = FAT_ATTR_DIRECTORY;
    dotDir->creationTimeTenth = newDir->creationTimeTenth;
    dotDir->creationTime = newDir->creationTime;
    dotDir->creationDate = newDir->creationDate;
    dotDir->lastAccessDate = newDir->lastAccessDate;
    dotDir->writeTime = newDir->writeTime;
    dotDir->writeDate = newDir->writeDate;
    dotDir->firstClusterHigh = newDir->firstClusterHigh;
    dotDir->firstClusterLow = newDir->firstClusterLow;

    dotDotDir->attribute = FAT_ATTR_DIRECTORY;
    dotDotDir->creationTimeTenth = newDir->creationTimeTenth;
    dotDotDir->creationTime = newDir->creationTime;
    dotDotDir->creationDate = newDir->creationDate;
    dotDotDir->lastAccessDate = newDir->lastAccessDate;
    dotDotDir->writeTime = newDir->writeTime;
    dotDotDir->writeDate = newDir->writeDate;
    dotDotDir->firstClusterHigh = parentClusterIndex >> 16;
    dotDotDir->firstClusterLow = (uint8_t)parentClusterIndex;
}

void fatCreateFile(struct FatVolume* volume, uint8_t* fileBuffer, uint32_t fileSize, char* path) {
    char pathElements[MAX_SUB_DIRS][FAT_DIR_NAME_SIZE] = {};
    uint32_t parentClusterIndex = 0;
    uint32_t pathElementCount = 0;
    changeToLastDir(volume, path, pathElements, &pathElementCount, &parentClusterIndex);

    char* newPathElement = pathElements[pathElementCount - 1];
    struct FatDirectoryEntry* existingEntry = fatFindDirectory(volume, parentClusterIndex, newPathElement);
    if (existingEntry) {
        printf("File '%*.s' already exists in path %s\n", FAT_DIR_NAME_SIZE, newPathElement, path);
        return;
    }

    struct FatDirectoryEntry* newDir = fatAllocateDirectory(volume, parentClusterIndex);
    newDir->fileSize = fileSize;
    memcpy(newDir->name, newPathElement, FAT_DIR_NAME_SIZE); 
    fatFillDateTime(newDir);

    uint32_t firstClusterIndex = fatAllocateCluster(volume);
    uint8_t* firstClusterData = fatGetDataCluster(volume, firstClusterIndex);
    memcpy(firstClusterData, fileBuffer, FAT_CLUSTER_SIZE);
    newDir->firstClusterLow = firstClusterIndex;
    newDir->firstClusterHigh = firstClusterIndex >> 16;

    uint8_t* fileSource = fileBuffer + FAT_CLUSTER_SIZE;
    uint32_t clusterCount = (fileSize / FAT_CLUSTER_SIZE) + ((fileSize % FAT_CLUSTER_SIZE) > 0);
    uint32_t lastClusterIndex = firstClusterIndex;
    for (uint32_t i = 0; (int32_t)i < (int32_t)(clusterCount - 1); i++) {
        uint32_t clusterIndex = fatAllocateCluster(volume);
        uint8_t* clusterData = fatGetDataCluster(volume, clusterIndex);
        uint32_t copySize = FAT_CLUSTER_SIZE;

        bool isLastCluster = i + 1 == clusterCount;
        if (isLastCluster) {
            copySize = (fileBuffer + fileSize) - fileSource;
            volume->fat[lastClusterIndex] = FAT_END_OF_CLUSTER;
        } else {
            volume->fat[lastClusterIndex] = clusterIndex;
        }

        memcpy(clusterData, fileSource, copySize);
        fileSource += copySize;
        lastClusterIndex = clusterIndex;
    }
}

void createFat32(uint8_t* dest) {
    const uint64_t FAT32_OEM_NAME = 0x312E344E4957534D; // MSWIN4.1
    const uint8_t FAT_MEDIA_TYPE_REMOVABLE = 0xF0;

    // Minimal amount of sectors for a valid FAT32 volume
    uint32_t fatTableSectorCount = 512;
    uint32_t fatDataSectorCount = 65536;
    uint32_t fatTotalSectorCount = FAT_SECTOR_SIZE * 2 + FAT_RESERVED_SECTOR_COUNT 
                                 + fatDataSectorCount;

    // TODO: generate random volume id
    const uint32_t VOLUME_ID = 0x219EB6E0;
    const char VOLUME_LABEL[11] = {
        'S', 'P', 'I', 'E', 'L', 'Z', 'U',
        'G', ' ', ' ', ' ',
    };
    const char FS_TYPE_FAT[sizeof(uint64_t)] = {
        'F', 'A', 'T', '3',
        '2', ' ', ' ', ' ',
    };

    struct Fat32BootSector* sector = (struct Fat32BootSector*)dest;
    sector->jmpBoot[0] = 0xEB; // jmp
    sector->jmpBoot[1] = 0x00; // target
    sector->jmpBoot[2] = 0x90; // nop
    sector->oemName = FAT32_OEM_NAME;
    sector->bytesPerSector = FAT_SECTOR_SIZE;
    sector->sectorsPerCluster = FAT_SECTOR_PER_CLUSTER;
    sector->reservedSectorCount = FAT_RESERVED_SECTOR_COUNT;
    sector->numberOfFats = FAT_NUM_OF_FAT;
    sector->rootEntryCount = 0;
    sector->totalSector16 = 0;
    sector->media = FAT_MEDIA_TYPE_REMOVABLE;
    sector->fatSize16 = 0;
    sector->sectorPerTrack = 0;
    sector->numberOfHeads = 0;
    sector->hiddenSector = 0;
    sector->totalSector32 = fatTotalSectorCount;
    sector->fatSize32 = fatTableSectorCount;
    sector->extFlags = 0;
    sector->fileSystemVersion = 0;
    sector->rootCluster = FAT_ROOT_CLUSTER;
    sector->fileSystemInfo = FAT_FSINFO_SECTOR;
    // TODO: put backup boot sector there
    sector->backupBootSector = 0;
    sector->reserved1 = 0;
    sector->reserved2 = 0;
    // This is only used for Int 0x13 but lets fill it in anyway...
    sector->driverNumber = 0x80;
    sector->reserved3 = 0;
    sector->bootSignature = FAT_EXTENDED_BOOT_SIGNATURE;
    sector->volumeId = VOLUME_ID;
    // TODO: should be the same as in root directory
    // NOTE: This is a really ugly way of efficiently copying 
    // the volume label which is only 11 bytes long but because 
    // it is followed by the fileSystemType we can write 12 bytes
    uint64_t* volLabel64 = (uint64_t*)&sector->volumeLabel;
    uint32_t* volLabel32 = (uint32_t*)&sector->volumeLabel[8];
    *volLabel64 = *(uint64_t*)VOLUME_LABEL;
    *volLabel32 = *(uint32_t*)&VOLUME_LABEL[8];

    sector->fileSystemType = *(uint64_t*)FS_TYPE_FAT;
    dest[510] = 0x55;
    dest[511] = 0xAA;

    const uint32_t FS_LEAD_SIGNATURE = 0x41615252;
    const uint32_t FS_STRUCT_SIGNATURE = 0x61417272;
    const uint32_t FS_TRAIL_SIGNATURE = 0xAA550000;

    struct FatFileSystemInfo* fsInfo = (struct FatFileSystemInfo*)(dest + FAT_FSINFO_SECTOR * LOGICAL_BLOCK_SIZE);
    memset((void*)fsInfo, 0, sizeof(struct FatFileSystemInfo));

    fsInfo->leadSignature = FS_LEAD_SIGNATURE;
    fsInfo->structSignature = FS_STRUCT_SIGNATURE;
    // TODO: fill in the actuall free cluster count 
    // and next free cluster
    fsInfo->freeCount = 0xFFFFFFFF;
    fsInfo->nextFree = 0xFFFFFFFF;
    fsInfo->trailSignature = FS_TRAIL_SIGNATURE;

    // After the FSInfo there is a third sector which just seams to
    // be empty and only contains a signature
    uint8_t* thirdSector = (uint8_t*)fsInfo + FAT_SECTOR_SIZE;
    thirdSector[510] = 0x55;
    thirdSector[511] = 0xAA;


    // Debug info
    uint32_t rootDirectorySectors = ((sector->rootEntryCount * 32) + (sector->bytesPerSector - 1)) / sector->bytesPerSector;
    uint32_t firstDataSector = sector->reservedSectorCount + (sector->numberOfFats * sector->fatSize32)
                             + rootDirectorySectors;

    uint32_t fatSize = 0;
    uint32_t totalSector = 0;

    if (sector->fatSize16 != 0) {
        fatSize = sector->fatSize16;
    } else {
        fatSize = sector->fatSize32;
    }

    if (sector->totalSector16 != 0) {
        totalSector = sector->totalSector16;
    } else {
        totalSector = sector->totalSector32;
    }
    uint32_t dataSector = totalSector - (sector->reservedSectorCount + (sector->numberOfFats * fatSize) + rootDirectorySectors);
    uint32_t countOfClusters = dataSector / sector->sectorsPerCluster;

    printf("====== FAT ======\n");
    printf("Root Dir Sectors: %u\n", rootDirectorySectors);
    printf("First Data Sector: %u\n", firstDataSector);
    printf("Data Sectors: %u\n", dataSector);
    printf("Count of Clusters: %u\n", countOfClusters);
    printf("Total Sector Count: %u\n", fatTotalSectorCount);
    if (countOfClusters < 4085) {
        printf("Type: FAT12\n");
    } else if (countOfClusters < 65525) {
        printf("Type: FAT16\n");
    } else {
        printf("Type: FAT32\n");
    }

    // FAT Table
    uint32_t* fat = (uint32_t*)(dest + FAT_RESERVED_SECTOR_COUNT * FAT_SECTOR_SIZE);
    fat[0] = (uint32_t)sector->media | 0x0FFFFF00;
    fat[1] = 0x0FFFFFFF;
    fat[2] = FAT_END_OF_CLUSTER; // Root Directory
    
    uint8_t* rootDir = dest + firstDataSector * FAT_SECTOR_SIZE;

    struct FatDirectoryEntry* volumeDir = (struct FatDirectoryEntry*)rootDir;
    memcpy(volumeDir->name, VOLUME_LABEL, FAT_DIR_NAME_SIZE); 
    volumeDir->attribute = FAT_ATTR_VOLUME_ID;
    fatFillDateTime(volumeDir);

    struct FatVolume volume = {
        .bootSector = sector,
        .fsInfo = fsInfo,
        .fat = fat,
        .dataSector = (uint8_t*)rootDir,
        .dataClusterCount = countOfClusters,
    };

    fatCreateDirectory(&volume, "EFI");
    fatCreateDirectory(&volume, "/EFI/BOOT/");

    HANDLE efiFile = CreateFileW(
        L"BOOTX64.EFI",
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

    fatCreateFile(&volume, efiFileBuffer, efiFileSize, "/EFI/BOOT/BOOTX64.EFI");

    VirtualFree(efiFileBuffer, 0, MEM_RELEASE);
    CloseHandle(efiFile);
}

uint32_t createGpt(uint8_t* image) {
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
    // TODO: calculate this
    const uint32_t PARTITION_SIZE_LBA = 66562; // Minimal size for FAT32
    
    const uint32_t LBA_PRIMARY_GPT = 1;
    const uint32_t LBA_GPT_PARTITION_ARRAY = 2;
    const uint32_t LBA_FIRST_USABLE = LBA_GPT_PARTITION_ARRAY + GPT_PARTITION_ARRAY_SIZE;
    const uint32_t LBA_LAST_USABLE = LBA_FIRST_USABLE + PARTITION_SIZE_LBA;
    const uint32_t LBA_ALTERNATE_GPT_PARTITION_ARRAY = LBA_LAST_USABLE + 1;
#define LBA_ALTERNATE_GPT   (LBA_ALTERNATE_GPT_PARTITION_ARRAY + GPT_PARTITION_ARRAY_SIZE)

    // Protective MBR

    struct MbrPartitionRecord* partRecord = (struct MbrPartitionRecord*)&image[PARTITION_RECORD_OFFSET];
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
    partRecord->sizeInLba = LBA_ALTERNATE_GPT;

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

    uint8_t* lbaFirstUsable = (uint8_t*)(image + LOGICAL_BLOCK_SIZE * LBA_FIRST_USABLE);
    createFat32(lbaFirstUsable);

    uint32_t imageSize = (LBA_ALTERNATE_GPT + 1) * LOGICAL_BLOCK_SIZE;

    // Debug information
    printf("====== GPT ======\n");
    printf("Primary GPT LBA: %u\n", LBA_PRIMARY_GPT);
    printf("Partition Array LBA: %u\n", LBA_GPT_PARTITION_ARRAY);
    printf("First Usable LBA: %u\n", LBA_FIRST_USABLE);
    printf("Last Usable LBA: %u\n", LBA_LAST_USABLE);
    printf("Alternate GPT LBA: %u\n", LBA_ALTERNATE_GPT);
    printf("LBA Size: %u\n", LOGICAL_BLOCK_SIZE);
    printf("Image Size: %u\n", imageSize);

    return imageSize;
}

int main() {
    HANDLE handle = GetCurrentProcess();
    uint8_t* image = (uint8_t*)VirtualAllocEx(handle, NULL, IMAGE_SIZE, MEM_COMMIT, PAGE_READWRITE);
    if (!image) {
        printf("Could not allocate image %lu\n", GetLastError());
        return 1;
    }

    uint32_t imageSize = createGpt(image);

    HANDLE file = CreateFileW(
        L"spielzeug.iso",
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
        imageSize,
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
