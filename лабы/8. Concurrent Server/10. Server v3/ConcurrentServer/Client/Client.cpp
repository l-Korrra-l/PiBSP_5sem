#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "WS2_32.lib")   

#include <stdio.h>
#include <tchar.h>
#include "iostream"
#include <string>
#include <ctime>
#include "time.h"

#include "Winsock2.h"                

using namespace std;

bool SystemMessage(char* ch) {
    bool result = false;
    
    try {
        char Timeout[50] = "Close: timeout;", Close[50] = "Close: finish;", Abort[50] = "Close: Abort;";
        if (strcmp(ch, Timeout) == NULL) result = true;
        else if (strcmp(ch, Abort) == NULL) result = true;
        else if (strcmp(ch, Close) == NULL) result = true;
    }
    catch (...) { cout << "error" << endl; }

    return result;
}

string  GetErrorMsgText(int code) {
    string msgText;
    switch (code)  {
        case WSAEINTR:                  msgText = "WSAEINTR";               break;    
        case WSAEACCES:                 msgText = "WSAEACCES";              break;    
        case WSAEFAULT:                 msgText = "WSAEFAULT";              break;    
        case WSAEINVAL:                 msgText = "WSAEINVAL";              break;    
        case WSAEMFILE:                 msgText = "WSAEMFILE";              break;    
        case WSAEWOULDBLOCK:            msgText = "WSAEWOULDBLOCK";         break;    
        case WSAEINPROGRESS:            msgText = "WSAEINPROGRESS";         break;    
        case WSAEALREADY:               msgText = "WSAEALREADY";            break;    
        case WSAENOTSOCK:               msgText = "WSAENOTSOCK";            break;    
        case WSAEDESTADDRREQ:           msgText = "WSAEDESTADDRREQ";        break;    
        case WSAEMSGSIZE:               msgText = "WSAEMSGSIZE";            break;    
        case WSAEPROTOTYPE:             msgText = "WSAEPROTOTYPE";          break;    
        case WSAENOPROTOOPT:            msgText = "WSAENOPROTOOPT";         break;    
        case WSAEPROTONOSUPPORT:        msgText = "WSAEPROTONOSUPPORT";     break;    
        case WSAESOCKTNOSUPPORT:        msgText = "WSAESOCKTNOSUPPORT";     break;    
        case WSAEOPNOTSUPP:             msgText = "WSAEOPNOTSUPP";          break;    
        case WSAEPFNOSUPPORT:           msgText = "WSAEPFNOSUPPORT";        break;    
        case WSAEAFNOSUPPORT:           msgText = "WSAEAFNOSUPPORT";        break;    
        case WSAEADDRINUSE:             msgText = "WSAEADDRINUSE";          break;    
        case WSAEADDRNOTAVAIL:          msgText = "WSAEADDRNOTAVAIL";       break;    
        case WSAENETDOWN:               msgText = "WSAENETDOWN";            break;    
        case WSAENETUNREACH:            msgText = "WSAENETUNREACH";         break;    
        case WSAENETRESET:              msgText = "WSAENETRESET";           break;    
        case WSAECONNABORTED:           msgText = "WSAECONNABORTED";        break;    
        case WSAECONNRESET:             msgText = "WSAECONNRESET";          break;    
        case WSAENOBUFS:                msgText = "WSAENOBUFS";             break;    
        case WSAEISCONN:                msgText = "WSAEISCONN";             break;    
        case WSAENOTCONN:               msgText = "WSAENOTCONN";            break;    
        case WSAESHUTDOWN:              msgText = "WSAESHUTDOWN";           break;    
        case WSAETIMEDOUT:              msgText = "WSAETIMEDOUT";           break;    
        case WSAECONNREFUSED:           msgText = "WSAECONNREFUSED";        break;    
        case WSAEHOSTDOWN:              msgText = "WSAEHOSTDOWN";           break;    
        case WSAEHOSTUNREACH:           msgText = "WSAEHOSTUNREACH";        break;    
        case WSAEPROCLIM:               msgText = "WSAEPROCLIM";            break;    
        case WSASYSNOTREADY:            msgText = "WSASYSNOTREADY";         break;    
        case WSAVERNOTSUPPORTED:        msgText = "WSAVERNOTSUPPORTED";     break;    
        case WSANOTINITIALISED:         msgText = "WSANOTINITIALISED";      break;    
        case WSAEDISCON:                msgText = "WSAEDISCON";             break;    
        case WSATYPE_NOT_FOUND:         msgText = "WSATYPE_NOT_FOUND";      break;    
        case WSAHOST_NOT_FOUND:         msgText = "WSAHOST_NOT_FOUND";      break;    
        case WSATRY_AGAIN:              msgText = "WSATRY_AGAIN";           break;    
        case WSANO_RECOVERY:            msgText = "WSANO_RECOVERY";         break;    
        case WSANO_DATA:                msgText = "WSANO_DATA";             break;    
        case WSA_INVALID_HANDLE:        msgText = "WSA_INVALID_HANDLE";     break;    
        case WSA_INVALID_PARAMETER:     msgText = "WSA_INVALID_PARAMETER";  break;    
        case WSA_IO_INCOMPLETE:         msgText = "WSA_IO_INCOMPLETE";      break;    
        case WSA_IO_PENDING:            msgText = "WSA_IO_PENDING";         break;    
        case WSA_NOT_ENOUGH_MEMORY:     msgText = "WSA_NOT_ENOUGH_MEMORY";  break;   
        case WSA_OPERATION_ABORTED:     msgText = "WSA_OPERATION_ABORTED";  break;    
        case WSASYSCALLFAILURE:         msgText = "WSASYSCALLFAILURE";      break;
        default:                        msgText = "Default error";      break;
    };
    return msgText;
}

string SetErrorMsgText(string msgText, int code) { return msgText + GetErrorMsgText(code); }

string error = "Error";

string GetError() { return error; }

bool GetServer(char* call, SOCKADDR_IN* from, int* flen, SOCKET* cC, SOCKADDR_IN* all) {
    char ibuf[50], obuf[50];
    int libuf = 0, lobuf = 0;                   

    if((lobuf = sendto(*cC, call, strlen(call) + 1, NULL,
        (sockaddr*)all, sizeof(*all))) == SOCKET_ERROR) { throw  SetErrorMsgText("Sendto:", WSAGetLastError()); }

    if ((libuf = recvfrom(*cC, ibuf, sizeof(ibuf), NULL, (sockaddr*)from, flen)) == SOCKET_ERROR) {
        if (WSAGetLastError() == WSAETIMEDOUT) { return false; }
        else { throw  SetErrorMsgText("Recv:", WSAGetLastError()); }
    }

    if (strcmp(call, ibuf) == 0) { return true; }
    else { return false; }
}

int _tmain(int argc, _TCHAR* argv[]) {
    setlocale( LC_ALL, "Russian" );

    SetConsoleTitle( "Client" );

    int port = 0;
    SOCKET  ClientSocket;
    WSADATA wsaData;

    string RandError = "RandService unavailable";
    string EchoError = "EchoService unavailable";
    string TimeError = "TimeService unavailable";

    try {
        if(WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) { throw SetErrorMsgText("Startup:", WSAGetLastError()); }
        if((ClientSocket = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET) { throw  SetErrorMsgText("Socket:", WSAGetLastError()); }

        int ch = 0, cCall = 0;
        bool fin = false;

        int max = 100, lobuf = 1, bport = 2000;
        char obuf[50] = "", ibuf[50] = "", Call[50], Name[50], Calls[50];

        SOCKADDR_IN Server = {0};
        Server.sin_family = AF_INET;
        Server.sin_port = htons(port);

        SOCKADDR_IN Server_IN;  
        int Flen = sizeof(Server), chs = 0;
        cout << "1. Connection by servername" << endl << "2. Connection bu callsighn" << endl << "> ";
        cin >> chs;

        if (chs == 1) {
            cout << "Server connection by name/port" << endl << "";
            cout << "Enter servername: ";
            cin >> Name;
            hostent* s = gethostbyname(Name);
            
            if (s == NULL) { throw "Can't find server"; }
            
            cout << "Enter server port: ";
            cin >> port;
            Server_IN.sin_addr = *(struct in_addr*)s->h_addr_list[0];
        }
        else if (chs == 2) {
            cout << "Server connection by callsign/port" << endl << "";
            cout << "Enter server callsign: ";
            cin >> Calls;
            SOCKET cC; 
            if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) throw SetErrorMsgText("Socket:", WSAGetLastError());

            int optval = 1;
            if (setsockopt(cC, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR) { throw SetErrorMsgText("Opt:", WSAGetLastError()); }

            SOCKADDR_IN all;
            all.sin_family = AF_INET;
            all.sin_port = htons(bport);
            all.sin_addr.s_addr = inet_addr("127.0.0.1");

            SOCKADDR_IN clnt;
            memset(&clnt, 0, sizeof(clnt));
            int lc = sizeof(clnt);

            bool bsr = GetServer(Calls, &clnt, &lc, &cC, &all);
            if (bsr == false) { 
                error = "Server not found;";
                throw error;
            }
            else {
                Server_IN.sin_addr.s_addr = clnt.sin_addr.s_addr;
                if (closesocket(cC) == SOCKET_ERROR) throw SetErrorMsgText("Closesocket:", WSAGetLastError());
                
                cout << "Enter server port: ";
                cin >> port;
            }
        }
        else {
            error = "Wrong code";
            throw error;
        }

        Server_IN.sin_family = AF_INET;         
        Server_IN.sin_port = htons(port);         

        if((connect(ClientSocket, (sockaddr*)& Server_IN, sizeof(Server_IN))) == SOCKET_ERROR) {
            error = "Connection error;";
            throw error;
        }

        while(!fin) {
            cout << "Request: " << endl << "1 - Rand" << endl << "2 - Time" << endl << "3 - Echo" << endl << "";
            cin >> cCall;

            if(cCall == 1 || cCall == 2 || cCall == 3) {
                fin = true;
                break;
            }
            else {
                if(closesocket( ClientSocket ) == SOCKET_ERROR) throw SetErrorMsgText("Closesocket:", WSAGetLastError());
                error = "Entered wrong request";
                throw error;
            }
        }

        switch (cCall) {
            case 1:
                strcpy_s(Call, "Rand");
                cout << "Enter something to stop." << endl;
                break;
            case 2:
                strcpy_s( Call, "Time" );
                cout << "Enter something to stop." << endl;
                break;
            case 3:
                strcpy_s( Call, "Echo" );
                cout << "Enter something to stop." << endl;
                break;
            default:
                strcpy_s( Call, "Echo" );
        }

        if((lobuf = send(ClientSocket, Call, sizeof(Call), NULL)) == SOCKET_ERROR) throw SetErrorMsgText("Send:", WSAGetLastError());
        char rCall[50];
        if((lobuf = recv(ClientSocket, rCall, sizeof(rCall), NULL)) == SOCKET_ERROR) throw SetErrorMsgText("Recv:", WSAGetLastError());
    
        if (strcmp(Call, rCall) != 0) {
            error = "Service unavailable";
            throw error;
        }
        else {
            bool Check = true;
            fin = false;
            u_long nonblk = 1;

            if (SOCKET_ERROR == ioctlsocket(ClientSocket, FIONBIO, &nonblk)) throw SetErrorMsgText("Ioctlsocket:", WSAGetLastError());
            
            clock_t StartTime = clock();
            bool rcv = true;
            char iib[50];
            cout << "->";
            cin >> iib;

            try {
                while(!fin) {
                    if (rcv) {
                        if ((lobuf = send(ClientSocket, iib, sizeof(iib), NULL)) == SOCKET_ERROR) {
                            error = "Error send";
                            throw error;
                        }
                        rcv = false;
                    }

                    if ((recv(ClientSocket, obuf, sizeof(obuf), NULL)) == SOCKET_ERROR) {
                        switch (WSAGetLastError()) {
                            case WSAEWOULDBLOCK: Sleep(100); break;
                            default: throw SetErrorMsgText("Recv:", WSAGetLastError());
                        }
                    }
                    else {
                        if (SystemMessage(obuf)) {
                            printf( "Server complete the connection: %s\n", obuf );
                            break;
                        }
                        else printf( "Received message:[%s]\n", obuf );
                        rcv = true;
                    }

                }
            }
            catch (...) { cout << error << endl; }

            clock_t FinishTime = clock();
            printf( "Time: %lf сек.\n", (double)( FinishTime - StartTime ) / CLOCKS_PER_SEC );

        }

        if (closesocket(ClientSocket) == SOCKET_ERROR) throw SetErrorMsgText("Closesocket:", WSAGetLastError());

        if (WSACleanup() == SOCKET_ERROR) throw SetErrorMsgText("Cleanup:", WSAGetLastError()); 
    }
    catch (char* errorMsgText) { cout << errorMsgText << endl; } 
    catch (...) { cout << error << endl; }

    system( "pause" );
    return 0;
}