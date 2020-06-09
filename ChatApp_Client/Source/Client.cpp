#include "stdafx.h"
#include "Client.h"

CClient::CClient()
{
	bIsConnected = false;
}

CClient::~CClient()
{

}

void CClient::Init()
{
	InitLog();

	int32_t Result = WSAStartup(MAKEWORD(2, 2), &Data);
	if (Result != 0)
	{
		spdlog::error("WSAStartup failed with error : {}", Result);
	}

	ClientAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	ClientAddress.sin_family = AF_INET;
	ClientAddress.sin_port = htons(1234);

	ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSocket == INVALID_SOCKET)
	{
		spdlog::error("socket failed with error : {}", WSAGetLastError());
	}

	Connect();
	std::cout << "Connected to server" << std::endl;
	bIsConnected = true;
}

void CClient::Run()
{
	while (bIsConnected)
	{
		char Buffer[1024];
		memset(Buffer, 0, sizeof(Buffer));
		recv(ClientSocket, Buffer, sizeof(Buffer), 0);

		std::string Message;
		getline(std::cin, Message);
		memcpy(Buffer, Message.c_str(), Message.length());

		send(ClientSocket, Message.c_str(), (int32_t)Message.length(), 0);
		recv(ClientSocket, Buffer, sizeof(Buffer), 0);
	}
}

void CClient::InitLog()
{
	spdlog::set_level(spdlog::level::trace);
	spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
	auto FileLogger = spdlog::basic_logger_mt("BasicLogger", "Logs/Log.txt");
	spdlog::set_default_logger(FileLogger);
}

void CClient::Connect()
{
	int32_t Result = connect(ClientSocket, (SOCKADDR*)&ClientAddress, sizeof(ClientAddress));
	if (Result != 0)
	{
		spdlog::error("connect failed with error : {}", WSAGetLastError());
	}
}

void CClient::GetLocalInput()
{
	std::string Command;

	while (bIsConnected)
	{
		if (strcmp(Command.c_str(), "@quit") == 0)
		{
			bIsConnected = false;
			std::cout << "Disconnected from server" << std::endl;
			break;
		}
	}
}
