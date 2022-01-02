#include <stdio.h>
#include <tchar.h>
#include "iostream"
#include "Windows.h"                
#include "ctime"
#include <string>

using std::string;

using namespace std;

string GetErrorMsgText(int code) {
    string msgText;
    switch (code) {
        default: msgText = "ERROR";
            break;
    };
    return msgText;
}
string SetPipeError(string msgText, int code) { return msgText + GetErrorMsgText(code); };

int _tmain(int argc, _TCHAR* argv[]) {
    setlocale( LC_ALL, "Rus" );

    SetConsoleTitle( "Remote Console" );

    try {
        printf_s( "\nAvailable commands:\n 1 - Start\n 2 - Stop\n 3 - Exit\n 4 - Statistics\n 5 - Wait\n 6 - Shutdown\n 7 - Close RConsole\n\n" );
        
        char ReadBuf[50] = ""; // ������ ��� ������ ��������� �� �������
        char WriteBuf[2] = ""; // ������ ��� �������� ��������� �������
        
        DWORD nBytesRead;
        DWORD nBytesWrite;

        int Code = 0; // ��� �������

        char serverName[256];
        char PipeName[512];
        bool result;

        cout << "Enter servername: ";
        cin >> serverName;
        result = sprintf_s( PipeName, "\\\\%s\\pipe\\cspipe", serverName );

        BOOL fSuccess;

        SECURITY_ATTRIBUTES SecurityAttributes;
        SECURITY_DESCRIPTOR SecurityDescriptor;

        fSuccess = InitializeSecurityDescriptor(
            &SecurityDescriptor,
            SECURITY_DESCRIPTOR_REVISION 
        );

        if (!fSuccess) { throw new string("InitializeSecurityDescriptor(): Error"); }

        fSuccess = SetSecurityDescriptorDacl(&SecurityDescriptor, TRUE, NULL, FALSE);

        if (!fSuccess) { throw new string("SetSecurityDescriptorDacl(): Error"); }

        SecurityAttributes.nLength = sizeof(SecurityAttributes);
        SecurityAttributes.lpSecurityDescriptor = &SecurityDescriptor;
        SecurityAttributes.bInheritHandle = FALSE;

        HANDLE hNamedPipe = CreateFile(PipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, &SecurityAttributes);

        do {
            printf_s("Command: ");
            scanf_s( "%d", &Code );
            
            if (Code > 0 && Code < 7) {
                sprintf_s(WriteBuf, "%d", Code - 1);

                if (!WriteFile(hNamedPipe, WriteBuf, strlen(WriteBuf) + 1, &nBytesWrite, NULL)) throw "WriteFile: Error";

                if (ReadFile(hNamedPipe, ReadBuf, sizeof(ReadBuf), &nBytesRead, NULL)) cout << ReadBuf << endl;
                else { throw "ReadFile: Error "; }
            }
            else if ( Code > 7 ) printf( "Wrong command!.\n\n" );
        } while (Code != 7 && Code != 3 && Code != 6);
    }
    catch (char* ErrorPipeText) {
        printf_s("%s", ErrorPipeText);
        cout << GetLastError() << endl;
    }
  
    cout << "RemoteConsole shutting down.";
    system("pause");
    return 0;
}

