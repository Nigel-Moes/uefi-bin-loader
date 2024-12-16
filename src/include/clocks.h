#ifndef CLOCKS_H
#define CLOCKS_H

// Include UEFI EDK2 header files
#include <Library/UefiLib.h>
#include <Protocol/ArmScmiClockProtocol.h>
#include <Protocol/ArmScmiClock2Protocol.h>
#include <Uefi.h>

// Declare function prototypes
VOID PrintClockInfo(IN SCMI_CLOCK_PROTOCOL*, IN UINT32);
EFI_STATUS ConfigureClocks(IN EFI_BOOT_SERVICES*);

#endif
