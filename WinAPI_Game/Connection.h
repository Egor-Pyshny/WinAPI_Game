#pragma once

#include <Winsock2.h>
#include <WS2tcpip.h>
#include <string>
#include <iostream>
#include <Windows.h>

class Connection
{
public:
	Connection(const char* serverAddress, int serverPort);
	~Connection();
	bool Connect();
	bool NextXY(POINTFLOAT& point);
	static const char* GetIp(HWND hText);
protected:
	bool GetCoord(float& coord);
private:
	const char* serverAddress_;
	int serverPort_;
	WSADATA wsaData_;
	SOCKET clientSocket_;
	sockaddr_in serverAddressInfo_;
};

