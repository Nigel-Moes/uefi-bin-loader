#ifndef LOADER_H
#define LOADER_H

// Include UEFI EDK II header files
#include <Guid/FileInfo.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Uefi.h>

// Define the path to the binary file on the bootable media
#define FILE_PATH L"\\file.bin"

// Define the index of the debug UART
#define UART_INDEX 1u

// Define the type of the entry point function in the binary file
typedef VOID (*EntryPoint)();

#endif
