#pragma once
#include "stdafx.h"
#include "cObject.h"

class cEffect : public cObject
{
	UINT target_id;

	volatile bool is_Active;

	int damaged = 20.f;
	float update_sec = 0.f;
	
	float lookvectorx = 0.f;
	float lookvectory = 0.f;
	float lookvectorz = 0.f;
	
	int effectType = 0;

public:
	cEffect();
	virtual ~cEffect();

};