#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <thread>
#include <mutex>
#include "Worker.h"

extern "C" int crypto_box_curve25519xsalsa20poly1305_keypair(unsigned char *pk,	unsigned char *sk);
extern FILE *resultFile;
extern std::mutex mutexConsole;
extern std::mutex mutexFile;

Worker::Worker()
{
	HCRYPTPROV hProvider = 0;
	if (!::CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
	{
		std::cout << "Can't get crypto provider\n";
		return;
	}

	if (!::CryptGenRandom(hProvider, KEY_LENGTH, keyData))
	{
		std::cout << "Can't get random data\n";
		::CryptReleaseContext(hProvider, 0);
		return;
	}
	if (!::CryptReleaseContext(hProvider, 0))
	{
	}

	std::cout << "Init with ";
	for (int i = 0; i < KEY_LENGTH; i++) {
		printf("%02hhX", keyData[i]);
	}
	std::cout << "\n";
}


Worker::~Worker()
{
	std::cout << "Worker stopped" << std::endl;
}

void Worker::ThreadCrunch()
{
	do
	{
		memcpy(keyPriv, keyData, KEY_LENGTH);
		crypto_box_curve25519xsalsa20poly1305_keypair(keyPub, keyPriv);
		if (isGoodKey())
		{
			mutexConsole.lock();
			for (int j = 0; j < KEY_LENGTH; j++)
				printf("%02hhX", keyPub[j]);
			printf("\n");
			mutexConsole.unlock();
			SaveGoodKey();
		}

		//take next key
		for (int i = KEY_LENGTH; i != 0; --i)
		{
			keyData[i - 1] += 1;

			if (keyData[i - 1] != 0)
				break;
		}

	} while (true);
}

bool Worker::StartCrunch()
{
	std::thread thrCrunch(&Worker::ThreadCrunch, this);
	thrCrunch.detach();

	return false;
}

//bool Worker::isGoodKey()
//{
//	for (int i = 0; i < 16; i++)
//		statistics[i] = 0;
//	for (int i = 0; i < KEY_LENGTH; i++)
//	{
//		statistics[keyPub[i] >> 4]++;
//		statistics[keyPub[i] & 0x0F]++;
//	}
//	for (int i = 0; i < 16; i++)
//		if (statistics[i] > 16)
//			return true;
//
//	return false;
//}

bool Worker::isGoodKey()
{
	int count = 0;
	
	for (int i = 0; i < KEY_LENGTH; i++)
	{
		keyChar[i * 2] = keyPub[i] >> 4;
		keyChar[i * 2 + 1] = keyPub[i] & 0x0F;
	}

	for (int i = 1; i < KEY_LENGTH * 2; i++)
	{
		if (keyChar[i - 1] == keyChar[i])
			count++;
	}
	if (count > 15)
		return true;
	else
		return false;
}

void Worker::SaveGoodKey()
{
	mutexFile.lock();
	fprintf(resultFile, "public={");

	for (int j = 0; j < KEY_LENGTH; j++) {
		fprintf(resultFile, "%02hhX", keyPub[j]);
	}

	fprintf(resultFile, "}, private={");

	for (int j = 0; j < KEY_LENGTH; j++) {
		fprintf(resultFile, "%02hhX", keyPriv[j]);
	}

	fprintf(resultFile, "}\n");
	mutexFile.unlock();
}