## NTFS Driver (read/write)

This project is an UEFI Driver extensions based on source code leaked from `]HackingTeam[`
The idea is to refactoring the code, upgrade to EDK II and to support new compilers;

## Instruction

To compile this code on your EDK project you must include this package into platform.


Add line NtfsPkg/NtfsDxe/Ntfs.inf into DSC platform file in *Components* Section

## Build Instruction

This is first import of project, and will be on Windows platform using the following command

```c
build -p YourPlatformProject\YourPlatformProjectArch.dsc -a [IA32|X64] -m NtfsPkg\NtfsDxe\Ntfs.inf
```

Note: The project was compiled with Microsoft Visual Studio C++ 2008, 2010 and 2019. Build from Developer Prompt.
Note 2: You can add -D _NTFS_READONLY to build command line, to avoid any kind of write operations on disk.. The symbol
disable ntfs_write_xx operations on disk.


## Unsupported platform

Actually, to compile the project with MS C++ build, a lot of "attributes" has been removed from ntfspkg structures, so the only
platform where is possible to build it is Windows. 

## NTFS3G

The project is based on NTFS3G, some elements has been removed and other stuff has been added.
