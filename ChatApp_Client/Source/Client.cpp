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
	char Buffer[4096];
	memset(Buffer, 0, sizeof(Buffer));
	std::string Message;

	while (bIsConnected)
	{
		std::cout << "> ";
		getline(std::cin, Message);
		if (Message.size() > 0)
		{
			GetLocalInput(Message);
			send(ClientSocket, Message.c_str(), (int32_t)Message.size() + 1, 0);
		}

		int32_t Bytes = recv(ClientSocket, Buffer, sizeof(Buffer), 0);
		if (Bytes > 0)
		{
			std::cout << "Server: " << std::string(Buffer, 0, Bytes) << std::endl;
		}
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

void CClient::GetLocalInput(std::string InCommand)
{
	if (strcmp(InCommand.c_str(), "@quit") == 0)
	{
		std::cout << "Disconnected from server" << std::endl;
		bIsConnected = false;
	}
}
