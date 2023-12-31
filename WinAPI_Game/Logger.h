#pragma once

#include <string>
#include <fstream>
#include <queue>
#include <windows.h>
#include <format>
#include <iomanip> 
#include "Target.h"
#include "resource.h"
#include "MyDefines.h"

using namespace std;

DWORD WINAPI logingAngles(LPVOID lpParam);
DWORD WINAPI logingCoords(LPVOID lpParam);
DWORD WINAPI logingTargets(LPVOID lpParam);

class Logger
{
public:
	Logger(int logger_type, DWORDLONG game_id);
	~Logger();
	bool start();
	void finish();
	void stop();	
	void setGameId(DWORDLONG game_id);
	union _queues {
		queue<POINT>* coords_queue;
		queue<POINTFLOAT>* angles_queue;
		vector<target>* targets_queue;
	} queues;
private:
	bool started = false;
	bool first_start = true;
	bool log = false;
	HANDLE hLoggerThread = NULL;
	ofstream stream;
	int logger_type;
	DWORDLONG game_id;

	friend DWORD WINAPI logingAngles(LPVOID lpParam);
	friend DWORD WINAPI logingCoords(LPVOID lpParam);
	friend DWORD WINAPI logingTargets(LPVOID lpParam);
};

