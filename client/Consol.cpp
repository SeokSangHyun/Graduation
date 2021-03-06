
#pragma once
#include "Consol.h"
#include "stdafx.h"

/*
	Return a string-based representation based on the attribute type.
*/
FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}




int main()
{

	////////////////
	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	bool lResult;

	// FBX SDK 준비
	lSdkManager = lSdkManager->Create();

	// IOSetting Object 생성. 이 오브젝트는 모든 import/export 세팅
	FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	FbxString lPath = FbxGetApplicationDirectory();
	lSdkManager->LoadPluginsDirectory(lPath.Buffer());

	lScene = FbxScene::Create(lSdkManager, "My Scene");

	FbxString lFilePath("Elephant.fbx");

	if (lFilePath.IsEmpty())
	{
		lResult = false;
	}
	else
	{
		int lFileMajor, lFileMinor, lFileRevision;
		int lSDKMajor, lSDKMinor, lSDKRevision;
		//int lFileFormat = -1;
		int i, lAnimStackCount;
		bool lStatus;
		char lPassword[1024];

		// Get the file version number generate by the FBX SDK.
		FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

		// Create an importer.
		FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

		// Initialize the importer by providing a filename.
		const bool lImportStatus = lImporter->Initialize(lFilePath.Buffer(), -1, lSdkManager->GetIOSettings());
		lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

		if (!lImportStatus)
		{
			FbxString error = lImporter->GetStatus().GetErrorString();
			FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
			FBXSDK_printf("Error returned: %s\n\n", error.Buffer());

			if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
			{
				FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
				FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", lFilePath.Buffer(), lFileMajor, lFileMinor, lFileRevision);
			}

		}
		lImporter->Import(lScene);


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

		FbxNode* lNode = lScene->GetRootNode();
		int *index;

		if (lNode)
		{
			for (i = 0; i < lNode->GetChildCount(); i++)
			{
				FbxNode *temp_Node = lNode->GetChild(i);
				FbxNodeAttribute::EType lAttributeType;

				//cout << "Node : " << i << endl;
				if (temp_Node->GetNodeAttribute() != NULL)
				{
					lAttributeType = (temp_Node->GetNodeAttribute()->GetAttributeType());

					if (lAttributeType == FbxNodeAttribute::eMesh)
					{
						FbxMesh* lMesh = (FbxMesh*)temp_Node->GetNodeAttribute();

						int j, lControlPointsCount = lMesh->GetControlPointsCount();
						GetControlPointsAndNormal(pxmf3Positions, pxmf3Normals, lMesh);
						
						/*===============*/
						GetPolygonVertexIndex(pnIndices, lMesh);

						/*===============*/
						getUVCoordinate(pxmf3TextureCoords0, lMesh);
					}
				}

				//cout << "==========================================" << endl;
				//cout << "==========================================" << endl;
				//cout << "==========================================" << endl;
			}
		}
	}

	/*
	////
	*/
	//ID3D12Resource *pd3dBuffer = NULL;

	//D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	//::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	//d3dHeapPropertiesDesc.Type = d3dHeapType;
	//d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	//d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	//d3dHeapPropertiesDesc.CreationNodeMask = 1;
	//d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	//D3D12_RESOURCE_DESC d3dResourceDesc;
	//::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	//d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//d3dResourceDesc.Alignment = 0;
	//d3dResourceDesc.Width = nBytes;
	//d3dResourceDesc.Height = 1;
	//d3dResourceDesc.DepthOrArraySize = 1;
	//d3dResourceDesc.MipLevels = 1;
	//d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	//d3dResourceDesc.SampleDesc.Count = 1;
	//d3dResourceDesc.SampleDesc.Quality = 0;
	//d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	//D3D12_RESOURCE_STATES d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;
	//if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD) d3dResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
	//else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK) d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	//HRESULT hResult = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, d3dResourceInitialStates, NULL, __uuidof(ID3D12Resource), (void **)&pd3dBuffer);
	/*
	1)======	fbx파일을 불러와서 object에 정보를 넘겨주어 화면에 모델을 출력한다.

	초기화
		FbxManager::Create()
		lManager->IOsetting
	
		FbxScene::Create()
	
		FbxImporter::Create()
		FbxImporter->initialize (FileName, -1, IOSetting)
		FbxImporter->import(FbxScene)
		FbxImporter::Destroy();

	노드 출력
		FbxNode *lNode, *node;
		lNode = FbxScene->GetRootNode() // 최상위 노드 가져오기
		
		while i in lNode->GetChildCount()
		{
			node = lNode->GetChild(i);		//노드를 하나씩 얻어온다.
			
			FbxMesh *mesh = node->GetNodeAttribute() // 매쉬 정보 ** 여기서 메쉬에 뭘 넣을껀지 모호

			exist mesh
			{
				while j in mesh->GetControlPointCount()
				{
					FbxVector4 vec = mesh->GetControlPointAt(j);//컨트롤 포인트만 얻어옴.
				}
				//to do
			}
		}f

		※ 이후 to do 의 자리에
			색인, 노말, uv 등의 로직을 작성할 것.

	2) Control Point를 메쉬에 넣기
		(메쉬에는  pos, normal, uv 행렬을 만든다.)
		1. 각 정보의 갯수에 대하여 동적 배열을 생성.
		2. 배열에 모든정보를 담는다.
		3. 아래 연구

			CMeshIlluminatedTextured::CMeshIlluminatedTextured(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nVertices, XMFLOAT3 *pxmf3Positions, XMFLOAT3 *pxmf3Normals, XMFLOAT2 *pxmf2UVs, UINT nIndices, UINT *pnIndices) : CMeshIlluminated(pd3dDevice, pd3dCommandList)
			{
			m_nStride = sizeof(CIlluminatedTexturedVertex);
			m_nVertices = nVertices;

			CIlluminatedTexturedVertex *pVertices = new CIlluminatedTexturedVertex[m_nVertices];
			for (UINT i = 0; i < m_nVertices; i++) pVertices[i] = CIlluminatedTexturedVertex(pxmf3Positions[i], pxmf3Normals[i], pxmf2UVs[i]);

			m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

			m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
			m_d3dVertexBufferView.StrideInBytes = m_nStride;
			m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

			if (nIndices > 0)
			{
			m_nIndices = nIndices;

			m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

			m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
			m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
			m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
		}
		}
	3) 모델을 출력하자.
		1. 정점의 정보로 출력을 한다.
		2. 정점의 저장된 정보를  VertexBuffer에 넣어서 출력한다.
		3. 정점하나하나를 선으로 연결하여 출력한다.

		m_FBXMeshLoader = NULL;
		m_nStride = sizeof(CVertex);
		m_nVertices = nVertices;

		CVertex *pVertices = new CVertex[m_nVertices];
		for (UINT i = 0; i < m_nVertices; i++) pVertices[i] = CVertex(pxmf3Positions[i]);

		m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

		m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
		m_d3dVertexBufferView.StrideInBytes = m_nStride;
		m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

		if (nIndices > 0)
		{
		m_nIndices = nIndices;

		m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

		m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
		m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
		}
	*/

	/*
	
	*/
}