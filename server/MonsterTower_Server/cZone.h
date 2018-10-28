#pragma once

#include "cSector.h"

const int SECTOR_WIDTH = 5100;
const int SECTOR_HEIGHT = 5100;

const int SECTOR_WIDTH_NUM = BOARD_WIDTH / SECTOR_WIDTH;
const int SECTOR_HEIGHT_NUM = BOARD_HEIGHT / SECTOR_HEIGHT;

class cZone
{
	cSector sectorlist[SECTOR_WIDTH_NUM][SECTOR_HEIGHT_NUM];
public:
	cZone();
	~cZone();
public:
	void UpdateSector(const UINT id);
	cSector* GetSector(const float x, const float y);
};