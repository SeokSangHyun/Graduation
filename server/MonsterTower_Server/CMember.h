#pragma once
#include "stdafx.h"
#include "LuaScript_API.h"

class CMember
{
protected:
	bool m_IsLogin;

	lua_State *m_lua;
public:
	CMember();
	~CMember();
	
	lua_State* LUA() { return m_lua; }
	bool GetIsLogin() { return m_IsLogin; }
	bool IsClass() const;

	const char* GetId() const ;
	const char* GetPassword() const;
	const char* GetScene() const;

	//
	void SetLogin(bool b) { m_IsLogin = b; }
	void SetInfo(const char*id, const char* password, const char * job);
	void SetLevelExp(int level, float exp);
	void SetNewChamp(const char* job, const int lv, const float exp, const int hp, const int mp, const int dam, const int def);

	void PersonalShow();
};

