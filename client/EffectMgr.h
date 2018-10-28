#pragma once
#include "stdafx.h"
#include "Object.h"

template<typename T>
class CEffect {
	T **particle = NULL;
	bool turn_act = false; // 이름을 고치자
	int count = 0;

public:
	CEffect();
	CEffect(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		ID3D12RootSignature *pd3dGraphicsRootSignature, const int num);
	~CEffect();
	void ReleaseUploadBuffer();

	//
	void Animate(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList * pd3dCommandList);

public:
	void OnAct() { turn_act = true; }
	void OffAct() { turn_act = false; }
	bool GetAct();

	//damaged 전용
	void ResetPos(float x, float y, float z);
	void Setdir(float dir_x, float dir_y, float dir_z);
};



class CEffectMgr
{
	//날씨
	CEffect<CRainobj> *m_ptRain;
	CEffect<CSnowobj> *m_ptSnow;

	CEffect<CDamagedParticle> *m_damaged;

	CEffect<CDamagedParticle> *m_damage;
	CEffect<CDamagedParticle> *m_damageOther;

public:
	CEffectMgr();
	CEffectMgr(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		ID3D12RootSignature *pd3dGraphicsRootSignature);
	~CEffectMgr();
	void ReleaseUploadBuffer();

	void Animate(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList * pd3dCommandList);

	void SceneOneSet();
	void SceneTwoSet();
	void SceneThreeSet();

	void PlayDamagedParticle(XMFLOAT3 t_pos);
	void PlayDamageParticle(XMFLOAT3 t_pos);
	void PlayDamageOtherParticle(XMFLOAT3 t_pos);
};

