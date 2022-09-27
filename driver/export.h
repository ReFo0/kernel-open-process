#pragma once
#include <ntifs.h>
#include <ntddk.h>
typedef unsigned long       DWORD;
typedef unsigned long long uint64_t;


extern "C"
{
	__declspec(dllimport) NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS* Process);
	__declspec(dllimport) PVOID NTAPI PsGetProcessSectionBaseAddress(PEPROCESS);
	__declspec(dllimport) PVOID NTAPI RtlFindExportedRoutineByName(PVOID, PCCH);
	__declspec(dllimport) PIMAGE_NT_HEADERS NTAPI RtlImageNtHeader(PVOID);
	__declspec(dllimport) NTSTATUS NTAPI ObReferenceObjectByName(PUNICODE_STRING, ULONG, PACCESS_STATE, ACCESS_MASK, POBJECT_TYPE, KPROCESSOR_MODE, PVOID OPTIONAL, PVOID*);
	__declspec(dllimport) NTKERNELAPI NTSTATUS IoCreateDriver(PUNICODE_STRING DriverName, PDRIVER_INITIALIZE InitializationFunction);
	__declspec(dllimport) NTSYSAPI NTSTATUS NTAPI ZwOpenProcess(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId);

}


NTSTATUS unsupported_io(PDEVICE_OBJECT device_obj, PIRP irp) {
	irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return irp->IoStatus.Status;
}

NTSTATUS create_io(PDEVICE_OBJECT device_obj, PIRP irp) {
	UNREFERENCED_PARAMETER(device_obj);

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return irp->IoStatus.Status;
}

NTSTATUS close_io(PDEVICE_OBJECT device_obj, PIRP irp) {
	UNREFERENCED_PARAMETER(device_obj);
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return irp->IoStatus.Status;
}
