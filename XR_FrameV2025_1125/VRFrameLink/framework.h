#pragma once

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
#define _CRT_SECURE_NO_WARNINGS

#define no_init_all deprecated

#define UDP_BUFF_SIZE 65560

#pragma comment(lib, "Ws2_32.lib")

// Windows ヘッダー ファイル
#include <windows.h>
#include <WinSock2.h>
#include <ws2ipdef.h>
#include <WS2tcpip.h>
