#pragma once

#define FRAME_BUFFER_WIDTH		1280
#define FRAME_BUFFER_HEIGHT		720

#include "Timer.h"
#include "Player.h"
#include "Scene.h"
#include "TitleScene.h"
#include "protocol.h"
#include "StageOneScene.h"
#include "StageTwoScene.h"
#include "StageThreeScene.h"
#include "SelecScene.h"
#include "Shader.h"
#include "UIShader.h"
#include "sound.h"
#include "EffectMgr.h"
#include "resource.h"
static bool bCharaterRunFront = false;
static bool bCharaterRunBack = false;
static bool bCharaterRunLeft = false;
static bool bCharaterRunRight = false;
static bool bCharaterAttackFireStorm = false;


const int damaged = 6;


class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	void CreateCommandQueueAndList();

	void OnResizeBackBuffers();

    void BuildObjects();
    void ReleaseObjects();

    void ProcessInput();
    void AnimateObjects();
    void FrameAdvance();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);


public:
	void Server(WPARAM wParam, LPARAM lParam);
	bool InitServer(const char* ip);
	void ShutDownServer();
	void ReadPacket(SOCKET sock);
	void ProcessPacket(char *ptr);
	float GetAngleBetweenTwoVectors2(float dVec1X, float dVec1Y, float dVec2X, float dVec2Y);

	bool SendLogInPack();
	bool GetSucessLogin() { return m_LoginSuccess; }

	void SetCurrSceneToSelectScene() { m_pCurrScene = m_pSelectScene; }
	
	char							m_ip[MAX_STR_SIZE];

private:
	WSABUF		send_wsabuf;
	char 		send_buffer[BUF_SIZE];
	WSABUF		recv_wsabuf;
	char		recv_buffer[BUF_SIZE];
	DWORD		in_packet_size;
	int			saved_packet_size;
	char		packet_buffer[BUF_SIZE];
private:
	WSADATA			wsa;
	int				retval;
	SOCKET			g_mysocket;
	SOCKADDR_IN		serveraddr;
	int		g_myid;
	bool			m_LoginSuccess = false;

private:
	HINSTANCE					m_hInstance;
	HWND						m_hWnd; 

	int							m_nWndClientWidth;
	int							m_nWndClientHeight;
        
	IDXGIFactory4				*m_pdxgiFactory = NULL;
	IDXGISwapChain3				*m_pdxgiSwapChain = NULL;
	ID3D12Device				*m_pd3dDevice = NULL;

	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;

	static const UINT			m_nSwapChainBuffers = 2;
	UINT						m_nSwapChainBufferIndex;

	ID3D12Resource				*m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap		*m_pd3dRtvDescriptorHeap = NULL;
	UINT						m_nRtvDescriptorIncrementSize;

	ID3D12Resource				*m_pd3dDepthStencilBuffer = NULL;
	//static const int m_nDepthStencilBuffers = 2;
	//ComPtr<ID3D12Resource> m_pDepthStencilBuffer[m_nDepthStencilBuffers];

	ID3D12DescriptorHeap		*m_pd3dDsvDescriptorHeap = NULL;
	UINT						m_nDsvDescriptorIncrementSize;

	ID3D12CommandAllocator		*m_pd3dCommandAllocator = NULL;
	ID3D12CommandQueue			*m_pd3dCommandQueue = NULL;
	ID3D12GraphicsCommandList	*m_pd3dCommandList = NULL;

	ID3D12Fence					*m_pd3dFence = NULL;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;

#if defined(_DEBUG)
	ID3D12Debug					*m_pd3dDebugController;
#endif

	CGameTimer					m_GameTimer;

	CScene						*m_pCurrScene = NULL;
	//진호 씬 여러개추가
	CScene						*m_pTitleScene = NULL;
	CScene						*m_pStageOneScene = NULL;
	CScene						*m_pStageTwoScene = NULL;
	CScene						*m_pStageThreeScene = NULL;
	CScene						*m_pSelectScene = NULL;


	CPlayer						*m_pTitlePlayer = NULL;
	CPlayer						*m_pStageOnePlayer = NULL;
	CPlayer						*m_pSwordPlayer = NULL;
	CPlayer						*m_pPlayer = NULL;
	CCamera						*m_pCamera = NULL;

	POINT						m_ptOldCursorPos;

	_TCHAR						m_pszFrameRate[50];

	// Other Player for network 0603 by Jin
	CPlayer							**m_ppOtherPlayers;
	CGameObject						**m_ppMonsters;

	CGameObject						**m_ppWalls;
	UIShader						*m_pUIShader;

	int								m_nWall;
	//총알때매 추가
	bool						m_bisFire;
	CFireBall					*m_fireball = NULL;
	CWaterHeightmap				*m_pWaterHeight = NULL;

	SoundMgr*					soundMgr = NULL;


	//
	CBall						*m_pCBall;
	int							m_nBalls = 10;
public:
	CEffectMgr					* m_pEffectMgr = NULL;
	CShadow						*m_pShadow = NULL;
	CMonsterShadow				**m_ppMonstersShadow;
};

