#pragma once

class CServer
{
public:
	CServer();
	~CServer();

	void Init();

	void Run();

private:
	void LogInit();

	void Bind();

	void Listen();

	SOCKET Accept();

private:
	WSADATA Data;
	SOCKET ServerSocket;
	SOCKET ClientSocket;
	SOCKADDR_IN ServerAddress;
	SOCKADDR_IN ClientAddress;
	uint16_t Port;
	bool bCanRun;
};