#include "stdafx.h"
#include "cThreadFunc.h"
#include "MonsterAI.h"
#include "cObject.h"
#include "cMonster.h"


/*==================================================*/
Ideal * Ideal::Instance()
{
	static Ideal instance;
	return &instance;
}

void Ideal::Enter(cObject* object)
{
	ideal_time = 0;
}


void Ideal::Excute(cObject* object)
{
	int temp = rand() % 3;
	if (object->CanSee(object->GetID(), obj_target->GetID())) {
		if(temp)
			object->ChangeState(Rush::Instance());
		else
			object->ChangeState(Move::Instance());
	}
	FSM_st = high_resolution_clock::now();
}


void Ideal::Exit(cObject* cObject)
{
}

void Ideal::Initialize(cObject * target)
{
	obj_target = target;
}


/*==================================================*/

//Attack
/*==================================================*/
Attack * Attack::Instance()
{
	static Attack instance;
	return &instance;
}

void Attack::Enter(cObject* object)
{
	//object->Initialize();
	attack_time = 0.f;
}


void Attack::Excute(cObject*object)
{
	Add_time(&attack_time);
	if (attack_time > 2) {
		//WORD temp = rand() % 2;
		WORD temp = 1;
		if (temp) {
			if (object->CanAttack(object->GetID(), obj_target->GetID())) {
				object->ChangeState(Attack::Instance());
				obj_target->Damaged(5);
				attack_time = 0.f;
			}
			else {
				object->ChangeState(Move::Instance());
				attack_time = 0.f;
			}
		}
		/*else {
			object->ChangeState(Wander::Instance());
		}*/
	}
	FSM_st = high_resolution_clock::now();
}


void Attack::Exit(cObject* cObject)
{
}

void Attack::Initialize(cObject * target)
{
	obj_target = target;
}

/*==================================================*/


//BeAttack
/*==================================================*/
BeAttack * BeAttack::Instance()
{
	static BeAttack instance;
	return &instance;
}

void BeAttack::Enter(cObject* object)
{
	hit_time = 0.f;
	m_bStiff = true;
}


void BeAttack::Excute(cObject*object)
{
	Add_time(&m_fStiffTime);
	if (m_fStiffTime >= 2)
		m_bStiff = false;

	if (m_bStiff == false) {
		Add_time(&hit_time);
		object->moveMutex.lock();
		object->Damaged(5);
		object->moveMutex.unlock();

		if (object->GetMyHP() <= 0) {
			object->SetX(0.f);
			object->SetZ(0.f);
		}
		object->ChangeState(Move::Instance());
		m_fStiffTime = 0.f;
	}
	FSM_st = high_resolution_clock::now();
}


void BeAttack::Exit(cObject* cObject)
{
}

void BeAttack::StiffTime()
{
	m_bStiff = true;
}

/*==================================================*/


//Move
/*==================================================*/
Move * Move::Instance()
{
	static Move instance;
	return &instance;
}

void Move::Enter(cObject* object)
{
	move_time = 0.f;
	//object->Initialize();
}


void Move::Excute(cObject*object)
{
	object->Move(MONSTER_SPEED*GetElapsedTime());
	Add_time(&move_time);

	if (object->CanAttack(object->GetID(), obj_target->GetID())) {
		object->ChangeState(Attack::Instance());
		move_time = 0.f;
	}
	FSM_st = high_resolution_clock::now();
}


void Move::Exit(cObject* object)
{
}

void Move::Initialize(cObject *object)
{
	obj_target = object;
}

/*==================================================*/

//Rush
/*==================================================*/
Rush * Rush::Instance()
{
	static Rush instance;
	return &instance;
}

void Rush::Enter(cObject* object)
{
	rush_time = 0.f;
	my_x = object->GetX();
	my_z = object->GetZ();
}

void Rush::Excute(cObject*object)
{
	object->Move(1.5*MONSTER_SPEED*GetElapsedTime());
	Add_time(&rush_time);

	//*temp_x += target_x;
	//*temp_z += target_x;
	//object->Move(temp_x, temp_z, MONSTER_SPEED * 10);
	if (object->CanAttack(object->GetID(), obj_target->GetID())){
		rush_time = 0.f;
		object->ChangeState(Attack::Instance());
	}
	FSM_st = high_resolution_clock::now();
}


void Rush::Exit(cObject* object)
{
}
void Rush::Initialize(cObject * target)
{
	obj_target = target;
	float temp_x = 0, temp_z = 0;

	temp_x = target->GetX()+my_x;
	temp_z = target->GetZ()+my_z;

	temp_x = (float)my_x / (float)sqrt(temp_x*temp_x);
	temp_z = (float)my_z / (float)sqrt(temp_z*temp_z);

	target_x = roundf((temp_x) * 100.f) * 0.01f;
	target_z = roundf((temp_z) * 100.f) * 0.01f;
}

/*==================================================*/


//Wander
/*==================================================*/
Wander * Wander::Instance()
{
	static Wander instance;
	return &instance;
}

void Wander::Enter(cObject* object)
{
	radian = 0.f;
	t_wander = 0.f;
	cur_x = object->GetX();
	cur_z = object->GetZ();
}


void Wander::Excute(cObject*object)
{
	Add_time(&t_wander);
	radian += (0.1f * GetElapsedTime());

	if (t_wander>1000) {
		t_wander = 0.f;
		WORD temp = rand() % 2;
		if (temp == 0) {
			object->ChangeState(Move::Instance());
		}
		else if (temp == 1) {
			object->ChangeState(Rush::Instance());
		}
	}
	FSM_st = high_resolution_clock::now();
}


void Wander::Exit(cObject* object)
{
}

void Wander::Initialize(cObject *player)
{
	mid_x = player->GetX();
	mid_z = player->GetZ();

	//radian = atan2(cur_x-mid_x, cur_z - mid_z);
	radian = atan2(cur_z - mid_z, cur_x - mid_x);
}
/*==================================================*/

