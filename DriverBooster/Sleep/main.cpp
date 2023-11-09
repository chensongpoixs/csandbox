#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <thread>

void work()
{


	while (1)
	{
		OutputDebugString(L"thread join Sleep \n");
		Sleep(5);
		OutputDebugString(L"thread join Sleep \n");
	}
}


int main(int argc, char* argv[])
{



	std::thread thread(work);
	OutputDebugString(L"thread join \n");
	if (thread.joinable())
	{
		OutputDebugString(L"thread join \n");
		thread.join();
		OutputDebugString(L"thread exit \n");
	}

	//printf("main thread exit \n");

	OutputDebugString(L"main thread exit \n");
	while (1)
	{
		Sleep(1);
	}

	//printf("main thread exit \n");
		/*while (1)
	{
		Sleep(300);
	}*/

	return 0;
}
