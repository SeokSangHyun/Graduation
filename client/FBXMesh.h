#pragma once
#include "stdafx.h"
//#include "Mesh.h"
#include "Object.h"


// 상수 버퍼를 위한 구조체
struct VS_CB_SKINNED
{
	XMMATRIX m_d3dxmtxBone[96];
};


struct BoneAnimationData
{
	int m_nFrameCount;
	float	*m_pfAniTime;
	XMFLOAT3 *m_pd3dxvScale;
	XMFLOAT3 *m_pd3dxvTranslate;
	XMFLOAT4 *m_pd3dxvQuaternion;
};

class CFBXMesh : public CMeshIlluminatedTextured
{
private:
	XMFLOAT3 *m_pd3dxvPositions;
	XMFLOAT3	*m_pd3dxvNormals;
	XMFLOAT2 *m_pd3dxvTexCoords;
	XMFLOAT4 *m_pd3dxvBoneWeights;
	XMFLOAT4 *m_pd3dxvBoneIndices;

	BoneAnimationData **m_ppBoneAnimationData;
	XMMATRIX *m_pd3dxmtxSQTTransform;
	XMFLOAT4X4 *m_pd3dxmtxFinalBone;

	int m_nBoneCount;
	int m_nAnimationClip;

	// i번 뼈대의 부모 색인(parentIndex)를 담는다.
	// i번 뼈대는 애니메이션 클립의 i번째 BoneAnimation 인스턴스에 대응.
	UINT *m_pBoneHierarchy;
	XMFLOAT4X4 *m_pd3dxmtxBoneOffsets;

	ID3D12Resource *m_pd3dWeightBuffer;
	ID3D12Resource *m_pd3dBoneIndiceBuffer;
	// 뼈대 상수버퍼
	ID3D12Resource *m_pd3dcbBones;

	float m_fFBXModelSize;		// 모델의 사이즈 (보통 Animate에서 조절해주기 위함)
	float m_fFBXAnimationTime;	// 모델의 AnimationTime (공통적으로 0.0333333f 씩 증가)
	int m_nFBXAnimationNum;		// 모델이 실행할 애니메이션의 값을 관리한다.
	int m_nFBXMaxFrameNum;		// 모델이 실행할 애니메이션의 최대 프레임 수.
	int m_nFBXNowFrameNum;		// 모델이 진행중인 애니메이션의 현재 프레임 값.

public:
	CFBXMesh::CFBXMesh(ID3D12Device *pd3dDevice, char *pszFileName, float fSize, ID3D12GraphicsCommandList *pd3dCommandList,bool isshadow =false);
	virtual ~CFBXMesh();

	// 해당 프레임의 SR(Q)T 회전이 반영된 행렬을 반환
	void MakeBoneMatrix(int nNowframe, int nAnimationNum, int nBoneNum, XMMATRIX& BoneMatrix);
	void UpdateBoneTransform(ID3D12GraphicsCommandList *pd3dCommandList, int nAnimationNum, int nNowFrame);
	//dx12에 맞게 내가 고친것
	//다시 고친것 ,commandlist랑 , pddevice 필요 
	// 뼈대 상수 버퍼 설정
	//void CreateConstantBuffer(ID3D12Device *pd3dDevice);

	float GetFBXModelSize() { return m_fFBXModelSize; }
	float GetFBXAnimationTime() { return m_fFBXAnimationTime; }
	int GetFBXAnimationNum() { return m_nFBXAnimationNum; }
	int GetFBXNowFrameNum() { return m_nFBXNowFrameNum; }
	int GetFBXMaxFrameNum() { return m_nFBXMaxFrameNum; }
	//void SetFBXAnimationTime(float fFBXAnimationTime) { m_fFBXAnimationTime = fFBXAnimationTime; }
	bool FBXFrameAdvance(float fTimeElapsed);
	void SetAnimation(int nFBXAnimationNum);
};

