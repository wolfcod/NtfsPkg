/*++
Module Name:

  open.c

Abstract:

  Routines dealing with file open

Revision History

--*/

#include <sys/stat.h>
#include <fcntl.h>
#include "Ntfs.h"
#include "ntfs/ntfsdir.h"
#include "ntfs/ntfsfile.h"

EFI_STATUS
EFIAPI
NtfsOpenFile (
  IN  EFI_FILE_PROTOCOL   *FHand,
  OUT EFI_FILE_PROTOCOL   **NewHandle,
  IN  CHAR16              *FileName,
  IN  UINT64              OpenMode,
  IN  UINT64              Attributes
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
{
	NTFS_IFILE   *IFile;
	NTFS_IFILE   *NewIFile;
	EFI_STATUS  Status;
	ntfs_inode	*inode;
	NTFS_VOLUME *Volume;
	struct _reent r;
	CHAR8	AsciiFileName[260], TempPath[260];
	int flags, mode, FileNameSize;
	CHAR8	*LastSeparator;

	//
	// Perform some parameter checking
	//
	if (FileName == NULL || (NewHandle == NULL)) {
		return EFI_INVALID_PARAMETER;
	}

	IFile = IFILE_FROM_FHAND (FHand);

	memset(AsciiFileName, 0, 260);
	memset(TempPath, 0, 260);

#ifdef DISABLE_NEW_DEPRECATED_INTERFACES
	UnicodeStrToAsciiStrS(FileName, TempPath, 260);	// local name
#else
	UnicodeStrToAsciiStr(FileName, TempPath);	// local name
#endif
	
	FileNameSize = CreateFileName(AsciiFileName, IFile->FullPath, TempPath);
 
  //
  // Check for a valid mode
  //
  switch (OpenMode) {
	  case EFI_FILE_MODE_READ:
		  flags = O_RDONLY; mode = 0; break;
	  case EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE:
		  flags = O_RDWR; mode = 0; break;
	  case EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE:
		  flags = O_CREAT | O_RDWR; mode = 0; break;
		break;

	  default:
		  flags = 0; mode = 0;
		return EFI_INVALID_PARAMETER;
  }
  
  //
  // Check for valid Attributes for file creation case. 
  //
  if (((OpenMode & EFI_FILE_MODE_CREATE) != 0) && (Attributes & (EFI_FILE_READ_ONLY | (~EFI_FILE_VALID_ATTR))) != 0)
  {
	return EFI_INVALID_PARAMETER;
  }
  
  Volume = IFile->Volume;
  
  //
  // Lock
  //
  NtfsAcquireLock ();
  
	if (IFile->Type == FSW_EFI_FILE_TYPE_DIR)
	{
		IFile->Volume->vd->cwd_ni = IFile->inode;	// set root parent
	}

	if ((OpenMode & EFI_FILE_MODE_CREATE))
	{	// Create a directory or a file...
		unsigned int type = (Attributes & EFI_FILE_DIRECTORY) ? S_IFDIR : S_IFREG;
		inode = ntfsCreate(IFile->Volume->vd, AsciiFileName, type, NULL);

		if (inode == NULL && type == S_IFREG) { // file exists.. try to open!
			inode = ntfsOpenEntry(IFile->Volume->vd, AsciiFileName);
		}
	}
	else if ((AsciiStrCmp(AsciiFileName, "\\" ) == 0) || (AsciiStrCmp(AsciiFileName, "/") == 0) || (AsciiStrCmp(AsciiFileName, "") == 0) ||  (AsciiStrCmp(AsciiFileName, ".") == 0))
	{	// ? we have a pointer to root! why we need to reopen?
		inode = ntfs_inode_open(Volume->vd->vol, FILE_root);	// access to root!
	}
	else
	{	// try to open ...
		inode = ntfsOpenEntry(IFile->Volume->vd, AsciiFileName);
	}

	if (inode != NULL)
	{	// return a valid handle!
		Ntfs_inode_to_FileHandle(inode, NewHandle);

		NewIFile = IFILE_FROM_FHAND(*NewHandle);
		
		CopyMem(NewIFile->FullPath, AsciiFileName, FileNameSize * sizeof(CHAR8));

		if (AsciiStrCmp(TempPath, ".") == 0 || AsciiStrCmp(TempPath, "..") == 0)
		{
			CopyMem(NewIFile->FileName, AsciiFileName, FileNameSize * sizeof(CHAR8));
		}
		else
		{
			CopyMem(NewIFile->FileName, TempPath, AsciiStrLen(TempPath) * sizeof(CHAR8));
		}

		NewIFile->Position = 0;
		NewIFile->Volume = Volume;
		if (inode->mft_no == FILE_root)
		{	// mark as root directory!
			NewIFile->RootDir = TRUE;
		}
		NewIFile->inode = inode;			//	current element
		NewIFile->dir_ni = IFile->inode;	//	root!

		if (NewIFile->Type == FSW_EFI_FILE_TYPE_FILE)
		{	// open file
			NewIFile->state.file = AllocateZeroPool(sizeof(struct _ntfs_file_state));

			NewIFile->state.file->vd = Volume->vd;	// sete reference
			NewIFile->state.file->ni = inode;
			ntfs_open_r(&r, NewIFile->state.file, AsciiFileName, flags, 0);
		}
		else if (NewIFile->Type == FSW_EFI_FILE_TYPE_DIR)
		{	//
			NewIFile->state.dir  = AllocateZeroPool(sizeof(ntfs_dir_state));
		}

		Status = EFI_SUCCESS;
	} 
	else 
	{
		Status = EFI_INVALID_PARAMETER;
		*NewHandle = INVALID_HANDLE_VALUE;
	}
	
	NtfsReleaseLock();
  	return Status;
}
