#include "stdafx.h"
#include "cObject.h"
#include "cMyNamespace.h"
#include "cThreadFunc.h"

cObject::cObject() : is_use(false), id(-1), x((float)(BOARD_WIDTH/2)), z((float)(BOARD_WIDTH/2)), sight_x(0.0f), sight_z(1.0f)
{
	exover.command = E_RECV;
	scene = STAGE_ONE;
	x = 0; z = 0;
	state_change = false;
}
cObject::~cObject()
{
}

int cObject::GetAnimNum()
{
	int temp_num = 0;
	//몬스터 모델 애니메이션 넘버에 따른 반환
	if (m_pCurrentState == Ideal::Instance())
		return NPC_IDLE- temp_num;
	else if (m_pCurrentState == Move::Instance())
		return NPC_MOVE- temp_num;
	else if (m_pCurrentState == Attack::Instance())
		return NPC_ATTACK- temp_num;
	else if (m_pCurrentState == BeAttack::Instance())
		return NPC_DAMAGED- temp_num;
	else if (m_pCurrentState == Rush::Instance())
		return NPC_SKILL - temp_num;
	else if (m_pCurrentState == Wander::Instance())
		return 0;
	else
		return NPC_IDLE- temp_num;
}


void cObject::UpdatesSectorSearch()
{
	RECT rc;
	rc.left = x - VIEW_RADIUS;
	rc.right = x + VIEW_RADIUS;
	rc.top = z - VIEW_RADIUS;
	rc.bottom = z + VIEW_RADIUS;

	AdjustRect(rc);

	near_sector.clear();
	near_list.clear();
	
	InsertSector(near_sector, GetCurrZone()->GetSector(x, z));
	InsertSector(near_sector, GetCurrZone()->GetSector(rc.left, rc.top));
	InsertSector(near_sector, GetCurrZone()->GetSector(rc.right, rc.top));
	InsertSector(near_sector, GetCurrZone()->GetSector(rc.left, rc.bottom));
	InsertSector(near_sector, GetCurrZone()->GetSector(rc.right, rc.bottom));

	for (auto sector : near_sector)
	{
		sector->sector_mutex.lock();
		unordered_set<UINT> temp = sector->GetInPlayer();
		sector->sector_mutex.unlock();
	
		for (auto near_player : temp)
		{
			if (near_player == id)
				continue;
			if (CanSee(near_player, id) == true)
			{
				mtx.lock();
				near_list.insert(near_player);
				mtx.unlock();
			}
		}
	}

}
void cObject::AdjustRect(RECT& rc)
{
	if (rc.left < 0)
		rc.left = 0;
	if (rc.top < 0)
		rc.top = 0;
	if (rc.right >= BOARD_HEIGHT - 1)
		rc.right = BOARD_HEIGHT - 1;
	if (rc.bottom >= BOARD_HEIGHT - 1)
		rc.bottom = BOARD_HEIGHT - 1;
}
bool cObject::CanSee(const UINT i, const UINT j)
{
	int x = objects[i]->GetX() - objects[j]->GetX();
	int y = objects[i]->GetZ() - objects[j]->GetZ();

	int dist = x * x + y * y;

	return (dist <= (VIEW_RADIUS) * (VIEW_RADIUS));
}
bool cObject::CanAttack(const UINT i, const UINT j)
{
	int x = objects[i]->GetX() - objects[j]->GetX();
	int y = objects[i]->GetZ() - objects[j]->GetZ();

	int dist = x * x + y * y;

	return (dist <= (MONSTER_ATTACK_RADIUS) * (MONSTER_ATTACK_RADIUS));
}
void cObject::InsertSector(vector<cSector*>& near_sector, cSector* sector)
{
	auto iter = find(near_sector.begin(), near_sector.end(), sector);

	if (iter == near_sector.end())
		near_sector.push_back(sector);
}



/*============================*/
void cObject::ChangeState(BaseState *nowState)
{
	nowState->Exit(this);
	m_pPrevState = m_pCurrentState;
	m_pCurrentState = nowState;
	state_change = false;
	nowState->Enter(this);
}

void cObject::Move(float speed)
{
	if (x >= 0.0f && x < BOARD_WIDTH && z >= 0.0f && z < BOARD_HEIGHT)
	{
		x = x + sight_x * speed;
		z = z + sight_z * speed;
	}
}
