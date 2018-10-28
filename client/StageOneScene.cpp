//-----------------------------------------------------------------------------
// File: CStageOneScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "StageOneScene.h"

CStageOneScene::CStageOneScene()
{
}

CStageOneScene::~CStageOneScene()
{
}

void CStageOneScene::BuildLightsAndMaterials()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 0.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(130.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 20.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_bEnable = true;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 20.0f;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(-150.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 8.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
	m_pLights->m_pLights[4].m_bEnable = true;
	m_pLights->m_pLights[4].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[4].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[4].m_xmf4Diffuse = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[4].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[4].m_xmf3Direction = XMFLOAT3(1.0f, 1.0f, 1.0f);

	m_pMaterials = new MATERIALS;
	::ZeroMemory(m_pMaterials, sizeof(MATERIALS));

	m_pMaterials->m_pReflections[0] = { XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f), XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) };  //플레이어 조명 
	m_pMaterials->m_pReflections[1] = { XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[2] = { XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[3] = { XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) }; //풀때기 색깔
	m_pMaterials->m_pReflections[4] = { XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[5] = { XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[6] = { XMFLOAT4(0.9f, 0.1f, 0.0f, 1.0f), XMFLOAT4(0.9f, 0.1f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 35.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) }; //진짜 풀때기 
	m_pMaterials->m_pReflections[7] = { XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 40.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };

	m_nMaterials = 8;

	for (int i = 0; i < m_nObjects; i++) UpdateMaterial(m_ppObjects[i]);

	//추가코드 --- 문제 많을수 있음
	//UpdateMaterial(m_pObjectShader);
}

void CStageOneScene::UpdateMaterial(CGameObject *pObject)
{
	CMaterial *pMaterial = pObject->m_pMaterial;
	if (pMaterial)
	{
		if (m_nMaterials < MAX_MATERIALS)
		{
			m_pMaterials->m_pReflections[m_nMaterials].m_xmf4Ambient = pMaterial->m_xmf4Albedo;
			m_pMaterials->m_pReflections[m_nMaterials].m_xmf4Diffuse = pMaterial->m_xmf4Albedo;
			m_pMaterials->m_pReflections[m_nMaterials].m_xmf4Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 20.0f);
			m_pMaterials->m_pReflections[m_nMaterials].m_xmf4Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			pMaterial->m_nReflection = m_nMaterials++;
		}
		else
			pMaterial->m_nReflection = 0;
	}

	if (pObject->m_pSibling) UpdateMaterial(pObject->m_pSibling);
	if (pObject->m_pChild) UpdateMaterial(pObject->m_pChild);
}

void CStageOneScene::SetMaterial(int nIndex, MATERIAL *pMaterial)
{
	m_pMaterials->m_pReflections[nIndex] = *pMaterial;
}

void CStageOneScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	m_nObjects = 20;
	m_ppObjects = new CGameObject*[m_nObjects];

	m_ppTreeShadow = new CTreeShadow*[m_nObjects];

	//Terrain 쓰기위한 변수 
	XMFLOAT3 xmf3Scale(20.0f, 20.0f, 20.0f);
	XMFLOAT4 xmf4Color(0.5f, 0.5f, 0.5f, 0.0f);
	//

#ifdef _WITH_APACHE_MODEL
	m_ppObjects[0] = new CApacheHellicopter(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_ppObjects[0]->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_ppObjects[0]->Rotate(0.0f, 90.0f, 0.0f);
#endif

#ifdef _WITH_TERRAIN_PARTITION
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Assets/Image/Terrain/HeightmapTest.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color,2);
	//테레인 위치 변경
#else
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Assets/Image/Terrain/HeightmapTest.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color,2);
	m_pTerrain->SetPosition(0, 0, 0);
#endif


	//m_ppObjects[0] = new CGunshipHellicopter(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);       //Terrian 때매 y 300 +  x +50
	//m_ppObjects[0]->SetPosition(XMFLOAT3(350.f, 400, 350.0+ 1200.f));
	//m_ppObjects[0]->Rotate(-90.f,0.0f,0.0f);
	//m_ppObjects[1] = new CGunshipHellicopter(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	//m_ppObjects[1]->SetPosition(XMFLOAT3(380.0f + 500.f, 400, 380.0f + 1200.f));
	//m_ppObjects[1]->Rotate(-90.f, 0.0f, 0.0f);
	//m_ppObjects[2] = new CGunshipHellicopter(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	//m_ppObjects[2]->SetPosition(XMFLOAT3(420.0f + 500.f, 400, 420.0f + 1200.f));
	//m_ppObjects[2]->Rotate(-90.f, 0.0f, 0.0f);
	//m_ppObjects[3] = new CGunshipHellicopter(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	//m_ppObjects[3]->SetPosition(XMFLOAT3(470.0f + 500.f, 400, 470.0f + 1200.f));
	//m_ppObjects[3]->Rotate(-90.f, 0.0f, 0.0f);
	//m_ppObjects[4] = new CGunshipHellicopter(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	//m_ppObjects[4]->SetPosition(XMFLOAT3(530.0f + 500.f, 400, 530.0f + 1200.f));
	//m_ppObjects[4]->Rotate(-90.f, 0.0f, 0.0f);
	int Terrianwidth = m_pTerrain->GetWidth();
	int Terrianlength = m_pTerrain->GetLength();


	for (int i = 0; i < m_nObjects; ++i)
	{
		int x = rand() % Terrianwidth;
		int y = rand() % Terrianlength;

		m_ppObjects[i] = new CGunshipHellicopter(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
		m_ppObjects[i]->SetPosition(XMFLOAT3(x, m_pTerrain->GetHeight(x, y), y));
		m_ppObjects[i]->Rotate(-90.f, 0.0f, 0.0f);
		
		m_ppTreeShadow[i] = new CTreeShadow(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
		m_ppTreeShadow[i]->SetPosition(XMFLOAT3(x, m_pTerrain->GetHeight(x, y) +10.f, y));
		m_ppTreeShadow[i]->Rotate(25.f, 0.0f, -180.0f);
	}

	//m_ppObjects[5] = new CApacheHellicopter(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	//m_ppObjects[5]->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	//m_ppObjects[5]->Rotate(0.0f, 90.0f, 0.0f);

	//m_ppObjects[m_nObjects - 1] = new CDinosour(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, 0); //용용이 
	//m_ppObjects[m_nObjects - 1]->SetPosition(XMFLOAT3(2050.f, m_pTerrain->GetHeight(1700.f, 1900.f), 1850.f));
	//m_ppObjects[m_nObjects - 1]->Rotate(0.0f, -90.0f, 0.0f);


	//예전에 있던 오브젝트 쉐이더를 이용해서 해보기.를 위해서는 Terrain 먼저 받아야 됨.
	//CObjectsShader *pObjectShader = new CObjectsShader();
	//pObjectShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	//pObjectShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);


	//CTreeShader *pTree = new CTreeShader();
	//pTree->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	//pTree->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	//m_pTreeShader = pTree;

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, 3);




	BuildLightsAndMaterials();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);


	//BuildUI(pd3dDevice, pd3dCommandList);
}




void CStageOneScene::ReBuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{

	////if() 총알 트루이고 , 플레이어 포인토 받아와 서 만듬 
	//if (isRenderBullet == true)
	//{
	//	BulletShader *pBulletShader = new BulletShader();
	//	BulletCnt++;
	//	pBulletShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	//	pBulletShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pPlayer);
	//	m_BulletShader[BulletCnt] = pBulletShader;
	//	isRenderBullet = false;
	//}


}

void CStageOneScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (m_ppObjects)
	{
		for (int i = 0; i < m_nObjects; i++)
		{
			delete m_ppObjects[i];
			delete m_ppTreeShadow[i];
		}
		delete[] m_ppObjects;
		delete[] m_ppTreeShadow;
	}


	if (m_pTerrain) delete m_pTerrain;
	if (m_pTreeShader) delete m_pTreeShader;


	ReleaseShaderVariables();

	if (m_pLights) delete m_pLights;
	if (m_pMaterials) delete m_pMaterials;
}

void CStageOneScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();

	for (int i = 0; i < m_nObjects; i++)
	{
		m_ppObjects[i]->ReleaseUploadBuffers();
		m_ppTreeShadow[i]->ReleaseUploadBuffers();
	}



	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
	if (m_pTreeShader) m_pTreeShader->ReleaseUploadBuffers();


	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();


}

ID3D12RootSignature *CStageOneScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice) //루트시그니처 두개 쓰는건 오바인가....
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[5];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 2; //Game Objects
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 3;
	pd3dDescriptorRanges[1].BaseShaderRegister = 0; //Texture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;

	//------------------Terrian 추가 코드

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 6; //t4: gtxtTerrainBaseTexture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 7; //t5: gtxtTerrainDetailTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 1;
	pd3dDescriptorRanges[4].BaseShaderRegister = 8; //t6: gtxtSkyBoxTexture
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[10];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //Player
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0]; //Game Objects
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3; //Materials
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1]; //Texture
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//Terrian 추가 
	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2];
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[3];
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[4];
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[9].Descriptor.ShaderRegister = 5; //Camera
	pd3dRootParameters[9].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc[2];
	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dSamplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc[0].MipLODBias = 0;
	d3dSamplerDesc[0].MaxAnisotropy = 1;
	d3dSamplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc[0].MinLOD = 0;
	d3dSamplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[0].ShaderRegister = 0;
	d3dSamplerDesc[0].RegisterSpace = 0;
	d3dSamplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	d3dSamplerDesc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dSamplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dSamplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dSamplerDesc[1].MipLODBias = 0;
	d3dSamplerDesc[1].MaxAnisotropy = 1;
	d3dSamplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc[1].MinLOD = 0;
	d3dSamplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[1].ShaderRegister = 1;
	d3dSamplerDesc[1].RegisterSpace = 0;
	d3dSamplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;



	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(d3dSamplerDesc);
	d3dRootSignatureDesc.pStaticSamplers = d3dSamplerDesc;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CStageOneScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);

	UINT ncbMaterialBytes = ((sizeof(MATERIALS) + 255) & ~255); //256의 배수
	m_pd3dcbMaterials = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbMaterialBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbMaterials->Map(0, NULL, (void **)&m_pcbMappedMaterials);
}

void CStageOneScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	::memcpy(m_pcbMappedMaterials, m_pMaterials, sizeof(MATERIALS));
}

void CStageOneScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
	if (m_pd3dcbMaterials)
	{
		m_pd3dcbMaterials->Unmap(0, NULL);
		m_pd3dcbMaterials->Release();
	}
}

bool CStageOneScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CStageOneScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		default:
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return false;
}

bool CStageOneScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

void CStageOneScene::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->AnimateObjects(fTimeElapsed);

	for (int i = 0; i < m_nObjects - 1; i++) 
	{
		m_ppObjects[i]->Animate(fTimeElapsed);
		m_ppObjects[m_nObjects - 1]->GetMesh()->FBXFrameAdvance(fTimeElapsed);
	}// 공룡 

	if (m_pLights)
	{
		m_pLights->m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights->m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
}
//따라 보기 추가
void CStageOneScene::AnimateObjects(float fTimeElapsed, CCamera *pCCamera)
{
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->AnimateObjects(fTimeElapsed, pCCamera);

	for (int i = 0; i < m_nObjects; i++)
	{
		m_ppObjects[i]->Animate(fTimeElapsed, pCCamera);
		m_ppTreeShadow[i]->Animate(fTimeElapsed);
	}

	if (m_pLights)
	{
		m_pLights->m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights->m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}

}

void CStageOneScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress); //Lights

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dcbMaterialsGpuVirtualAddress); //Materials

																							//추가코드
	m_pSkyBox->Render(pd3dCommandList, pCamera);
	m_pTerrain->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nObjects-1; i++) m_ppObjects[i]->UpdateTransform(NULL);
	for (int i = 0; i < m_nObjects-1; i++) m_ppObjects[i]->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nObjects - 1; i++) m_ppTreeShadow[i]->UpdateTransform(NULL);
	for (int i = 0; i < m_nObjects - 1; i++) m_ppTreeShadow[i]->Render(pd3dCommandList, pCamera);

	//for (UINT i = 0; i < m_nUIShaders; ++i)
		//m_ppUIShaders[i]->Render(pd3dCommandList);
}


