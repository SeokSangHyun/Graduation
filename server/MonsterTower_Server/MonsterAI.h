#pragma once
#include <chrono>

using namespace std;
using namespace chrono;


/*
	처리 해야할 상태
	=> 선공화
		1. 추적 범위 (시야 보다 작은) : 이동
			-->ProcessPack 내에서 검사

		2. 추적 범위 & 공격 범위 내 : 공격
			-->공격 viewlist를 만들것 : ProcessPack 내에서
			-->timer에서 이전상태 Attack 지금상태 Attack이면 2초마다 공격 모션이 갱신되게 함.

	=> 넉백
		3. 공격을 받을 경우 : 피격
			--> 공격을 받으면 피격 모션을 하고 3초동안 뒤로 이동
				피격시 WakeUP 하고 ProcessPack에서 일정시간 동안 뒤로 밀리도록함.

		CurrentState	지금상태
		PrevState		이전 상태
						추적범위
						공격범위
						공격 viewlist
*/
static high_resolution_clock::time_point FSM_st = high_resolution_clock::now();
static high_resolution_clock::time_point g_tElapsed;

class cObject;

/*==================================================*/
class BaseState {
public:
	virtual void Enter(cObject*) = 0;
	virtual void Excute(cObject*object) = 0;
	virtual void Exit(cObject*) = 0;
	virtual void Initialize(cObject* target) = 0;
};

/*==================================================*/
class Ideal : public BaseState {
private:
	unsigned int ideal_time;
	cObject * obj_target;

public:
	static Ideal* Instance();

	virtual void Enter(cObject*);
	virtual void Excute(cObject*object);
	virtual void Exit(cObject*);
	virtual void Initialize(cObject* target);
public:
	void Add_idealTime();
};


/*==================================================*/
class Attack : public BaseState {
	float attack_time = 0.f;
	cObject * obj_target;
public:

	static Attack* Instance();

	virtual void Enter(cObject*);
	virtual void Excute(cObject*object);
	virtual void Exit(cObject*);
	virtual void Initialize(cObject* target);
};

/*==================================================*/
class BeAttack : public BaseState {
	
	float hit_time = 0.f;

	float m_fStiffTime = 0.f;
	bool m_bStiff = false;
public:

	static BeAttack* Instance();

	virtual void Enter(cObject*);
	virtual void Excute(cObject*object);
	virtual void Exit(cObject*);
	virtual void Initialize(cObject* target) {}

	void StiffTime();
};


/*==================================================*/
class Move : public BaseState {

	cObject* obj_target;
	float move_time = 0.f;
public:
	static Move* Instance();

	virtual void Enter(cObject*);
	virtual void Excute(cObject*object);
	virtual void Exit(cObject*);

public:
	void Initialize(cObject*);
};

/*==================================================*/
class Rush : public BaseState {
	cObject* obj_target;
	float rush_time = 0.f;

	float my_x = 0, my_z = 0;
	float target_x = 0, target_z = 0;
public:

	static Rush* Instance();

	virtual void Enter(cObject*);
	virtual void Excute(cObject*object);
	virtual void Exit(cObject*);
	virtual void Initialize(cObject* target);
};


/*==================================================*/
class Wander : public BaseState {

	float mid_x, mid_z;
	float cur_x, cur_z;
	float radian = 0.f;

	float t_wander = 0.f;

public:
	static Wander* Instance();

	virtual void Enter(cObject*);
	virtual void Excute(cObject*object);
	virtual void Exit(cObject*);

	virtual void Initialize(cObject*);
	void Round();
};

/*===========================================================*/
/*===========================================================*/
static double GetElapsedTime()
{
	g_tElapsed = high_resolution_clock::now();

	return (double)((duration_cast<milliseconds>(g_tElapsed - FSM_st)).count()*0.001f);
}

static void Add_time(float *time) {
	(*time) = (GetElapsedTime() + (*time));
}


