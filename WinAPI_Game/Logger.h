#pragma once

#include <string>
#include <fstream>
#include <windows.h>

using namespace std;

class Logger
{
public:
	Logger(string logger_type);
	~Logger();
	DWORD WINAPI logAngles(LPVOID lpParam);
	DWORD WINAPI logCoordinates(LPVOID lpParam);
	DWORD WINAPI logGameInfo(LPVOID lpParam);
private:
	bool started = false;
	bool log = false;
	HANDLE hLoggerThread;
	ofstream stream;
	const string angles_file = "";
	const string coordinates_file = "";
	const string gameinfo_file = "";
	string logger_type;
};

