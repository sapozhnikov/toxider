#include <Windows.h>
#pragma once
#define KEY_LENGTH 32

class Worker
{
	BYTE keyData[KEY_LENGTH];
	byte keyPub[KEY_LENGTH];
	byte keyPriv[KEY_LENGTH];
	bool isGoodKey();
	//byte statistics[16];
	byte keyChar[KEY_LENGTH * 2];

	void ThreadCrunch();
	void SaveGoodKey();
public:
	Worker();
	~Worker();
	bool StartCrunch();
};

