#pragma once

#include "Overlapped_Ex.h"
#include "cSector.h"
#include "cScene.h"
#include "MonsterAI.h"

class cObject
{
protected:
	bool is_use;
	EXOver exover;
	
	UINT id;
	int hp = 100;

	
	float x, z;

	float sight_x, sight_z;

	cZone* my_zone;

	cSector* my_sector;

	unordered_set<UINT> near_list;

	vector<cSector*> near_sector;

	mutex sectorMutex;

	SCENE scene;

protected:
	bool state_change;			// 상태변화가 있었는지?
	BaseState *m_pCurrentState; // 현재 상태
	BaseState *m_pPrevState;	// 이전 상태

	unordered_set<UINT> near_attack_list;
	unordered_set<UINT> temp_obj;
	mutex mtx;

public:
	mutex moveMutex;
	mutex sceneMutex;
	int anim_num = 0;
public:
	cObject();
	virtual ~cObject();
public:
	const bool GetIsUse() const { return is_use; }
	const int GetID() const { return id; }
	const float GetX() const { return x; }
	const float GetZ() const { return z; }
	const float GetSightX() const { return sight_x; }
	const float GetSightZ() const { return sight_z; }
	EXOver& GetOverEx() { return exover; }
	cSector* GetCurrSector() { return my_sector; }
	cZone* GetCurrZone() { return my_zone; }
	const SCENE GetScene() { return scene; }
	int GetMyHP(void) { return hp; }
public:
	void SetIsUse(const bool used) { is_use = used; }
	void SetID(const int n) { id = n; }
	void SetX(const float n) { x = n; }
	void SetZ(const float n) { z = n; }
	void SetSightX(const float n) { sight_x = n; }
	void SetSightZ(const float n) { sight_z = n; }
	void SetSector(cSector* sector) { my_sector = sector; }
	void SetZone(cZone* zone) { my_zone = zone; }
	void SetScene(SCENE sc) {scene = sc;}

	void SetMyHP(int temp_hp) { hp = temp_hp; }
	void Damaged(int temp_hp) { hp -= temp_hp; if (hp <= 0) { hp = 0; } }
public:
	void UpdatesSectorSearch();
	bool CanSee(const UINT i, const UINT j);
	bool CanAttack(const UINT i, const UINT j);
	void AdjustRect(RECT& rc);
	void InsertSector(vector<cSector*>& near_sector, cSector* sector);

	// MonsterFSM
public:
	bool GetStateChange() { return state_change; }
	BaseState* GetCurState() { return m_pCurrentState; }
	BaseState* GetPrevState() { return m_pPrevState; }
	
	int GetAnimNum();
	void SetStateChange(bool exist) { state_change = exist; }

	//
	void ChangeState(BaseState *newState);
	void Move(float speed);
};
