#ifndef UART_H
#define UART_H

// Include UEFI EDK2 header files
#include <Library/UefiLib.h>
#include <Protocol/SerialIo.h>
#include <Uefi.h>

// Declare function prototypes
VOID PrintSerialIoSettings(IN EFI_SERIAL_IO_PROTOCOL*);
EFI_STATUS WriteSerialIo(IN EFI_SERIAL_IO_PROTOCOL*, IN CHAR8*);
EFI_STATUS GetSerialIoHandles(IN EFI_BOOT_SERVICES*, OUT EFI_HANDLE**);
EFI_STATUS UninstallSerialIoProtocol(IN EFI_BOOT_SERVICES*, IN EFI_HANDLE**, IN UINTN);

#endif
