/*++
Module Name:

  flush.c

Abstract:

  Routines that check references and flush OFiles

Revision History

--*/

#include "Ntfs.h"
#include "ntfs/ntfsdir.h"
#include "ntfs/ntfsfile.h"

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
{
	NTFS_IFILE *IFile;
	EFI_STATUS Status;
	struct _reent r;

	IFile = IFILE_FROM_FHAND(FHand);	// cast from "EFI" to FHAND

	if (IFile->Type == FSW_EFI_FILE_TYPE_FILE)
	{
		ZeroMem(&r, sizeof(struct _reent));
		ntfs_close_r(&r, IFile->state.file);

		Status = EFI_SUCCESS;
	}
	else if (IFile->Type == FSW_EFI_FILE_TYPE_DIR)
	{	// unimplemented!
		Status = EFI_SUCCESS;
		ntfsCloseEntry(IFile->Volume->vd, IFile->inode);
	} 
	else
		Status = EFI_INVALID_PARAMETER;

	Ntfs_Deallocate(IFile);

	if (Status == EFI_SUCCESS)
		FreePool(IFile);

	return Status;
}

