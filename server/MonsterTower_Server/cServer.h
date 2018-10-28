#pragma once
#include "Protocol.h"

class cServer
{
private:
	UINT			cpuNumber;
	vector<thread*>	w_threads;
	thread			a_thread;
	thread			t_thread;
public:
	cServer();
	~cServer();
public:
	void Initialize();
	void Progress();
	void Release();
};