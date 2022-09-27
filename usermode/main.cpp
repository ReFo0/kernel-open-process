#include <iostream>
#include "driver.h"



int main() {
	driver = CreateFileA("\\\\.\\refodrv", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	auto pid = process_id("notepad.exe");
	auto base = drv.baseaddress(pid);
	std::cout << "[?]base_adress : " << base << "\n";
	auto access = drv.openprocess(pid, PROCESS_ALL_ACCESS); 
	std::cout << "[?]access mask : " << access << "\n";
	std::cin.get();
	return 0;
}