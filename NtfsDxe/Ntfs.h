/*++

Copyright (c) 2005 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the Software
License Agreement which accompanies this distribution.


Module Name:

  Ntfs.h

Abstract:

  Main header file for EFI NTFS file system driver

Revision History

--*/

#ifndef __NTFS_DRIVER_H_
#define __NTFS_DRIVER_H_

#include <Uefi.h>

#ifndef _WIN32
#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/UnicodeCollation.h>

#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#endif

#include "NtfsFileSystem.h"
#include "ntfs/volume.h"
#include "ntfs/inode.h"
#include "ntfs/ntfsinternal.h"

//
// The NTFS signature
//
#define NTFS_VOLUME_SIGNATURE         SIGNATURE_32 ('n', 't', 'f', 'v')
#define NTFS_IFILE_SIGNATURE          SIGNATURE_32 ('n', 't', 'f', 'i')
#define NTFS_ODIR_SIGNATURE           SIGNATURE_32 ('n', 't', 'f', 'd')
#define NTFS_DIRENT_SIGNATURE         SIGNATURE_32 ('n', 't', 'f', 'e')
#define NTFS_OFILE_SIGNATURE          SIGNATURE_32 ('n', 't', 'f', 'o')

#define ASSERT_VOLUME_LOCKED(a)      ASSERT_LOCKED (&NtfsFsLock)

#define IFILE_FROM_FHAND(a)          CR (a, NTFS_IFILE, Handle, NTFS_IFILE_SIGNATURE)

#define DIRENT_FROM_LINK(a)          CR (a, NTFS_DIRENT, Link, NTFS_DIRENT_SIGNATURE)

#define VOLUME_FROM_ROOT_DIRENT(a)   CR (a, NTFS_VOLUME, RootDirEnt, NTFS_VOLUME_SIGNATURE)

#define VOLUME_FROM_VOL_INTERFACE(a) CR (a, NTFS_VOLUME, VolumeInterface, NTFS_VOLUME_SIGNATURE);

#define ODIR_FROM_DIRCACHELINK(a)    CR (a, NTFS_ODIR, DirCacheLink, NTFS_ODIR_SIGNATURE)

#define OFILE_FROM_CHECKLINK(a)      CR (a, NTFS_OFILE, CheckLink, NTFS_OFILE_SIGNATURE)

#define OFILE_FROM_CHILDLINK(a)      CR (a, NTFS_OFILE, ChildLink, NTFS_OFILE_SIGNATURE)

//
// Minimum sector size is 512B, Maximum sector size is 4096B
// Max sectors per cluster is 128
//
#define MAX_BLOCK_ALIGNMENT               12
#define MIN_BLOCK_ALIGNMENT               9
#define MAX_SECTORS_PER_CLUSTER_ALIGNMENT 7

//
// Efi Time Definition
//
#define IS_LEAP_YEAR(a)                   (((a) % 4 == 0) && (((a) % 100 != 0) || ((a) % 400 == 0)))

//
// Path name separator is back slash
//
#define PATH_NAME_SEPARATOR     L'\\'


#define EFI_PATH_STRING_LENGTH  260
#define EFI_FILE_STRING_LENGTH  255
#define LC_ISO_639_2_ENTRY_SIZE 3
#define MAX_LANG_CODE_SIZE      100

typedef CHAR8                   LC_ISO_639_2;

//
// Hash table size
//
#define HASH_TABLE_SIZE  0x400
#define HASH_TABLE_MASK  (HASH_TABLE_SIZE - 1)

//
// The directory entry for opened directory
//
typedef struct _NTFS_DIRENT {
  UINTN               Signature;
  UINT16              EntryPos;               // The position of this directory entry in the parent directory file
  UINT8               EntryCount;             // The count of the directory entry in the parent directory file
  BOOLEAN             Invalid;                // Indicate whether this directory entry is valid
  CHAR16              *FileString;            // The unicode long file name for this directory entry
  struct _NTFS_OFILE   *OFile;                 // The OFile of the corresponding directory entry
  struct _NTFS_DIRENT  *ShortNameForwardLink;  // Hash successor link for short filename
  struct _NTFS_DIRENT  *LongNameForwardLink;   // Hash successor link for long filename
  LIST_ENTRY          Link;                   // Connection of every directory entry
  NTFS_DIRECTORY_ENTRY Entry;                  // The physical directory entry stored in disk
  ntfs_inode		*inode_entry;				// The physical directory entry stored in disk
} NTFS_DIRENT;

typedef struct _NTFS_ODIR {
  UINTN               Signature;
  UINT32              CurrentEndPos;          // Current end position of the directory
  UINT32              CurrentPos;             // Current position of the directory
  LIST_ENTRY          *CurrentCursor;         // Current directory entry pointer
  LIST_ENTRY          ChildList;              // List of all directory entries
  BOOLEAN             EndOfDir;               // Indicate whether we have reached the end of the directory
  LIST_ENTRY          DirCacheLink;           // Linked in Volume->DirCacheList when discarded
  UINTN               DirCacheTag;            // The identification of the directory when in directory cache
  NTFS_DIRENT          *LongNameHashTable[HASH_TABLE_SIZE];
  NTFS_DIRENT          *ShortNameHashTable[HASH_TABLE_SIZE];
} NTFS_ODIR;

#define FSW_EFI_FILE_TYPE_FILE			0
#define FSW_EFI_FILE_TYPE_DIR			1

struct _ntfs_dir_state;
struct _ntfs_file_state;

typedef struct {
  UINTN               Signature;
  EFI_FILE_PROTOCOL   Handle;
  UINT64              Position;
  BOOLEAN             ReadOnly;
  UINT8				  Type;
  ntfs_inode		  *inode;
  ntfs_inode		  *dir_ni;
  struct _NTFS_VOLUME  *Volume;
  union {
    struct _ntfs_dir_state	*dir;  // valid only for directory
    struct _ntfs_file_state	*file; // valid only for file
  } state;

  BOOLEAN			 RootDir;
  CHAR8				FileName[260];			// ""
  CHAR8				FullPath[260];			// full path
} NTFS_IFILE;

//
// NTFS_OFILE - Each opened file
//
typedef struct _NTFS_OFILE {
  UINTN               Signature;
  struct _NTFS_VOLUME  *Volume;
  //
  // A permanant error code to return to all accesses to
  // this opened file
  //
  EFI_STATUS          Error;
  //
  // A list of the IFILE instances for this OFile
  //
  LIST_ENTRY          Opens;

  //
  // The opened parent, full path length and currently opened child files
  //
  struct _NTFS_OFILE   *Parent;

  //
  // The opened directory structure for a directory; if this
  // OFile represents a file, then ODir = NULL
  //
  NTFS_ODIR            *ODir;
  //
  // The directory entry for the Ofile
  //
  NTFS_DIRENT          *DirEnt;

  //
  // Link in Volume's reference list
  //
  LIST_ENTRY          CheckLink;

  // inode
  ntfs_inode		*inode;
  struct _reent		r;
} NTFS_OFILE;

//#define mutex_t int

struct _ntfs_vd;

typedef struct _NTFS_VOLUME {
	UINTN                           Signature;

	EFI_HANDLE                      Handle;
	BOOLEAN                         Valid;
	BOOLEAN                         DiskError;

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL VolumeInterface;

	//
	// If opened, the parent handle and BlockIo interface
	//
	EFI_BLOCK_IO_PROTOCOL           *BlockIo;
	EFI_DISK_IO_PROTOCOL            *DiskIo;
	UINT32                          MediaId;
	BOOLEAN                         ReadOnly;

	//
	// The root directory entry and opened root file
	//
	//NTFS_DIRENT                      RootDirEnt;
	//
	// File Name of root OFile, it is empty string
	//
	CHAR8                          RootFileString[16];
	//struct _NTFS_IFILE               *Root;

	// struct ntfs_device *dev;                /* NTFS device handle */
	ntfs_volume *vol;                       /* NTFS volume handle */
	mutex_t lock;                           /* Volume lock mutex */
	s64 id;                                 /* Filesystem id */
	u32 flags;                              /* Mount flags */
	char name[128];                         /* Volume name (cached) */
	u16 uid;                                /* User id for entry creation */
	u16 gid;                                /* Group id for entry creation */
	u16 fmask;                              /* Unix style permission mask for file creation */
	u16 dmask;                              /* Unix style permission mask for directory creation */
	ntfs_atime_t atime;                     /* Entry access time update strategy */
	bool showHiddenFiles;                   /* If true, show hidden files when enumerating directories */
	bool showSystemFiles;                   /* If true, show system files when enumerating directories */
	ntfs_inode *cwd_ni;                     /* Current directory */
	ntfs_inode *root;						/* Root inode */
	struct _ntfs_vd*	vd;	/* vd for wrapper */
} NTFS_VOLUME;

//
// Function Prototypes
//
EFI_STATUS
EFIAPI
NtfsOpenFile (
  IN  EFI_FILE_PROTOCOL *FHand,
  OUT EFI_FILE_PROTOCOL **NewHandle,
  IN  CHAR16            *FileName,
  IN  UINT64            OpenMode,
  IN  UINT64            Attributes
  )
/*++
Routine Description:

  Implements Open() of Simple File System Protocol.

Arguments:

  FHand                 - File handle of the file serves as a starting reference point.
  NewHandle             - Handle of the file that is newly opened.
  FileName              - File name relative to FHand.
  OpenMode              - Open mode.
  Attributes            - Attributes to set if the file is created.

Returns:

  EFI_INVALID_PARAMETER - The FileName is NULL or the file string is empty.
                          The OpenMode is not supported.
                          The Attributes is not the valid attributes.
  EFI_OUT_OF_RESOURCES  - Can not allocate the memory for file string.
  EFI_SUCCESS           - Open the file successfully.
  Others                - The status of open file.

--*/
;


EFI_STATUS
EFIAPI
NtfsGetPositionFile (
  IN  EFI_FILE_PROTOCOL *FHand,
  OUT UINT64            *Position
  )
/*++

Routine Description:

  Get the file's position of the file

Arguments:

  FHand                 - The handle of file.
  Position              - The file's position of the file.

Returns:

  EFI_SUCCESS           - Get the info successfully.
  EFI_DEVICE_ERROR      - Can not find the OFile for the file.
  EFI_UNSUPPORTED       - The open file is not a file.

--*/
;

EFI_STATUS
EFIAPI
NtfsGetInfoFile (
  IN     EFI_FILE_PROTOCOL      *FHand,
  IN     EFI_GUID               *Type,
  IN OUT UINTN                  *BufferSize,
     OUT VOID                   *Buffer
  )
/*++

Routine Description:

  Get the some types info of the file into Buffer

Arguments:

  FHand                 - The handle of file.
  Type                  - The type of the info.
  BufferSize            - Size of Buffer.
  Buffer                - Buffer containing volume info.

Returns:

  EFI_SUCCESS           - Get the info successfully.
  EFI_DEVICE_ERROR      - Can not find the OFile for the file.

--*/
;

EFI_STATUS
EFIAPI
NtfsSetInfoFile (
  IN EFI_FILE_PROTOCOL  *FHand,
  IN EFI_GUID           *Type,
  IN UINTN              BufferSize,
  IN VOID               *Buffer
  )
/*++

Routine Description:

  Set the some types info of the file into Buffer

Arguments:

  FHand                 - The handle of file.
  Type                  - The type of the info.
  BufferSize            - Size of Buffer.
  Buffer                - Buffer containing volume info.

Returns:

  EFI_SUCCESS           - Set the info successfully.
  EFI_DEVICE_ERROR      - Can not find the OFile for the file.

--*/
;

EFI_STATUS
EFIAPI
NtfsFlushFile (
  IN EFI_FILE_PROTOCOL  *FHand
  )
/*++

Routine Description:

  Flushes all data associated with the file handle

Arguments:

  FHand                 - Handle to file to flush

Returns:

  EFI_SUCCESS           - Flushed the file successfully
  EFI_WRITE_PROTECTED   - The volume is read only
  EFI_ACCESS_DENIED     - The volume is not read only
                          but the file is read only
  Others                - Flushing of the file is failed

--*/
;

EFI_STATUS
EFIAPI
NtfsCloseFile (
  IN EFI_FILE_PROTOCOL  *FHand
  )
/*++

Routine Description:

  Flushes & Closes the file handle.

Arguments:

  FHand                 - Handle to the file to delete.

Returns:

  EFI_SUCCESS           - Closed the file successfully.

--*/
;

EFI_STATUS
EFIAPI
NtfsDeleteFile (
  IN EFI_FILE_PROTOCOL  *FHand
  )
/*++

Routine Description:

  Deletes the file & Closes the file handle.

Arguments:

  FHand                    - Handle to the file to delete.

Returns:

  EFI_SUCCESS              - Delete the file successfully.
  EFI_WARN_DELETE_FAILURE  - Fail to delete the file.

--*/
;

EFI_STATUS
EFIAPI
NtfsSetPositionFile (
  IN EFI_FILE_PROTOCOL  *FHand,
  IN UINT64             Position
  )
/*++

Routine Description:

  Set the file's position of the file

Arguments:

  FHand                 - The handle of file
  Position              - The file's position of the file

Returns:

  EFI_SUCCESS           - Set the info successfully
  EFI_DEVICE_ERROR      - Can not find the OFile for the file
  EFI_UNSUPPORTED       - Set a directory with a not-zero position

--*/
;

EFI_STATUS
EFIAPI
NtfsReadFile (
  IN     EFI_FILE_PROTOCOL    *FHand,
  IN OUT UINTN                *BufferSize,
     OUT VOID                 *Buffer
  )
/*++

Routine Description:

  Get the file info.

Arguments:

  FHand                 - The handle of the file.
  BufferSize            - Size of Buffer.
  Buffer                - Buffer containing read data.

Returns:

  EFI_SUCCESS           - Get the file info successfully.
  EFI_DEVICE_ERROR      - Can not find the OFile for the file.
  EFI_VOLUME_CORRUPTED  - The file type of open file is error.
  other                 - An error occurred when operation the disk.

--*/
;

EFI_STATUS
EFIAPI
NtfsWriteFile (
  IN     EFI_FILE_PROTOCOL      *FHand,
  IN OUT UINTN                  *BufferSize,
  IN     VOID                   *Buffer
  )
/*++

Routine Description:

  Set the file info.

Arguments:

  FHand                 - The handle of the file.
  BufferSize            - Size of Buffer.
  Buffer                - Buffer containing write data.

Returns:

  EFI_SUCCESS           - Set the file info successfully.
  EFI_WRITE_PROTECTED   - The disk is write protected.
  EFI_ACCESS_DENIED     - The file is read-only.
  EFI_DEVICE_ERROR      - The OFile is not valid.
  EFI_UNSUPPORTED       - The open file is not a file.
                        - The writing file size is larger than 4GB.
  other                 - An error occurred when operation the disk.

--*/
;


EFI_STATUS EFIAPI 
Ntfs_inode_to_FileHandle(
  IN ntfs_inode *inode, 
  OUT EFI_FILE **NewFileHandle);

//
EFI_STATUS
NtfsAllocateVolume (
  IN  EFI_HANDLE                Handle,
  IN  EFI_DISK_IO_PROTOCOL      *DiskIo,
  IN  EFI_BLOCK_IO_PROTOCOL     *BlockIo
  );

EFI_STATUS NtfsReadDirectory(IN NTFS_IFILE *File,
                            IN OUT UINTN *BufferSize,
                            OUT EFI_FILE_INFO *FileInfo);
//
// Misc.c
//
VOID
NtfsAcquireLock (
  VOID
  );

VOID
NtfsReleaseLock (
  VOID
  );

EFI_STATUS
NtfsAcquireLockOrFail (
  VOID
  );

VOID 
	fsw_efi_decode_time(
		OUT EFI_TIME *EfiTime, IN UINT32 UnixTime
	);

EFI_STATUS
NtfsAbandonVolume (
  IN NTFS_VOLUME *Volume
  );

//
// Open.c
//

//
// OpenVolume.c
//
EFI_STATUS
EFIAPI
NtfsOpenVolume (
  IN  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This,
  OUT EFI_FILE_PROTOCOL               **File
  );


// Handle.c
UINTN EFIAPI CreateFileName(CHAR8 *Destination, CHAR8 *Path, CHAR8 *FileName);
EFI_STATUS EFIAPI Ntfs_Deallocate(NTFS_IFILE	*IFile);

//
// Global Variables
//
extern EFI_DRIVER_BINDING_PROTOCOL     gNtfsDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL     gNtfsComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL    gNtfsComponentName2;
extern EFI_LOCK                        NtfsFsLock;
extern EFI_FILE_PROTOCOL               NtfsFileInterface;

#endif
