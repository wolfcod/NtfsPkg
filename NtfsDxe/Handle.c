/*++
Module Name:

  Handle.c

Abstract:

  Handle functions of Simple File System Protocol

Revision History

--*/

#include "Ntfs.h"
#include "ntfs/ntfsdir.h"

UINTN EFIAPI CreateFileName(CHAR8 *Destination, CHAR8 *Path, CHAR8 *FileName)
{
	CHAR8 *Ptr, *ClearPtr;
	BOOL bReset;

	Ptr = Destination;
	ClearPtr = Destination;

	
	while(*Path != 0x00)
	{
		*Destination++ = *Path++;
	}

	Destination--;

	if (AsciiStrCmp(FileName, ".") == 0)
	{
		Destination++;
		goto end;
	}

	if (AsciiStrCmp(FileName, "..") == 0)
	{	
		bReset = TRUE;
		Destination++;
		*Destination = 0x00;

		while(Destination > Ptr && bReset)
		{
			if (*Destination == '\\')
				bReset = FALSE;

			*Destination-- = 0x00;
		}
		Destination++;
		goto end;
	}

	if (strlen(FileName) > 0)
		while(*Path == *FileName)
		{	// skip path
			Path++;
			FileName++;
		}

	if (*Destination == '\\' && *FileName == '\\')
	{
		while(*FileName != 0x00)
		{
			*Destination++ = *FileName++;
		}
	}
	else
	{
		if (*Destination != '\\' && *FileName != '\\')
		{
			Destination++;
			*Destination = '\\';

		}

		Destination++;

		while(*FileName != 0x00)
		{
			*Destination++ = *FileName++;
		}
	}

end:

	bReset = FALSE;
	while(ClearPtr < Destination && *ClearPtr != 0x00)
	{
		if (ClearPtr[0] == '\\' && ClearPtr[1] == '\\')
		{
			ClearPtr++;
			ClearPtr[0] = ClearPtr[1];
			bReset = TRUE;
		}
		else
		{
			if (bReset)
			{
				ClearPtr[0] = ClearPtr[1];
			}
			ClearPtr++;
		}
	}

	return (UINTN) (Destination - Ptr);
}

static VOID
Ntfs_FileHandle_init(
	EFI_FILE_PROTOCOL *Handle)
{
	Handle->Revision = 0x00010000;
	Handle->Open = NtfsOpenFile;
	Handle->Close = NtfsCloseFile;
	Handle->Delete = NtfsDeleteFile;
	Handle->Read = NtfsReadFile;
	Handle->Write = NtfsWriteFile;
	Handle->GetPosition = NtfsGetPositionFile;
	Handle->SetPosition = NtfsSetPositionFile;
	Handle->GetInfo = NtfsGetInfoFile;
	Handle->SetInfo = NtfsSetInfoFile;
	Handle->Flush = NtfsFlushFile;
}

EFI_STATUS 
EFIAPI
Ntfs_inode_to_FileHandle(
  IN ntfs_inode *inode,
  OUT EFI_FILE **NewFileHandle)
{
	EFI_STATUS		Status;
	NTFS_IFILE		*NewIFile;

	if (inode == NULL)
	{
		return EFI_VOLUME_CORRUPTED;
	}

	NewIFile = AllocateZeroPool(sizeof(NTFS_IFILE));
	NewIFile->Signature = NTFS_IFILE_SIGNATURE;

	Ntfs_FileHandle_init(&NewIFile->Handle);

	NewIFile->inode = inode;	//
	NewIFile->Position = -1;

	memset(NewIFile->FileName, 0, 260);
	memset(NewIFile->FullPath, 0, 260);

	if ((inode->mrec->flags & MFT_RECORD_IS_DIRECTORY) != 0)
	{
		NewIFile->Type = FSW_EFI_FILE_TYPE_DIR;
		NewIFile->Position = 0;
	}
	else
	{
		NewIFile->Type = FSW_EFI_FILE_TYPE_FILE;
		NewIFile->Position = 0;
	}

	*NewFileHandle = &NewIFile->Handle;

	return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
Ntfs_Deallocate(NTFS_IFILE	*IFile)
{
	if (IFile->state.file)	// IFile->state is union.. directory and file have same task
	{
		FreePool(IFile->state.file);
		IFile->state.file = NULL;
	}

	return EFI_SUCCESS;
}