#include <Windows.h>
#include <stdio.h>
#include "../DriverBooster/PriorityBoosterCommon.h"
#include <stdlib.h>

int Error(const char* message)
{
	printf("%s(error = %d)\n", message, GetLastError());
	return 0;
}

int main(int argc, char* argv[])
{

	if (argc < 3)
	{
		printf("Usage£º ClientBooster <threadid> <priority> \n");
		return EXIT_SUCCESS;
	}


	HANDLE hDevice = CreateFile(L"\\\\.\\PriorityBooster", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hDevice)
	{
		return Error("Failed to open device");
	}

	struct ThreadData data;
	data.ThreadId = atoi(argv[1]); //
	data.Priority = atoi(argv[2]);

	DWORD ret;

	BOOL success = DeviceIoControl(hDevice, IOCTL_PRIORITY_BOOSTER_SET_PRIORITY, &data, sizeof(data), NULL, 0, &ret, NULL);
	if (success)
	{
		printf("Priority change succeeded !!!\n");
	}
	else
	{
		Error("Priority change failed !!!");
	}

	CloseHandle(hDevice);
	return EXIT_SUCCESS;
}


