 /*++
Module Name:

  NtfsSetPosition.c

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
NtfsSetPositionFile (
  IN EFI_FILE_PROTOCOL  *FHand,
  IN UINT64             Position
  )
/*++

Routine Description:

  Set the file's position of the file.

Arguments:

  FHand                 - The handle of file.
  Position              - The file's position of the file.

Returns:

  EFI_SUCCESS           - Set the info successfully.
  EFI_DEVICE_ERROR      - Can not find the OFile for the file.
  EFI_UNSUPPORTED       - Set a directory with a not-zero position.

--*/
{
	NTFS_IFILE *IFile;
	struct _reent r;
	int position;

	IFile = IFILE_FROM_FHAND(FHand);

	if (IFile->Type == FSW_EFI_FILE_TYPE_FILE)
	{
		ZeroMem(&r, sizeof(struct _reent));
		position = (int) Position;
		position = ntfs_seek_r(&r, IFile->state.file, position, SEEK_SET);
		//*Position = position;
		return EFI_SUCCESS;
	}
	else if (IFile->Type == FSW_EFI_FILE_TYPE_DIR)
	{	// on directory?!?!
		if (Position != 0)
		{
			return EFI_UNSUPPORTED;
		}
		//*Position = 0;
		IFile->Position = 0;
	}

	return EFI_SUCCESS;
}
