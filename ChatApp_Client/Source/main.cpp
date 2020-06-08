#include "stdafx.h"
#include "Client.h"

int main()
{
	std::unique_ptr<CClient> Client = std::make_unique<CClient>();
	Client->Init();
	Client->Run();

	return 0;
}