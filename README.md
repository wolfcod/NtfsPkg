## NTFS Driver (read/write)

This project is an UEFI Driver extensions based on source code leaked from ]HackingTeam[
The idea is to refactoring the code, upgrade to EDK II and to support new compilers;

## Instruction

To compile this code on your EDK project you must include into your platform

Add line NtfsPkg/NtfsDxe/Ntfs.inf into DSC platform file in *Components* Section

## Build Instruction

This is first import of project, and will be on Windows platform using the following command

```c
build -p YourPlatformProject\YourPlatformProjectArch.dsc -a [IA32|X64] -m NtfsPkg\NtfsDxe\Ntfs.inf
```


