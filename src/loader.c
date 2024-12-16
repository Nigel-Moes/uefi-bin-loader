#include "include/clocks.h"
#include "include/loader.h"
#include "include/uart.h"

// UEFI main
EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) 
{
	/*
	// Configure the system clocks
	Status = ConfigureClocks(gBS);
	if (EFI_ERROR(Status)) { Print(L"Failed to configure the clocks: %r\n", Status); }

	// Print memory content
	UINT32 *MemoryLocation = (UINT32*)0x0c280000u;
    Print(L"Memory content at address %p: 0x%x\n", MemoryLocation, *MemoryLocation);
	*/

	EFI_STATUS							Status;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL 	*FileSystem;
	EFI_FILE_PROTOCOL					*Root;
	EFI_FILE_PROTOCOL					*FileHandle;
	EFI_FILE_INFO						*FileInfo;
	UINTN								FileInfoSize = 0;
	VOID								*Buffer = NULL;

	// Locate the simple file system protocol
	Status = gBS->LocateProtocol(&gEfiSimpleFileSystemProtocolGuid, NULL, (VOID**)&FileSystem);	
	if (EFI_ERROR(Status)) { Print(L"Failed to locate file system protocol: %r\n", Status); return Status; }
	
	// Open the root volume
	Status = FileSystem->OpenVolume(FileSystem, &Root);	
	if (EFI_ERROR(Status)) { Print(L"Failed to open root volume: %r\n", Status); return Status; }
	
	// Open the file
	Status = Root->Open(Root, &FileHandle, FILE_PATH, EFI_FILE_MODE_READ, 0);
	if (EFI_ERROR(Status)) { Print(L"Failed to open file: %r\n", Status); return Status; }
		
	// Get the file info size
	Status = FileHandle->GetInfo(FileHandle, &gEfiFileInfoGuid, &FileInfoSize, NULL);
	if (Status == EFI_BUFFER_TOO_SMALL) {
		// Allocate memory for file info
		FileInfo = AllocatePool(FileInfoSize);
		if (FileInfo == NULL) { Print(L"Failed to allocate memory for file info: %r\n", Status); return EFI_OUT_OF_RESOURCES; }
	
		// Get the file info
		Status = FileHandle->GetInfo(FileHandle, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);
		if (EFI_ERROR(Status)) { Print(L"Failed to get file info: %r\n", Status); FreePool(FileInfo); return Status; }
	}
	else {
		Print(L"Failed to get file info size: %r\n", Status); return Status;
	}

	// Allocate memory for the file
	Status = gBS->AllocatePool(EfiLoaderData, FileInfo->FileSize, &Buffer);
	if (EFI_ERROR(Status)) { Print(L"Failed to allocate memory for file: %r\n", Status); FreePool(FileInfo); return Status; }
	
	// Read the file into memory
	Status = FileHandle->Read(FileHandle, &FileInfo->FileSize, Buffer);
	if (EFI_ERROR(Status)) { Print(L"Failed to read file: %r\n", Status); FreePool(Buffer); FreePool(FileInfo); return Status; }
	
	// Close the file
	FileHandle->Close(FileHandle);

	// Free allocated memory
	FreePool(FileInfo);
	
	// Get the memory map
    EFI_MEMORY_DESCRIPTOR 	*MemoryMap = NULL;
    UINTN 					MemoryMapSize = 0;
    UINTN 					MapKey = 0;
    UINTN 					DescriptorSize = 0;
    UINT32 					DescriptorVersion = 0;

    // First call GetMemoryMap to determine the size needed
    Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    if (Status == EFI_BUFFER_TOO_SMALL) {
        // Allocate memory for the memory map
        MemoryMap = (EFI_MEMORY_DESCRIPTOR*)AllocatePool(MemoryMapSize);
        if (MemoryMap == NULL) { Print(L"Failed to allocate memory for memory map.\n"); return EFI_OUT_OF_RESOURCES; }

        // Now call GetMemoryMap again with a valid buffer
        Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
        if (EFI_ERROR(Status)) { Print(L"Failed to get memory map: %r\n", Status); return Status;}
    } 
	else if (EFI_ERROR(Status)) {
        Print(L"Failed to get memory map: %r\n", Status); return Status;
    }

	// Get the serial I/O handles
	EFI_HANDLE *HandleBuffer;
	Status = GetSerialIoHandles(gBS, &HandleBuffer);
	if (EFI_ERROR(Status)) { Print(L"Failed to get the handles : %r\n", Status); }

	// Disable the debug UART
	Status = UninstallSerialIoProtocol(gBS, &HandleBuffer, UART_INDEX);
	if (EFI_ERROR(Status)) { Print(L"Failed to uninstall the serial I/O protocol: %r\n", Status); }

	// Free the handle buffer
	FreePool(HandleBuffer);

	// Exit boot services
    // Status = gBS->ExitBootServices(ImageHandle, MapKey);
    // if (EFI_ERROR(Status)) { return Status; }

	// Cast the buffer to a function pointer
	EntryPoint theEntryPoint = (EntryPoint)Buffer;

	// Jump to the entry point
	theEntryPoint();

	return Status;
}
