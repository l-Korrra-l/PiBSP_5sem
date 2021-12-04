#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <clocale>
#include <ctime>

#include "Errors.h"
#include "Winsock2.h"
#pragma comment(lib, "WS2_32.lib")

//#define NAME "DZUSHMASTER"		//dima
//#define NAME "DESKTOP-E4TFQEB"	//solnishko
//#define NAME "LAPTOP-N0QMVJ9U"	//me
#define NAME "DESKTOP-8LFC8O6"		//ann

#define PORT 2000


bool GetServerByName(const char* name, const char* call, struct sockaddr* from, int* flen);

int main()
{
	setlocale(LC_ALL, "rus");

	WSADATA wsaData;
	SOCKADDR_IN clnt;
	int lc = sizeof(clnt);
	char name[] = "Hello";

	try {
		cout << "ClientS\n\n";

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			throw SetErrorMsgText("Startup: ", WSAGetLastError());
		}

		GetServerByName(NAME, name, (sockaddr*)&clnt, &lc);

		if (WSACleanup() == SOCKET_ERROR) {
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
		}
	}
	catch (string errorMsgText) {
		std::cout << std::endl << errorMsgText;
	}

	system("pause");
	return 0;
}

bool GetServerByName(
	const char* name, //[in] имя компьютера в сети
	const char* call, //[in] позывной
	struct sockaddr* from, //[in,out] указатель на SOCKADDR_IN
	int* flen //[in,out] указатель на размер from
) {
	hostent* host = gethostbyname(name);

	try {
		SOCKET cC;
		int optval = 1, lb = 0, lobuf = 0;
		char buf[50];
		in_addr* addr_list = (in_addr*)*(host->h_addr_list);

		SOCKADDR_IN all; 
		all.sin_family = AF_INET;
		all.sin_port = htons(PORT); 
		all.sin_addr = *addr_list;

		if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) {
			throw SetErrorMsgText("socket:", WSAGetLastError());
		}
		if (setsockopt(cC, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR) {
			throw SetErrorMsgText("setsocketopt:", WSAGetLastError());
		}
		if ((lb = sendto(cC, call, sizeof(call) + 2, NULL, (sockaddr*)&all, sizeof(all))) == SOCKET_ERROR) {
			throw SetErrorMsgText("sendto:", WSAGetLastError());
		}
		if (lobuf = recvfrom(cC, buf, sizeof(buf), NULL, (sockaddr*)from, flen) == SOCKET_ERROR) {
			throw SetErrorMsgText("recvfrom:", WSAGetLastError());
		}

		struct sockaddr_in* server = (struct sockaddr_in*)from;
		std::cout << "Server port: " << ntohs(server->sin_port) << std::endl;
		std::cout << "Server IP: " << inet_ntoa(*addr_list) << std::endl;

		if (closesocket(cC) == SOCKET_ERROR) {
			throw SetErrorMsgText("closesocket: ", WSAGetLastError());
		}

		return strcmp(call, buf) == 0 ? true : false;
	}
	catch (int errCode) {
		return errCode == WSAETIMEDOUT ? false : throw SetErrorMsgText("recvfrom:", WSAGetLastError());
	}
}
