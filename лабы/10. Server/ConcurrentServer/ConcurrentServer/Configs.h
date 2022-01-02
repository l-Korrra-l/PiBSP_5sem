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

struct Contact{            //������� ������ �����������                   
    enum TE {              //��������� ������� �����������
        EMPTY,             //������ ������� ������ ����������� 
        ACCEPT,            //���������, �� �� �������������
        CONTACT            //������� �������������� �������  
    } type;

                        
    enum ST {              //��������� �������������� �������
        WORK,              //���� ����� ������� � ��������
        ABORT,             //������������� ������ ���������� ����������� 
        TIMEOUT,           //������������� ������ ���������� �� ������� 
        FINISH             //������������� ������ ����������  ��������� 
    } sthread;

    SOCKET      s;         //����� ��� ������ ������� � ��������
    SOCKADDR_IN prms;      //���������  ������ 
    int         lprms;     //����� prms 
    HANDLE      hthread;   //���������� ������ 
    HANDLE      htimer;    //���������� �������
    bool        TimerOff;  //����� ������������ �������
    bool        CloseConn; //����� ���������� ������������

    char msg[50];        //��������� 
    char srvname[15];    //��� �������������� ������ 

    HANDLE hAcceptServer; // Handle �������������� ������

    Contact( TE t = EMPTY, const char* namesrv = "" ){
        memset( &prms, 0, sizeof( SOCKADDR_IN ) );
        lprms = sizeof( SOCKADDR_IN ); //������ ������
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
  Contact* client = (Contact*)Lprm; //����������� ���������� ��������

  EnterCriticalSection( &scListContact ); //������ � ����������� ������
  client->TimerOff = true; 	//������ ����� ������������ �������
  client->sthread = Contact::TIMEOUT;
  LeaveCriticalSection( &scListContact ); 	//������� �� ����������� ������

  if ( ( send( client->s, obuf, sizeof( obuf ) + 1, NULL ) ) == SOCKET_ERROR )
    throw  SetErrorMsgText( "Send:", WSAGetLastError( ) );

  SYSTEMTIME stt;
  // �������� ������� ����� � ����
  GetLocalTime( &stt );
  // ������� ���������
  printf( "%d.%d.%d %d:%02d Timeout ", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute );
  cout << client->srvname << ";" << endl;
}


void CALLBACK Test( LPVOID Lprm, DWORD, DWORD ) {

  char obuf[ 50 ] = "Close: Service unavailable;";
  Contact* client = (Contact*)Lprm; //����������� ���������� ��������

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