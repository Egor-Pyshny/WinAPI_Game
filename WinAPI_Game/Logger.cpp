#include "Logger.h"

Logger::Logger(int logger_type)
{
	this->logger_type = logger_type;
}

Logger::~Logger()
{
	if (this->started) {
		this->started = false;
		WaitForSingleObject(this->hLoggerThread, INFINITE);
	}
}

void Logger::changeType(int logger_type)
{
	this->logger_type = logger_type;
}

bool Logger::start(LPVOID lpParam)
{
	started = true;
	if (first_start) {
		switch (this->logger_type) {
		case ANGLES:
		{
			hLoggerThread = CreateThread(nullptr, 0, logAngles, lpParam, 0, nullptr);;
			break;
		}
		case COORDS:
		{
			hLoggerThread = CreateThread(nullptr, 0, logCoordinates, lpParam, 0, nullptr);;
			break;
		}
		case GAMEINFO:
		{
			hLoggerThread = CreateThread(nullptr, 0, logGameInfo, lpParam, 0, nullptr);;
			break;
		}
		default:
		{
			started = false;
			return false;
		}
		}
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

DWORD WINAPI logAngles(LPVOID lpParam)
{
	ofstream stream("");
	while (started) {
		if (log) {
		
		}
	}
	return 0;
}

DWORD WINAPI logCoordinates(LPVOID lpParam)
{
	ofstream stream("");
	while (started) {
		if (log) {

		}
	}
	return 0;
}

DWORD WINAPI logGameInfo(LPVOID lpParam)
{
	ofstream stream("");
	while (started) {
		if (log) {

		}
	}
	return 0;
}
