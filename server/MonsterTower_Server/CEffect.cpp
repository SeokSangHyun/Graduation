#include "stdafx.h"
#include "cEffect.h"
#include "cMyNamespace.h"
#include "cThreadFunc.h"
#include "cPlayer.h"

cEffect::cEffect()
{
	my_sector = nullptr;
	is_Active = false;
	target_id = -1;

	m_pPrevState = Ideal::Instance();
	m_pCurrentState = Ideal::Instance();
	m_pCurrentState->Enter(this);

	hp = 0.f;
}

cEffect::~cEffect()
{

}

