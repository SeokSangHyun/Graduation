#include "stdafx.h"
#include "cSector.h"

cSector::cSector()
{
}

cSector::~cSector()
{
}

void cSector::Insert(UINT id)
{
	sector_mutex.lock();
	if (0 != insector_object.count(id))
	{
		sector_mutex.unlock();
		return;
	}
	insector_object.insert(id);
	sector_mutex.unlock();
}

void cSector::Remove(UINT id)
{
	sector_mutex.lock();
	if (0 == insector_object.count(id))
	{
		sector_mutex.unlock();
		return;
	}
	insector_object.erase(id);
	sector_mutex.unlock();
}
