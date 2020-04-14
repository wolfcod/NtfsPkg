/*++

Copyright (c) 2005, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the Software
License Agreement which accompanies this distribution.


Module Name:

  FatFileSystem.h

Abstract:

  Definitions for on-disk FAT structures

Revision History

--*/

#ifndef _FATFILESYSTEM_H_
#define _FATFILESYSTEM_H_

#pragma pack(1)

// FAT entry values
//
#define FAT_CLUSTER_SPECIAL_EXT       (-1 & (~0xF))
//#define FAT_CLUSTER_SPECIAL           ((FAT_CLUSTER_SPECIAL_EXT) | 0x07)
#define FAT_CLUSTER_SPECIAL           (FAT_CLUSTER_SPECIAL_EXT)
#define FAT_CLUSTER_FREE              0
#define FAT_CLUSTER_RESERVED          (FAT_CLUSTER_SPECIAL | 0x06)
#define FAT_CLUSTER_BAD               (FAT_CLUSTER_SPECIAL | 0x07)
#define FAT_CLUSTER_LAST              (-1)
#define FAT_END_OF_FAT_CHAIN(Cluster) ((Cluster) > (FAT_CLUSTER_SPECIAL))
#define FAT_MIN_CLUSTER               2
#define FAT_MAX_FAT12_CLUSTER         0xFF5
#define FAT_MAX_FAT16_CLUSTER         0xFFF5
#define FAT_CLUSTER_SPECIAL_FAT12     0xFF7
#define FAT_CLUSTER_SPECIAL_FAT16     0xFFF7
#define FAT_CLUSTER_SPECIAL_FAT32     0x0FFFFFF7
#define FAT_CLUSTER_MASK_FAT12        0xFFF
#define FAT_CLUSTER_UNMASK_FAT12      0xF000
#define FAT_CLUSTER_MASK_FAT32        0x0FFFFFFF
#define FAT_CLUSTER_UNMASK_FAT32      0xF0000000
#define FAT_POS_FAT12(a)              ((a) * 3 / 2)
#define FAT_POS_FAT16(a)              ((a) * 2)
#define FAT_POS_FAT32(a)              ((a) * 4)
#define FAT_ODD_CLUSTER_FAT12(a)      (((a) & 1) != 0)


//
// FAT attribute define
//
#define FAT_ATTRIBUTE_READ_ONLY 0x01
#define FAT_ATTRIBUTE_HIDDEN    0x02
#define FAT_ATTRIBUTE_SYSTEM    0x04
#define FAT_ATTRIBUTE_VOLUME_ID 0x08
#define FAT_ATTRIBUTE_DIRECTORY 0x10
#define FAT_ATTRIBUTE_ARCHIVE   0x20
#define FAT_ATTRIBUTE_DEVICE    0x40
#define FAT_ATTRIBUTE_LFN       0x0F
//
// Some Long File Name definitions
//
#define FAT_LFN_LAST            0x40  // Ordinal field
#define MAX_LFN_ENTRIES         20
#define LFN_CHAR1_LEN           5
#define LFN_CHAR2_LEN           6
#define LFN_CHAR3_LEN           2
#define LFN_CHAR_TOTAL          (LFN_CHAR1_LEN + LFN_CHAR2_LEN + LFN_CHAR3_LEN)
#define LFN_ENTRY_NUMBER(a)     (((a) + LFN_CHAR_TOTAL - 1) / LFN_CHAR_TOTAL)
//
// Some 8.3 File Name definitions
//
#define FAT_MAIN_NAME_LEN       8
#define FAT_EXTEND_NAME_LEN     3
#define FAT_NAME_LEN            (FAT_MAIN_NAME_LEN + FAT_EXTEND_NAME_LEN)
//
// Some directory entry information
//
#define FAT_ENTRY_INFO_OFFSET   13
#define DELETE_ENTRY_MARK       0xE5
#define EMPTY_ENTRY_MARK        0x00

// internal flag
//
#define FAT_CASE_MIXED          0x01
#define FAT_CASE_NAME_LOWER     0x08
#define FAT_CASE_EXT_LOWER      0x10

//
// Directory Entry
//
#define FAT_MAX_YEAR_FROM_1980  0x7f
typedef struct {
  UINT16  Day : 5;
  UINT16  Month : 4;
  UINT16  Year : 7;                 // From 1980
} FAT_DATE;

typedef struct {
  UINT16  DoubleSecond : 5;
  UINT16  Minute : 6;
  UINT16  Hour : 5;
} FAT_TIME;

typedef struct {
  FAT_TIME  Time;
  FAT_DATE  Date;
} FAT_DATE_TIME;

typedef struct {
  CHAR8         FileName[11];       // 8.3 filename
  UINT8         Attributes;
  UINT8         CaseFlag;
  UINT8         CreateMillisecond;  // (creation milliseconds - ignored)
  FAT_DATE_TIME FileCreateTime;
  FAT_DATE      FileLastAccess;
  UINT16        FileClusterHigh;    // >= FAT32
  FAT_DATE_TIME FileModificationTime;
  UINT16        FileCluster;
  UINT32        FileSize;
} NTFS_DIRECTORY_ENTRY;

#pragma pack()

#endif
