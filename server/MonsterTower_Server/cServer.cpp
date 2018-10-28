#include "stdafx.h"
#include "cServer.h"

#include "cPlayer.h"
#include "cMonster.h"
#include "CEffect.h"

#include "cThreadFunc.h"
#include "cMyNamespace.h"


cServer::cServer()
{
	Initialize();
	g_manager = new CLogIn();
}

cServer::~cServer()
{
	Release();
}

void cServer::Initialize()
{
	srand(unsigned(time(NULL)));

	wcout.imbue(locale("korean"));
	wcout << L"서버 시작!\n ";
	thread_running = true;
	SYSTEM_INFO systeminfo;
	GetSystemInfo(&systeminfo);
	cpuNumber = (systeminfo.dwNumberOfProcessors /2 )* 1.5;

	cout << "CPU 갯수 : " << cpuNumber << endl;
	//CPU 갯수는 코어갯수의 1.5배

	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);


	for (int i = 0; i < MAX_OBJECT_INDEX ; ++i)
	{
		if (i < MAX_USER) {
			objects[i] = new cPlayer();
			objects[i]->SetX(512);
			objects[i]->SetZ(512);
		}
		else
		{
			objects[i] = new cMonster();
			objects[i]->SetID(i);
			objects[i]->SetX(2550);
			objects[i]->SetZ(1500);
			objects[i]->SetIsUse(true);
			if( i == 3)
				objects[i]->SetScene(STAGE_ONE);
			else if (i == 4)
				objects[i]->SetScene(STAGE_TWO);
			else  if (i == 5)
				objects[i]->SetScene(STAGE_THREE);
			g_scene.UpdateZone(i);
		}
	}
	for (int i = MAX_OBJECT_INDEX; i < MAX_EFFECT_INDEX; ++i)
	{
		objects[i] = new cEffect();
		objects[i]->SetID(i);
	}

	a_thread = thread{ AcceptThread };

	for (unsigned int i = 0; i < cpuNumber; ++i)
		w_threads.push_back(new thread{ WorkerThread });

	t_thread = thread{ TimerThread };
}

void cServer::Progress()
{
	cout << " 서버 종료 [y/Y] " << endl;

	int key;

	while (true)
	{
		if (_kbhit() != false)
		{
			key = _getch();

			if (key == 'y' || key == 'Y')
			{
				thread_running = false;
				break;
			}
		}
	}
	SYSTEM_INFO systeminfo;
	GetSystemInfo(&systeminfo);
	DWORD cpu_count = systeminfo.dwNumberOfProcessors;

	// worker_thread 종료
	for (int i = 0; i < cpuNumber; ++i)
		PostQueuedCompletionStatus(g_iocp, 0, 0, nullptr);

	// accept_thread 종료
	while (true)
	{
		SOCKET tmp_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

		SOCKADDR_IN server_addr;
		ZeroMemory(&server_addr, sizeof(SOCKADDR_IN));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(MY_SERVER_PORT);
		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

		int retval = WSAConnect(tmp_sock, reinterpret_cast<SOCKADDR*>(&server_addr), sizeof(server_addr), NULL, NULL, NULL, NULL);
		if (SOCKET_ERROR != retval)
			break;
	}
}

void cServer::Release()
{
	a_thread.join();

	for (auto worker : w_threads)
	{
		worker->join();
		delete worker;
	}
	w_threads.clear();

	t_thread.join();

	for (auto object : objects)
	{
		delete object.second;
		object.second = nullptr;
	}
	objects.clear();
}