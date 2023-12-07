#pragma once

#define ANGLES 1
#define COORDS 2
#define TARGETS 3

#include <string>
#include <fstream>
#include <queue>
#include <windows.h>
#include <format>
#include "Target.h"

using namespace std;

struct _info {
	Logger* instance;
	union _queues {
		queue<POINT>* coords_queue;
		queue<POINTFLOAT>* angles_queue;
		queue<target>* targets_queue;
	} queues;
} info;

DWORD WINAPI logingAngles(LPVOID lpParam);
DWORD WINAPI logingCoords(LPVOID lpParam);
DWORD WINAPI logingTargets(LPVOID lpParam);

class Logger
{
public:
	Logger(int logger_type, int game_id);
	~Logger();
	bool start(LPVOID lpParam);
	void stop();
private:
	bool started = false;
	bool first_start = true;
	bool log = false;
	HANDLE hLoggerThread = NULL;
	ofstream stream;
	int logger_type;
	int game_id;
	const string angles_file = "";
	const string coordinates_file = "";
	const string gameinfo_file = "";
	friend DWORD WINAPI logingAngles(LPVOID lpParam);
	friend DWORD WINAPI logingCoords(LPVOID lpParam);
	friend DWORD WINAPI logingTargets(LPVOID lpParam);
};

