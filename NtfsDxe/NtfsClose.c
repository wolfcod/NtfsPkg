/*++

Copyright (c) 2005 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the Software
License Agreement which accompanies this distribution.


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
NtfsClose (
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

	if (IFile->state.file != NULL)
	{	// free filestate object or dirstate object
		FreePool(IFile->state.file);
		IFile->state.file = NULL;
	}

	if (Status == EFI_SUCCESS)
		FreePool(IFile);

	return Status;
}

