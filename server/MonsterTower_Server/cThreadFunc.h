#pragma once

static high_resolution_clock::time_point g_MoveSt= high_resolution_clock::now();

void AcceptThread();

void WorkerThread();

void TimerThread();

void AfterRecv(const UINT id, const int transferred);

void ProcessPacket(UINT cl, BYTE * packet);

void ProcessEvent(NPC_EVENT& event_object);

void DisconnectPlayer(UINT cl);

void SendPacket(UINT cl, void * packet);

void Errordisplay(const char * msg, int err_no);

void SendSceneObject(int client, SCENE scene);

void SendMoveObject(int client, int object_id);

void SendMoveObject(int client);

void SendPutObject(int client, int object_id);

void SendRemoveObject(int client, int object_id);

void UpdataPlayerView(const UINT id);

void AddTimer(const UINT id, const UINT temp_id, const int type, const UINT time);

static UINT GetCurrTime();

static double GetElapsedTime(high_resolution_clock::time_point time)
{
	auto g_tElapsed = high_resolution_clock::now();

	return (double)((duration_cast<milliseconds>(g_tElapsed - time)).count()*0.001f);
}

bool CollisionCheck(float x1, float y1, float x2, float y2, float size1, float size2);

