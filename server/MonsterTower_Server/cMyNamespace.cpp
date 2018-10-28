#include "stdafx.h"
#include "cMyNamespace.h"

volatile bool thread_running = false;
volatile HANDLE g_iocp = NULL;

map<UINT, cObject*> objects;
cScene g_scene;

mutex timerMutex;
mutex timerQueueMutex;
priority_queue<NPC_EVENT, vector<NPC_EVENT>, Comparison> event_queue;

CManager *g_manager;
