#pragma once
#pragma comment(lib,"WS2_32.lib")

#include "locale.h"
#include <iostream>
#include <string>
#include <list>
#include <time.h>

#include "Error.h"

#include "Winsock2.h"
#include "Windows.h"

using std::string;
using std::list;

int port = 2000;                                        
int uport = 2000;                                       
const char * dllname = "ServiceLibrary";                
const char * npname = "cspipe";                         
const char * ucall = "HELLO";                           
int AS_SQUIRT = 10;                                     
//int squirt = AS_SQUIRT;                               

HANDLE( *ts1 )( char*, LPVOID );                        
HANDLE hAcceptServer;                                   
HMODULE st1;                                            
HANDLE Event = CreateEvent( NULL, FALSE, FALSE, NULL ); 

enum TalkersCmd { Start, Stop, Exit, Statistics, Wait, Shutdown, GetCommand }; 


volatile LONG Accept = 0;  
volatile LONG Fail = 0;   
volatile LONG Finished = 0;
volatile LONG Work = 0;    

CRITICAL_SECTION scListContact; 

struct Contact{            //элемент списка подключений                   
    enum TE {              //состо€ние сервера подключени€
        EMPTY,             //пустой элемент списка подключений 
        ACCEPT,            //подключен, но не обслуживаетс€
        CONTACT            //передан обслуживающему серверу  
    } type;

                        
    enum ST {              //состо€ние обслуживающего сервера
        WORK,              //идет обмен данными с клиентом
        ABORT,             //обслуживающий сервер завершилс€ ненормально 
        TIMEOUT,           //обслуживающий сервер завершилс€ по таймеру 
        FINISH             //обслуживающий сервер завершилс€  нормально 
    } sthread;

    SOCKET      s;         //сокет дл€ обмена данными с клиентом
    SOCKADDR_IN prms;      //параметры  сокета 
    int         lprms;     //длина prms 
    HANDLE      hthread;   //дескриптор потока 
    HANDLE      htimer;    //дескриптор таймера
    bool        TimerOff;  //метка срабатывани€ таймера
    bool        CloseConn; //метка завершени€ обслуживани€

    char msg[50];        //сообщение 
    char srvname[15];    //им€ обслуживающего потока 

    HANDLE hAcceptServer; // Handle обслуживающего потока

    Contact( TE t = EMPTY, const char* namesrv = "" ){
        memset( &prms, 0, sizeof( SOCKADDR_IN ) );
        lprms = sizeof( SOCKADDR_IN ); //размер сокета
        type = t;
        strcpy_s( srvname, namesrv );
        msg[ 0 ] = 0;
        CloseConn = false;
        TimerOff = false;
    };

    void SetST( ST sth, const char* m = "" ) {
        sthread = sth;
        strcpy_s( msg, m );
    }
};

typedef list<Contact> ListContact;
ListContact Contacts;  


void CALLBACK ASWTimer( LPVOID Lprm, DWORD, DWORD ) {

  char obuf[ 50 ] = "Close: timeout;";
  Contact* client = (Contact*)Lprm; //преобразуем переданный параметр

  EnterCriticalSection( &scListContact ); //входим в критическую секцию
  client->TimerOff = true; 	//ставим метку срабатывани€ таймера
  client->sthread = Contact::TIMEOUT;
  LeaveCriticalSection( &scListContact ); 	//выходим из критической секции

  if ( ( send( client->s, obuf, sizeof( obuf ) + 1, NULL ) ) == SOCKET_ERROR )
    throw  SetErrorMsgText( "Send:", WSAGetLastError( ) );

  SYSTEMTIME stt;
  // ѕолучаем текущее врем€ и дату
  GetLocalTime( &stt );
  // ¬ыводим сообщение
  printf( "%d.%d.%d %d:%02d Timeout ", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute );
  cout << client->srvname << ";" << endl;
}


void CALLBACK Test( LPVOID Lprm, DWORD, DWORD ) {

  char obuf[ 50 ] = "Close: Service unavailable;";
  Contact* client = (Contact*)Lprm; //преобразуем переданный параметр

  EnterCriticalSection( &scListContact ); 
  client->TimerOff = true; 	
  client->sthread = Contact::FINISH;
  LeaveCriticalSection( &scListContact ); 

  if ( ( send( client->s, obuf, sizeof( obuf ) + 1, NULL ) ) == SOCKET_ERROR )
    throw  SetErrorMsgText( "Send:", WSAGetLastError( ) );

  SYSTEMTIME stt;
  GetLocalTime( &stt );
  printf( "%d.%d.%d %d:%02d Service unavailable ", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute );
  cout << client->srvname << ";" << endl;
}


void CALLBACK ASStartMessage( DWORD Lprm ) {
  Contact* client = (Contact*)Lprm;
  /*EnterCriticalSection(&scListContact);*/
  char* sn = client->srvname;
  //LeaveCriticalSection(&scListContact);
  SYSTEMTIME stt;
  GetLocalTime( &stt );
  printf( "%d.%d.%d %d:%02d ", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute );
 
}

void CALLBACK ASFinishMessage( DWORD Lprm ) {
  Contact* client = (Contact*)Lprm;
  /*EnterCriticalSection(&scListContact);*/
  char* sn = client->srvname;
  /*LeaveCriticalSection(&scListContact);*/
  SYSTEMTIME stt;
  GetLocalTime( &stt );
  printf( "%d.%d.%d %d:%02d ", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute );
  std::cout << sn << " stoped;" << std::endl;
}