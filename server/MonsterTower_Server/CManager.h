#pragma once
#include "CMember.h"

class CManager
{
protected:
	unsigned int serialnum =0;
	lua_State *main_lua;

	CMember *m_member[MAX_USER];

public:
	CManager();
	~CManager();

	void FileLoad(const char* str_path);
	void FileSave();
	//
	bool IsUser(const char* id, int *num);
	void ShowAllMemberInfo();
};

