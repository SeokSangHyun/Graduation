//------------------------------------------------------- ----------------------
// File: Object.h
//-----------------------------------------------------------------------------

#pragma once

#include "Mesh.h"
#include "FBXMesh.h"
#include "Camera.h"
#include "protocol.h"

#define DIR_FORWARD					0x01
#define DIR_BACKWARD				0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT					0x08
#define DIR_UP						0x10
#define DIR_DOWN					0x20

#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

class CShader;

//class CHeightMapTerrain;
class CMesh;
class CHeightMapImage;
//class CWaterHeightmap;
//class CGameObject;

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4						m_xmf4x4World;
	UINT							m_nMaterial;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct SRVROOTARGUMENTINFO
{
	UINT							m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGpuDescriptorHandle;
};

class CTexture
{
public:
	CTexture(int nTextureResources = 1, UINT nResourceType = RESOURCE_TEXTURE2D, int nSamplers = 0);
	virtual ~CTexture();

private:
	int								m_nReferences = 0;

	UINT							m_nTextureType = RESOURCE_TEXTURE2D;
	int								m_nTextures = 0;
	ID3D12Resource					**m_ppd3dTextures = NULL;
	ID3D12Resource					**m_ppd3dTextureUploadBuffers;
	SRVROOTARGUMENTINFO				*m_pRootArgumentInfos = NULL;

	int								m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_pd3dSamplerGpuDescriptorHandles = NULL;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dsrvGpuDescriptorHandle);
	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex);
	void ReleaseShaderVariables();

	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, wchar_t *pszFileName, UINT nIndex);
	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const char *pszFileName, UINT nIndex);

	int GetTextureCount() { return(m_nTextures); }
	ID3D12Resource *GetTexture(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	UINT GetTextureType() { return(m_nTextureType); }
	UINT GetTextureType_origin() { return(m_nTextureType); }

	SRVROOTARGUMENTINFO& GetArgumentInfos(UINT nIndex) { return m_pRootArgumentInfos[nIndex]; }

	void ReleaseUploadBuffers();
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	XMFLOAT4						m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	UINT							m_nReflection = 0;
	CTexture						*m_pTexture = NULL;
	CShader							*m_pShader = NULL;

	void SetAlbedo(XMFLOAT4 xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetTexture(CTexture *pTexture);
	void SetShader(CShader *pShader);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseShaderVariables();

	void ReleaseUploadBuffers();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGameObject
{
public:
	CGameObject(int nMeshes=1);
	virtual ~CGameObject();

	int hp;

private:
	int								m_nReferences = 0;

	//////서버용
private:
	bool visible;
	SCENE scene;
public:
	SCENE GetScene() { return scene; }
	void SetScene(SCENE s) { scene = s; }
	void SetVisible(bool o) { visible = o; }
	bool Getvisible() { return visible; }


public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	TCHAR							m_pstrFrameName[256];
	bool							m_bActive = true;

	XMFLOAT4X4						m_xmf4x4ToParentTransform;
	XMFLOAT4X4						m_xmf4x4ToRootTransform;
	XMFLOAT4X4						m_xmf4x4World;

	CMesh							**m_ppMeshes;
	int								m_nMeshes;

	CMaterial						*m_pMaterial = NULL;
	CMaterial						*m_pBulletMaterial = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

protected:
	ID3D12Resource					*m_pd3dcbGameObject = NULL;
	CB_GAMEOBJECT_INFO				*m_pcbMappedGameObject = NULL;

public:
	void SetMesh(int nIndex, CMesh *pMesh);
	void SetShader(CShader *pShader);
	void SetMaterial(CMaterial *pMaterial);

	//CMesh *GetMesh(int nIndex = 0) { return(m_ppMeshes[nIndex]); } //04.28
	CMesh* GetMesh(int nIndex = 0) {if (m_ppMeshes)return (m_ppMeshes[nIndex]);}

	//==============================================================================

	void ResizeMeshes(int nMeshes);

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	virtual ID3D12Resource *CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void OnPrepareRender();
	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void Animate(float fTimeElapsed);
	virtual void Animate(float fTimeElapsed, CCamera *pCCamera);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);
	
	virtual void BuildMaterials(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) { }
	virtual void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetLocalPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);
	void SetLocalScale(float x, float y, float z);
	void SetScaleWorldM(float x, float y, float z)
	{
		XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
		m_xmf4x4World = Matrix4x4::Multiply(mtxScale, m_xmf4x4World);
		//m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParentTransform);
	}

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void RotateWorldM(float fPitch, float fYaw, float fRoll)
	{
		XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
		m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
	}
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4 *pxmf4Quaternion);

	void UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent=NULL);

public:
	CGameObject 					*m_pParent = NULL;
	CGameObject 					*m_pChild = NULL;
	CGameObject 					*m_pSibling = NULL;

	void SetChild(CGameObject *pChild);
	CGameObject *GetParent() { return(m_pParent); }
	CGameObject *FindFrame(_TCHAR *pstrFrameName);

	void LoadFrameHierarchyFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, wifstream& InFile, UINT nFrame);
	void LoadGeometryFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, TCHAR *pstrFileName);
	void LoadGeometryFromShadowFBXMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *pstrFileName);
	
	//dx11보고 fbx로 mesh를 바꿈 
	void LoadGeometryFromFBXMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *pstrFileName);
	//LoadGeometryFromFile 카피 FBX 
	void LoadGeometryFromFBX(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, TCHAR *pstrFileName);
	void PrintFrameInfo(CGameObject *pGameObject, CGameObject *pParent);
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject(int nMeshes=1);
	virtual ~CRotatingObject();

private:
	XMFLOAT3					m_xmf3RotationAxis;
	float						m_fRotationSpeed;
	float					    m_fMovingSpeed;

	XMFLOAT3				m_xmf3MovingDirection;
	float					m_fMovingRange;

public:

	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) {
		m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection);
	}
	void SetMovingSpeed(float fSpeed) {
		m_fMovingSpeed = fSpeed;
	}
	void SetMovingRange(float fRange) {
		m_fMovingRange = fRange;
	}

	BoundingOrientedBox		m_xmOOBB;
	BoundingOrientedBox		m_xmOOBBTransformed;
	CGameObject				*m_pCollider;

	void SetOOBB(XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation) {
		m_xmOOBBTransformed = m_xmOOBB = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation);
	}

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }

	void SetMoveSpeed(float speed);

	void SetLookAt(XMFLOAT3& xmf3Target);
	virtual void Animate(float fTimeElapsed, CCamera *pCamera);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};

class CBillboardObject : public CGameObject
{
public:
	CBillboardObject(int nMeshes = 1);
	virtual ~CBillboardObject();

protected:
	XMFLOAT3					m_xmf3RotationAxis;
	float						m_fRotationSpeed;

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }

	void SetLookAt(XMFLOAT3& xmf3Target, CCamera *pCamera);
	virtual void Animate(float fTimeElapsed, CCamera *pCamera);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};


//===================================================

class CRevolvingObject : public CGameObject
{
public:
	CRevolvingObject(int nMeshes=1);
	virtual ~CRevolvingObject();

private:
	XMFLOAT3					m_xmf3RevolutionAxis;
	float						m_fRevolutionSpeed;

public:
	void SetRevolutionSpeed(float fRevolutionSpeed) { m_fRevolutionSpeed = fRevolutionSpeed; }
	void SetRevolutionAxis(XMFLOAT3 xmf3RevolutionAxis) { m_xmf3RevolutionAxis = xmf3RevolutionAxis; }

	virtual void Animate(float fTimeElapsed);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CApacheHellicopter : public CGameObject
{
public:
	CApacheHellicopter(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CApacheHellicopter();

	virtual void Animate(float fTimeElapsed);

	CGameObject					*m_pRotorFrame = NULL;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGunshipHellicopter : public CGameObject
{
public:
	CGunshipHellicopter(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CGunshipHellicopter();

	virtual void Animate(float fTimeElapsed);

	CGameObject					*m_pRotorFrame = NULL;
	CGameObject					*m_pBackRotorFrame = NULL;
	CGameObject					*m_pHellfileMissileFrame = NULL;
};
class CTreeShadow : public CGameObject
{
public:
	CTreeShadow(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CTreeShadow();

	virtual void Animate(float fTimeElapsed);

	CGameObject					*m_pRotorFrame = NULL;
	CGameObject					*m_pBackRotorFrame = NULL;
	CGameObject					*m_pHellfileMissileFrame = NULL;
};


//08.06선인장 추가 
class CCatus : public CGameObject
{
public:
	CCatus(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CCatus();

	virtual void Animate(float fTimeElapsed);

	//CGameObject					*m_pRotorFrame = NULL;
	//CGameObject					*m_pBackRotorFrame = NULL;
	//CGameObject					*m_pHellfileMissileFrame = NULL;
};

//벽추가 
class CWall : public CGameObject
{
public:
	CWall(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CWall();

	virtual void Animate(float fTimeElapsed);

	//CGameObject					*m_pRotorFrame = NULL;
	//CGameObject					*m_pBackRotorFrame = NULL;
	//CGameObject					*m_pHellfileMissileFrame = NULL;
};

//////////////////////////////////////////////////////////////////Terrian 추가
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CWaterHeightmap : public CGameObject
{
public:
	CWaterHeightmap(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CWaterHeightmap();

private:
	CHeightMapImage				*m_pHeightMapImage;

	int							m_nWidth;
	int							m_nLength;

	XMFLOAT3					m_xmf3Scale;

public:
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad) * m_xmf3Scale.y); } //World
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};
//////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CDinosour : public CGameObject
{
public:
	CDinosour(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature , int Scene);
	virtual ~CDinosour();

	virtual void Animate(float fTimeElapsed);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CFlyerShip : public CGameObject
{
public:
	CFlyerShip(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CFlyerShip();
};


//////////////////////////////////////////////////////////////////Terrian 추가
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, int stage);
	virtual ~CHeightMapTerrain();

private:
	CHeightMapImage				*m_pHeightMapImage;

	int							m_nWidth;
	int							m_nLength;

	XMFLOAT3					m_xmf3Scale;

public:
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad) * m_xmf3Scale.y); } //World
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};

//////////////////////////////////////////////////////////////////Terrian 추가
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CSkyBox : public CGameObject
{
public:
	CSkyBox(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, int stage);
	virtual ~CSkyBox();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};




//////////////////////Effet오브젝트
//=============================================
class CBall : public CGameObject
{
public:
	CBall(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CBall();
	void SetScale(float x, float y, float z)
	{
		XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
		m_xmf4x4World = Matrix4x4::Multiply(mtxScale, m_xmf4x4World);
		//m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParentTransform);
	}
	virtual void Animate(float fTimeElapsed);

};
//weather
//=============================================
class CRainobj : public CGameObject
{
public:
	CRainobj(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CRainobj();
	virtual ID3D12Resource *CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Animate(float fTimeElapsed);
public:
	float							 m_CurrentTime = 0.f;
	XMFLOAT3						m_speed;
};
class CSnowobj : public CGameObject
{
public:
	CSnowobj(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CSnowobj();
	virtual ID3D12Resource *CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Animate(float fTimeElapsed);
public:
	float							 m_CurrentTime = 0.f;
	XMFLOAT3						m_speed;
};


//=============================================
class CDamagedParticle : public CGameObject
{
public:
	CDamagedParticle(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CDamagedParticle();
	virtual ID3D12Resource *CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Animate(float fTimeElapsed);
public:
	void SetForce(float x, float y, float z);

	//////
public:
	float							 m_CurrentTime = 0.f;
	XMFLOAT3						m_Force;
};


