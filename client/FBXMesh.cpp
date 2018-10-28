#include "stdafx.h"
#include "FBXMesh.h"

// 정욱이형 꺼


CFBXMesh::CFBXMesh(ID3D12Device *pd3dDevice, char *pszFileName, float fSize , ID3D12GraphicsCommandList *pd3dCommandList,bool isshadow) : CMeshIlluminatedTextured(pd3dDevice, pd3dCommandList)
{
	m_fFBXModelSize = fSize;
	m_fFBXAnimationTime = 0.0f;
	m_nFBXAnimationNum = 0;
	m_nFBXNowFrameNum = 0; 
	m_nBuffers = 0;
	ifstream fin;

	int filename_length = strlen(pszFileName);
	char* ch = new char[200];
	char* sToken = new char[50];
	char* temp;
	fin.open(pszFileName);
	if (!fin.fail())
	{
		// 데이터를 읽어와 필요한 정점, 인덱스, 본, 애니메이션 수 파악
		fin.getline(ch, 200);
		fin.getline(ch, 200);
		m_nVertices = stoi(ch);
		fin.getline(ch, 200);
		m_nIndices = stoi(ch);
		fin.getline(ch, 200);
		m_nBoneCount = stoi(ch);
		fin.getline(ch, 200);
		m_nAnimationClip = stoi(ch);

		// 정점 데이터를 저장
		fin.getline(ch, 200);
		m_pd3dxvPositions = new XMFLOAT3[m_nVertices];
		m_pd3dxvNormals = new XMFLOAT3[m_nVertices];
		m_pd3dxvTexCoords = new XMFLOAT2[m_nVertices];
		if (m_nBoneCount)
		{
			m_pd3dxvBoneIndices = new XMFLOAT4[m_nVertices];
			m_pd3dxvBoneWeights = new XMFLOAT4[m_nVertices];
		}

		for (int i = 0; i < m_nVertices; i++)
		{
			// Vertice
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);	 m_pd3dxvPositions[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); m_pd3dxvPositions[i].y = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); m_pd3dxvPositions[i].z = stof(sToken);
			// Normal
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   m_pd3dxvNormals[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); m_pd3dxvNormals[i].y = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); m_pd3dxvNormals[i].z = stof(sToken);
			// UV
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   m_pd3dxvTexCoords[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); m_pd3dxvTexCoords[i].y = stof(sToken);
			if (m_nBoneCount)
			{			// BoneIndice
				fin.getline(ch, 200);
				sToken = strtok_s(ch, " ", &temp); 	 m_pd3dxvBoneIndices[i].x = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxvBoneIndices[i].y = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxvBoneIndices[i].z = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxvBoneIndices[i].w = stof(sToken);
				// BoneWeight
				fin.getline(ch, 200);
				sToken = strtok_s(ch, " ", &temp);   m_pd3dxvBoneWeights[i].x = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxvBoneWeights[i].y = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxvBoneWeights[i].z = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxvBoneWeights[i].w = stof(sToken);
			}
		}

		// 인덱스 데이터 저장
		fin.getline(ch, 200);
		m_pnIndices = new UINT[m_nIndices];  // 메쉬에있음. 

		for (int i = 0; i < m_nIndices; i += 3)
		{
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   m_pnIndices[i] = stoi(sToken);
			sToken = strtok_s(NULL, " ", &temp); m_pnIndices[i + 1] = stoi(sToken);
			sToken = strtok_s(NULL, " ", &temp); m_pnIndices[i + 2] = stoi(sToken);
		}

		// (애니메이션을 포함한 메쉬일 경우) 본 정보와 애니메이션 정보 저장
		if (m_nBoneCount)
		{
			fin.getline(ch, 200);
			m_pBoneHierarchy = new UINT[m_nBoneCount];
			m_pd3dxmtxBoneOffsets = new XMFLOAT4X4[m_nBoneCount];
			m_pd3dxmtxSQTTransform = new XMMATRIX[m_nBoneCount];
			m_pd3dxmtxFinalBone = new XMFLOAT4X4[m_nBoneCount];

			// 부모 뼈대를 가리키는 BoneHierarchy를 저장
			for (int i = 0; i < m_nBoneCount; i++)
			{
				fin.getline(ch, 200);
				m_pBoneHierarchy[i] = stoi(ch);
			}
			// 뼈대 자체의 오프셋 행렬을 저장
			fin.getline(ch, 200);
			for (int i = 0; i < m_nBoneCount; i++)
			{
				fin.getline(ch, 200);
				sToken = strtok_s(ch, " ", &temp);   m_pd3dxmtxBoneOffsets[i]._11 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._12 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._13 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._14 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._21 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._22 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._23 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._24 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._31 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._32 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._33 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._34 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._41 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._42 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._43 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._44 = stof(sToken);
				if (isshadow)
				{
					
					//m_pd3dxmtxBoneOffsets[i]._11 = 0;
					//m_pd3dxmtxBoneOffsets[i]._12 = 0;
					//m_pd3dxmtxBoneOffsets[i]._13 = 0;
					//m_pd3dxmtxBoneOffsets[i]._14 = 0;
					//m_pd3dxmtxBoneOffsets[i]._21 = 0;
					//m_pd3dxmtxBoneOffsets[i]._22 = 0;
					//m_pd3dxmtxBoneOffsets[i]._23 = 0;
					//m_pd3dxmtxBoneOffsets[i]._24 = 0;
					//m_pd3dxmtxBoneOffsets[i]._31 = 0;
					//m_pd3dxmtxBoneOffsets[i]._32 = 0;
					//m_pd3dxmtxBoneOffsets[i]._33 = 0;
					//m_pd3dxmtxBoneOffsets[i]._34 = 0;
					//m_pd3dxmtxBoneOffsets[i]._41 = 0;
					//m_pd3dxmtxBoneOffsets[i]._42 = 0;
					//m_pd3dxmtxBoneOffsets[i]._43 = 0;
					//m_pd3dxmtxBoneOffsets[i]._44 = 0;
				}
			}

			// 여기에서부터 애니메이션을 담는다.
			m_ppBoneAnimationData = new BoneAnimationData*[m_nAnimationClip];

			BoneAnimationData *pBoneAnimationData;
			for (int k = 0; k < m_nAnimationClip; k++)
			{
				pBoneAnimationData = new BoneAnimationData[m_nBoneCount];

				fin.getline(ch, 200);
				for (int i = 0; i < m_nBoneCount; i++)
				{
					fin.getline(ch, 200);
					sToken = strtok_s(ch, " ", &temp);
					sToken = strtok_s(NULL, " ", &temp);
					pBoneAnimationData[i].m_nFrameCount = stoi(sToken);
					pBoneAnimationData[i].m_pd3dxvTranslate = new XMFLOAT3[pBoneAnimationData[i].m_nFrameCount];
					pBoneAnimationData[i].m_pd3dxvScale = new XMFLOAT3[pBoneAnimationData[i].m_nFrameCount];
					pBoneAnimationData[i].m_pd3dxvQuaternion = new XMFLOAT4[pBoneAnimationData[i].m_nFrameCount];
					pBoneAnimationData[i].m_pfAniTime = new float[pBoneAnimationData[i].m_nFrameCount];

					for (int j = 0; j < pBoneAnimationData[i].m_nFrameCount; j++)
					{
						fin.getline(ch, 200);
						sToken = strtok_s(ch, " ", &temp);
						pBoneAnimationData[i].m_pfAniTime[j] = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvTranslate[j].x = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvTranslate[j].y = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvTranslate[j].z = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvScale[j].x = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvScale[j].y = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvScale[j].z = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvQuaternion[j].x = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvQuaternion[j].y = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvQuaternion[j].z = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvQuaternion[j].w = stof(sToken);
						if (isshadow)
						{
							//pBoneAnimationData[i].m_pd3dxvTranslate[j].y = 0;
							//pBoneAnimationData[i].m_pd3dxvScale[j].z = 0;
							//pBoneAnimationData[i].m_pd3dxvQuaternion[j].y = 0;
						}
					}
				}
				m_ppBoneAnimationData[k] = pBoneAnimationData;
			}
		}
	}
	fin.close();

	if (isshadow)
	{
		for (int i = 0; i < m_nVertices; i++)
		{
			m_pd3dxvPositions[i].z = 0;
			//m_pd3dxvPositions[i].x = 0;
			//m_pd3dxvTexCoords[i].z = 0;
			//m_pd3dxvBoneIndices[i].y = 0;
			//m_pd3dxvBoneWeights[i].y = 0;
		}
	}

	if (0 != m_nBoneCount)
	{
		m_nStride = sizeof(CSceletonTexturedVertex); //변경코드 
		CSceletonTexturedVertex *pVertices = new CSceletonTexturedVertex[m_nVertices];
		for (UINT i = 0; i < m_nVertices; i++) pVertices[i] = CSceletonTexturedVertex(m_pd3dxvPositions[i], m_pd3dxvNormals[i], m_pd3dxvTexCoords[i], m_pd3dxvBoneWeights[i], m_pd3dxvBoneIndices[i]);
		m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

		m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
		m_d3dVertexBufferView.StrideInBytes = m_nStride;
		m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

		if (m_nIndices > 0)
		{
			m_nIndices = m_nIndices;

			m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

			m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
			m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
			m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
		}
	}
	else
	{
		// 0512 추가 .. 애니메이션 없는 메시일경우 
		m_nStride = sizeof(CIlluminatedTexturedVertex); //변경코드 
		CIlluminatedTexturedVertex *pVertices = new CIlluminatedTexturedVertex[m_nVertices];
		for (UINT i = 0; i < m_nVertices; i++) pVertices[i] = CIlluminatedTexturedVertex(m_pd3dxvPositions[i], m_pd3dxvNormals[i], m_pd3dxvTexCoords[i]);
		m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

		m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
		m_d3dVertexBufferView.StrideInBytes = m_nStride;
		m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

		if (m_nIndices > 0)
		{
			m_nIndices = m_nIndices;

			m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

			m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
			m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
			m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
		}
	}


	//--------------------------------------------------------------------------------

	// Dx11 코드 --------------------------------------------------------------------------------
	//m_nStride = sizeof(CVertex);

	//UINT ncbXMFLOAT3Bytes = ((sizeof(XMFLOAT3) + 255) & ~255); //256의 배수
	//UINT ncbXMFLOAT2Bytes = ((sizeof(XMFLOAT2) + 255) & ~255);
	//UINT ncbXMFLOAT4Bytes = ((sizeof(XMFLOAT4) + 255) & ~255);
	////m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	////m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);

	//m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dxvPositions, ncbXMFLOAT3Bytes, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
	//m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dxvNormals, ncbXMFLOAT3Bytes, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);
	//m_pd3dTexCoordBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dxvTexCoords, ncbXMFLOAT2Bytes, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTexCoordUploadBuffer);
	//m_pd3dWeightBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dxvBoneWeights, ncbXMFLOAT4Bytes, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dWeightUploadBuffer);
	//m_pd3dBoneIndiceBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dxvBoneIndices, ncbXMFLOAT4Bytes, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBoneIndiceUploadBuffer);
	//ID3D12Resource *pd3dBuffers[5] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer, m_pd3dTexCoordBuffer, m_pd3dWeightBuffer, m_pd3dBoneIndiceBuffer };
	//UINT pnBufferStrides[5] = { sizeof(XMFLOAT3), sizeof(XMFLOAT3), sizeof(XMFLOAT2), sizeof(XMFLOAT4), sizeof(XMFLOAT4) };
	//UINT pnBufferOffsets[5] = { 0, 0, 0, 0, 0 };
	//AssembleToVertexBuffer(5, pd3dBuffers, pnBufferStrides, pnBufferOffsets);  //  04.08 버텍스 버퍼 합치는 건가봐여 
																			   //11 정의 코드인듯.

	
	//m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	//m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	//m_d3dVertexBufferView.StrideInBytes = m_nStride;
	//m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	//인덱스버퍼와 인덱스 업로드 버퍼는 원래 DX12 코드에도 있있으므로 혼선 및 충돌이 있을수 있음. 유의해서 동작.
	// 이쪽에서 충돌이 생기면 이쪽을 주석하고 새로운 변수 할당하는 것도 좋은 것 같음 04.09
	//m_pd3dIndexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	//CreateRasterizerState(pd3dDevice); //레스터라이즈 상태를 만드는데 DX11에는 있는데 DX12 쓸지 잘모르겠음? 일단 주석
	//CreateConstantBuffer(pd3dDevice);
	UINT ncbElementBytes = ((sizeof(VS_CB_SKINNED) + 255) & ~255);
	m_pd3dcbBones = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	// idle 상태의 프레임 값을 체크.
	if (m_nBoneCount)
		m_nFBXMaxFrameNum = m_ppBoneAnimationData[0][0].m_nFrameCount - 1;
}


CFBXMesh::~CFBXMesh()
{
//	if (m_pd3dWeightBuffer) m_pd3dWeightBuffer->Release();
	//if (m_pd3dBoneIndiceBuffer) m_pd3dBoneIndiceBuffer->Release();
	if (m_pd3dcbBones) m_pd3dcbBones->Release();

	if (m_pd3dxvPositions) delete[] m_pd3dxvPositions;
	if (m_pd3dxvNormals) delete[] m_pd3dxvNormals;
	if (m_pd3dxvTexCoords) delete[] m_pd3dxvTexCoords;
//	if (m_pd3dxvBoneWeights) delete[] m_pd3dxvBoneWeights;
//	if (m_pd3dxvBoneIndices) delete[] m_pd3dxvBoneIndices;
//	if (m_ppBoneAnimationData) delete[] m_ppBoneAnimationData;
//	if (m_pd3dxmtxFinalBone) delete[] m_pd3dxmtxFinalBone;
//	if (m_pd3dxmtxSQTTransform) delete[] m_pd3dxmtxSQTTransform;
//	if (m_pBoneHierarchy) delete[] m_pBoneHierarchy;
//	if (m_pd3dxmtxBoneOffsets) delete[] m_pd3dxmtxBoneOffsets;

}


void CFBXMesh::UpdateBoneTransform(ID3D12GraphicsCommandList *pd3dCommandList, int nAnimationNum, int nNowFrame)
{
	for (int i = 0; i < m_nBoneCount; i++)
	{
		
		MakeBoneMatrix(nNowFrame, nAnimationNum, i, *(m_pd3dxmtxSQTTransform + i));
	}


	// 마지막으로 본의 기본 오프셋행렬을 곱해주어 최종 행렬을 만들어준다.
	for (int i = 0; i < m_nBoneCount; i++)
	{
		XMFLOAT4X4 offset = m_pd3dxmtxBoneOffsets[i];
		XMFLOAT4X4 toRoot;

		// 12 로 변환
		XMStoreFloat4x4(&toRoot, m_pd3dxmtxSQTTransform[i]);
		m_pd3dxmtxFinalBone[i] = Matrix4x4::Multiply(offset, toRoot);

	}

	VS_CB_SKINNED* m_pcbMappedBones;
	m_pd3dcbBones->Map(0, NULL, (void **)&m_pcbMappedBones);


	for (int i = 0; i < m_nBoneCount; i++)
	{
		XMMATRIX temp = XMLoadFloat4x4(&m_pd3dxmtxFinalBone[i]);
		m_pcbMappedBones->m_d3dxmtxBone[i] = XMMatrixTranspose(temp);
	}
	m_pd3dcbBones->Unmap(0, NULL);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneGpuVirtualAddress = m_pd3dcbBones->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(9, d3dcbBoneGpuVirtualAddress);
}

void CFBXMesh::MakeBoneMatrix(int nNowframe, int nAnimationNum, int nBoneNum, XMMATRIX& BoneMatrix)
{
	// XMAffine 함수에서는 scale의 VECTOR3을 쓰지만
	// D3DXAffine 함수에서는 scale의 계수를 사용한다.

	if (m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_nFrameCount != 0)
	{

		//if (nNowframe >=0) //nNowframe = 0;
		//{
			XMFLOAT3 fScale = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvScale[nNowframe];
			XMVECTOR fScal = XMLoadFloat3(&fScale);


			XMFLOAT3 d3dxvTranslate = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvTranslate[nNowframe];
			XMVECTOR d3dxvTrans = XMLoadFloat3(&d3dxvTranslate);

			XMFLOAT4 d3dxvQuaternion = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvQuaternion[nNowframe];
			XMVECTOR d3dxvQuater = XMLoadFloat4(&d3dxvQuaternion);

			XMVECTOR d3dxvZero = { 0.0f, 0.0f, 0.0f };
			//D3DXMatrixAffineTransformation(&BoneMatrix, fScale, &d3dxvZero, &d3dxvQuaternion, &d3dxvTranslate);
			BoneMatrix = XMMatrixAffineTransformation(fScal, d3dxvZero, d3dxvQuater, d3dxvTrans);
		//}
		
	}
	else // 해당 본에 애니메이션 프레임이 아예 없을 경우 단위행렬을 리턴하자.
	{
		//D3DXMatrixIdentity(&BoneMatrix);
		BoneMatrix = XMMatrixIdentity();
	}
}



// 다음 애니메이션을 위한 프레임으로 넘긴다.
// 추가적인 애니메이션 관리를 위해 마지막 프레임일 경우 true를 리턴한다.
bool CFBXMesh::FBXFrameAdvance(float fTimeElapsed)
{
	m_fFBXAnimationTime += fTimeElapsed;

	if (m_fFBXAnimationTime > ANIFRAMETIME)	// 0.0333333f초가 지나면 1프레임 올리자.
	{
		if (m_nFBXNowFrameNum < m_nFBXMaxFrameNum - 1)
		{
			m_nFBXNowFrameNum++;
			m_fFBXAnimationTime = 0.0f;
			return false;
		}
		else
		{
			m_nFBXNowFrameNum = 0;
			m_fFBXAnimationTime = 0.0f;
			return true;
		}
	}
	else
		return false;
}

void CFBXMesh::SetAnimation(int nFBXAnimation)
{
	m_nFBXAnimationNum = nFBXAnimation;
	m_nFBXNowFrameNum = 0;
	m_nFBXMaxFrameNum = m_ppBoneAnimationData[nFBXAnimation][0].m_nFrameCount;
	m_fFBXAnimationTime = 0.0f;
}