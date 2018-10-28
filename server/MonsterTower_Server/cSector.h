#pragma once


class cSector
{
	unordered_set<UINT> insector_object;
public:
	mutex sector_mutex;
	cSector();
	~cSector();
public:
	void Insert(UINT id);
	void Remove(UINT id);
public:
	unordered_set<UINT>& GetInPlayer() { return insector_object; }
};