#pragma once

#pragma comment(lib, "ws2_32.lib")
#include <Winsock2.h>
#include <WS2tcpip.h>
#include <queue>
#include <iostream>
#include <Windows.h>
#include "Logger.h"

using namespace std;

class Connection
{
public:
	Connection(const char* serverAddress, int serverPort);
	~Connection();
	void setLogger(Logger* log);
	bool Connect();
	bool NextXY(POINTFLOAT& point);
protected:
	bool GetCoord(float& coord);
private:
	const char* serverAddress_;
	int serverPort_;
	Logger* log;
	queue<POINTFLOAT> points;
	WSADATA wsaData_;
	SOCKET clientSocket_;
	sockaddr_in serverAddressInfo_;
};

