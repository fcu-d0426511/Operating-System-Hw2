// winsocket_server.cpp: 定義主控台應用程式的進入點。
//
#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#define MAX_CONNECTIONS 10

#pragma comment(lib, "Ws2_32.lib")

DWORD WINAPI RunForClientThread(LPVOID);
SOCKET connections[MAX_CONNECTIONS];
int connection_count = 0;

int main()
{
	DWORD ThreadID;
	HANDLE ThreadHandle;

	//用WSAStartup開始Winsock-DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 1), &wsaData); //MAKEWORD(2, 1)為Winsocket-DLL版本
	
	//宣告socket位址資訊
	struct sockaddr_in addr;
	int addrLen = sizeof(addr);

	//建立socket
	SOCKET sConnect;
	sConnect = socket(AF_INET, SOCK_STREAM, NULL);
	
	//設定位址資訊的資料
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1234);

	//設定監聽Listen Socket
	SOCKET sListen;
	sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (sockaddr*)&addr, addrLen);
	listen(sListen, SOMAXCONN);

	//宣告clientAddr儲存client的位址資訊
	SOCKADDR_IN clientAddr;
	printf("Server starting...\n");
	while (true)
	{
		//等待client連線
		if (sConnect = accept(sListen, (SOCKADDR*)&clientAddr, &addrLen))
		{
			printf("a connection was found!!\n");
			
			//檢查是否有未建立連線的Socket可用
			int sokcet_index = -1;
			for (int i=0; i<MAX_CONNECTIONS; i++)
			{
				if (connections[i] == 0)
				{
					sokcet_index = i;
					break;
				}
			}
			if (sokcet_index == -1)
			{
				printf("Connection full... \n");
				return 1;
			}
			connections[sokcet_index] = sConnect;
			connection_count++;			
			ThreadHandle = CreateThread(NULL, 0, RunForClientThread, &sokcet_index, 0, &ThreadID);
		}
	}

	for(int i=0;  i<MAX_CONNECTIONS;i++)
		closesocket(connections[i]);
	WSACleanup();
    return 0;
}

DWORD WINAPI RunForClientThread(LPVOID input_sIndex) {
	
	//char sendbuf[200];
		char recvbuf[200],name[200],nametmp[200];
	int sockIndex = *(int*)input_sIndex;
	bool index = true;
	int read_size;
	ZeroMemory(recvbuf, 200);

	//傳送訊息給 client 端
	

	//strcpy_s(sendbuf, "Hello client!! please input your name: ");
	char sendbuf[200];
	ZeroMemory(sendbuf, 200);

	strcat_s(sendbuf, "Hello client!!  ");
	

	ZeroMemory(name, 200);
	recv(connections[sockIndex], name, sizeof(name), 0);
	//printf("your user name is %s", name);
	strcat_s(name, " : ");
	strcpy_s(nametmp , name);
	send(connections[sockIndex], sendbuf, (int)strlen(sendbuf), 0);
	do {

		ZeroMemory(name, 200);

		ZeroMemory(recvbuf, 200);
		recv(connections[sockIndex], recvbuf, sizeof(recvbuf), 0);
		strcpy_s(name,nametmp);

		strcat_s(name, recvbuf);
		if (strcmp(recvbuf, "exit") == 0) { //判斷是否為結束		
			send(connections[sockIndex], "exit", (int)strlen("exit"), 0);
			connections[sockIndex] = 0;		//回收socket
			index = false;
		}
		for (int i = 0; i < MAX_CONNECTIONS; i++) {
			if (i == sockIndex) continue;
			send(connections[i], name, (int)strlen(name), 0);
			//send(connections[i], recvbuf, (int)strlen(recvbuf), 0);
		}
			
		
		

	} while (index);
	
	
	//接收 client 端的訊息, 
	//recv(input_socket, sendbuf, sizeof(sendbuf), 0);
	return 0;
}