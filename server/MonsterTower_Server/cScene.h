#pragma once

#include "cZone.h"

class cScene
{
	cZone stageOne;
	cZone stageTwo;
	cZone stageThree;
public:
	cScene();
	~cScene();
public:
	void UpdateZone(const UINT id);
};