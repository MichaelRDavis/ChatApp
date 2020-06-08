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

	void Select();

	void HandleNewClient(SOCKET InClientSocket, SOCKADDR_IN InClientAddress);
	
	void HandleClientMessage(const char* InBuffer);

private:
	WSADATA Data;
	SOCKET ServerSocket;
	SOCKET ClientSocket;
	std::vector<SOCKET> InSockets;
	std::vector<SOCKET> OutSockets;
	SOCKADDR_IN ServerAddress;
	SOCKADDR_IN ClientAddress;
	uint16_t Port;
	bool bCanRun;
};