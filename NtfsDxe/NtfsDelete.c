/*++
Module Name:

  delete.c

Abstract:

  Function that deletes a file

Revision History

--*/

#include "Ntfs.h"

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
{
	NTFS_IFILE	*IFile;
	EFI_STATUS Status;
	u8 name_len;
	ntfs_inode *ni, *dir_ni;
	CHAR16  *unicode;

	IFile = IFILE_FROM_FHAND(FHand);

	// Default error
	Status = EFI_WARN_DELETE_FAILURE;
	
	// check IFile integrity
	if (IFile->inode == NULL || 
		IFile->FileName == NULL) 	// inode not valid.. release mem and exit
		goto free;

	ni = IFile->inode;
	dir_ni = IFile->dir_ni;

	if (ni->mft_no < FILE_first_user)	// cannot remove system file!
		goto free;

	if (ntfsUnlink(IFile->Volume->vd, IFile->FullPath) != 0)
		goto free;

	Status = EFI_SUCCESS;

/*++
	The Delete() function closes and deletes a files. In all cases the file handle is closed.
	If the file cannot be deleted, the warning code EFI_WARN_DELETE_FAILURE is returned, but
	the handle is still closed.
--*/
free:
	if (EFI_ERROR(Status))
	{
		ntfs_inode_close(ni);
	}

	Ntfs_Deallocate(IFile);	// deallocate memory...
exit:
	return Status;
}
