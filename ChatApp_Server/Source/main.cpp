#include "stdafx.h"
#include "Server.h"

int main()
{
	std::unique_ptr<CServer> Server = std::make_unique<CServer>();
	Server->Init();
	Server->Run();

	return 0;
}