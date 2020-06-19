#pragma once

class CLocalUserInput
{
public:
	static void Init()
	{
		UserInputThread = new std::thread( []()
		{
			while (true)
			{
				// Read input
				std::string Input;
				getline(std::cin, Input);

				UserInputMutex.lock();
				UserInputQueue.push(Input);
				UserInputMutex.unlock();
			}
		});
	}

	static std::mutex UserInputMutex;
	static std::thread* UserInputThread;
	static std::queue<std::string> UserInputQueue;
};