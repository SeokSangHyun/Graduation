
#include "Shadow.h"



CShadow::CShadow(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext, int nMeshes) : CGameObject(nMeshes)
{
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;



	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
}

CShadow::~CShadow()
{
	ReleaseShaderVariables();
}

ID3D12Resource *CShadow::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_PLAYER_INFO) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dcbPlayer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbPlayer->Map(0, NULL, (void **)&m_pcbMappedPlayer);

	return(m_pd3dcbPlayer);
}

void CShadow::ReleaseShaderVariables()
{
	if (m_pd3dcbPlayer)
	{
		m_pd3dcbPlayer->Unmap(0, NULL);
		m_pd3dcbPlayer->Release();
	}

	CGameObject::ReleaseShaderVariables();
}

void CShadow::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedPlayer->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbPlayer->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_PLAYER, d3dGpuVirtualAddress);
}


//=================================================================================//
//=================================================================================//


void CShadow::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
	}
}
//
//void CShadow::Rotate(float x, float y, float z)
//{
//	DWORD nCurrentCameraMode = m_pCamera->GetMode();
//	if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
//	{
//		if (x != 0.0f)
//		{
//			m_fPitch += x;
//			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
//			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
//		}
//		if (y != 0.0f)
//		{
//			m_fYaw += y;
//			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
//			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
//		}
//		if (z != 0.0f)
//		{
//			m_fRoll += z;
//			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
//			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
//		}
//		m_pCamera->Rotate(x, y, z);
//		if (y != 0.0f)
//		{
//			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
//			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
//			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
//		}
//	}
//	else if (nCurrentCameraMode == SPACESHIP_CAMERA)
//	{
//		m_pCamera->Rotate(x, y, z);
//		XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z));
//		m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
//		if (x != 0.0f)
//		{
//			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));
//			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
//			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
//		}
//		if (y != 0.0f)
//		{
//			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
//			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
//			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
//		}
//		if (z != 0.0f)
//		{
//			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z));
//			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
//			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
//		}
//	}
//
//	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
//	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
//	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
//}

void CShadow::Rotate1(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
}


void CShadow::Update(float fTimeElapsed)
{
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;
	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	Move(m_xmf3Velocity, false);

	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);

	//
	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

void CShadow::Animate(float fTimeElapsed)
{
	m_xmf4x4ToParentTransform._11 = m_xmf3Right.x; m_xmf4x4ToParentTransform._12 = m_xmf3Right.y; m_xmf4x4ToParentTransform._13 = m_xmf3Right.z;
	m_xmf4x4ToParentTransform._21 = m_xmf3Up.x; m_xmf4x4ToParentTransform._22 = m_xmf3Up.y; m_xmf4x4ToParentTransform._23 = m_xmf3Up.z;
	m_xmf4x4ToParentTransform._31 = m_xmf3Look.x; m_xmf4x4ToParentTransform._32 = m_xmf3Look.y; m_xmf4x4ToParentTransform._33 = m_xmf3Look.z;
	m_xmf4x4ToParentTransform._41 = m_xmf3Position.x; m_xmf4x4ToParentTransform._42 = m_xmf3Position.y; m_xmf4x4ToParentTransform._43 = m_xmf3Position.z+100;

	//XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f);
	//m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
}

void CShadow::OnPrepareRender()
{
}

void CShadow::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	if (nCameraMode == THIRD_PERSON_CAMERA) CGameObject::Render(pd3dCommandList, pCamera);
}


