#include "stdafx.h"
#include "LuaScript_API.h"


//
int API_CharacterList(lua_State *new_state)
{
	int count_method = 0;

	int top = lua_gettop(new_state);
	std::cout << "==============================" << std::endl;
	lua_pushstring(new_state, "전사");
	lua_gettable(new_state, top);
	std::cout << "전사" << std::endl;

		lua_pushstring(new_state, "hp");
		lua_gettable(new_state, top + 1);
		int info = lua_tointeger(new_state, -1);
		std::cout << "hp : "<< info << std::endl;
		lua_pop(new_state, 1);
		
		lua_pushstring(new_state, "mp");
		lua_gettable(new_state, top + 1);
		info = lua_tointeger(new_state, -1);
		std::cout << "mp : " << info << std::endl;
		lua_pop(new_state, 1);

		lua_pushstring(new_state, "damage");
		lua_gettable(new_state, top + 1);
		info = lua_tointeger(new_state, -1);
		std::cout << "damage : " << info << std::endl;
		lua_pop(new_state, 1);

		lua_pushstring(new_state, "def");
		lua_gettable(new_state, top + 1);
		info = lua_tointeger(new_state, -1);
		std::cout << "def : " << info << std::endl;
		lua_pop(new_state, 1);

	lua_pop(new_state, 1);

	std::cout << "----------------------------------" << std::endl;
	lua_pushstring(new_state, "궁수"); count_method++;//먼저 키를 스택에 푸쉬
	lua_gettable(new_state, top); count_method;//1번째 테이블을
	std::cout << "궁수" << std::endl;

		lua_pushstring(new_state, "hp");
		lua_gettable(new_state, top + 1);
		info = lua_tointeger(new_state, -1);
		std::cout << "hp : " << info << std::endl;
		lua_pop(new_state, 1);

		lua_pushstring(new_state, "mp");
		lua_gettable(new_state, top + 1);
		info = lua_tointeger(new_state, -1);
		std::cout << "mp : " << info << std::endl;
		lua_pop(new_state, 1);

		lua_pushstring(new_state, "damage");
		lua_gettable(new_state, top + 1);
		info = lua_tointeger(new_state, -1);
		std::cout << "damage : " << info << std::endl;
		lua_pop(new_state, 1);

		lua_pushstring(new_state, "def");
		lua_gettable(new_state, top + 1);
		info = lua_tointeger(new_state, -1);
		std::cout << "def : " << info << std::endl;
		lua_pop(new_state, 1);

	lua_pop(new_state, 1);

	std::cout << "----------------------------------" << std::endl;
	lua_pushstring(new_state, "마법사"); count_method++;//먼저 키를 스택에 푸쉬
	lua_gettable(new_state, top); count_method;//1번째 테이블을
	std::cout << "마법사" << std::endl;

		lua_pushstring(new_state, "hp");
		lua_gettable(new_state, top + 1);
		info = lua_tointeger(new_state, -1);
		std::cout << "hp : " << info << std::endl;
		lua_pop(new_state, 1);

		lua_pushstring(new_state, "mp");
		lua_gettable(new_state, top + 1);
		info = lua_tointeger(new_state, -1);
		std::cout << "mp : " << info << std::endl;
		lua_pop(new_state, 1);

		lua_pushstring(new_state, "damage");
		lua_gettable(new_state, top + 1);
		info = lua_tointeger(new_state, -1);
		std::cout << "damage : " << info << std::endl;
		lua_pop(new_state, 1);

		lua_pushstring(new_state, "def");
		lua_gettable(new_state, top + 1);
		info = lua_tointeger(new_state, -1);
		std::cout << "def : " << info << std::endl;
		lua_pop(new_state, 1);

	lua_pop(new_state, 1);

	std::cout << "==============================" << std::endl;

	char job[MAX_STR_SIZE] = "\0";
	while (true)
	{
		std::cout << "====캐릭터 선택" << std::endl;
		std::cin >> job;

		if (strcmp(job, "전사") == 0 || strcmp(job, "궁수") == 0 || strcmp(job, "마법사") == 0)
			break;
		std::cout << "알맞은 직업을 선택하세요!!!!!!!" << std::endl;
	}

	lua_pushstring(new_state, job);
	return 1;
}