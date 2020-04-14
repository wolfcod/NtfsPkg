/*++

Copyright (c) 2005 - 2007, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the Software
License Agreement which accompanies this distribution.


Module Name:

  DirectoryManage.c

Abstract:

  Functions for performing directory entry io

Revision History

--*/

#include "Ntfs.h"
#include "ntfs/ntfsdir.h"
#include "ntfs/types.h"
#include "ntfs/mft.h"
#include "ntfs/attrib.h"
#include "ntfs/layout.h"
#include "ntfs/inode.h"
#include "ntfs/utils.h"
#include "ntfs/dir.h"
#include "ntfs/list.h"
#include "ntfs/ntfstime.h"
/* #include "version.h" */
#include "logging.h"


void ntfs_to_efitime(EFI_TIME *EfiTime, ntfs_time ntfstime)
{
	struct timespec spec;
	s64 cputime;

	cputime = sle64_to_cpu(ntfstime);
	spec.tv_sec = (cputime - (NTFS_TIME_OFFSET)) / 10000000;
	spec.tv_nsec = (cputime - (NTFS_TIME_OFFSET)
			- (s64)spec.tv_sec*10000000)*100;
		/* force zero nsec for overflowing dates */
	if ((spec.tv_nsec < 0) || (spec.tv_nsec > 999999999))
		spec.tv_nsec = 0;
		
	fsw_efi_decode_time(EfiTime, spec.tv_sec);
}

EFI_STATUS fsw_efi_dir_read(IN NTFS_IFILE *File,
                            IN OUT UINTN *BufferSize,
                            OUT EFI_FILE_INFO *FileInfo)
{
	EFI_STATUS Status;
	NTFS_VOLUME	*Volume = File->Volume;
	struct _reent r;
	struct stat filestat;
	ntfs_dir_state *dir;
	ntfs_inode *inode;
	UINTN RequiredSize;
	ntfs_attr *data_na;
	UINTN FileNameLength;

	ZeroMem(&r, sizeof(struct _reent));
	
//	Print(L"fsw_efi_dir_read\n\r");

	if (File->Position < 0)	{
		*BufferSize = 0;
		return EFI_NOT_FOUND;
	}

	dir = File->state.dir;

	if (dir == NULL)
	{
		File->state.dir = AllocateZeroPool(sizeof(ntfs_dir_state));
		dir = File->state.dir;

		r.dummy = 0;
		r._errno = 0;

		dir->ni = File->inode;
		dir->vd = File->Volume->vd;

		ntfs_diropen_r(&r, dir, NULL);
	}

	if (dir->current == NULL)
	{
		r.dummy = 0;
		r._errno = 0;

		if (File->inode == NULL)
		{
			return EFI_DEVICE_ERROR;
		}

		if (dir->ni == NULL)
		{	// first fetch
			dir->ni = File->inode;
			dir->vd = Volume->vd;
			ntfs_diropen_r(&r, dir, NULL);
		}

		if (dir->current == NULL)
		{
			ZeroMem(FileInfo, sizeof(EFI_FILE_INFO));
			*BufferSize = 0;
			return EFI_SUCCESS;
		}
		
	}

	FileNameLength = strlen(dir->current->name) + 1;
	RequiredSize = sizeof(EFI_FILE_INFO) + (FileNameLength * sizeof(CHAR16));

	if (*BufferSize < RequiredSize)
	{
		*BufferSize = RequiredSize;
		return EFI_BUFFER_TOO_SMALL;
	}

	ZeroMem(FileInfo, RequiredSize);

	inode = ntfs_inode_open(File->Volume->vol, dir->current->mref);	
	
	// check if mft_no is under "FILE_first_user"
	if (inode->mft_no < FILE_first_user)
	{
		FileInfo->Attribute |= EFI_FILE_SYSTEM;	
		FileInfo->Attribute |= EFI_FILE_READ_ONLY;
	}

	if (inode->flags & MFT_RECORD_IS_DIRECTORY)
	{
		FileInfo->Attribute |= EFI_FILE_DIRECTORY;
		FileInfo->Attribute |= EFI_FILE_READ_ONLY;
	}
	else
	{
		//FileInfo->Attribute |= EFI_FILE_ARCHIVE;
	}

	if (inode->flags & FILE_ATTR_READONLY)
		FileInfo->Attribute |= EFI_FILE_READ_ONLY;

	if (inode->flags & FILE_ATTR_HIDDEN)
		FileInfo->Attribute |= EFI_FILE_HIDDEN;

	if (inode->flags & FILE_ATTR_SYSTEM)
		FileInfo->Attribute |= EFI_FILE_SYSTEM;
	
	if (inode->flags & FILE_ATTR_ARCHIVE)
		FileInfo->Attribute |= EFI_FILE_ARCHIVE;

#ifdef DISABLE_NEW_DEPRECATED_INTERFACES
	AsciiStrToUnicodeStrS(dir->current->name, FileInfo->FileName, FileNameLength);
#else
	AsciiStrToUnicodeStr(dir->current->name, FileInfo->FileName);
#endif
	data_na = ntfs_attr_open(inode, AT_DATA, AT_UNNAMED, 0);

	FileInfo->PhysicalSize = inode->allocated_size;

	FileInfo->FileSize = data_na->data_size;
	FileInfo->PhysicalSize = inode->data_size;

	ntfs_to_efitime(&FileInfo->CreateTime, inode->creation_time);
	ntfs_to_efitime(&FileInfo->ModificationTime, inode->last_data_change_time);
	ntfs_to_efitime(&FileInfo->LastAccessTime, inode->last_access_time);

	ntfs_attr_close(data_na);

	FileInfo->Size = RequiredSize;		// required size (Size of the EFI_FILE_INFO structure)
		
	//FileInfo->CreateTime = 0;
	// close inode
	ntfs_inode_close(inode);

	if (ntfs_dirnext_r(&r, File->state.dir, dir->current->name, &filestat) == -1)
		File->Position = -1;	// move to next position!
	else
		File->Position++;

	*BufferSize = RequiredSize;

	return EFI_SUCCESS;
}
