#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Configs.h"

#include <WS2tcpip.h>
using std::iterator;

DWORD WINAPI GarbageCleaner(LPVOID pPrm) {
    
    DWORD rc = 0;
    try {
        while (*((TalkersCmd*)pPrm) != Exit && &Work > 0) {
            EnterCriticalSection(&scListContact);
      
            for (std::list<Contact>::iterator first = Contacts.begin(); first != Contacts.end();) {
                if (first->sthread == Contact::FINISH || first->sthread == Contact::TIMEOUT || first->sthread == Contact::ABORT || first->type == Contact::EMPTY) {
                    char dst[16] = "";
                    inet_ntop(AF_INET, &(first->prms.sin_addr), dst, sizeof dst);

                    printf_s("Client IP: %s", dst);
                    cout << " , code: " << first->sthread << ";" << endl;
                    if (first->type == Contact::EMPTY) InterlockedIncrement(&Fail);
                    else {
                        if (first->sthread == Contact::FINISH) {
                            InterlockedIncrement(&Finished);
                            InterlockedDecrement(&Accept);
                        }
                        if (first->sthread == Contact::TIMEOUT) { InterlockedIncrement(&Fail); InterlockedDecrement(&Accept); }
                        if (first->sthread == Contact::ABORT) { InterlockedIncrement(&Fail); InterlockedDecrement(&Accept); }
                    
                        CloseHandle(first->hthread);
                        CloseHandle(first->htimer);
                    }
                    closesocket(first->s);

                    first = Contacts.erase(first);
                    InterlockedDecrement(&Work);
                }
                else first++;
            }
            LeaveCriticalSection(&scListContact);
            Sleep(1000);
        }
    }
    catch (string errorMsgText) { std::cout << errorMsgText << endl; }
    catch (...) { cout << "Error GarbageCleaner" << endl; }

    cout << "GarbageCleaner stopped\n" << endl;

    ExitThread( rc );
}