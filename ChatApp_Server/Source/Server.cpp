#include "stdafx.h"
#include "Server.h"
#include "Client.h"

CServer::CServer()
{
	ServerSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;
	Port = 1234;
	bCanRun = false;
}

CServer::~CServer()
{

}

void CServer::Init()
{
	LogInit();

	int32_t Result = WSAStartup(MAKEWORD(2,2), &Data);
	if (Result != 0)
	{
		spdlog::error("WSAStartup failed with error : {}", Result);
	}

	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr.s_addr = INADDR_ANY;
	ServerAddress.sin_port = htons(Port);

	ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ServerSocket == INVALID_SOCKET)
	{
		spdlog::error("socket failed with error : {}", WSAGetLastError());
	}

	Bind();
	Listen();

	InSockets.push_back(ServerSocket);

	bCanRun = true;
}

void CServer::Run()
{
	while (bCanRun)
	{
		std::cout << "Listening for connections" << std::endl;

		char Buffer[1024];
		ClientSocket = Accept();
		if (ClientSocket != INVALID_SOCKET)
		{
			std::cout << "Client connected to server" << std::endl;
			HandleNewClient(ClientSocket, ClientAddress);

			memset(Buffer, 0, sizeof(Buffer));
			recv(ClientSocket, Buffer, sizeof(Buffer), 0);
			std::cout << "Client message: " << Buffer << std::endl;

			std::string Message = "Server: ";
			Message += Buffer;
			HandleClientMessage(Buffer);

			memcpy(Buffer, Message.c_str(), Message.length());

			send(ClientSocket, Buffer, sizeof(Buffer), 0);
		}
	}
}

void CServer::LogInit()
{
	spdlog::set_level(spdlog::level::trace);
	spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
	auto FileLogger = spdlog::basic_logger_mt("BasicLogger", "Logs/Log.txt");
	spdlog::set_default_logger(FileLogger);
}

void CServer::Bind()
{
	int32_t Result = bind(ServerSocket, (SOCKADDR*)&ServerAddress, sizeof(ServerAddress));
	if (Result != 0)
	{
		spdlog::error("bind failed with error : {}", WSAGetLastError());
	}
}

void CServer::Listen()
{
	int32_t Result = listen(ServerSocket, 0);
	if (Result != 0)
	{
		spdlog::error("listen failed with error : {}", WSAGetLastError());
	}
}

SOCKET CServer::Accept()
{
	int32_t AddressSize = sizeof(ClientAddress);
	SOCKET NewSocket = accept(ServerSocket, (SOCKADDR*)&ClientAddress, &AddressSize);
	if (NewSocket == INVALID_SOCKET)
	{
		spdlog::error("accept failed with error : {}", WSAGetLastError());
		return INVALID_SOCKET;
	}

	return NewSocket;
}

void CServer::Select()
{
	
}

void CServer::HandleNewClient(SOCKET InClientSocket, SOCKADDR_IN InClientAddress)
{
	SClient NewClient;
	NewClient.Socket = InClientSocket;
	NewClient.Address = InClientAddress;
	NewClient.UniqueID = NewClient.GenerateID();

	// Get the clients name
	char Buffer[1024];
	std::string ServerMessage = "Insert your name: ";
	memcpy(Buffer, ServerMessage.c_str(), ServerMessage.length());
	send(ClientSocket, Buffer, sizeof(Buffer), 0);

	memset(Buffer, 0, sizeof(Buffer));
	recv(ClientSocket, Buffer, sizeof(Buffer), 0);

	NewClient.Name = Buffer;
	std::string HelloMessage = "Hello, " + NewClient.Name;
	send(ClientSocket, Buffer, sizeof(Buffer), 0);
}

void CServer::HandleClientMessage(const char* InBuffer)
{
	
}
