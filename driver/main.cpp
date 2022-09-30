#include "export.h"

#define open_process CTL_CODE(FILE_DEVICE_UNKNOWN, 0x186, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define base_adresss CTL_CODE(FILE_DEVICE_UNKNOWN, 0x187, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

struct req {
	uintptr_t process;
	uintptr_t address;
	DWORD access;
};

NTSTATUS command(PDEVICE_OBJECT device_obj, PIRP irp) {
	auto stacklocation = IoGetCurrentIrpStackLocation(irp);
	auto buffer = (req*)irp->AssociatedIrp.SystemBuffer;

	if (stacklocation) {
		if (buffer && sizeof(*buffer) >= sizeof(req)) {
			const auto ctl_code = stacklocation->Parameters.DeviceIoControl.IoControlCode;
			if (ctl_code == open_process) {
				PEPROCESS process = nullptr;
				HANDLE pid = 0;
				OBJECT_ATTRIBUTES attr;
				InitializeObjectAttributes(&attr, NULL, 0, NULL, NULL);
				auto status = PsLookupProcessByProcessId(HANDLE(buffer->process), &process);

				if (!NT_SUCCESS(status))
					return 0;

				CLIENT_ID client_id;
				client_id.UniqueProcess = (HANDLE)buffer->process;
				client_id.UniqueThread = (HANDLE)0;
				buffer->address = ZwOpenProcess(&pid, buffer->access, &attr, &client_id);
			}
		}
	}

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}


NTSTATUS driver_object(PDRIVER_OBJECT driver, PUNICODE_STRING registery_path) {
	auto status = STATUS_SUCCESS;
	UNICODE_STRING sym_link, dev_name;
	PDEVICE_OBJECT dev_obj;

	RtlInitUnicodeString(&dev_name, L"\\Device\\refodrv");
	status = IoCreateDevice(driver, 0, &dev_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &dev_obj);

	if (status != STATUS_SUCCESS) {
		return status;
	}

	RtlInitUnicodeString(&sym_link, L"\\DosDevices\\refodrv");
	status = IoCreateSymbolicLink(&sym_link, &dev_name);

	if (status != STATUS_SUCCESS) {
		return status;
	}

	dev_obj->Flags |= DO_BUFFERED_IO;

	for (int t = 0; t <= IRP_MJ_MAXIMUM_FUNCTION; t++)
		driver->MajorFunction[t] = unsupported_io;

	driver->MajorFunction[IRP_MJ_CREATE] = create_io;
	driver->MajorFunction[IRP_MJ_CLOSE] = close_io;
	driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = command;
	driver->DriverUnload = NULL;

	dev_obj->Flags &= ~DO_DEVICE_INITIALIZING;

	return status;
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT object, PUNICODE_STRING registry)
{
	UNREFERENCED_PARAMETER(object);
	UNREFERENCED_PARAMETER(registry);
	UNICODE_STRING  drv_name;
	RtlInitUnicodeString(&drv_name, L"\\Driver\\refodrv");
	IoCreateDriver(&drv_name, &driver_object);
	return STATUS_SUCCESS;
}
