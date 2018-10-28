//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::BuildLightsAndMaterials()
{

}

void CScene::UpdateMaterial(CGameObject *pObject)
{

}

void CScene::SetMaterial(int nIndex, MATERIAL *pMaterial)
{
}

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{

}
void CScene::ReBuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{


}

void CScene::ReleaseObjects()
{

}

void CScene::ReleaseUploadBuffers()
{


}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice) //루트시그니처 두개 쓰는건 오바인가....
{
	return false;
}

void CScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{

}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{

}

void CScene::ReleaseShaderVariables()
{

}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return false;
}


//따라 보기 추가
void CScene::AnimateObjects(float fTimeElapsed, CCamera *pCCamera)
{

}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{

}