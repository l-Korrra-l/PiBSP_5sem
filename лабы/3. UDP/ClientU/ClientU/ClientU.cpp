#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Errors.h"
#include <iostream>


#include <clocale>
#include <ctime>
#include "Winsock2.h"                
#pragma comment(lib, "WS2_32.lib")   

int main()
{
    setlocale(LC_ALL, "rus");

    WSADATA wsaData;
    SOCKET cC;
    SOCKADDR_IN serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(2000);
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");

    try {
        cout << "ClientU\n\n";

        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
            throw  SetErrorMsgText("Startup: ", WSAGetLastError());
        }
        if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) {
            throw  SetErrorMsgText("socket: ", WSAGetLastError());
        }


        clock_t start, end;
        char ibuf[50] = "server: принято ";
        int  libuf = 0, lobuf = 0;
        int sc = sizeof(serv);

        int countMessage;
        cout << "Number of messages: ";
        cin >> countMessage;

        start = clock();
        for (int i = 1; i <= countMessage; i++) {
            string obuf = "Hello";

            if ((lobuf = sendto(cC, obuf.c_str(), strlen(obuf.c_str()) + 1, NULL, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR) {
                throw SetErrorMsgText("send: ", WSAGetLastError());
            }

            //if ((libuf = recvfrom(cC, ibuf, sizeof(ibuf), NULL, (sockaddr*)&serv, &sc)) == SOCKET_ERROR) {
            //    throw SetErrorMsgText("recv: ", WSAGetLastError());
            //}

            cout << ibuf << endl;
        }
        end = clock();
        string obuf = "=-=-=-=-=-=-=-=-=-=-=";

        if ((lobuf = sendto(cC, obuf.c_str(), strlen(obuf.c_str()) + 1, NULL, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR) {
            throw SetErrorMsgText("send: ", WSAGetLastError());
        }

        cout << "Time for send and recv: " << ((double)(end - start) / CLK_TCK) << " c" << endl;

        if (closesocket(cC) == SOCKET_ERROR) {
            throw SetErrorMsgText("closesocket: ", WSAGetLastError());
        }
        if (WSACleanup() == SOCKET_ERROR) {
            throw  SetErrorMsgText("Cleanup: ", WSAGetLastError());
        }
    }
    catch (string errorMsgText) {
        cout << endl << errorMsgText;
    }

    system("pause");
    return 0;
}

