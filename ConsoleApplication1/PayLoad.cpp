#include <iostream>
#include <Windows.h>
#include <WtsApi32.h>
#include <TlHelp32.h>
#include <UserEnv.h>
#include "Bri.h"
#include "Consts.h"
#include <io.h>
#include <string>

#pragma comment(lib,"UserEnv.lib")
#pragma comment(lib,"WtsApi32.lib")

std::string DataDir = "C:\\yService\\Booter";
std::string LogName = "log.txt";
std::string ConfigName = "config.txt";
std::string fullPath = "";
std::string shortPath = "";
CHAR c[50] = { 0 };

VOID PayLoad() {
	PROCESSENTRY32 pi;
	pi.dwSize = sizeof(PROCESSENTRY32);
	HANDLE p = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (p==INVALID_HANDLE_VALUE){
		Log("Walk process failed");
		StopService();
	}
	BOOL found = FALSE;
	BOOL bRet = Process32First(p, &pi);
	if (!bRet) {
		Log("Walk First Failed" + std::to_string(GetLastError()));
		StopService();
	}
	found = FALSE;
	while (bRet)
	{
		 
		 sprintf(c, "%ws", pi.szExeFile);
		 // Log((string)c + " " + shortPath + " " + std::to_string(strcmp(c, shortPath.c_str())));
		 if (strcmp(c,shortPath.c_str())==0) {
			found = TRUE;
		}
		bRet = Process32Next(p, &pi);
	}
	if (found==FALSE) {
		Crt();
	}
}

INT AvailiabilityCheck() {
	CHAR c[50] = { 0 }; sprintf(c, "%s\\%s", DataDir.c_str(), ConfigName.c_str());
	CHAR r[200] = { 0 };
	if (access(DataDir.c_str(), 00) != 0) {
		// FILE* fp; fp = fopen(c, "w+"); fclose(fp);
		// Log("Data Folder Created, wating for checked, exited");
		return -1;
	}
	else {
		if (access(c, 00) != 0) {
			FILE* fp; fp = fopen(c, "w+"); fclose(fp);
			Log("Config not found,created,exiting");
			return -2;
		}
		FILE* fp; fp = fopen(c, "r"); fseek(fp, 0, SEEK_END); int len = ftell(fp); rewind(fp);
		if (len == 0) {
			Log("Empty config,exiting");
			return -3;
		}
		fread(r, 1, len, fp); fclose(fp);
		if (access((LPCSTR)r, 00) != 0) {
			Log("File not found");
			return -4;
		}
		fullPath = r;
		shortPath = fullPath.substr(fullPath.find_last_of("/") + 1, -1);
		Log("getT "+fullPath + " and " + shortPath);
		return 0;
	}
}

VOID Crt() {
	HANDLE hToken, hTokenN;
	BOOL bRet = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
	if (!bRet) {
		Log("Get Token Failed");
		CloseHandle(hToken);
		CloseHandle(hTokenN);
		StopService();
	}
	bRet = DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityIdentification, TokenPrimary, &hTokenN);
	if (!bRet) {
		Log("Copy Token Failed");
		CloseHandle(hToken);
		CloseHandle(hTokenN);
		StopService();
	}
	DWORD SessionIdN = WTSGetActiveConsoleSessionId();
	bRet = WTSQueryUserToken(SessionIdN, &hTokenN);
	// bRet = SetTokenInformation(hTokenN, TokenSessionId, &SessionIdN, sizeof(DWORD));
	if (!bRet) {
		Log("Set Token Failed");
		CloseHandle(hToken);
		CloseHandle(hTokenN);
		StopService();
	}
	LPVOID lpEnv;
	bRet = CreateEnvironmentBlock(&lpEnv, hTokenN, FALSE);
	if (!bRet) {
		Log("Create Env Failed");
	}

	PROCESS_INFORMATION pi;
	STARTUPINFOA s;
	memset(&s, 0, sizeof(s));
	memset(&pi, 0, sizeof(pi));

	bRet = CreateProcessAsUserA(
		hTokenN,
		(LPCSTR)fullPath.c_str(),
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS,
		lpEnv,
		NULL,
		&s,
		&pi
	);
	Log("Booting..");

	if (!bRet) {
		Log("Start Failed"+std::to_string(GetLastError()));
		CloseHandle(hToken);
		CloseHandle(hTokenN);
		StopService();
	}
	else {
		Log("Booted"+std::to_string(pi.dwProcessId));
	}
	CloseHandle(hToken);
	CloseHandle(hTokenN);
}



