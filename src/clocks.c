#include "include/clocks.h"

VOID PrintClockInfo(IN SCMI_CLOCK_PROTOCOL *ScmiClockProtocol, IN UINT32 ClockId) {
    EFI_STATUS Status;
    BOOLEAN Enabled;
    CHAR8 ClockName[8];
    UINT64 ClockRate;
    
    // Get the clock attributes
    Status = ScmiClockProtocol->GetClockAttributes(ScmiClockProtocol, ClockId, &Enabled, ClockName);
    if (EFI_ERROR(Status)) { Print(L"Failed to get clock attributes of clock device %d: %r\n\n", ClockId, Status); return; }

    // Get the clock rate
    Status = ScmiClockProtocol->RateGet(ScmiClockProtocol, ClockId, &ClockRate);
    if (EFI_ERROR(Status)) { Print(L"Failed to get clock rate of clock device %d: %r\n\n", ClockId, Status); return; }

    // Print the clock information
    Print(L"Clock device %d: %a, %s\n", ClockId, Enabled ? "Enabled" : "Disabled", ClockName);
    Print(L"Clock rate: %lld Hz\n", ClockRate);
    // Print a newline
    Print(L"\n");
}

EFI_STATUS ConfigureClocks(IN EFI_BOOT_SERVICES *BootServices) {
    EFI_STATUS Status;
    SCMI_CLOCK_PROTOCOL *ScmiClockProtocol;
    SCMI_CLOCK2_PROTOCOL *ScmiClock2Protocol;
    UINT32 TotalClocks;

    // Get the SCMI_CLOCK_PROTOCOL instance
    Status = BootServices->LocateProtocol(&gArmScmiClockProtocolGuid, NULL, (VOID**)&ScmiClockProtocol);
    if (EFI_ERROR(Status)) { Print(L"Failed to locate the SCMI_CLOCK_PROTOCOL: %r\n", Status); return Status; }

    // Get the SCMI_CLOCK2_PROTOCOL instance
    Status = BootServices->LocateProtocol(&gArmScmiClock2ProtocolGuid, NULL, (VOID**)&ScmiClock2Protocol);
    if (EFI_ERROR(Status)) { Print(L"Failed to locate the SCMI_CLOCK2_PROTOCOL: %r\n", Status); return Status; }

    // Get and print the total number of clocks
    Status = ScmiClockProtocol->GetTotalClocks(ScmiClockProtocol, &TotalClocks);
    if (EFI_ERROR(Status)) { Print(L"Failed to get the total number of clock devices: %r\n", Status); return Status; }
    Print(L"Total number of clock devices: %d\n", TotalClocks);

    // Iterate through all the clocks
    for (UINT32 ClockId = 0; ClockId <= TotalClocks; ClockId++) {
        PrintClockInfo(ScmiClockProtocol, ClockId);
    }
    
    return Status;
}
