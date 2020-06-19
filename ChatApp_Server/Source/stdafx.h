#pragma once

#ifdef _WIN64
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#pragma comment (lib, "Ws2_32.lib")

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <thread>

#include "LocalUserInput.h"