#pragma once

#include <stdint.h>
#include <basetsd.h>

#include "UefiBaseType.h"
#include "UefiSpec.h"

typedef struct {
	UINT32	Data1;
	UINT16	Data2;
	UINT16	Data3;
	UINT8	Data4[8];
} EFI_GUID;

typedef struct _list_entry {
	struct _list_entry* prev;
	struct _list_entry* next;
} LIST_ENTRY;
#include "DevicePath.h"
#include "Protocol/DriverBinding.h"
#include "Protocol/BlockIo.h"
#include "Protocol/DiskIo.h"
#include "Protocol/SimpleFileSystem.h"
#include "FileInfo.h"
