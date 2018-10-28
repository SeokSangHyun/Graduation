#include "EffectMgr.h"

template<typename T>
CEffect<T>::CEffect() {}

template<typename T>
CEffect<T>::CEffect(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, const int num)
{
	turn_act = true;
	count = num;
	particle = new T*[count];

	for (int i = 0; i < count; ++i) {
		particle[i] = new T(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}
}
template<typename T>
void CEffect<T>::ReleaseUploadBuffer()
{
	for (int i = 0; i < count; ++i)
		particle[i]->ReleaseUploadBuffers();
}

template<typename T>
CEffect<T>::~CEffect() {}

template<typename T>
void CEffect<T>::Animate(float fTimeElapsed)
{
	for (int i = 0; i < count; ++i)
		particle[i]->Animate(fTimeElapsed);
}

template<typename T>
void CEffect<T>::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	for (int i = 0; i < count; ++i) {
		particle[i]->UpdateShaderVariables(pd3dCommandList);
		particle[i]->Render(pd3dCommandList);
	}
	
}

template<typename T>
bool CEffect<T>::GetAct()
{
	return turn_act;
}

template<typename T>
void CEffect<T>::ResetPos(float x, float y, float z)
{
	for (int i = 0; i < count; ++i)
		particle[i]->SetPosition(x, y, z);
}

template<typename T>
void CEffect<T>::Setdir(float dir_x, float dir_y, float dir_z)
{
	for (int i = 0; i < count; ++i)
		particle[i]->SetForce(x, y, z);
}



/*==========================================================================================
//
===========================================================================================*/


CEffectMgr::CEffectMgr()
{
}

CEffectMgr::CEffectMgr(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	m_ptRain = new CEffect<CRainobj>(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, 100);
	m_ptRain->OffAct();
	m_ptSnow = new CEffect<CSnowobj>(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, 80);
	m_ptSnow->OnAct();

	m_damaged = new CEffect<CDamagedParticle>(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, 35);
	m_damaged->ResetPos(2500, -200, 2500);
	m_damaged->OffAct();

	m_damage = new CEffect<CDamagedParticle>(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, 35);
	m_damage->ResetPos(2500, -200, 2500);
	m_damage->OffAct();
	m_damageOther = new CEffect<CDamagedParticle>(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, 35);
	m_damageOther->ResetPos(2500, -200, 2500);
	m_damageOther->OffAct();
}

CEffectMgr::~CEffectMgr()
{
}

void CEffectMgr::ReleaseUploadBuffer()
{
	if (m_ptRain->GetAct())
		m_ptRain->ReleaseUploadBuffer();
	if (m_ptSnow->GetAct())
		m_ptSnow->ReleaseUploadBuffer();
	if (m_damaged->GetAct())
		m_damaged->ReleaseUploadBuffer();
}

//////////

void CEffectMgr::Animate(float fTimeElapsed)
{
	if(m_ptRain->GetAct())
		m_ptRain->Animate(fTimeElapsed);
	if (m_ptSnow->GetAct())
		m_ptSnow->Animate(fTimeElapsed);

	if (m_damaged->GetAct())
		m_damaged->Animate(fTimeElapsed);
	if (m_damage->GetAct())
		m_damage->Animate(fTimeElapsed);
	if (m_damageOther->GetAct())
		m_damageOther->Animate(fTimeElapsed);
}

void CEffectMgr::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	if (m_ptRain->GetAct())
		m_ptRain->Render(pd3dCommandList);
	if (m_ptSnow->GetAct())
		m_ptSnow->Render(pd3dCommandList);

	if (m_damaged->GetAct())
		m_damaged->Render(pd3dCommandList);
	if (m_damage->GetAct())
		m_damage->Render(pd3dCommandList);
	if (m_damageOther->GetAct())
		m_damageOther->Render(pd3dCommandList);
}

void CEffectMgr::SceneOneSet()
{
	m_ptRain->OffAct();
	m_ptSnow->OnAct();
	m_damaged->OffAct();
	m_damage->OffAct();
	m_damageOther->OffAct();
}

void CEffectMgr::SceneTwoSet()
{
	m_ptRain->OnAct();
	m_ptSnow->OffAct();
	m_damaged->OffAct();
	m_damage->OffAct();
	m_damageOther->OffAct();
}

void CEffectMgr::SceneThreeSet()
{
	m_ptRain->OffAct();
	m_ptSnow->OffAct();
	m_damaged->OffAct();
	m_damage->OffAct();
	m_damageOther->OffAct();
}


void CEffectMgr::PlayDamagedParticle(XMFLOAT3 t_pos)
{
	m_damaged->ResetPos(t_pos.x, t_pos.y+50, t_pos.z);
	m_damaged->OnAct();
}

void CEffectMgr::PlayDamageParticle(XMFLOAT3 t_pos)
{
	m_damage->ResetPos(t_pos.x, t_pos.y + 50, t_pos.z);
	m_damage->OnAct();
}
void CEffectMgr::PlayDamageOtherParticle(XMFLOAT3 t_pos)
{
	m_damageOther->ResetPos(t_pos.x, t_pos.y + 50, t_pos.z);
	m_damageOther->OnAct();
}

