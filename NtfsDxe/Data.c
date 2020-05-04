/*++
Module Name:

  Data.c

Abstract:

  Global data in the NTFS Filesystem driver

Revision History

--*/

#include "Ntfs.h"

//
// Globals
//
//
// NtfsFsLock - Global lock for synchronizing all requests.
//
EFI_LOCK NtfsFsLock = EFI_INITIALIZE_LOCK_VARIABLE(TPL_CALLBACK);

//
// Filesystem interface functions
//
EFI_FILE_PROTOCOL               NtfsFileInterface = {
  EFI_FILE_PROTOCOL_REVISION,
  NtfsOpenFile,
  NtfsCloseFile,
  NtfsDeleteFile,
  NtfsReadFile,
  NtfsWriteFile,
  NtfsGetPositionFile,
  NtfsSetPositionFile,
  NtfsGetInfoFile,
  NtfsSetInfoFile,
  NtfsFlushFile
};
