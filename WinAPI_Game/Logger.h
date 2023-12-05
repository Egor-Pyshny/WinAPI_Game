#pragma once

#define ANGLES 1
#define COORDS 2
#define GAMEINFO 3

#include <string>
#include <fstream>
#include <windows.h>

using namespace std;

DWORD WINAPI logAngles(LPVOID lpParam);
DWORD WINAPI logCoordinates(LPVOID lpParam);
DWORD WINAPI logGameInfo(LPVOID lpParam);

class Logger
{
public:
	Logger(int logger_type);
	~Logger();
	void changeType(int logger_type);
	bool start(LPVOID lpParam);
	void stop();

private:
	bool started = false;
	bool first_start = true;
	bool log = false;
	HANDLE hLoggerThread = NULL;
	ofstream stream;
	int logger_type;
	const string angles_file = "";
	const string coordinates_file = "";
	const string gameinfo_file = "";
};

