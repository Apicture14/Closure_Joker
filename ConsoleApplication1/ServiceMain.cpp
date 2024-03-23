#include <windows.h>
#include <iostream>
#include "Consts.h"
#include "Bri.h"

#define ENABLE_LOG TRUE

SERVICE_STATUS s;
SERVICE_STATUS_HANDLE hs;
SYSTEMTIME stime;

int main() {
    int a = AvailiabilityCheck();
    if (a!=0){
        switch (a)
        {
        case -1:
            printf("Incorrect Dir %s\r\n", DataDir.c_str());
            break;
        case -2:
            printf("Config Not Found\r\n");
            break;
        case -3:
            printf("Empty Config\r\n");
            break;
        case -4:
            printf("Ivalid Config\r\n");
        }
        system("pause");
        return -1;
    }
    Log("*RST");
    Log("Main Started!");
    SERVICE_TABLE_ENTRY e[2]{
        {(LPWSTR)L"Test",(LPSERVICE_MAIN_FUNCTION)ServiceMain},
        {NULL,NULL}
    };

    StartServiceCtrlDispatcher(e);

    return 0;
}

VOID Log(std::string text) {
    if (ENABLE_LOG==0) {
        return;
    }
    GetLocalTime(&stime);
    CHAR t[30] = { 0 };
    sprintf(t, "[%02d:%02d] ", stime.wHour, stime.wMinute);
    CHAR p[50] = { 0 };
    sprintf(p, "%s\\%s", DataDir.c_str(), LogName.c_str());
    // MessageBoxA(NULL, p, p, 0);
    FILE* fp;
    if (text == "*RST") {
        fp = fopen(p, "w+");
        fclose(fp);
        return;
    }
    fp = fopen(p, "a+");
    if (fp == NULL) {
        return;
    }
    fprintf_s(fp, "%s\n",((string)t + text).c_str());
    fclose(fp);
}

VOID wLog(std::wstring text) {
    if (ENABLE_LOG == 0) {
        return;
    }
    CHAR p[50] = { 0 };
    sprintf(p, "%s\\%s", DataDir.c_str(), LogName.c_str());
    // MessageBoxA(NULL, p, p, 0);
    FILE* fp;
    if (text == L"*RST") {
        fp = fopen(p, "w+");
        fclose(fp);
        return;
    }
    fp = fopen(p, "a+");
    if (fp == NULL) {
        return;
    }
    fwprintf_s(fp, L"%S\n", text.c_str());
    fclose(fp);
}

VOID WINAPI ServiceMain(int argc, char* argv[]) {
    hs = RegisterServiceCtrlHandler(L"Test", (LPHANDLER_FUNCTION)ServiceController);
    s.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    s.dwControlsAccepted = SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_PAUSE_CONTINUE;
    s.dwCurrentState = SERVICE_START_PENDING;
    s.dwCheckPoint = 0;
    s.dwServiceSpecificExitCode = 0;
    s.dwWaitHint = 0;
    s.dwWin32ExitCode = 0;
    if (!SetServiceStatus(hs, &s)) {
        Log("Start Failed");
    }

    Log("Start success");
    s.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hs, &s);

    // AvailiabilityCheck();

    while (s.dwCurrentState!=SERVICE_STOPPED)
    {
        while (s.dwCurrentState == SERVICE_RUNNING) {
            PayLoad();
            Sleep(1500);
        }
        
    }
    
}

VOID WINAPI ServiceController(DWORD ControlCode) {
    switch (ControlCode)
    {
    case SERVICE_CONTROL_PAUSE:
        s.dwCurrentState = SERVICE_PAUSED;
        Log("Service Paused");
        SetServiceStatus(hs, &s);
        break;
    case SERVICE_CONTROL_CONTINUE:
        s.dwCurrentState = SERVICE_RUNNING;
        Log("Service Paused");
        SetServiceStatus(hs, &s);
        break;
    case SERVICE_CONTROL_STOP:
        StopService();
        break;
    default:
        break;
    }
}

VOID StopService() {
    Log("Stopping...");
    s.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(hs, &s);
}
