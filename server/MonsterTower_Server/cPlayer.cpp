#include "stdafx.h"
#include "cPlayer.h"
#include "cObject.h"
#include "cMonster.h"

#include "cThreadFunc.h"
#include "cMyNameSpace.h"

cPlayer::cPlayer() : packet_size(0), saved_size(0)
{
	ZeroMemory(recv_buf, sizeof(recv_buf));
	ZeroMemory(&exover.wsaover, sizeof(exover.wsaover));

	viewMutex.lock();
	viewlist.clear();
	viewMutex.unlock();
	exover.wsabuf.buf = reinterpret_cast<char*>(recv_buf);
	exover.wsabuf.len = sizeof(recv_buf);
	my_sector = nullptr;
	hp = 100.f;
}

cPlayer::~cPlayer()
{
}

void cPlayer::RecvPacket()
{
	DWORD flags = 0;

	int ret_val = WSARecv(sock, &exover.wsabuf, 1, NULL, &flags, &exover.wsaover, NULL);
	if (ret_val != 0)
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
			cout << " [error] WSARecv() on " << id << " Player::RecvPacket() " << endl;
	}
}

void cPlayer::ReadPacket(const UINT id, const int transferred)
{
	BYTE* ptr = recv_buf;

	int remained = transferred;

	while (0 < remained)
	{
		if (0 == packet_size)
			packet_size = ptr[0];

		int required = packet_size - saved_size;

		if (remained >= required)
		{
			memcpy(packet_buf + saved_size, ptr, required);

			ProcessPacket(id, packet_buf);

			remained -= required;
			ptr += required;

			packet_size = 0;
			saved_size = 0;
		}

		else
		{
			memcpy(packet_buf + saved_size, ptr, remained);

			saved_size += remained;

			remained = 0;
		}
	}
}

void cPlayer::UpdateViewList()
{
	//sector_search가 업데이트 된 리스트
	//viewlist는 오브젝트가 사용하는 viewlist

	unordered_set<UINT> add_list;
	unordered_set<UINT> remove_list;
	unordered_set<UINT> exist_list;

	//새로 더해진 오브젝트를 add_list에 추가
	for (auto add_player : near_list)
	{
		if (0 == viewlist.count(add_player))
		{
			add_list.insert(add_player);
		}
	}
	//없어진 플레이어를 remove_list에 추가
	for (auto remove_player : viewlist)
	{
		if (0 == near_list.count(remove_player))
		{
			remove_list.insert(remove_player);
		}
	}
	//원래 있었고, 지금도 있는 플레이어들을 exist_list에 추가
	for (auto exist_player : near_list)
	{
		if (viewlist.count(exist_player))
		{
			exist_list.insert(exist_player);
		}
	}

	//1. 새로 들어온 플레이어 처리
	for (auto add : add_list)
	{
		SendPutObject(id, add);

		//viewlist에 없던 플레이어를 넣어준다.
		viewMutex.lock();
		viewlist.insert(add);
		viewMutex.unlock();
		
		//상대 뷰리스트에 없으면 넣어주고, 상대방이 플레이어라면 내 정보를 보내준다.
		if (add < MAX_USER)
		{
			cPlayer* other = dynamic_cast<cPlayer*>(objects[add]);

			other->viewMutex.lock();
			if (0 == other->GetViewlist().count(id))
			{
				other->GetViewlist().insert(id);
				other->viewMutex.unlock();
				SendPutObject(add, id);
			}
			//상대 뷰리스트에 있으면 내가 움직인 정보를 보내준다.
			else
			{
				other->viewMutex.unlock();
				SendMoveObject(add, id);
			}
		}
		else if (add >= MAX_USER && add < MAX_OBJECT_INDEX) //몬스터 이면 깨워준다.
		{
			cMonster* monster = dynamic_cast<cMonster*>(objects[add]);

			monster->WakeUpMonster(id);
		}
	}
	//2. 이미 존재하던 플레이어들 처리
	for (auto exist : exist_list)
	{
		SendMoveObject(id, exist);

		if (exist < MAX_USER)
		{
			cPlayer* other = dynamic_cast<cPlayer*>(objects[exist]);
			//상대 뷰리스트에 없으면 상대 뷰리스트에 나를 집어 넣어주고, 내가 접속한것을 알린다.
			other->viewMutex.lock();
			if (0 == other->GetViewlist().count(id))
			{
				other->GetViewlist().insert(id);
				other->viewMutex.unlock();
				SendPutObject(exist, id);
			}
			//상대 뷰리스트에 있으면, 내가 움직인 정보를 보내준다.
			else
			{
				other->viewMutex.unlock();
				SendMoveObject(exist, id);
			}
		} 
	}
	//3. 제거할 플레이어들 처리
	for (auto remove : remove_list)
	{
		//내 뷰리스트에서 제거시키고, 나에게 그 플레이어를 제거시킨다는 메세지를 보낸다.
		viewMutex.lock();
		viewlist.erase(remove);
		viewMutex.unlock();
		SendRemoveObject(id, remove);

		if (remove < MAX_USER)
		{
			cPlayer* other = dynamic_cast<cPlayer*>(objects[remove]);

			other->viewMutex.lock();
			if (0 != other->GetViewlist().count(id))
			{
				other->GetViewlist().erase(id);
				other->viewMutex.unlock();
				SendRemoveObject(remove, id);
			}
			else
			{
				other->viewMutex.unlock();
			}
		}
	}


	// ViewList = Near_list
	// 내 뷰리스트는 업데이트 되었다.
}

bool cPlayer::AttackMonster()
{
	for (int i = MAX_USER; i < MAX_OBJECT_INDEX; ++i) {
		if (objects[id]->GetScene() != objects[i]->GetScene()) continue;
		if (!CanAttack(id, i)) continue;
		NonTartgetStep(i);
		return true;
	}
	return false;
}

void cPlayer::NonTartgetStep(int i)
{
	objects[i]->ChangeState(BeAttack::Instance());
	objects[i]->Damaged(10);
}

void cPlayer::Clear()
{
	for (auto send : viewlist)
	{
		if (send < MAX_USER)
		{
			cPlayer* other = dynamic_cast<cPlayer*>(objects[send]);

			if (0 != other->GetViewlist().count(id))
			{
				other->viewMutex.lock();
				other->GetViewlist().erase(id);
				other->viewMutex.unlock();
				SendRemoveObject(send, id);
			}
		}
	}
	viewlist.clear();
	if (my_sector) {
		my_sector->Remove(id);
	}
}
