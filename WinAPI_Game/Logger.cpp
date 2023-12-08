#include "Logger.h"

Logger::Logger(int logger_type, DWORDLONG game_id)
{
	this->logger_type = logger_type;
	this->game_id = game_id;
}

Logger::~Logger()
{
	if (this->started) {
		WaitForSingleObject(this->hLoggerThread, INFINITE);
	}
}

bool Logger::start()
{
	if (!started) {
		started = true;
		this->log = true;
		typedef DWORD(FUNC)(LPVOID lpParam);
		FUNC* logingFunc;
		switch (this->logger_type) {
		case ANGLES:
		{
			logingFunc = logingAngles;
			break;
		}
		case COORDS:
		{
			logingFunc = logingCoords;
			break;
		}
		case TARGETS:
		{
			logingFunc = logingTargets;
			break;
		}
		default:
		{
			started = false;
			return false;
		}
		}
		hLoggerThread = CreateThread(nullptr, 0, logingFunc, this, 0, nullptr);;
		if (hLoggerThread == NULL) {
			return false;
		}
	}
	else return false;
	return true;
}

void Logger::stop()
{
	this->log = false;
}

void Logger::setGameId(DWORDLONG game_id)
{
	this->game_id = game_id;
}

void Logger::finish()
{
	if (this->started) {
		this->started = false;
		this->log = false;
		WaitForSingleObject(this->hLoggerThread, INFINITE);
	}
}

DWORD WINAPI logingAngles(LPVOID lpParam)
{
	/*Logger* l = (Logger*)lpParam;
	queue<POINTFLOAT>* data = l->queues.angles_queue;
	ofstream file(ANGLESFILE, std::ios::app);
	file << format("[GAMEID = {}]", l->game_id);
	while (l->started) {
		if (l->log) {
			if (!(data->empty())) {
				POINTFLOAT p = data->front();
				data->pop();
				file << "\n{ ""x"":" << to_string(p.x) << ", ""y"":" << to_string(p.y) << " }";
			}
		}
	}
	while (!(data->empty())) {
		POINTFLOAT p = data->front();
		data->pop();
		file << "\n{ ""x"":" << to_string(p.x) << ", ""y"":" << to_string(p.y) << " }";
	}
	file << "\n";
	file.close();*/
	return 0;
}

DWORD WINAPI logingCoords(LPVOID lpParam)
{
	Logger* l = (Logger*)lpParam;
	queue<POINT>* data = l->queues.coords_queue;
	ofstream file(COORDINATESFILE, std::ios::app);
	file << format("[GAMEID = {}]", l->game_id);
	while (l->started) {
		if (l->log) {
			if (data->size()>0) {
				POINT p = data->front();
				data->pop();
				file << "\n{ ""x"":"<<to_string(p.x)<<", ""y"":"<<to_string(p.y)<< " }";
			}
		}
	}
	/*while (!(data->empty())) {
		POINT p = data->front();
		data->pop();
		file << "\n{ ""x"":" << to_string(p.x) << ", ""y"":" << to_string(p.y) << " }";
	}*/
	file << "\n";
	file.close();
	return 0;
}

DWORD WINAPI logingTargets(LPVOID lpParam)
{
	//Logger* l = (Logger*)lpParam;
	//vector<target>* data = l ->queues.targets_queue;
	//ofstream file(TARGETSFILE, std::ios::app);
	//file << format("[GAMEID = {}]", l->game_id);
	//for(target t : *data){
	//	file << "1";
	//	//file << format("\n{ ""x"":{}, ""y"":{}, ""sections"":{}, ""ttl"":{} }", t.getX(), t.getY(), t.getSections());
	//}
	//file << "\n";
	//file.close();
	return 0;
}
