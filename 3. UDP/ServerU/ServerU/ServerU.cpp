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
    SOCKET  sS;
    SOCKADDR_IN serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(2000);
    serv.sin_addr.s_addr = INADDR_ANY;

    try {
        cout << "ServerU\n\n";

        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
            throw  SetErrorMsgText("Startup: ", WSAGetLastError());
        }
        if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) {
            throw  SetErrorMsgText("socket: ", WSAGetLastError());
        }
        if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) {
            throw  SetErrorMsgText("bind: ", WSAGetLastError());
        }

        SOCKADDR_IN clnt;
        memset(&clnt, 0, sizeof(clnt));
        int lc = sizeof(clnt);

        clock_t start, end;//---
        char ibuf[50], obuf[50] = "server: принято ";
        int libuf = 0, lobuf = 0;
        bool flag = true;

        while (true) {

            while (true) {
                if ((libuf = recvfrom(sS, ibuf, sizeof(ibuf), NULL, (sockaddr*)&clnt, &lc)) == SOCKET_ERROR) {
                    throw SetErrorMsgText("recv: ", WSAGetLastError());
                }
                else {
                    if (flag) {
                        flag = false;
                        start = clock();
                    }
                }

                string obuf = ibuf;
                Sleep(10);
                //if ((lobuf = sendto(sS, obuf.c_str(), strlen(obuf.c_str()) + 1, NULL, (sockaddr*)&clnt, sizeof(clnt))) == SOCKET_ERROR) {
                //    throw SetErrorMsgText("send: ", WSAGetLastError());
                //}

                if (strcmp(ibuf, "") == 0) {
                    flag = true;
                    end = clock();
                    cout << "Time for send and recv: " << ((double)(end - start) / CLK_TCK) << " c\n\n";
                    break;
                }
                else {
           /*         cout << ibuf << endl;*/
                }
            }
        }


        if (closesocket(sS) == SOCKET_ERROR) {
            throw  SetErrorMsgText("closesocket: ", WSAGetLastError());
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