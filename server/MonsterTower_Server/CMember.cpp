#include "stdafx.h"
#include "CMember.h"



CMember::CMember()
{
	m_IsLogin = false;

	m_lua = luaL_newstate();
	luaL_openlibs(m_lua);

	//필요한 함수들 추가
	if (luaL_loadfile(m_lua, "./test.lua") || lua_pcall(m_lua, 0, 0, 0)) {
		printf("%s\n", lua_tostring(m_lua, -1));
		lua_pop(m_lua, 1);
	}
}

CMember::~CMember()
{
	lua_close(m_lua);
}


bool CMember::IsClass() const
{
	lua_getglobal(m_lua, "my_info");
	int top = lua_gettop(m_lua);
	lua_gettable(m_lua, top);

	bool tf = lua_istable(m_lua, top);
	lua_pop(m_lua, 1);

	return tf;
}



const char * CMember::GetId() const
{
	lua_getglobal(m_lua, "my_id");
	const char *id = lua_tostring(m_lua, -1);
	lua_pop(m_lua, 1);
	return id;
}

const char* CMember::GetPassword() const
{
	lua_getglobal(m_lua, "my_password");
	const char *password = lua_tostring(m_lua, -1);
	lua_pop(m_lua, 1);
	return password;
}

const char* CMember::GetScene() const
{
	lua_getglobal(m_lua, "my_scene");
	const char *scene = lua_tostring(m_lua, -1);
	lua_pop(m_lua, 1);
	return scene;
}


void CMember::SetInfo(const char * id, const char * password, const char * job)
{
	m_IsLogin = true;
	lua_pushstring(m_lua, id);
	lua_pushstring(m_lua, password);
	lua_pushstring(m_lua, job);

	lua_setglobal(m_lua, "my_scene");
	lua_setglobal(m_lua, "my_password");
	lua_setglobal(m_lua, "my_id");

}

void CMember::SetLevelExp(int level, float exp)
{
	lua_getglobal(m_lua, "my_info");
	int top = lua_gettop(m_lua);

	lua_pushstring(m_lua, "level");
	lua_pushnumber(m_lua, level);
	lua_settable(m_lua, top);

	lua_pushstring(m_lua, "exp");
	lua_pushnumber(m_lua, exp);
	lua_settable(m_lua, top);

	lua_pop(m_lua, 1);
}

void CMember::SetNewChamp(const char* job, const int lv, const float exp, const int hp, const int mp, const int dam, const int def)
{
	lua_getglobal(m_lua, "my_info");
	int top = lua_gettop(m_lua);
	lua_newtable(m_lua);

		lua_pushstring(m_lua, "job");
		lua_pushstring(m_lua, job);
		lua_settable(m_lua, top);

		lua_pushstring(m_lua, "level");
		lua_pushnumber(m_lua, lv);
		lua_settable(m_lua, top);

		lua_pushstring(m_lua, "exp");
		lua_pushnumber(m_lua, exp);
		lua_settable(m_lua, top);

		lua_pushstring(m_lua, "hp");
		lua_pushnumber(m_lua, hp);
		lua_settable(m_lua, top);

		lua_pushstring(m_lua, "mp");
		lua_pushnumber(m_lua, mp);
		lua_settable(m_lua, top);

		lua_pushstring(m_lua, "damage");
		lua_pushnumber(m_lua, dam);
		lua_settable(m_lua, top);

		lua_pushstring(m_lua, "def");
		lua_pushnumber(m_lua, def);
		lua_settable(m_lua, top);
}

void CMember::PersonalShow()
{
	lua_getglobal(m_lua, "showAll");
	lua_pcall(m_lua, 0, 0, 0);
}





