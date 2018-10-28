#pragma once

#define MY_SERVER_PORT  4000

#define MAX_BUFF_SIZE   4000
#define MAX_PACKET_SIZE  255

#define BOARD_WIDTH   5100
#define BOARD_HEIGHT  5100

#define MAX_USER 3
#define NPC_START  3
#define MAX_OBJECT_INDEX 6
#define MAX_EFFECT_INDEX 20

#define MAX_STR_SIZE  100

#define CS_UP		1
#define CS_DOWN		2
#define CS_LEFT		3
#define CS_RIGHT    4
#define CS_TELEPORT 5
#define CS_ATTACK	6
#define CS_SIGHT	7
#define CS_CHAT		8
#define CS_ANIM		9
#define CS_NEXTSTAGE 10
#define CS_THREESTAGE 11
#define CS_ONESTAGE 12
#define CS_LOGIN		20
#define CS_SIGNUP		21
#define CS_SIGNIN		22
#define CS_PASSWORD_CHANGE	23


#define SC_POS           1
#define SC_PUT_PLAYER    2
#define SC_REMOVE_PLAYER 3
#define SC_CHAT			4
#define SC_SCENE		5
#define SC_ANIM			6
#define SC_NEXTSTAGE	7
#define SC_CONDITION	8
#define SC_LOGIN	20

enum player_animaition
{
	player_idle, front_run, back_run, left_run, right_run
};


#pragma pack (push, 1)
struct cs_packet_move {
	BYTE size;
	BYTE type;
	char dir_x;
	char dir_z;
};
struct cs_packet_sight {
	BYTE size;
	BYTE type;
	char dir_x;
	char dir_z;
};
struct cs_packet_attack
{
	BYTE size;
	BYTE type;
};

struct cs_packet_chat {
	BYTE size;
	BYTE type;
	BYTE state;
	char message[MAX_STR_SIZE];
};

struct cs_packet_signup {
	BYTE size;
	BYTE type;
	char id[MAX_STR_SIZE];
	char password[MAX_STR_SIZE];
};

struct cs_packet_signin {
	BYTE size;
	BYTE type;
	char id[MAX_STR_SIZE];
	char password[MAX_STR_SIZE];
};

struct cs_packet_passwordchange {
	BYTE size;
	BYTE type;
	char id[MAX_STR_SIZE];
	char password[MAX_STR_SIZE];
};
struct cs_packet_login {
	BYTE size;
	BYTE type;
	bool success;
};


////////
////////
struct sc_packet_pos {
	BYTE size;
	BYTE type;
	WORD id;
	float x;
	float y;
	char sight_x;
	char sight_z;
};

struct sc_packet_put_player {
	BYTE size;
	BYTE type;
	WORD id;
	float x;
	float y;
	char sight_x;
	char sight_z;
	BYTE hp;
};

struct sc_packet_remove_player {
	BYTE size;
	BYTE type;
	WORD id;
};

struct sc_packet_scene_player
{
	BYTE size;
	BYTE type;
	WORD id;
	BYTE scene;
};

struct sc_packet_chat {
	BYTE size;
	BYTE type;
	WORD id;
	WCHAR message[MAX_STR_SIZE];
};


struct cs_packet_animation {
	BYTE size;
	BYTE type;
	WORD id;
	WORD target_id;
	char anim_num;
	WORD hp;
	float x;
	float z;
};

//======
//======
struct cs_packet_npc_anim {
	BYTE size;
	BYTE type;
	WORD id;
	char anim_num;
};

struct cs_packet_npc_condition {
	BYTE size;
	BYTE type;
	WORD id;
	WORD hp;
};

//
#pragma pack (pop)