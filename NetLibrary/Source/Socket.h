#pragma once

enum class ESocketType
{
	Streaming,
	Datagram,
	Unknown
};

class CSocketAddress;

class CSocket
{
public:
	CSocket();

	static std::shared_ptr<CSocket> CreateSocket(CSocketAddress InAddress);

private:
	SOCKET Socket;
};