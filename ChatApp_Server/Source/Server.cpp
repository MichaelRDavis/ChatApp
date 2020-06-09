#include "stdafx.h"
#include "Server.h"

CServer::CServer()
{
	ServerSocket = INVALID_SOCKET;
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

	SetNonBlocking(ServerSocket, true);

	Bind();
	Listen();

	bCanRun = true;
}

void CServer::Run()
{
	while (bCanRun)
	{
		char Buffer[1024];
		std::cout << "Listen for incoming connections" << std::endl;
		SOCKET NewSocket = Accept();
		if (NewSocket != INVALID_SOCKET)
		{
			SetNonBlocking(NewSocket, true);

			//memset(Buffer, 0, sizeof(Buffer));
			//std::cout << "Client connected!" << std::endl;
			//recv(NewSocket, Buffer, sizeof(Buffer), 0);
			//std::cout << "Client Says: " << Buffer << std::endl;

			//std::string ResponceText = "Hello ";
			//ResponceText += Buffer;

			//memcpy(Buffer, ResponceText.c_str(), ResponceText.length());

			//send(NewSocket, Buffer, sizeof(Buffer), 0);
			OnConnected(NewSocket);
			GetMessages(NewSocket);
			GetUserInput();
		}
	}

	std::cout << "Shutting down server" << std::endl;

	for (auto Client : ClientMap)
	{
		SendMessageToClient(Client.first, "Server is shutting down. Bye.");
		closesocket(Client.first);
	}

	ClientMap.clear();
	closesocket(ServerSocket);
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

void CServer::Send(SOCKET InSocket, const char* InBuffer)
{
	int32_t Result = send(InSocket, InBuffer, sizeof(InBuffer), 0);
	if (Result != 0)
	{
		spdlog::error("send failed with error : {}", WSAGetLastError());
	}
}

void CServer::Receive(SOCKET InSocket, const char* Buffer)
{
	int32_t Result = recv(InSocket, (char*)Buffer, sizeof(Buffer), 0);
	if (Result != 0)
	{
		spdlog::error("recv failed with error : {}", WSAGetLastError());
	}
}

void CServer::SetNonBlocking(SOCKET InSocket, bool bShouldBlock)
{
	u_long bBlock = bShouldBlock ? 1 : 0;
	int32_t Result = ioctlsocket(InSocket, FIONBIO, &bBlock);
	if (Result != 0)
	{
		spdlog::error("ioctlsocket failed with error : {}", WSAGetLastError());
	}
}

void CServer::GetUserInput()
{
	std::string Command;
	getline(std::cin, Command);
	if (strcmp(Command.c_str(), "@quit") == 0)
	{
		bCanRun = false;
		std::cout << "Shutdown server" << std::endl;
	}
}

void CServer::GetMessages(SOCKET InSocket)
{
	char Buffer[1024];
	memset(Buffer, 0, sizeof(Buffer));
	recv(InSocket, Buffer, sizeof(Buffer), 0);

	std::string Command;
	Command = Buffer;
	if (strcmp(Command.c_str(), "@set_username") == 0)
	{
		// Change the users name
		SetUsername(InSocket, Buffer);
	}

	SendMessageToAllClients(Buffer);
}

void CServer::SendMessageToAllClients(const char* InBuffer)
{
	for (auto& Client : ClientMap)
	{
		Send(Client.first, InBuffer);
	}
}

void CServer::SendMessageToClient(SOCKET InSocket, const char* InBuffer)
{
	Send(InSocket, InBuffer);
}

void CServer::AddClient(SOCKET& InSocket, const char* Username)
{
	ClientMap[InSocket];
	ClientMap[InSocket].Name = Username;
}

void CServer::SetUsername(SOCKET Socket, const char* Username)
{
	ClientMap[Socket].Name = Username;
}

void CServer::OnConnected(SOCKET InSocket)
{
	// Check for a new connection
	if (ClientMap.find(InSocket) == ClientMap.end())
	{
		std::cout << "Client connected to server" << std::endl;

		// Generate a user name
		char Buffer[1024];
		memset(Buffer, 0, sizeof(Buffer));

		std::string Username = "JohnSmith";
		AddClient(InSocket, Username.c_str());

		std::string WelcomeMessage = "Hello";
		memcpy(Buffer, Username.c_str(), Username.length());
		SendMessageToClient(InSocket, Buffer);
	}
}
