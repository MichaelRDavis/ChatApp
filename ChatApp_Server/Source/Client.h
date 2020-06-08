#pragma once

struct SClient
{
	SOCKET Socket;
	SOCKADDR_IN Address;
	uint64_t UniqueID;
	std::string Name;

	uint64_t GenerateID()
	{
		uint64_t ID{0};
		return ++ID;
	}
};