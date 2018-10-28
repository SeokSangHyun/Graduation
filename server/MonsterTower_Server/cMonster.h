#pragma once
#include "cObject.h"

class cMonster : public cObject
{
	UINT target_id;

	volatile bool is_Active;

	float update_sec = 0.f;
public:
	cMonster();
	virtual ~cMonster();
public:
	void HearBeat(const UINT target_id);
	void WakeUpMonster(UINT temp_id);
	void EventSendProcess(int target, int AnimNum);
public:
};