#pragma once
#include "image.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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
    uint32_t sectorSize;
    struct Fat32BootSector* bootSector;
    struct FatFileSystemInfo* fsInfo;
    uint32_t* fat;
    uint32_t dataClusterCount;
    uint8_t* dataSector;
};

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

uint8_t* fatGetDataCluster(struct FatVolume* volume, uint32_t clusterNum) {
    // TODO: check upper bounds
    if (clusterNum < 2) {
        printf("Invalid data cluster number '%u'\n", clusterNum);
        return NULL;
    }

    const uint32_t CLUSTER_SIZE = FAT_SECTOR_SIZE * FAT_SECTOR_PER_CLUSTER;
    return volume->dataSector + (clusterNum - 2) * CLUSTER_SIZE;
}

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

void createFat32(struct FatVolume* volume, uint8_t* dest, uint32_t sectorSize) {
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

    struct FatFileSystemInfo* fsInfo = (struct FatFileSystemInfo*)(dest + FAT_FSINFO_SECTOR * sectorSize);
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

    volume->bootSector = sector;
    volume->fsInfo = fsInfo;
    volume->fat = fat;
    volume->dataSector = (uint8_t*)rootDir;
    volume->dataClusterCount = countOfClusters;
    volume->sectorSize = sectorSize;
}
