#pragma once
#include "CManager.h"

class CLogIn : public CManager
{
public:
	static CLogIn* Instance();

	CLogIn();
	~CLogIn();

	void SignUp(const char* id, const char* password);
	int SignIn(const char* id, const char* password);

	void ChangePassword(const char* id, const char* password);
};

