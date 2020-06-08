#pragma once

class CSocketAddress
{
public:
	CSocketAddress(const char* InAddress, uint16_t InPort);

	inline size_t GetSize() const
	{
		return sizeof(SOCKADDR_IN);
	}

private:
	SOCKADDR_IN SocketAddress;
};