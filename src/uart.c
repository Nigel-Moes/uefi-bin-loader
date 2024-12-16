#include "include/uart.h"

VOID PrintSerialIoSettings(IN EFI_SERIAL_IO_PROTOCOL *SerialIo) {
	// Define EFI_SERIAL_IO_PROTOCOL instance settings
	UINT32 ControlMask = SerialIo->Mode->ControlMask;
	UINT32 Timeout = SerialIo->Mode->Timeout;
	UINT64 BaudRate = SerialIo->Mode->BaudRate;
	UINT32 ReceiveFifoDepth = SerialIo->Mode->ReceiveFifoDepth;
	UINT32 DataBits = SerialIo->Mode->DataBits;
	UINT32 Parity = SerialIo->Mode->Parity;
	UINT32 StopBits = SerialIo->Mode->StopBits;

	// Print the settings
	Print(L"ControlMask: %d\n", ControlMask);
	Print(L"Timeout: %d\n", Timeout);
	Print(L"BaudRate: %d\n", BaudRate);
	Print(L"ReceiveFifoDepth: %d\n", ReceiveFifoDepth);
	Print(L"DataBits: %d\n", DataBits);
	Print(L"Parity: %d\n", Parity);
	Print(L"StopBits: %d\n", StopBits);
}

EFI_STATUS WriteSerialIo(IN EFI_SERIAL_IO_PROTOCOL *SerialIo, IN CHAR8 *Message) {
	EFI_STATUS Status;
	UINTN BufferSize = AsciiStrLen(Message);
	Status = SerialIo->Write(SerialIo, &BufferSize, Message);
	if (EFI_ERROR(Status)) { Print(L"Failed to write message to serial I/O protocol instance: %r\n", Status); return Status; }
	return Status;
}

EFI_STATUS GetSerialIoHandles(IN EFI_BOOT_SERVICES *BootServices, OUT EFI_HANDLE **HandleBuffer) {
    EFI_STATUS Status;
	UINTN HandleCount;
	EFI_HANDLE *LocalBuffer;

	// Retrieve the array of handles that support the EFI_SERIAL_IO_PROTOCOL and place them in the handle buffer
	Status = BootServices->LocateHandleBuffer(ByProtocol, &gEfiSerialIoProtocolGuid, NULL, &HandleCount, &LocalBuffer);
	if (EFI_ERROR(Status)) { Print(L"Failed to locate handles for EFI_SERIAL_IO_PROTOCOL: %r\n", Status); return Status; }

	// Iterate through the handle buffer
	for (UINTN Index = 0; Index < HandleCount; Index++)
	{
		// Get the handle from the handle buffer
		EFI_HANDLE Handle = LocalBuffer[Index];
		Print(L"Handle %p supports the EFI_SERIAL_IO_PROTOCOL\n", Handle);

		// Get the EFI_SERIAL_IO_PROTOCOL instance
		EFI_SERIAL_IO_PROTOCOL *SerialIo;
		Status = BootServices->HandleProtocol(Handle, &gEfiSerialIoProtocolGuid, (VOID **)&SerialIo);
		if (EFI_ERROR(Status)) { Print(L"Failed to locate EFI_SERIAL_IO_PROTOCOL: %r\n", Status); return Status; }

		// Print the settings of the EFI_SERIAL_IO_PROTOCOL instance
		PrintSerialIoSettings(SerialIo);

		// Write a message to the serial I/O protocol instance
		CHAR8 *Message = "TEST\n";
		Status = WriteSerialIo(SerialIo, Message);
		if (EFI_ERROR(Status)) { Print(L"Failed to write test message: %r\n", Status); return Status; }
	}

	// Assign the handle buffer to the output parameter
	*HandleBuffer = LocalBuffer;

	return Status;
}

EFI_STATUS UninstallSerialIoProtocol(IN EFI_BOOT_SERVICES *BootServices, IN EFI_HANDLE **HandleBuffer, IN UINTN HandleIndex) {
	EFI_STATUS Status;
	EFI_HANDLE *LocalBuffer = *HandleBuffer;
	EFI_HANDLE Handle = LocalBuffer[HandleIndex];

	// Get the EFI_SERIAL_IO_PROTOCOL instance
	EFI_SERIAL_IO_PROTOCOL *SerialIo;
	Status = BootServices->HandleProtocol(Handle, &gEfiSerialIoProtocolGuid, (VOID **)&SerialIo);
	if (EFI_ERROR(Status)) { Print(L"Failed to locate EFI_SERIAL_IO_PROTOCOL: %r\n", Status); return Status; }
	
	// Uninstall the EFI_SERIAL_IO_PROTOCOL instance
    Status = BootServices->UninstallMultipleProtocolInterfaces(Handle, &gEfiSerialIoProtocolGuid, SerialIo);
	if (EFI_ERROR(Status)) { Print(L"Failed to uninstall EFI_SERIAL_IO_PROTOCOL: %r\n", Status); return Status; }

	return Status;
}
