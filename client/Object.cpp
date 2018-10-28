//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Object.h"
#include "Shader.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers)
{
	m_nTextureType = nTextureType;
	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_pRootArgumentInfos = new SRVROOTARGUMENTINFO[m_nTextures];
		m_ppd3dTextureUploadBuffers = new ID3D12Resource*[m_nTextures];
		m_ppd3dTextures = new ID3D12Resource*[m_nTextures];
	}

	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (m_ppd3dTextures)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextures[i]) m_ppd3dTextures[i]->Release();
	}

	if (m_pRootArgumentInfos)
	{
		delete[] m_pRootArgumentInfos;
	}

	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void CTexture::SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pRootArgumentInfos[nIndex].m_nRootParameterIndex = nRootParameterIndex;
	m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pd3dSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_nTextureType == RESOURCE_TEXTURE2D_ARRAY)
	{
		pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[0].m_nRootParameterIndex, m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
	}
	else
	{
		for (int i = 0; i < m_nTextures; i++)
		{
			pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[i].m_nRootParameterIndex, m_pRootArgumentInfos[i].m_d3dSrvGpuDescriptorHandle);
		}
	}
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[nIndex].m_nRootParameterIndex, m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle);
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextureUploadBuffers[i]) m_ppd3dTextureUploadBuffers[i]->Release();
		delete[] m_ppd3dTextureUploadBuffers;
		m_ppd3dTextureUploadBuffers = NULL;
	}
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, wchar_t *pszFileName, UINT nIndex)
{
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}
void CTexture::LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const char *pszFileName, UINT nIndex)
{
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromFile(pd3dDevice, pd3dCommandList, (wchar_t*)pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMaterial::CMaterial()
{
}

CMaterial::~CMaterial()
{
	if (m_pTexture) m_pTexture->Release();
	if (m_pShader) m_pShader->Release();
}

void CMaterial::SetTexture(CTexture *pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}

void CMaterial::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pShader) m_pShader->ReleaseShaderVariables();
	if (m_pTexture) m_pTexture->ReleaseShaderVariables();
}

void CMaterial::ReleaseUploadBuffers()
{
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject(int nMeshes)
{
	m_xmf4x4ToParentTransform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh*[m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;
	}
	hp = 100;
}

CGameObject::~CGameObject()
{
	ReleaseShaderVariables();

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}
	if (m_pMaterial) m_pMaterial->Release();

	if (m_pSibling) delete m_pSibling;
	if (m_pChild) delete m_pChild;
}

void CGameObject::ResizeMeshes(int nMeshes)
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
		m_ppMeshes = NULL;
	}

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh*[m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;
	}
}

void CGameObject::SetChild(CGameObject *pChild)
{
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
	if (pChild) pChild->m_pParent = this;
}

void CGameObject::SetMesh(int nIndex, CMesh *pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}

void CGameObject::SetShader(CShader *pShader)
{
	if (!m_pMaterial)
	{
		CMaterial *pMaterial = new CMaterial();
		SetMaterial(pMaterial);
	}
	if (m_pMaterial) m_pMaterial->SetShader(pShader);
}

void CGameObject::SetMaterial(CMaterial *pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}

ID3D12Resource *CGameObject::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObject->Map(0, NULL, (void **)&m_pcbMappedGameObject);

	return(m_pd3dcbGameObject);
}

void CGameObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObject)
	{
		m_pd3dcbGameObject->Unmap(0, NULL);
		m_pd3dcbGameObject->Release();
	}
	if (m_pMaterial) m_pMaterial->ReleaseShaderVariables();
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	if (m_pMaterial) m_pcbMappedGameObject->m_nMaterial = m_pMaterial->m_nReflection;
}

void CGameObject::Animate(float fTimeElapsed)
{
	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);
}
void CGameObject::Animate(float fTimeElapsed , CCamera *pCCamera)
{
	if (m_pSibling) m_pSibling->Animate(fTimeElapsed , pCCamera);
	if (m_pChild) m_pChild->Animate(fTimeElapsed , pCCamera);
}

void CGameObject::SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_OBJECT, m_d3dCbvGPUDescriptorHandle);
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (!m_bActive) return;

	OnPrepareRender();

	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);

			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
		{
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
		}
	}

	if (m_ppMeshes && (m_nMeshes > 0))
	{
		SetRootParameter(pd3dCommandList);

		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) {

				int AnimationNum =m_ppMeshes[i]->GetFBXAnimationNum();
				//if (AnimationNum < 0) AnimationNum = 0; 
				int FrameNum = m_ppMeshes[i]->GetFBXNowFrameNum();
				//if (FrameNum < 0) FrameNum = 0;

				m_ppMeshes[i]->UpdateBoneTransform(pd3dCommandList, AnimationNum, FrameNum);
				
				m_ppMeshes[i]->Render(pd3dCommandList);
				
			}
		}
	}

	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
}



void CGameObject::ReleaseUploadBuffers()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}

	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();

	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();
}

void CGameObject::UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParentTransform, *pxmf4x4Parent) : m_xmf4x4ToParentTransform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

CGameObject *CGameObject::FindFrame(_TCHAR *pstrFrameName)
{
	CGameObject *pFrameObject = NULL;
	if (!_tcsncmp(m_pstrFrameName, pstrFrameName, _tcslen(pstrFrameName))) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4ToParentTransform._41 = x;
	m_xmf4x4ToParentTransform._42 = y;
	m_xmf4x4ToParentTransform._43 = z;

	
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
	
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetLocalPosition(XMFLOAT3 xmf3Position)
{
	XMMATRIX mtxTranslation = XMMatrixTranslation(xmf3Position.x, xmf3Position.y, xmf3Position.z);
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(m_xmf4x4ToParentTransform, mtxTranslation);
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParentTransform);
}

void CGameObject::SetLocalScale(float x, float y, float z)
{
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
}

void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
}

void CGameObject::Rotate(XMFLOAT4 *pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
}

#define _WITH_DEBUG_FRAME_HIERARCHY

void CGameObject::LoadFrameHierarchyFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, wifstream& InFile, UINT nFrame)
{
	XMFLOAT3 *pxmf3Positions = NULL, *pxmf3Normals = NULL;
	XMFLOAT2 *pxmf3TextureCoords0 = NULL, *pxmf3TextureCoords1 = NULL;
	UINT *pnIndices = NULL;

	TCHAR pstrMeshName[64] = { '\0' };
	TCHAR pstrAlbedoTextureName[64] = { '\0' };
	TCHAR pstrToken[64] = { '\0' };
	TCHAR pstrDebug[128] = { '\0' };

	XMFLOAT3 xmf3FrameLocalPosition, xmf3FrameLocalRotation, xmf3FrameLocalScale, xmf3FrameScale;
	XMFLOAT4 xmf4FrameLocalQuaternion, xmf4MaterialAlbedo;
	int nVertices = 0, nNormals = 0, nTextureCoords = 0, nIndices = 0;

	for ( ; ; )
	{
		InFile >> pstrToken;
		if (!InFile) break;

		if (!_tcscmp(pstrToken, _T("FrameName:")))
		{
			InFile >> m_pstrFrameName;

			nVertices = nNormals = nTextureCoords = nIndices = 0;
			xmf4MaterialAlbedo = XMFLOAT4(-1.0f, -1.0f, -1.0f, -1.0f);
			pxmf3Positions = pxmf3Normals = NULL;
			pxmf3TextureCoords0 = pxmf3TextureCoords1 = NULL;
			pstrAlbedoTextureName[0] = '\0';
			pnIndices = NULL;
		}
		else if (!_tcscmp(pstrToken, _T("Transform:")))
		{
			InFile >> xmf3FrameLocalPosition.x >> xmf3FrameLocalPosition.y >> xmf3FrameLocalPosition.z;
			InFile >> xmf3FrameLocalRotation.x >> xmf3FrameLocalRotation.y >> xmf3FrameLocalRotation.z;
			InFile >> xmf4FrameLocalQuaternion.x >> xmf4FrameLocalQuaternion.y >> xmf4FrameLocalQuaternion.z >> xmf4FrameLocalQuaternion.w;
			InFile >> xmf3FrameLocalScale.x >> xmf3FrameLocalScale.y >> xmf3FrameLocalScale.z;
			InFile >> xmf3FrameScale.x >> xmf3FrameScale.y >> xmf3FrameScale.z;
		}
		else if (!_tcscmp(pstrToken, _T("TransformMatrix:")))
		{
			InFile >> m_xmf4x4ToRootTransform._11 >> m_xmf4x4ToRootTransform._12 >> m_xmf4x4ToRootTransform._13 >> m_xmf4x4ToRootTransform._14;
			InFile >> m_xmf4x4ToRootTransform._21 >> m_xmf4x4ToRootTransform._22 >> m_xmf4x4ToRootTransform._23 >> m_xmf4x4ToRootTransform._24;
			InFile >> m_xmf4x4ToRootTransform._31 >> m_xmf4x4ToRootTransform._32 >> m_xmf4x4ToRootTransform._33 >> m_xmf4x4ToRootTransform._34;
			InFile >> m_xmf4x4ToRootTransform._41 >> m_xmf4x4ToRootTransform._42 >> m_xmf4x4ToRootTransform._43 >> m_xmf4x4ToRootTransform._44;

			InFile >> m_xmf4x4ToParentTransform._11 >> m_xmf4x4ToParentTransform._12 >> m_xmf4x4ToParentTransform._13 >> m_xmf4x4ToParentTransform._14;
			InFile >> m_xmf4x4ToParentTransform._21 >> m_xmf4x4ToParentTransform._22 >> m_xmf4x4ToParentTransform._23 >> m_xmf4x4ToParentTransform._24;
			InFile >> m_xmf4x4ToParentTransform._31 >> m_xmf4x4ToParentTransform._32 >> m_xmf4x4ToParentTransform._33 >> m_xmf4x4ToParentTransform._34;
			InFile >> m_xmf4x4ToParentTransform._41 >> m_xmf4x4ToParentTransform._42 >> m_xmf4x4ToParentTransform._43 >> m_xmf4x4ToParentTransform._44;
		}
		else if (!_tcscmp(pstrToken, _T("MeshName:")))
		{
			InFile >> pstrMeshName;
		}
		else if (!_tcscmp(pstrToken, _T("Vertices:")))
		{
			InFile >> nVertices;
			//추가 실험 코드 0224
			nVertices = nVertices - 60;
			pxmf3Positions = new XMFLOAT3[nVertices];
			for (int i = 0; i < nVertices; i++)
			{
				InFile >> pxmf3Positions[i].x >> pxmf3Positions[i].y >> pxmf3Positions[i].z;
			}
		}
		else if (!_tcscmp(pstrToken, _T("Normals:")))
		{
			InFile >> nNormals; 
			pxmf3Normals = new XMFLOAT3[nNormals];
			for (int i = 0; i < nNormals; i++)
			{
				InFile >> pxmf3Normals[i].x >> pxmf3Normals[i].y >> pxmf3Normals[i].z;
			}
		}
		else if (!_tcscmp(pstrToken, _T("TextureCoordinates0:")))
		{
			InFile >> nTextureCoords;
			pxmf3TextureCoords0 = new XMFLOAT2[nTextureCoords];
			for (int i = 0; i < nTextureCoords; i++)
			{
				InFile >> pxmf3TextureCoords0[i].x >> pxmf3TextureCoords0[i].y;
			}
		}
		else if (!_tcscmp(pstrToken, _T("TextureCoordinates1:")))
		{
			InFile >> nTextureCoords;
			pxmf3TextureCoords1 = new XMFLOAT2[nTextureCoords];
			for (int i = 0; i < nTextureCoords; i++)
			{
				InFile >> pxmf3TextureCoords1[i].x >> pxmf3TextureCoords1[i].y;
			}
		}
		else if (!_tcscmp(pstrToken, _T("Indices:")))
		{
			InFile >> nIndices;
			pnIndices = new UINT[nIndices];
			for (int i = 0; i < nIndices; i++)
			{
				InFile >> pnIndices[i];
			}
		}
		else if (!_tcscmp(pstrToken, _T("AlbedoColor:")))
		{
			/*InFile >> xmf4MaterialAlbedo.x >> xmf4MaterialAlbedo.y >> xmf4MaterialAlbedo.z >> xmf4MaterialAlbedo.w;*/
		}
		else if (!_tcscmp(pstrToken, _T("AlbedoTextureName:")))
		{
			InFile >> pstrAlbedoTextureName;
		}
		else if (!_tcscmp(pstrToken, _T("Children:")))
		{
			int nChilds = 0;
			InFile >> nChilds;
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					CGameObject *pChild = new CGameObject(1);
					pChild->LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, InFile, nFrame+1);
					SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
					_stprintf_s(pstrDebug, 128, _T("(Frame: %p) (Parent: %p)\n"), pChild, this);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!_tcscmp(pstrToken, _T("EndOfFrame")))
		{
			CMesh *pMesh = NULL;
			CMaterial *pMaterial = NULL;
			if ((nNormals > 0) && (nTextureCoords > 0) && (pstrAlbedoTextureName[0] != '\0'))
			{
				if (nVertices > 0) pMesh = new CMeshIlluminatedTextured(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, pxmf3Normals, pxmf3TextureCoords0, nIndices, pnIndices);

				TCHAR pstrPathName[128] = { '\0' };
				_tcscpy_s(pstrPathName, 128, _T("../Assets/Model/"));
				_tcscat_s(pstrPathName, 128, pstrAlbedoTextureName);
				_tcscat_s(pstrPathName, 128, _T(".dds"));

				CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
				pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pstrPathName, 0);

				pMaterial = new CMaterial();
				pMaterial->m_xmf4Albedo = xmf4MaterialAlbedo;

				pMaterial->SetTexture(pTexture);

				UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

				ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

				CIlluminatedTexturedShader *pShader = new CIlluminatedTexturedShader();
				pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
				pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
				pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
				pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
				pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);

				SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

				pMaterial->SetShader(pShader);
			}
			else if (nNormals > 0)
			{
				if (nVertices > 0) pMesh = new CMeshIlluminated(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, pxmf3Normals, nIndices, pnIndices);

				pMaterial = new CMaterial();
				pMaterial->m_xmf4Albedo = xmf4MaterialAlbedo;

				UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

				ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

				CIlluminatedShader *pShader = new CIlluminatedShader();
				pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
				pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
				pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 0);
				pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);

				SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

				pMaterial->SetShader(pShader);
			}
			else if (nTextureCoords > 0)
			{
				if (nVertices > 0) pMesh = new CMeshTextured(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, pxmf3TextureCoords0, nIndices, pnIndices);
			}
			else 
			{
				if (nVertices > 0) pMesh = new CMesh(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, nIndices, pnIndices);
			}
					
			if (pMesh) 
				SetMesh(0, pMesh);
			else 
				ResizeMeshes(0);

			if (pMaterial) SetMaterial(pMaterial);

			if (pxmf3Positions) delete[] pxmf3Positions;
			if (pxmf3Normals) delete[] pxmf3Normals;
			if (pxmf3TextureCoords0) delete[] pxmf3TextureCoords0;
			if (pxmf3TextureCoords1) delete[] pxmf3TextureCoords1;
			if (pnIndices) delete[] pnIndices;

			break;
		}
	}
}

void CGameObject::PrintFrameInfo(CGameObject *pGameObject, CGameObject *pParent)
{
	TCHAR pstrDebug[128] = { 0 };

	_stprintf_s(pstrDebug, 128, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) PrintFrameInfo(pGameObject->m_pSibling, pParent);
	if (pGameObject->m_pChild) PrintFrameInfo(pGameObject->m_pChild, pGameObject);
}

void CGameObject::LoadGeometryFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, TCHAR *pstrFileName)
{
	wifstream InFile(pstrFileName);
	LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, InFile, 0);

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[128] ={ 0 };
	_stprintf_s(pstrDebug, 128, _T("Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	PrintFrameInfo(this, NULL);
#endif
}
void CGameObject::LoadGeometryFromFBXMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *pstrFileName)
{
	//m
	CMesh *TestMesh = new CFBXMesh(pd3dDevice, pstrFileName, 10.0f,pd3dCommandList);
	if (TestMesh)
	SetMesh(0,TestMesh);



}
void CGameObject::LoadGeometryFromShadowFBXMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *pstrFileName)
{
	//m
	CMesh *TestMesh = new CFBXMesh(pd3dDevice, pstrFileName, 10.0f, pd3dCommandList,true);
	if (TestMesh)
		SetMesh(0, TestMesh);



}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
CRotatingObject::CRotatingObject(int nMeshes)
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 15.0f;
}

CRotatingObject::~CRotatingObject()
{
}
//빌보드 카메라 따라가기 추가
void CRotatingObject::SetLookAt(XMFLOAT3& xmf3Target)
{
	XMFLOAT3 xmf3Up(0.0f, 1.0f, 0.0f);
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::Subtract(xmf3Target, xmf3Position);
	XMFLOAT3 xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look);
	m_xmf4x4World._11 = -xmf3Right.x; m_xmf4x4World._12 = xmf3Right.y; m_xmf4x4World._13 = -xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x; m_xmf4x4World._22 = xmf3Up.y; m_xmf4x4World._23 = xmf3Up.z;
	//m_xmf4x4World._31 = xmf3Look.x; m_xmf4x4World._32 = xmf3Look.y; m_xmf4x4World._33 = xmf3Look.z;
}

void CRotatingObject::Animate(float fTimeElapsed, CCamera *pCamera)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
	CGameObject::MoveForward();
	//빌보드 따라가기 추가
	//XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	//SetLookAt(xmf3CameraPosition);
}
void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
	CGameObject::MoveForward();
	//빌보드 따라가기 추가	
}

void CRotatingObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CGameObject::Render(pd3dCommandList, pCamera);
}
void CRotatingObject::SetMoveSpeed(float speed)
{
	m_fMovingSpeed = speed;
}

//빌보드 오브젝트 CBillboardObject
CBillboardObject::CBillboardObject(int nMeshes)
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 15.0f;
}

CBillboardObject::~CBillboardObject()
{
}
//빌보드 카메라 따라가기 추가
void CBillboardObject::SetLookAt(XMFLOAT3& xmf3Target, CCamera *pCamera)
{
	XMFLOAT3 xmf3Up = pCamera->GetUpVector();
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::ScalarProduct(pCamera->GetLookVector(), -1);
	XMFLOAT3 xmf3Right = Vector3::ScalarProduct(pCamera->GetRightVector(), -1);
	m_xmf4x4World._11 = -xmf3Right.x; m_xmf4x4World._12 = xmf3Right.y; m_xmf4x4World._13 = -xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x; m_xmf4x4World._22 = xmf3Up.y; m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x; m_xmf4x4World._32 = xmf3Look.y; m_xmf4x4World._33 = xmf3Look.z;
}

void CBillboardObject::Animate(float fTimeElapsed, CCamera *pCamera)
{
	//CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
	//빌보드 따라가기 추가
	XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	SetLookAt(xmf3CameraPosition, pCamera);
}
void CBillboardObject::Animate(float fTimeElapsed)
{
	//CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
	//빌보드 따라가기 추가	
	
	//SetLookAt();
	
}

void CBillboardObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CGameObject::Render(pd3dCommandList, pCamera);
}
////////////////////////////////////////////////////////////////////////////////////
//UIObject - billboard상속
//CUIObject::CUIObject()
//{
//	
//}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
CRevolvingObject::CRevolvingObject(int nMeshes)
{
	m_xmf3RevolutionAxis = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_fRevolutionSpeed = 0.0f;
}

CRevolvingObject::~CRevolvingObject()
{
}

void CRevolvingObject::Animate(float fTimeElapsed)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3RevolutionAxis), XMConvertToRadians(m_fRevolutionSpeed * fTimeElapsed));
	m_xmf4x4World = Matrix4x4::Multiply(m_xmf4x4World, mtxRotate);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CApacheHellicopter::CApacheHellicopter(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/Apache.txt");

	m_pRotorFrame = FindFrame(_T("rotor"));
}

CApacheHellicopter::~CApacheHellicopter()
{
}

void CApacheHellicopter::Animate(float fTimeElapsed)
{
	if (m_pRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 3.0f) * fTimeElapsed);
		m_pRotorFrame->m_xmf4x4ToParentTransform = Matrix4x4::Multiply(xmmtxRotate, m_pRotorFrame->m_xmf4x4ToParentTransform);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
CDinosour::CDinosour(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, int i)
{
	if (i == 0)
		LoadGeometryFromFBXMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Assets/Model/Allosaurus.data");
	else if (i == 1)
		LoadGeometryFromFBXMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Assets/Model/RockWarrior.data");
	else
		LoadGeometryFromFBXMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Assets/Model/Boss_SCORPION_DIFFUSE.data");

	//texture 추가 ===========================================================================05.05
	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	TCHAR pstrPathName[64] = { '\0' };
	if (i == 0)
		_tcscat_s(pstrPathName, 128, _T("Assets/Model/diffuse.dds"));
	else if (i == 1)
		_tcscat_s(pstrPathName, 128, _T("Assets/Model/RockWarrior.dds"));
	else
		_tcscat_s(pstrPathName, 128, _T("Assets/Model/Boss_SCORPION_DIFFUSE.dds"));

	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pstrPathName, 0);


	CMaterial *pMaterial = NULL;
	pMaterial = new CMaterial();
	//pMaterial->m_xmf4Albedo = xmf4MaterialAlbedo;

	pMaterial->SetTexture(pTexture);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CIlluminatedTexturedShader *pShader = new CIlluminatedTexturedShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);

	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

	pMaterial->SetShader(pShader);
	if (pMaterial) SetMaterial(pMaterial);

	if (i == 0) {
		XMFLOAT3 pos = {2500, 80, 2500};
		SetPosition(pos);
		SetScale(1.0f, 1.0f, 1.0f);
		Rotate(0, 180, 0);
	}
	else if (i == 1) {
		Rotate(0, 180, 0);
		SetScale(100.0f, 100.0f, 100.0f);
	}
	else {
		Rotate(0, 180, 0);
		SetScale(1.5f, 1.5, 1.5f);
	}

}

CDinosour::~CDinosour()
{
}

void CDinosour::Animate(float fTimeElapsed)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCatus::CCatus(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	//LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/Gunship.txt");
	static char Catuscnt = 0;
	Catuscnt++;
	//m_pRotorFrame = FindFrame(_T("Rotor"));
	//m_pBackRotorFrame = FindFrame(_T("Back_Rotor"));

	//m_pHellfileMissileFrame = FindFrame(_T("Hellfire_Missile"));
	////if (m_pHellfileMissileFrame) //m_pHellfileMissileFrame->m_bActive = false;

	//SetScale(5.0f, 5.0f, 5.0f);
	//Rotate(0.0f, 0.0f, 0.0f);

	if (Catuscnt % 3 == 0)
		LoadGeometryFromFBXMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Assets/Model/CactusShort03.data");
	else if (Catuscnt % 3 == 1)
		LoadGeometryFromFBXMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Assets/Model/CactusShort02.data");
	else
		LoadGeometryFromFBXMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Assets/Model/CactusShort01.data");


	//texture 추가 ===========================================================================05.05
	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	TCHAR pstrPathName[64] = { '\0' };
	_tcscat_s(pstrPathName, 128, _T("Assets/Model/CatusPack1.dds"));
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pstrPathName, 0);


	CMaterial *pMaterial = NULL;
	pMaterial = new CMaterial();
	//pMaterial->m_xmf4Albedo = xmf4MaterialAlbedo;

	pMaterial->SetTexture(pTexture);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CNONAnimateShader *pShader = new CNONAnimateShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);

	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

	pMaterial->SetShader(pShader);
	if (pMaterial) SetMaterial(pMaterial);

}

CCatus::~CCatus()
{
}

void CCatus::Animate(float fTimeElapsed)
{
	//if (m_pHellfileMissileFrame->m_bActive ==true)
	//{
	//	//에니메이션 추가코드
	//	//GetLook();
	//	
	//	XMMATRIX xmmtranslate = XMMatrixTranslation(0,0, 5.0f * fTimeElapsed);
	//	m_pHellfileMissileFrame->m_xmf4x4ToParentTransform = Matrix4x4::Multiply(xmmtranslate, m_pHellfileMissileFrame->m_xmf4x4ToParentTransform);
	//}
}

CWall::CWall(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	//LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/Gunship.txt");
	//static char Catuscnt = 0;
	//Catuscnt++;
	//m_pRotorFrame = FindFrame(_T("Rotor"));
	//m_pBackRotorFrame = FindFrame(_T("Back_Rotor"));

	//m_pHellfileMissileFrame = FindFrame(_T("Hellfire_Missile"));
	////if (m_pHellfileMissileFrame) //m_pHellfileMissileFrame->m_bActive = false;

	//SetScale(5.0f, 5.0f, 5.0f);
	//Rotate(0.0f, 0.0f, 0.0f);


	LoadGeometryFromFBXMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Assets/Model/Wall2.data");



	//texture 추가 ===========================================================================05.05
	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	TCHAR pstrPathName[64] = { '\0' };
	_tcscat_s(pstrPathName, 128, _T("Assets/Model/WallSand1.dds"));
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pstrPathName, 0);


	CMaterial *pMaterial = NULL;
	pMaterial = new CMaterial();
	//pMaterial->m_xmf4Albedo = xmf4MaterialAlbedo;

	pMaterial->SetTexture(pTexture);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CNONDepthShader *pShader = new CNONDepthShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);

	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

	pMaterial->SetShader(pShader);
	if (pMaterial) SetMaterial(pMaterial);

}

CWall::~CWall()
{
}

void CWall::Animate(float fTimeElapsed)
{
	//if (m_pHellfileMissileFrame->m_bActive ==true)
	//{
	//	//에니메이션 추가코드
	//	//GetLook();
	//	
	//	XMMATRIX xmmtranslate = XMMatrixTranslation(0,0, 5.0f * fTimeElapsed);
	//	m_pHellfileMissileFrame->m_xmf4x4ToParentTransform = Matrix4x4::Multiply(xmmtranslate, m_pHellfileMissileFrame->m_xmf4x4ToParentTransform);
	//}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGunshipHellicopter::CGunshipHellicopter(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	//LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/Gunship.txt");

	//m_pRotorFrame = FindFrame(_T("Rotor"));
	//m_pBackRotorFrame = FindFrame(_T("Back_Rotor"));

	//m_pHellfileMissileFrame = FindFrame(_T("Hellfire_Missile"));
	////if (m_pHellfileMissileFrame) //m_pHellfileMissileFrame->m_bActive = false;

	//SetScale(5.0f, 5.0f, 5.0f);
	//Rotate(0.0f, 0.0f, 0.0f);
	LoadGeometryFromFBXMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Assets/Model/Maple4069.data");
	//texture 추가 ===========================================================================05.05
	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	TCHAR pstrPathName[64] = { '\0' };
	_tcscat_s(pstrPathName, 128, _T("Assets/Model/Klen-Bark.dds"));
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pstrPathName, 0);


	CMaterial *pMaterial = NULL;
	pMaterial = new CMaterial();
	//pMaterial->m_xmf4Albedo = xmf4MaterialAlbedo;

	pMaterial->SetTexture(pTexture);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CNONAnimateShader *pShader = new CNONAnimateShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);

	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

	pMaterial->SetShader(pShader);
	if (pMaterial) SetMaterial(pMaterial);

}

CGunshipHellicopter::~CGunshipHellicopter()
{
}


void CGunshipHellicopter::Animate(float fTimeElapsed)
{
	if (m_pRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 3.0f) * fTimeElapsed);
		m_pRotorFrame->m_xmf4x4ToParentTransform = Matrix4x4::Multiply(xmmtxRotate, m_pRotorFrame->m_xmf4x4ToParentTransform);
	}

	if (m_pBackRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 3.0f) * fTimeElapsed);
		m_pBackRotorFrame->m_xmf4x4ToParentTransform = Matrix4x4::Multiply(xmmtxRotate, m_pBackRotorFrame->m_xmf4x4ToParentTransform);
	}
	//if (m_pHellfileMissileFrame->m_bActive ==true)
	//{
	//	//에니메이션 추가코드
	//	//GetLook();
	//	
	//	XMMATRIX xmmtranslate = XMMatrixTranslation(0,0, 5.0f * fTimeElapsed);
	//	m_pHellfileMissileFrame->m_xmf4x4ToParentTransform = Matrix4x4::Multiply(xmmtranslate, m_pHellfileMissileFrame->m_xmf4x4ToParentTransform);
	//}
}

CTreeShadow::CTreeShadow(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	//LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/Gunship.txt");

	//m_pRotorFrame = FindFrame(_T("Rotor"));
	//m_pBackRotorFrame = FindFrame(_T("Back_Rotor"));

	//m_pHellfileMissileFrame = FindFrame(_T("Hellfire_Missile"));
	////if (m_pHellfileMissileFrame) //m_pHellfileMissileFrame->m_bActive = false;

	//SetScale(5.0f, 5.0f, 5.0f);
	//Rotate(0.0f, 0.0f, 0.0f);
	LoadGeometryFromFBXMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Assets/Model/Maple4069.data");
	//texture 추가 ===========================================================================05.05
	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	TCHAR pstrPathName[64] = { '\0' };
	_tcscat_s(pstrPathName, 128, _T("Assets/Model/Klen-Bark.dds"));
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pstrPathName, 0);


	CMaterial *pMaterial = NULL;
	pMaterial = new CMaterial();
	//pMaterial->m_xmf4Albedo = xmf4MaterialAlbedo;

	pMaterial->SetTexture(pTexture);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CNONAnimateShadowShader *pShader = new CNONAnimateShadowShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);

	SetScale(0.5f,0.5f,0.5f);
	SetScaleWorldM(0.5f,0.5f,0.5f);

	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

	pMaterial->SetShader(pShader);
	if (pMaterial) SetMaterial(pMaterial);

}

CTreeShadow::~CTreeShadow()
{
}

void CTreeShadow::Animate(float fTimeElapsed)
{
	if (m_pRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 3.0f) * fTimeElapsed);
		m_pRotorFrame->m_xmf4x4ToParentTransform = Matrix4x4::Multiply(xmmtxRotate, m_pRotorFrame->m_xmf4x4ToParentTransform);
	}

	if (m_pBackRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 3.0f) * fTimeElapsed);
		m_pBackRotorFrame->m_xmf4x4ToParentTransform = Matrix4x4::Multiply(xmmtxRotate, m_pBackRotorFrame->m_xmf4x4ToParentTransform);
	}
	//if (m_pHellfileMissileFrame->m_bActive ==true)
	//{
	//	//에니메이션 추가코드
	//	//GetLook();
	//	
	//	XMMATRIX xmmtranslate = XMMatrixTranslation(0,0, 5.0f * fTimeElapsed);
	//	m_pHellfileMissileFrame->m_xmf4x4ToParentTransform = Matrix4x4::Multiply(xmmtranslate, m_pHellfileMissileFrame->m_xmf4x4ToParentTransform);
	//}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
CFlyerShip::CFlyerShip(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	//LoadGeometryFromFBX(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/spider.txt");
	//LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/Flyer.txt");
	LoadGeometryFromFBXMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,"../Assets/Model/Allosaurus_Default.data");
}

CFlyerShip::~CFlyerShip()
{
}
//HeightMap //////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CHeightMapTerrain::CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color,int stage) : CGameObject(0)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	m_nMeshes = cxBlocks * czBlocks;
	m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;

	CHeightMapGridMesh *pHeightMapGridMesh = NULL;
	for (int z = 0, zStart =0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z*cxBlocks), pHeightMapGridMesh);
		}
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pTerrainTexture = new CTexture(2, RESOURCE_TEXTURE2D, 0);

	if (stage == 1)
	{
		pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Terrain/Base_Texture.dds", 0);
		pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Terrain/Detail_Texture_7.dds", 1);
	}
	if (stage == 2)
	{
		pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Terrain/stone.dds", 0);
		pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Terrain/stone.dds", 1);
	}
	if (stage == 3)
	{
		pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Terrain/desert.dds", 0);
		pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Terrain/desert.dds", 1);
	}

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CTerrainShader *pTerrainShader = new CTerrainShader();
	pTerrainShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTerrainShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 2);
	pTerrainShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pTerrainShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTerrainTexture, 6, true);

	pTerrainShader->CreateInputLayout();

	CMaterial *pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);

	SetMaterial(pTerrainMaterial);

	SetCbvGPUDescriptorHandle(pTerrainShader->GetGPUCbvDescriptorStartHandle());

	SetShader(pTerrainShader);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

//WaterHeightmap //////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//WaterHeightmap //////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CWaterHeightmap::CWaterHeightmap(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CGameObject(0)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	m_nMeshes = cxBlocks * czBlocks;
	m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;

	CHeightMapGridMesh *pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z*cxBlocks), pHeightMapGridMesh);
		}
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pTerrainTexture = new CTexture(2, RESOURCE_TEXTURE2D, 0);

	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Terrain/Water_Detail_Texture_0.dds", 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Terrain/water7.dds", 1);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CTerrainWaterShader *pWaterShader = new CTerrainWaterShader();
	pWaterShader->CreateAlphaBlendingWaterShader(pd3dDevice, pd3dGraphicsRootSignature);
	pWaterShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pWaterShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 2);
	pWaterShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pWaterShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTerrainTexture, 6, true);

	//pTerrainShader->CreateInputLayout();

	CMaterial *pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);

	SetMaterial(pTerrainMaterial);

	SetCbvGPUDescriptorHandle(pWaterShader->GetGPUCbvDescriptorStartHandle());

	SetShader(pWaterShader);
}

CWaterHeightmap::~CWaterHeightmap(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
CSkyBox::CSkyBox(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, int stage) : CGameObject(6)
{
	CTexturedRectMesh *pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
	SetMesh(0, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -10.0f);
	SetMesh(1, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, -10.0f, 0.0f, 0.0f);
	SetMesh(2, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, +10.0f, 0.0f, 0.0f);
	SetMesh(3, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 0.0f, 20.0f, 0.0f, +10.0f, 0.0f);
	SetMesh(4, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 0.0f, 20.0f, 0.0f, -10.0f, 0.0f);
	SetMesh(5, pSkyBoxMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pSkyBoxTexture = new CTexture(6, RESOURCE_TEXTURE2D, 0);

	if (stage == 1) {
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/2.dds", 0);
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/3.dds", 1);
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/4.dds", 2);
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/1.dds", 3);
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/6.dds", 4);
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/5.dds", 5);
	}
	if (stage == 2) {
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/BlueNebular_back_1.dds", 1);
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/BlueNebular_bottom_1.dds", 0);
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/BlueNebular_front_1.dds", 2);
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/BlueNebular_left_1.dds", 3);
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/BlueNebular_right_1.dds", 4);
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/BlueNebular_top_1.dds", 5);
	}

	if (stage == 3) {
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/66_1.dds", 0);
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/33_1.dds", 1);
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/44_1.dds", 2);
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/11_1.dds", 3);
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/22_1.dds", 4);
		pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/55_1.dds", 5);
	}
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CSkyBoxShader *pSkyBoxShader = new CSkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pSkyBoxShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 6);
	pSkyBoxShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pSkyBoxShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pSkyBoxTexture, 8, false);

	CMaterial *pSkyBoxMaterial = new CMaterial();
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);

	SetMaterial(pSkyBoxMaterial);

	SetCbvGPUDescriptorHandle(pSkyBoxShader->GetGPUCbvDescriptorStartHandle());

	SetShader(pSkyBoxShader);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	OnPrepareRender();

	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);

			UpdateShaderVariables(pd3dCommandList);
		}
	}

	pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dCbvGPUDescriptorHandle);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_pMaterial)
			{
				if (m_pMaterial->m_pTexture) m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, i);
			}
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}



/////////////////////////////////////////

CBall::CBall(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{

	LoadGeometryFromFBXMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Assets/Model/circlecolum.data");
	//texture 추가 ===========================================================================05.05
	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	TCHAR pstrPathName[64] = { '\0' };
	_tcscat_s(pstrPathName, 128, _T("Assets/Model/reversefirewalleffect.dds"));
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pstrPathName, 0);


	CMaterial *pMaterial = NULL;
	pMaterial = new CMaterial();
	//pMaterial->m_xmf4Albedo = xmf4MaterialAlbedo;

	pMaterial->SetTexture(pTexture);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CNONAnimateShader *pShader = new CNONAnimateShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);

	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

	SetScale(1.0f, 1.0f, 1.0f);
	pMaterial->SetShader(pShader);
	if (pMaterial) SetMaterial(pMaterial);

}

CBall::~CBall()
{

}

void CBall::Animate(float fTimeElapsed)
{
	//if (m_pRotorFrame)
	//{
	//   XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 3.0f) * fTimeElapsed);
	//   m_pRotorFrame->m_xmf4x4ToParentTransform = Matrix4x4::Multiply(xmmtxRotate, m_pRotorFrame->m_xmf4x4ToParentTransform);
	//}
	//m_xmf4x4World.x
	//m_xmf4x4ToParentTransform._41 = x;
	//   m_xmf4x4ToParentTransform._42 = y;
	//   m_xmf4x4ToParentTransform._43 = z;


	//m_xmf4x4World._41 = x;
	//m_xmf4x4World._42 = y;
	//m_xmf4x4World._43 = z;
	XMMATRIX mtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 0.5f) * fTimeElapsed);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);

	//XMMATRIX mtxTranslation = XMMatrixTranslation(m_xmf4x4World._41, m_xmf4x4World._42, 0.00001f *fTimeElapsed + m_xmf4x4World._43);
	//m_xmf4x4World = Matrix4x4::Multiply(m_xmf4x4World, mtxTranslation);

	//XMMATRIX mtxParentsTranslation = XMMatrixTranslation(m_xmf4x4ToParentTransform._41, m_xmf4x4ToParentTransform._42, 0.000001f *fTimeElapsed + m_xmf4x4ToParentTransform._43);
	//m_xmf4x4ToParentTransform = Matrix4x4::Multiply(m_xmf4x4ToParentTransform, mtxTranslation);

	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	//XMStoreFloat4x4(&m_pcbMappedGameObject->m, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	//   D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbPlayer->GetGPUVirtualAddress();
	//   pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_OBJECT, d3dGpuVirtualAddress);
	//if (m_pBackRotorFrame)
	//{
	//   XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 3.0f) * fTimeElapsed);
	//   m_pBackRotorFrame->m_xmf4x4ToParentTransform = Matrix4x4::Multiply(xmmtxRotate, m_pBackRotorFrame->m_xmf4x4ToParentTransform);
	//}

}




//==========CEffect
//==============================
CRainobj::CRainobj(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{

	LoadGeometryFromFBXMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Assets/Model/box.data");
	//texture 추가 ===========================================================================05.05
	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	TCHAR pstrPathName[64] = { '\0' };
	_tcscat_s(pstrPathName, 128, _T("Assets/Model/rain.dds"));
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pstrPathName, 0);


	CMaterial *pMaterial = NULL;
	pMaterial = new CMaterial();
	//pMaterial->m_xmf4Albedo = xmf4MaterialAlbedo;

	pMaterial->SetTexture(pTexture);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CEffectShader *pShader = new CEffectShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);

	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

	pMaterial->SetShader(pShader);
	if (pMaterial) SetMaterial(pMaterial);

	//
	float t_x = rand() % 5000;
	float t_y = rand() % 400 + 100;
	float t_z = rand() % 5000;
	SetPosition(t_x, t_y, t_z);
	//
	m_speed.x = 0.f;
	m_speed.y = ((rand() / (float)RAND_MAX) - 1.5f)*10.f;
	m_speed.z = 0.f;
}

CRainobj::~CRainobj()
{

}

void CRainobj::Animate(float fTimeElapsed)
{
	m_CurrentTime += (fTimeElapsed);
	XMMATRIX mtxTranslate = ::XMMatrixTranslation(m_speed.x, m_CurrentTime*m_speed.y, m_speed.z);
	//XMMATRIX mtxRotate = ::XMMatrixRotationY(XMConvertToRadians(360.0f * 0.5f) * fTimeElapsed);

	XMFLOAT3 pos = GetPosition();
	if (pos.y <= 0) {
		pos.y = 800;
		SetPosition(pos);
		m_CurrentTime = 0.f;
	}

	m_xmf4x4World = Matrix4x4::Multiply(mtxTranslate, m_xmf4x4World);
	//	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);



	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, ::XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

}
ID3D12Resource *CRainobj::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObject->Map(0, NULL, (void **)&m_pcbMappedGameObject);

	return(m_pd3dcbGameObject);
}

void CRainobj::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObject)
	{
		m_pd3dcbGameObject->Unmap(0, NULL);
		m_pd3dcbGameObject->Release();
	}

	if (m_pMaterial) m_pMaterial->ReleaseShaderVariables();
}

void CRainobj::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	if (m_pMaterial) m_pcbMappedGameObject->m_nMaterial = m_pMaterial->m_nReflection;
	//D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneGpuVirtualAddress = m_pd3dcbParticle->GetGPUVirtualAddress();
	//pd3dCommandList->SetGraphicsRootConstantBufferView(10, d3dcbBoneGpuVirtualAddress);
}

//////////

CSnowobj::CSnowobj(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{

	LoadGeometryFromFBXMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Assets/Model/snow.data");
	//texture 추가 ===========================================================================05.05
	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	TCHAR pstrPathName[64] = { '\0' };
	_tcscat_s(pstrPathName, 128, _T("Assets/Model/snow.dds"));
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pstrPathName, 0);


	CMaterial *pMaterial = NULL;
	pMaterial = new CMaterial();
	//pMaterial->m_xmf4Albedo = xmf4MaterialAlbedo;

	pMaterial->SetTexture(pTexture);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CEffectShader *pShader = new CEffectShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);

	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

	pMaterial->SetShader(pShader);
	if (pMaterial) SetMaterial(pMaterial);

	//
	float t_x = rand() % 5000;
	float t_y = rand() % 400 + 100;
	float t_z = rand() % 5000;
	SetPosition(t_x, t_y, t_z);
	//
	m_speed.x = rand() % 10 + 2;
	m_speed.y = ((rand() / (float)RAND_MAX) - 1.f)*2.f;
	m_speed.z = rand() % 10 + 2;

	m_CurrentTime = rand() % 1000 * 0.05f;
}

CSnowobj::~CSnowobj()
{

}

void CSnowobj::Animate(float fTimeElapsed)
{
	m_CurrentTime += (fTimeElapsed);
	float offsetX = sin(m_CurrentTime*3.141592*m_speed.x);
	float offsetZ = cos(m_CurrentTime*3.141592*m_speed.z);
	XMMATRIX mtxTranslate = ::XMMatrixTranslation(offsetX, m_CurrentTime*m_speed.y, offsetZ);
	XMMATRIX mtxRotate = ::XMMatrixRotationY(XMConvertToRadians(360.0f * 0.5f)* m_CurrentTime);

	XMFLOAT3 pos = GetPosition();
	if (pos.y <= 0) {
		m_CurrentTime = 0.f;

		float t_x = rand() % 5000;
		float t_y = 800;
		float t_z = rand() % 5000;
		SetPosition(t_x, t_y, t_z);
	}

	m_xmf4x4World = Matrix4x4::Multiply(mtxTranslate, m_xmf4x4World);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);



	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, ::XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

}
ID3D12Resource *CSnowobj::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObject->Map(0, NULL, (void **)&m_pcbMappedGameObject);

	return(m_pd3dcbGameObject);
}

void CSnowobj::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObject)
	{
		m_pd3dcbGameObject->Unmap(0, NULL);
		m_pd3dcbGameObject->Release();
	}

	if (m_pMaterial) m_pMaterial->ReleaseShaderVariables();
}

void CSnowobj::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	if (m_pMaterial) m_pcbMappedGameObject->m_nMaterial = m_pMaterial->m_nReflection;
	//D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneGpuVirtualAddress = m_pd3dcbParticle->GetGPUVirtualAddress();
	//pd3dCommandList->SetGraphicsRootConstantBufferView(10, d3dcbBoneGpuVirtualAddress);
}



//==============================
//==============================
CDamagedParticle::CDamagedParticle(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	LoadGeometryFromFBXMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Assets/Model/capsule.data");
	//texture 추가 ===========================================================================05.05
	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	TCHAR pstrPathName[64] = { '\0' };
	_tcscat_s(pstrPathName, 128, _T("Assets/Model/blood.dds"));
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pstrPathName, 0);


	CMaterial *pMaterial = NULL;
	pMaterial = new CMaterial();

	pMaterial->SetTexture(pTexture);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CEffectShader *pShader = new CEffectShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);

	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

	pMaterial->SetShader(pShader);
	if (pMaterial) SetMaterial(pMaterial);


	m_Force.x = (rand() % 100 + 5) * (pow(-1, rand() % 2));
	m_Force.y = rand() % 20+50;
	m_Force.z = (rand() % 100 + 5) * (pow(-1, rand() % 2));
}
CDamagedParticle::~CDamagedParticle()
{
}

void CDamagedParticle::Animate(float fTimeElapsed)
{
	m_CurrentTime += (fTimeElapsed);

	if (m_CurrentTime >= 3) {
		SetPosition(2500, -200, 2500);
		m_Force.y = rand() % 20+50;
		m_CurrentTime = 0.f;
	}


	m_Force.y -= 0.01f;
	XMMATRIX mtxTranslate = ::XMMatrixTranslation(m_Force.x*fTimeElapsed, m_Force.y*fTimeElapsed, m_Force.z*fTimeElapsed);
	m_xmf4x4World = Matrix4x4::Multiply(mtxTranslate, m_xmf4x4World);

	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, ::XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

}
void CDamagedParticle::SetForce(float x, float y, float z)
{
	m_Force.x = x;	m_Force.y = y; m_Force.z = z;
}
ID3D12Resource *CDamagedParticle::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObject->Map(0, NULL, (void **)&m_pcbMappedGameObject);

	return(m_pd3dcbGameObject);
}

void CDamagedParticle::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObject)
	{
		m_pd3dcbGameObject->Unmap(0, NULL);
		m_pd3dcbGameObject->Release();
	}

	if (m_pMaterial) m_pMaterial->ReleaseShaderVariables();
}

void CDamagedParticle::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	if (m_pMaterial) m_pcbMappedGameObject->m_nMaterial = m_pMaterial->m_nReflection;
}

