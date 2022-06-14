#pragma once

#ifndef __UEFIBASETYPE_H
#define __UEFIBASETYPE_H

#ifndef VOID

#define VOID void
#define IN
#define OUT
#define INOUT

#endif
#define OPTIONAL

#define BOOLEAN int


/* Some definitions for building */
typedef uint32_t EFI_STATUS;

typedef uint32_t	UINT32;
typedef uint16_t	UINT16;
typedef uint8_t		UINT8;

typedef unsigned int	UINTN;

#define EFIAPI __stdcall
#define UINTN unsigned int
typedef char CHAR8;
#ifdef __cplusplus
typedef wchar_t CHAR16;
#else
typedef short CHAR16;
#endif

#define GLOBAL_REMOVE_IF_UNREFERENCED
#define EFI_HANDLE void*

#define EFI_SUCCESS		0x00000000
#define EFI_UNSUPPORTED 0x80000003

struct _EFI_COMPONENT_NAME_PROTOCOL;

typedef EFI_STATUS(EFIAPI* EFI_COMPONENT_NAME2_GET_DRIVER_NAME)(struct _EFI_COMPONENT_NAME_PROTOCOL* This, IN  CHAR8* Language, OUT CHAR16** DriverName);
typedef EFI_STATUS(EFIAPI* EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME)(struct _EFI_COMPONENT_NAME_PROTOCOL* This,
	IN  EFI_HANDLE                                      ControllerHandle,
	IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
	IN  CHAR8* Language,
	OUT CHAR16** ControllerName
	);

typedef struct _EFI_COMPONENT_NAME_PROTOCOL {
	EFI_COMPONENT_NAME2_GET_DRIVER_NAME GetDriverName;
	EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME GetControllerName;
	const char* SupportedLanguages;
} EFI_COMPONENT_NAME_PROTOCOL, EFI_COMPONENT_NAME2_PROTOCOL;

typedef struct _EFI_UNICODE_STRING_TABLE {
	const char* ascii;
	const wchar_t* unicode;
} EFI_UNICODE_STRING_TABLE;

typedef unsigned __int64	EFI_LBA;

typedef struct _EFI_TIME {
	char dummy;
} EFI_TIME;
#endif

