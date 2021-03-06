// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#define WIN32_LEAN_AND_MEAN  
#define INITGUID

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <windows.h>  
#include <windowsx.h>
#include <conio.h>
#include <iostream>

#include <vector>
#include <map>
#include <queue>
#include <unordered_set>

#include <thread>
#include <mutex>

#include <chrono>
#include <stdlib.h>

extern "C" {
#include "LuaScript\lauxlib.h"
#include "LuaScript\lua.h"
#include "LuaScript\lualib.h"
}
#pragma comment(lib, "lua53.lib")

#include "Protocol.h"
#include "MyHeader.h"
using namespace std;

using namespace chrono;