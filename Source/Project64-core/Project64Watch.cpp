#include "stdafx.h"
#include "Project64Watch.h"
#include "N64System\N64Class.h"
#include "N64System\SystemGlobals.h"
#include <thread>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

namespace Project64Watch
{
	void Run();
	void OrderFlip();
	int ReadPacket();
	void SendPacket();

	WSAData wsaData;
	SOCKADDR_IN addr;
	int addrlen;
	SOCKET sListen;
	SOCKET sClient;
	int Port = 0;
	int timeout = 5000;

	char* sBuffer;
	unsigned int sSize;

	char rBuffer[512];
	char strBuffer[256];

	unsigned int MAGIC = 0x34364A50;
	unsigned int pMagic;
	unsigned int pCommand;
	unsigned int pAddress;
	unsigned int pSize;

	unsigned int aAddress;
	unsigned int aSize;

	unsigned int VKeys[4];
	unsigned int VKeyTimers[4];

	char* mBuffer;
	unsigned int mSize;

	void Start(int port)
	{
		Port = port;
		std::thread t1(Run);
		t1.detach();
	}

	void Run()
	{
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			g_Notify->DisplayMessage(5, "WSAStartup failed");
			return;
		}

		addrlen = sizeof(addr);
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(Port);
		addr.sin_family = AF_INET;

		sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sListen == INVALID_SOCKET)
		{
			g_Notify->DisplayMessage(5, "socket failed");
			return;
		}
		if (bind(sListen, (SOCKADDR*)&addr, sizeof(addr)) != 0)
		{
			g_Notify->DisplayMessage(5, "bind failed");
			return;
		}
		if (listen(sListen, SOMAXCONN) != 0)
		{
			g_Notify->DisplayMessage(5, "listen failed");
			return;
		}

		mBuffer = (char*)malloc(0x100);
		sBuffer = (char*)malloc(0x100);
		sprintf(strBuffer, "Listening on port ", Port);
		g_Notify->DisplayMessage(5, strBuffer);
		while (true)
		{
			sClient = accept(sListen, (SOCKADDR*)&addr, &addrlen);
			setsockopt(sClient, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(int));

			g_Notify->DisplayMessage(5, "Client Connected");
			/*sSize = 0x0C;
			sBuffer = (char*)realloc(sBuffer, sSize);
			memset(sBuffer, 0x00, sSize);
			memcpy(sBuffer, &MAGIC, 4);
			SendPacket();*/

			int result = 0;
			while (true)
			{
				memset(sBuffer, 0x00, sSize);
				memset(rBuffer, 0x00, 512);
				result = ReadPacket();
				if (result == -1)
				{
					g_Notify->DisplayMessage(5, "Client Disconnected");
					sClient = NULL;
					break;
				}
				else
				{
					//sprintf(strBuffer, "Received Packet %d", result);
					//g_Notify->DisplayMessage(5, strBuffer);
					memcpy(&pMagic, rBuffer, 4);
					if (pMagic == MAGIC)
					{
						memcpy(&pCommand, rBuffer + 0x04, 4);
						pAddress = pCommand & 0xFFFFFF;
						pCommand = ((pCommand & 0xFF000000) >> 0x18);
						memcpy(&pSize, rBuffer + 0x08, 4);

						if ((pAddress + pSize) < 0x800000)//data in bounds of memory
						{
							switch (pCommand)
							{
								case 0x01://Read Memory
								{
									//sprintf(strBuffer, "Received Memory Read %02X %06X %06X", pCommand, pAddress, pSize);
									//g_Notify->DisplayMessage(5, strBuffer);
									aAddress = pAddress & 0xFFFFFC;
									aSize = (((pSize + (pAddress & 0x03) + 0x04 - 0x01) / 0x04) * 0x04);

									mSize = aSize;
									mBuffer = (char*)realloc(mBuffer, mSize);

									memcpy(mBuffer, g_MMU->Rdram() + aAddress, mSize);
									OrderFlip();

									pCommand = (pCommand << 0x18) + pAddress;
									sSize = pSize + 0x0C;
									sBuffer = (char*)realloc(sBuffer, sSize);
									memset(sBuffer, 0x00, sSize);
									memcpy(sBuffer, &MAGIC, 4);
									memcpy(sBuffer + 0x04, &pCommand, 4);
									memcpy(sBuffer + 0x08, &pSize, 4);
									memcpy(sBuffer + 0x0C, mBuffer + (pAddress & 0x03), pSize);
									SendPacket();
								}
								break;

								case 0x02://Write Memory
								{
									//sprintf(strBuffer, "Received Memory Write %02X %06X %06X", pCommand, pAddress, pSize);
									//g_Notify->DisplayMessage(5, strBuffer);
									if (sizeof(rBuffer) > pSize + 0x0C)//If packet is large enough for pSize
									{
										aAddress = pAddress & 0xFFFFFC;
										aSize = (((pSize + (pAddress & 0x03) + 0x04 - 0x01) / 0x04) * 0x04);

										mSize = aSize;
										mBuffer = (char*)realloc(mBuffer, mSize);

										memcpy(mBuffer, g_MMU->Rdram() + aAddress, mSize);
										OrderFlip();

										memcpy(mBuffer + (pAddress & 0x03), rBuffer + 0x0C, pSize);
										OrderFlip();

										memcpy(g_MMU->Rdram() + aAddress, mBuffer, mSize);
									}
								}
								break;

								case 0x03://Input
								{
									//sprintf(strBuffer, "Received Input %02X %08X", pAddress, pSize);
									//g_Notify->DisplayMessage(5, strBuffer);
									VKeys[pAddress] = pSize;
									VKeyTimers[pAddress] = 2;//frames?
								}
								break;

								case 0x04://Misc
								{
									//sprintf(strBuffer, "Received Command %02X %02X", pAddress, pSize);
									//g_Notify->DisplayMessage(5, strBuffer);
									switch (pAddress)
									{
										case 0x00:
										{
											WriteTrace(TraceUserInterface, TraceDebug, "ID_SYSTEM_PAUSE");
											g_BaseSystem->ExternalEvent(g_Settings->LoadBool(GameRunning_CPU_Paused) ? SysEvent_ResumeCPU_FromMenu : SysEvent_PauseCPU_FromMenu);
											WriteTrace(TraceUserInterface, TraceDebug, "ID_SYSTEM_PAUSE 1");
										}
										break;

										case 0x01:
										{
											WriteTrace(TraceUserInterface, TraceDebug, "ID_SYSTEM_RESET_HARD");
											g_BaseSystem->ExternalEvent(SysEvent_ResetCPU_Hard);
										}
										break;

										case 0x02:
										{
											sprintf(strBuffer, "Save Slot (Slot %d) selected", pSize);
											g_Notify->DisplayMessage(3, strBuffer);
											g_Settings->SaveDword(Game_CurrentSaveState, pSize);
										}
										break;

										case 0x03:
										{
											WriteTrace(TraceUserInterface, TraceDebug, "ID_SYSTEM_SAVE");
											g_BaseSystem->ExternalEvent(SysEvent_SaveMachineState);
										}
										break;

										case 0x04:
										{
											WriteTrace(TraceUserInterface, TraceDebug, "ID_SYSTEM_RESTORE");
											g_BaseSystem->ExternalEvent(SysEvent_LoadMachineState);
										}
										break;
									}
								}
								break;
							}
						}
					}
				}
			}
		}
	}

	void OrderFlip()
	{
		char temp = 0;
		for (int i = 0; i < mSize; i += 4)
		{
			temp = mBuffer[i];
			mBuffer[i] = mBuffer[i + 3];
			mBuffer[i + 3] = temp;
			temp = mBuffer[i + 1];
			mBuffer[i + 1] = mBuffer[i + 2];
			mBuffer[i + 2] = temp;
		}
	}

	int ReadPacket()
	{
		int result = 0;
		try
		{
			result = recv(sClient, rBuffer, sizeof(rBuffer), NULL);
		}
		catch (...)
		{
			g_Notify->DisplayMessage(5, "ReadPacket() threw exception");
			result = -1;
		}
		return result;
	}

	void SendPacket()
	{
		try
		{
			send(sClient, sBuffer, sSize, NULL);
		}
		catch (...)
		{
			g_Notify->DisplayMessage(5, "SendPacket() threw exception");
			sClient = NULL;
		}
	}

	uint32_t GetVKeys(int32_t Control, bool timer)
	{
		if (timer)
		{
			if (VKeyTimers[Control] != 0x00)
			{
				VKeyTimers[Control]--;
			}
			else
			{
				VKeys[Control] = 0x00;
			}
		}
		return VKeys[Control];
	}
}