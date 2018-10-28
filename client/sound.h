#pragma once
#include <conio.h>
#include <iostream>
using namespace std;

#include "fmod.hpp"
#pragma   comment (lib, "fmodex64_vc.lib")
using namespace FMOD;



class SoundMgr {
public:
	System * pSystem;
	//   사운드의 개수만큼 필요
	Sound*         pSound[5];
	//   동시에 출력될 사운드 만큼 필요
	Channel*      pChannel[2];


	/////
	bool   bPlay = true;
	bool   IsBGPlaying = true;
	bool   IsEffectPlaying = false;
	bool   IsEffectPlaying2 = false;
	float   fVolume = 1;
public:

	void sound_bgm()
	{
		//사운드 시스템을 생성
		System_Create(&pSystem);
		//   사운드 시스템 기본 세팅
		pSystem->init(2, FMOD_INIT_NORMAL, NULL);

		//   음악파일 생성
		pSystem->createSound(
			"onyak__background_LostTheWar.mp3",   //   사운드파일경로
			FMOD_HARDWARE | FMOD_LOOP_NORMAL,   //   사운드파일옵션
			NULL,         //   확장데이터(쓰지않음)
			&pSound[0]);   //   저장할 사운드 포인터

		pSystem->createSound(
			"wav_walk.wav",
			FMOD_LOOP_OFF | FMOD_HARDWARE,
			NULL,
			&pSound[1]);

		pSystem->createSound(
			"wav_punch.wav",
			FMOD_LOOP_OFF | FMOD_HARDWARE,
			NULL,
			&pSound[2]);

		pSystem->createSound(
			"wav_damage.wav",
			FMOD_LOOP_OFF | FMOD_HARDWARE,
			NULL,
			&pSound[3]);




		pChannel[0]->isPlaying(&IsBGPlaying);
		pChannel[1]->isPlaying(&IsEffectPlaying);

		pSystem->playSound(FMOD_CHANNEL_REUSE, pSound[0], false, &pChannel[0]);
		pChannel[0]->setVolume(fVolume);
		pChannel[1]->setVolume(fVolume);


		//음악파일 해제
		//pSound[0]->release();
		//pSound[1]->release();

		//   시스템 해제
		//pSystem->release();
		////   시스템 닫기
		//pSystem->close();


		//////////////////////////////////////////////////////////////
	}



	void sound_title()
	{
		//사운드 시스템을 생성
		System_Create(&pSystem);
		//   사운드 시스템 기본 세팅
		pSystem->init(2, FMOD_INIT_NORMAL, NULL);

		//   음악파일 생성
		pSystem->createSound(
			"title.wav",   //   사운드파일경로
			FMOD_HARDWARE | FMOD_LOOP_NORMAL,   //   사운드파일옵션
			NULL,         //   확장데이터(쓰지않음)
			&pSound[0]);   //   저장할 사운드 포인터

		pSystem->createSound(
			"Sega.wav",
			FMOD_LOOP_OFF | FMOD_HARDWARE,
			NULL,
			&pSound[1]);

		bool   bPlay = true;
		bool   IsBGPlaying = true;
		bool   IsEffectPlaying = false;
		float   fVolume = 1;


		pChannel[0]->isPlaying(&IsBGPlaying);
		pChannel[1]->isPlaying(&IsEffectPlaying);


		pSystem->playSound(FMOD_CHANNEL_REUSE, pSound[0], false, &pChannel[0]);
		pChannel[1]->setVolume(fVolume);

		pSystem->playSound(FMOD_CHANNEL_REUSE, pSound[1], false, &pChannel[1]);


		//   음악파일 해제
		//pSound[0]->release();
		//pSound[1]->release();
		//
		////   시스템 해제
		//pSystem->release();
		////   시스템 닫기
		//pSystem->close();


		//////////////////////////////////////////////////////////////
	}


	void sound_complete()
	{
		//사운드 시스템을 생성
		System_Create(&pSystem);
		//   사운드 시스템 기본 세팅
		pSystem->init(2, FMOD_INIT_NORMAL, NULL);

		//   음악파일 생성
		pSystem->createSound(
			"title.wav",   //   사운드파일경로
			FMOD_HARDWARE | FMOD_LOOP_NORMAL,   //   사운드파일옵션
			NULL,         //   확장데이터(쓰지않음)
			&pSound[0]);   //   저장할 사운드 포인터

		pSystem->createSound(
			"Complete.wav",
			FMOD_LOOP_OFF | FMOD_HARDWARE,
			NULL,
			&pSound[1]);

		bool   bPlay = true;
		bool   IsBGPlaying = true;
		bool   IsEffectPlaying = false;
		float   fVolume = 1;


		pChannel[0]->isPlaying(&IsBGPlaying);
		pChannel[1]->isPlaying(&IsEffectPlaying);


		pSystem->playSound(FMOD_CHANNEL_REUSE, pSound[0], false, &pChannel[0]);
		pChannel[1]->setVolume(fVolume);

		pSystem->playSound(FMOD_CHANNEL_REUSE, pSound[1], false, &pChannel[1]);


		//   음악파일 해제
		//pSound[0]->release();
		//pSound[1]->release();
		//
		////   시스템 해제
		//pSystem->release();
		////   시스템 닫기
		//pSystem->close();


		//////////////////////////////////////////////////////////////
	}
};