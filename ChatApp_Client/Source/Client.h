#pragma once

class CClient
{
public:
	CClient();
	~CClient();

	void Init();

	void Run();

private:
	void InitLog();

	void Connect();

	void GetLocalInput(std::string InCommand);
	
private:
	WSADATA Data;
	SOCKET ClientSocket;
	SOCKADDR_IN ClientAddress;
	bool bIsConnected;
};