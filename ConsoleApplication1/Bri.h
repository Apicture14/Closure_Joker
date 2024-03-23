#pragma once
#define BRI_H_
#include <iostream>
#include <string>
#include <Windows.h>
#include <optional>

using namespace std;

VOID WINAPI ServiceMain(int argc, char* argv[]);
VOID WINAPI ServiceController(DWORD ControlCode);
INT AvailiabilityCheck();
VOID StopService();
VOID Log(std::string text);
VOID PayLoad();
VOID Crt();

extern std::string DataDir;
extern std::string LogName;
extern std::string ConfigName;
extern std::string fullPath;
extern std::string shortPath;

