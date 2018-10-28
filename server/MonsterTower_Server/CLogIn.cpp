#include "stdafx.h"
#include "CLogIn.h"

using namespace std;


CLogIn * CLogIn::Instance()
{
	static CLogIn instance;
	return &instance;
}

CLogIn::CLogIn() : CManager()
{
}


CLogIn::~CLogIn()
{
}
 


void CLogIn::SignUp(const char* id, const char* password)
{
	char temp_id[MAX_STR_SIZE];
	char temp_password[MAX_STR_SIZE];
	char temp_scene[MAX_STR_SIZE];

	ZeroMemory(temp_id, sizeof(char)*MAX_STR_SIZE);
	ZeroMemory(temp_password, sizeof(char)*MAX_STR_SIZE);
	ZeroMemory(temp_scene, sizeof(char)*MAX_STR_SIZE);

	strcpy_s(temp_id, sizeof(char)*MAX_STR_SIZE, id);
	strcpy_s(temp_password, sizeof(char)*MAX_STR_SIZE, password);
	strcpy_s(temp_scene, sizeof(char)*MAX_STR_SIZE, "1");

	int user_num = -1;
	bool exist = true;
	for (int i = 0; i < MAX_USER; ++i)
	{
		exist = m_member[i]->GetIsLogin();
		if (exist == false) {
			user_num = i;
			break;
		}
	}

	if (user_num == -1) {
		cout << "더 이상 가입불가!" << endl;
	}
	else {
		lua_getglobal(m_member[user_num]->LUA(), "SetMyInfo");
		lua_pushstring(m_member[user_num]->LUA(), temp_id);
		lua_pushstring(m_member[user_num]->LUA(), temp_password);
		lua_pushstring(m_member[user_num]->LUA(), temp_scene);
		lua_pcall(m_member[user_num]->LUA(), 3, 0, 0);
		m_member[user_num]->SetLogin(true);
	}
}

//캐릭터 선택창
int CLogIn::SignIn(const char* id, const char* password)
{
	bool exist = false;
	int serial_num = -1;

	exist = IsUser((const char*)id, &serial_num);
	if (!exist || serial_num == -1) {
		cout << "올바른 아이디가 없습니다." << endl;
		return serial_num;
	}
	cout <<serial_num<< "로그인 되셨습니다. 되셨습니다!!" << endl;
	
	return serial_num;
}




void CLogIn::ChangePassword(const char* id, const char* password)
{
	bool exist = false;
	int serial_num = -1;


	exist = IsUser(id, &serial_num);
	if (!exist || serial_num ==-1) {
		cout << "올바른 아이디가 없습니다." << endl;
		return;
	}

	lua_pushstring(m_member[serial_num]->LUA(), password);
	lua_setglobal(m_member[serial_num]->LUA(), "my_password");
}

