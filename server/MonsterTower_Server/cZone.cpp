#include "stdafx.h"
#include "cZone.h"
#include "cObject.h"
#include "cSector.h"
#include "cMyNamespace.h"

cZone::cZone()
{
}

cZone::~cZone()
{
}

void cZone::UpdateSector(const UINT id)
{
	cObject* object = objects[id];
	
	cSector* curr_sector = object->GetCurrSector();
	cSector* new_sector = GetSector(object->GetX(), object->GetZ());

	if (curr_sector != new_sector)
	{
		if (curr_sector)
			curr_sector->Remove(id);
			new_sector->Insert(id);

		object->SetSector(new_sector);
	}
}

cSector* cZone::GetSector(const float x, const float y)
{
	int index_x = x / SECTOR_WIDTH;
	int index_y = y / SECTOR_HEIGHT;

	if (y / SECTOR_HEIGHT >= SECTOR_HEIGHT_NUM || x / SECTOR_WIDTH >= SECTOR_WIDTH_NUM || x < 0 || y < 0)
	{
		cout << " sector init error !! : " << x << ", " << y << endl;
		return nullptr;
	}


	return &sectorlist[index_x][index_y];
}
