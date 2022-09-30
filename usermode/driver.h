#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>

#define open_process CTL_CODE(FILE_DEVICE_UNKNOWN, 0x186, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

struct req {
	uintptr_t process;
	uintptr_t address;
	DWORD access;
};

HANDLE driver;
uintptr_t Process;
class kernel {
public:

	uint16_t openprocess(const uintptr_t pid, const DWORD access) {
		req request;
		request.process = pid;
		request.access = access;
		DeviceIoControl(driver, open_process, &request, sizeof(request), &request, sizeof(request), 0, 0);
		return request.address;
	}
};

std::uintptr_t process_id(const std::string& name)
{
	const auto snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snap == INVALID_HANDLE_VALUE) {
		return 0;
	}

	PROCESSENTRY32 proc_entry{};
	proc_entry.dwSize = sizeof proc_entry;

	auto found_process = false;
	if (!!Process32First(snap, &proc_entry)) {
		do {
			if (name == proc_entry.szExeFile) {
				found_process = true;
				break;
			}
		} while (!!Process32Next(snap, &proc_entry));
	}

	CloseHandle(snap);
	return found_process
		? proc_entry.th32ProcessID
		: 0;
}

kernel drv;
