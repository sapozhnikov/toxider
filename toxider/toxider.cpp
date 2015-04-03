// toxider.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <mutex>

#include "Worker.h"

#pragma comment(lib, "advapi32.lib")

FILE *resultFile = NULL;
Worker *pWorker;
std::mutex mutexConsole;
std::mutex mutexFile;

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		printf("Finishing work\n");
		delete[] pWorker;
		
		if (resultFile)
			fclose(resultFile);
		exit(0);
	default:
		return FALSE;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	int errNo = 0;

	if (fopen_s(&resultFile, "toxider.out", "a+") != 0)
	{
		std::cout << "Can't open file for writing";
		errNo = 1;
		goto finish;
	}

	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
	{
		std::cout << "Could not set control handler\n";
	}

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	DWORD numCPU = sysinfo.dwNumberOfProcessors;
	if (numCPU == 0)
		numCPU = 1;

	printf("Running on %d CPU cores\n", numCPU);

	pWorker = new Worker[numCPU];

	for (DWORD i = 0; i < numCPU; i++)
		pWorker[i].StartCrunch();

	Sleep(INFINITE);

finish:

	return errNo;
}
