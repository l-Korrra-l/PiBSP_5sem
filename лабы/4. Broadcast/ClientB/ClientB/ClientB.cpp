#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Errors.h"
#include <iostream>


#include <clocale>
#include <ctime>
#include "Winsock2.h"                
#pragma comment(lib, "WS2_32.lib")   

bool  GetServer(char* call, short port, struct sockaddr* from, int* flen);
SOCKET cC;

int main()
{
    setlocale(LC_ALL, "rus");

    WSADATA wsaData;

    try {
        cout << "ClientB\n\n";

        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
            throw  SetErrorMsgText("Startup: ", WSAGetLastError());
        }
        
        char name[] = "Hello";
        SOCKADDR_IN client;
        int lc = sizeof(client);

        for(;;)
        {
            GetServer(name, 2000, (sockaddr*)&client, &lc);
        }

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



bool  GetServer(
    char* call, //[in] позывной сервера  
    short            port, //[in] номер порта сервера    
    struct sockaddr* from, //[out] указатель на SOCKADDR_IN
    int* flen  //[out] указатель на размер from 
)
{

    if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
        throw SetErrorMsgText("socket:", WSAGetLastError());
    int optval = 1;
    if (setsockopt(cC, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR)
        throw SetErrorMsgText("opt:", WSAGetLastError());

    SOCKADDR_IN all; 
    all.sin_family = AF_INET; 
    all.sin_port = htons(2000); 
    all.sin_addr.s_addr = inet_addr("192.168.227.255");
    char buf[] = "answer anyone!";


    if ((sendto(cC, call, strlen(call) + 1, NULL,(sockaddr*)&all, sizeof(all))) == SOCKET_ERROR)
        throw SetErrorMsgText("sendto:", WSAGetLastError());

    int  libuf = 0;
    char ibuf[50] = "";

    if ((libuf = recvfrom(cC, ibuf, sizeof(ibuf), NULL, from, flen)) == SOCKET_ERROR) {
        if (WSAGetLastError() == WSAETIMEDOUT) return false;

        else throw  SetErrorMsgText("recvfrom:", WSAGetLastError());
    }
    
    //Внесите изменения в программу ClientB для того, чтобы она выводила на экран  консоли параметры сокета  сервера откликнувшегося на позывной.
    //std::cout << "Server name: " << ibuf;
    //SOCKADDR_IN* addr = (SOCKADDR_IN*)&from;
    //std::cout << "\nPort: " << ntohs(addr->sin_port);
    //std::cout << "\nIP: " << inet_ntoa(addr->sin_addr);

    hostent* shostname = gethostbyaddr((char*)&((SOCKADDR_IN*)from)->sin_addr, sizeof(SOCKADDR_IN), AF_INET);
    cout << "\tIP: " << inet_ntoa(((SOCKADDR_IN*)from)->sin_addr) << endl;
    cout << "\t\tPort: " << ntohs(((struct sockaddr_in*)from)->sin_port) << endl;
    cout << "\t\tHostname: " << shostname->h_name << endl;

    return true;
}