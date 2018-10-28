//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------


#pragma once

#include "Shader.h"
#include "Player.h"
#include "Scene.h"

class CStageThreeScene : public CScene
{
public:
	CStageThreeScene();
	~CStageThreeScene();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReBuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseObjects();

	virtual void BuildLightsAndMaterials();
	virtual void SetMaterial(int nIndex, MATERIAL *pMaterial);
	virtual void UpdateMaterial(CGameObject *pObject);

	virtual ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	virtual ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }
	virtual void SetGraphicsRootSignature(ID3D12GraphicsCommandList *pd3dCommandList) { pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual bool ProcessInput(UCHAR *pKeysBuffer);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCCamera);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
	virtual void ReleaseUploadBuffers();

	virtual CHeightMapTerrain *GetTerrain() { return(m_pTerrain); }



	bool						isRenderBullet = false;
	int							BulletCnt = 0;
	//skybox 추가코드


protected:
	ID3D12RootSignature * m_pd3dGraphicsRootSignature = NULL;

	CGameObject					**m_ppObjects = NULL;
	int							m_nObjects = 0;

	CShader						**m_ppShaders = NULL;
	int							m_nShaders = 0;

	int							m_nTree = 0;
	int							m_nCatus = 0;
	int							m_nWall = 0;

	LIGHTS						*m_pLights = NULL;

	ID3D12Resource				*m_pd3dcbLights = NULL;
	LIGHTS						*m_pcbMappedLights = NULL;

	MATERIALS					*m_pMaterials = NULL;
	int							m_nMaterials = 0;

	ID3D12Resource				*m_pd3dcbMaterials = NULL;
	MATERIAL					*m_pcbMappedMaterials = NULL;

	//오브젝트 쉐이더
	//CObjectsShader				*m_pObjectShader = NULL;
	TmpShader				*m_pTmpShader = NULL;
	CTreeShader	            *m_pTreeShader = NULL;
	//BulletShader              * m_BulletShader[100];

	CSkyBox						*m_pSkyBox = NULL;
	CHeightMapTerrain			*m_pTerrain = NULL;

};
