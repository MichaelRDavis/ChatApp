#pragma once

//// Non-blocking user input via threads
//std::mutex UserInput;
//std::thread* UserInputThread;
//std::queue<std::string> MessageQueue;
//bool bCanQuit = false;
//
//void GetLocalUserInput()
//{
//	UserInputThread = new std::thread([]()
//	{
//		while (bCanQuit)
//		{
//			char Buffer[1024];
//		}
//	});
//} 

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

	void Bind();

	void Listen();

	SOCKET Accept();

	void Select();

	void GetUserInput();

	void GetMessages();

	void SendMessage();

	void AddClient(SOCKET InSocket, const char* Username);

private:
	WSADATA Data;
	SOCKET ServerSocket;
	SOCKET ClientSocket;
	SOCKADDR_IN ServerAddress;
	SOCKADDR_IN ClientAddress;
	uint16_t Port;
	bool bCanRun;
	std::map<SOCKET, SClient> ClientMap;
};