#pragma once

#include <string>
#include <fstream>
#include <windows.h>

using namespace std;

class Logger
{
public:
	Logger(string logger_type);
	void logAngles(POINTFLOAT p);
	void logCoordinates(POINT p);
private:
	ofstream stream;
	const string angles_file = "";
	const string coordinates_file = "";
};

