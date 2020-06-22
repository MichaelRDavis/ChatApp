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

	//SetNonBlocking(ServerSocket, true);

	Bind();
	Listen();

	FD_ZERO(&ServerSet);
	FD_SET(ServerSocket, &ServerSet);

	bCanRun = true;
}

void CServer::Run()
{
	while (bCanRun)
	{
		std::cout << "Listen for incoming connections" << std::endl;

		int32_t NumSockets = select(0, &ServerSet, nullptr, nullptr, nullptr);
		for (int32_t i = 0; i < NumSockets; i++)
		{
			SOCKET InSocket = ServerSet.fd_array[i];

			// Check to see if this is new connection
			if (InSocket == ServerSocket)
			{
				// Accept the connection to server
				SOCKET ClientSocket = accept(ServerSocket, nullptr, nullptr);
				if (ClientSocket != INVALID_SOCKET)
				{
					FD_SET(ClientSocket, &ServerSet);

					// Add the client to the client map and send welcome message
					OnConnected(ClientSocket);
				}
			}
			else
			{
				// Receives messages from connected clients and echo the messages to all the connected clients 
				GetMessages(InSocket);
			}
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
	if (Result == SOCKET_ERROR)
	{
		spdlog::error("send failed with error : {}", WSAGetLastError());
	}
}

int32_t CServer::Receive(SOCKET InSocket, const char* Buffer)
{
	int32_t Result = recv(InSocket, (char*)Buffer, sizeof(Buffer), 0);
	if (Result != 0)
	{
		spdlog::error("recv failed with error : {}", WSAGetLastError());
	}

	return Result;
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
	char Buffer[4096];
	memset(Buffer, 0, sizeof(Buffer));
	int32_t Bytes = recv(InSocket, Buffer, sizeof(Buffer), 0);
	if (Bytes <= 0)
	{
		closesocket(InSocket);
		FD_CLR(InSocket, &ServerSet);
	}
	else
	{
		auto Client = ClientMap.find(InSocket);

		for (int32_t i = 0; i < ServerSet.fd_count; i++)
		{
			SOCKET OutSocket = ServerSet.fd_array[i];
			if (OutSocket != ServerSocket && OutSocket != InSocket)	// This check fails for some reason :(
			{
				std::string Command;
				Command = Buffer;

				// This is really bad way to evaluate user commands, but it works
				// Set the users name 
				if (strcmp(Command.c_str(), "@set_name") == 0)
				{
					const char* Name = Command.c_str() + 9;

					// Tell all clients that a user has changed their name
					std::string OriginalName = Client->second.Name;
					std::string AllMessage =  OriginalName + " changed their name to " + Name;
					SendMessageToAllClients(AllMessage.c_str());

					// Send a reply to the client that changed their name
					std::string Message = OriginalName + " you changed your name to " + Name;
					SendMessageToClient(Client->first, Message.c_str());
					
					// Change the users name
					SetUsername(InSocket, Name);
				}

				// Send a goodbye message to the client that quit the server
				if (strcmp(Command.c_str(), "@quit") == 0)
				{
					SendMessageToClient(Client->first, "Goodbye");
				}

				for (auto& Client : ClientMap)
				{
					send(OutSocket, Buffer, Bytes, 0);
				}
			}
		}
	}
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

		std::string Username = "JohnSmith_";
		AddClient(InSocket, Username.c_str());
		std::string WelcomeMessage = "Hello " + Username;
		send(InSocket, WelcomeMessage.c_str(), WelcomeMessage.size(), 0);
	}
}
