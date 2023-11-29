#include "Connection.h"

Connection::Connection(const char* serverAddress, int serverPort) : serverAddress_(serverAddress), serverPort_(serverPort), clientSocket_(INVALID_SOCKET), serverAddressInfo_()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData_) != 0)
	{
		std::cerr << "Failed to initialize Winsock\n";
	}
}

Connection::~Connection()
{
	if (clientSocket_ != INVALID_SOCKET)
	{
		closesocket(clientSocket_);
	}

	WSACleanup();
}

bool Connection::Connect()
{
	CHAR receivedByte;
	INT bytesRead, bytesSent;

	clientSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (clientSocket_ == INVALID_SOCKET)
	{
		std::cerr << "Failed to create socket\n";
		return false;
	}

	serverAddressInfo_.sin_family = AF_INET;
	serverAddressInfo_.sin_port = htons(serverPort_);
	inet_pton(AF_INET, serverAddress_, &serverAddressInfo_.sin_addr);

	if (connect(clientSocket_, (struct sockaddr*)&serverAddressInfo_, sizeof(serverAddressInfo_)) == SOCKET_ERROR)
	{
		closesocket(clientSocket_);
		return false;
	}

	bytesRead = recv(clientSocket_, &receivedByte, sizeof(receivedByte), 0);
	if (bytesRead == SOCKET_ERROR || bytesRead == 0 || receivedByte != 23)
	{
		closesocket(clientSocket_);
		return false;
	}

	bytesSent = send(clientSocket_, &receivedByte, sizeof(receivedByte), 0);
	if (bytesSent == SOCKET_ERROR)
	{
		closesocket(clientSocket_);
		return false;
	}

	return true;
}

bool Connection::NextXY(POINTFLOAT& point)
{
	FLOAT x, y, z;
	if (GetCoord(x) && GetCoord(y) && GetCoord(z))
	{
		point = { x, y };
		return true;
	}
	return false;
}

bool Connection::GetCoord(float& coord)
{
	char buffer[4];
	int bytesRead = recv(clientSocket_, buffer, sizeof(buffer), 0);

	if (bytesRead == SOCKET_ERROR)
	{
		return FALSE;
	}
	else if (bytesRead == 0)
	{
		return FALSE;
	}
	else if (bytesRead != sizeof(buffer))
	{
		return FALSE;
	}

	coord = reinterpret_cast<float*>(buffer)[0];

	return TRUE;
}