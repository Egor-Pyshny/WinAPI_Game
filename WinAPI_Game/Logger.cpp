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
		typedef DWORD(FUNC)(LPVOID lpParam);
		FUNC* logingFunc;
		info inf;
		inf.instance = this;
		switch (this->logger_type) {
		case ANGLES:
		{
			inf.queues.angles_queue = (queue<POINTFLOAT>*)lpParam;
			logingFunc = logingAngles;
			break;
		}
		case COORDS:
		{
			inf.queues.coords_queue = (queue<POINT>*)lpParam;
			logingFunc = logingCoords;
			break;
		}
		case TARGETS:
		{
			inf.queues.targets_queue = (queue<target>*)lpParam;
			logingFunc = logingTargets;
			break;
		}
		default:
		{
			started = false;
			return false;
		}
		}
		hLoggerThread = CreateThread(nullptr, 0, logingFunc, &inf, 0, nullptr);;
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

string __str(LPVOID lpParam, int type){

}

DWORD WINAPI logingAngles(LPVOID lpParam)
{
	info* inf = (info*)lpParam;
	Logger* l = inf->instance;
	queue<POINTFLOAT>* data = inf->queues.angles_queue;
	ofstream file(l->angles_file);
	while (l->started) {
		if (l->log) {
			if (!(data->empty())) {
				POINTFLOAT p = data->front();
				data->pop();
				file << format("\n{ ""x"":{}, ""y"":{} }",p.x,p.y);
			}
		}
	}
	return 0;
}
