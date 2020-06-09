#pragma once

/**
 * The clients data storage, we store all of the clients data within this struct
 */
struct SClient
{
	std::string Name;
};

/**
 * A manager class for a chat server console application
 */
class CServer
{
public:
	/** Default constructor */
	CServer();

	/** Default destructor */
	~CServer();

	/** Initialize the chat server console application */
	void Init();

	/** Run the chat server console application loop, every CPU tick */
	void Run();

private:
	/** Initialize useful debugging tools, prints error messages to the console and to a log file */
	void LogInit();

	// WinSock API Wrapper functions

	/** Binds the server socket to an address */
	void Bind();

	/** Sets the socket to listen for incoming client connections */
	void Listen();

	/** Accept an incoming client connection */
	SOCKET Accept();

	/** Sends data to a connected socket */
	void Send(SOCKET InSocket, const char* Buffer);

	/** Receives data from a connection */
	void Receive(SOCKET InSocket, const char* Buffer);

	// Chat console application protocol functions

	/** Enables local user input, polls every CPU tick */
	void GetUserInput();

	/** Get messages sent to the server */
	void GetMessages(SOCKET InSocket);

	/** Sends messages to all clients connected on this server */
	void SendMessageToAllClients(const char* InBuffer);

	/** Send a message to a specific client connected to this server */
	void SendMessageToClient(SOCKET InSocket, const char* InBuffer);

	/** Add a client to the map when they connect to his server */
	void AddClient(SOCKET& InSocket, const char* Username);

	/** Set the client username */
	void SetUsername(SOCKET Socket, const char* Username);

	/** Called when a client connects to this server */
	void OnConnected(SOCKET Socket);

private:
	// Winsock API Wrapper variables

	/** WinSock API data */
	WSADATA Data;

	/** The server socket */
	SOCKET ServerSocket;

	/** The server address */
	SOCKADDR_IN ServerAddress;

	/** The client address */
	SOCKADDR_IN ClientAddress;

	/** The servers port number */
	uint16_t Port;

	// Chat console application protocol variables

	/** Map of client connections */
	std::map<SOCKET, SClient> ClientMap;

	/** True if server loop can run */
	bool bCanRun;
};