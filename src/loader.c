#include "include/loader.h"

// UEFI main
EFI_STATUS
EFIAPI
UefiMain (
	IN EFI_HANDLE		ImageHandle,
	IN EFI_SYSTEM_TABLE	*SystemTable
	) 
{
	EFI_STATUS				Status;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL 	*FileSystem;
	EFI_FILE_PROTOCOL			*Root;
	EFI_FILE_PROTOCOL			*FileHandle;
	EFI_FILE_INFO				*FileInfo;
	UINTN					FileInfoSize = 0;
	VOID					*Buffer = NULL;
	
	// Locate the simple file system protocol
    	Status = gBS->LocateProtocol(
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        (VOID**)&FileSystem
    	);	
	if (EFI_ERROR(Status)) 
	{
		Print(L"Failed to locate file system protocol: %r\n", Status);
  		return Status;
	}
	
	// Open root volume
	Status = FileSystem->OpenVolume(FileSystem, &Root);	
	if (EFI_ERROR(Status)) 
	{
		Print(L"Failed to open root volume: %r\n", Status);
  		return Status;
	}
	
	// Open the file
	Status = Root->Open(
		Root, 
		&FileHandle, 
		FILE_PATH, 
		EFI_FILE_MODE_READ,
		0
		);
	if (EFI_ERROR(Status)) 
	{
		Print(L"Failed to open file: %r\n", Status);
  		return Status;
	}
		
	// Get the file info size
	Status = FileHandle->GetInfo(
		FileHandle,
		&gEfiFileInfoGuid,
		&FileInfoSize,
		NULL
	);
	if (Status == EFI_BUFFER_TOO_SMALL) {
		// Allocate memory for file info
		FileInfo = AllocatePool(FileInfoSize);
		if (FileInfo == NULL) {
			Print(L"Failed to allocate memory for file info: %r\n", Status);
			return EFI_OUT_OF_RESOURCES;
		}
	
		// Get the file info
		Status = FileHandle->GetInfo(
			FileHandle,
			&gEfiFileInfoGuid,
			&FileInfoSize,
			FileInfo
		);
		if (EFI_ERROR(Status)) 
		{
			Print(L"Failed to get file info: %r\n", Status);
			FreePool(FileInfo);
	  		return Status;
		}
	}
	else {
		Print(L"Failed to get file info size: %r\n", Status);
	}
	
	// Allocate memory for the file
	Status = gBS->AllocatePool(
		EfiLoaderData,
		FileInfo->FileSize,
		&Buffer		
	);
	if (EFI_ERROR(Status)) 
	{
		Print(L"Failed to allocate memory for file: %r\n", Status);
		FreePool(FileInfo);
  		return Status;
	}
	
	// Read the file into memory
	Status = FileHandle->Read(
		FileHandle,
		&FileInfo->FileSize,
		Buffer
	);
	if (EFI_ERROR(Status)) 
	{
		Print(L"Failed to read file: %r\n", Status);
		FreePool(Buffer);
		FreePool(FileInfo);
  		return Status;
	}
	
	// Close the file
	FileHandle->Close(FileHandle);
	
	// Cast the buffer to a function pointer and call it
	EntryPoint theEntryPoint = (EntryPoint)Buffer;
	theEntryPoint();
	
	// Free allocated memory
	FreePool(Buffer);
	FreePool(FileInfo);
	
	return Status;
}

