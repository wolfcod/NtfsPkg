 /*++
Module Name:

  ReadWrite.c

Abstract:

  Functions that perform file read/write

Revision History

--*/

#include "Ntfs.h"
#include "ntfs/ntfsfile.h"
#include "ntfs/ntfsdir.h"
#include "fcntl.h"

EFI_STATUS
EFIAPI
NtfsReadFile (
  IN     EFI_FILE_PROTOCOL  *FHand,
  IN OUT UINTN              *BufferSize,
     OUT VOID               *Buffer
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
{
	NTFS_IFILE *IFile;
	EFI_STATUS Status;
	struct _reent r;
	int rbytes;

	IFile = IFILE_FROM_FHAND(FHand);	// cast from "EFI" to FHAND

	if (IFile->Type == FSW_EFI_FILE_TYPE_FILE)
	{
		ZeroMem(&r, sizeof(struct _reent));
		rbytes = ntfs_read_r(&r, IFile->state.file, Buffer, *BufferSize);

		if (rbytes >= 0)
		{
			*BufferSize = rbytes;
			return EFI_SUCCESS;
		}
			
		*BufferSize = 0;
		return EFI_DEVICE_ERROR;
	}
	else if (IFile->Type == FSW_EFI_FILE_TYPE_DIR)
	{	// unimplemented!
		Status = NtfsReadDirectory(IFile, BufferSize, Buffer);
		return Status;
	}
	else
	{	// what's?
	}
	return EFI_DEVICE_ERROR;
  
}
