#pragma once

#define ANGLES 1
#define COORDS 2
#define GAMEINFO 3

#include <string>
#include <fstream>
#include <queue>
#include <windows.h>
#include "Target.h"

using namespace std;



typedef struct _info {
	Logger* instance;
	int type;
	union _queues {
		queue<POINT>* coords_queue;
		queue<POINTFLOAT>* angles_queue;
		queue<target>* targets_queue;
	} queues;
} info;

DWORD WINAPI loging(LPVOID lpParam);

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
};

