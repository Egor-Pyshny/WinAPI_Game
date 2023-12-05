#include "Logger.h"

Logger::Logger(int logger_type, int game_id)
{
	this->logger_type = logger_type;
	this->game_id = game_id;
}

Logger::~Logger()
{
	if (this->started) {
		this->started = false;
		WaitForSingleObject(this->hLoggerThread, INFINITE);
	}
}

bool Logger::start(LPVOID lpParam)
{
	started = true;
	if (first_start) {
		switch (this->logger_type) {
		case ANGLES:
		{
			
			break;
		}
		case COORDS:
		{
			
			break;
		}
		case GAMEINFO:
		{
			
			break;
		}
		default:
		{
			started = false;
			return false;
		}
		}
		hLoggerThread = CreateThread(nullptr, 0, loging, lpParam, 0, nullptr);;
		if (hLoggerThread == NULL) {
			return false;
		}
		first_start = false;
	}
	return true;
}

void Logger::stop()
{
	this->log = false;
}

DWORD WINAPI loging(LPVOID lpParam)
{
	info* inf = (info*)lpParam;
	switch (inf->type)
	{

	default:
		break;
	}
	while (started) {
		if (log) {
		
		}
	}
	return 0;
}
