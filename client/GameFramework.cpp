//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"

extern SOCKET g_mysocket;
extern WSABUF	send_wsabuf;
extern char 	send_buffer[BUF_SIZE];
extern WSABUF	recv_wsabuf;
extern char	recv_buffer[BUF_SIZE];
extern char	packet_buffer[BUF_SIZE];
extern int		g_myid;
extern char g_id[MAX_STR_SIZE];
extern char g_password[MAX_STR_SIZE];

CGameFramework::CGameFramework()
{
	m_pdxgiFactory = NULL;
	m_pdxgiSwapChain = NULL;
	m_pd3dDevice = NULL;

	for (int i = 0; i < m_nSwapChainBuffers; i++) m_ppd3dSwapChainBackBuffers[i] = NULL;
	m_nSwapChainBufferIndex = 0;

	m_pd3dCommandAllocator = NULL;
	m_pd3dCommandQueue = NULL;
	m_pd3dCommandList = NULL;

	m_pd3dRtvDescriptorHeap = NULL;
	m_pd3dDsvDescriptorHeap = NULL;

	m_nRtvDescriptorIncrementSize = 0;
	m_nDsvDescriptorIncrementSize = 0;

	m_hFenceEvent = NULL;
	m_pd3dFence = NULL;
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	m_pCurrScene = NULL;
	m_pPlayer = NULL;

	in_packet_size = 0;
	saved_packet_size = 0;

	_tcscpy_s(m_pszFrameRate, _T("Monster Tower ("));
}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();

	BuildObjects();

	return(true);
}

//#define _WITH_SWAPCHAIN

void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

#ifdef _WITH_CREATE_SWAPCHAIN_FOR_HWND
	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	dxgiSwapChainDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	dxgiSwapChainDesc.Flags = 0;
#else
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Windowed = TRUE;

	HRESULT hResult = m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hWnd, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1 **)&m_pdxgiSwapChain);
#else
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	dxgiSwapChainDesc.Flags = 0;
#else
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif

	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain **)&m_pdxgiSwapChain);
#endif

	if (!m_pdxgiSwapChain)
	{
		MessageBox(NULL, L"Swap Chain Cannot be Created.", L"Error", MB_OK);
		::PostQuitMessage(0);
		return;
	}

	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

void CGameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;

#if defined(_DEBUG)
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)&m_pd3dDebugController);
	m_pd3dDebugController->EnableDebugLayer();
#endif

	hResult = ::CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void **)&m_pdxgiFactory);

	IDXGIAdapter1 *pd3dAdapter = NULL;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice))) break;
	}

	if (!m_pd3dDevice)
	{
		hResult = m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void **)&pd3dAdapter);
		hResult = D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice);
	}

	if (!m_pd3dDevice)
	{
		MessageBox(NULL, L"Direct3D 12 Device Cannot be Created.", L"Error", MB_OK);
		::PostQuitMessage(0);
		return;
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	hResult = m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void **)&m_pd3dFence);
	for (UINT i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 1;
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	::gnCbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	if (pd3dAdapter) pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void **)&m_pd3dCommandQueue);

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void **)&m_pd3dCommandAllocator);

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void **)&m_pd3dCommandList);
	hResult = m_pd3dCommandList->Close();
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dRtvDescriptorHeap);
	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dDsvDescriptorHeap);
	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void CGameFramework::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void **)&m_ppd3dSwapChainBackBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dSwapChainBackBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void **)&m_pd3dDepthStencilBuffer);

	D3D12_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	::ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
//	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, d3dDsvCPUDescriptorHandle);
}

void CGameFramework::OnResizeBackBuffers()
{
	WaitForGpuComplete();

	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	m_nSwapChainBufferIndex = 0;
#else
	//m_pdxgiSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);
	m_nSwapChainBufferIndex = 0;
#endif
	CreateRenderTargetViews();
	CreateDepthStencilView();

	m_pd3dCommandList->Close();

	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pCurrScene) m_pCurrScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			::SetCapture(hWnd);
			::GetCursorPos(&m_ptOldCursorPos);
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			::ReleaseCapture();
			break;
		case WM_MOUSEMOVE:
			break;
		default:
			break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pCurrScene) m_pCurrScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

	cs_packet_animation *my_packet = reinterpret_cast<cs_packet_animation*>(send_buffer);
	my_packet->size = sizeof(my_packet);
	send_wsabuf.len = sizeof(my_packet);
	my_packet->type = CS_ANIM;

	DWORD iobyte;

	int anim_num = player_idle;

	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'W':
			if (!bCharaterRunFront)
			{
				anim_num = front_run;
				m_pPlayer->GetMesh()->SetAnimation(anim_num);
				bCharaterRunFront = true;
				bCharaterRunBack = false;
				bCharaterRunLeft = false;
				bCharaterRunRight = false;
				bCharaterAttackFireStorm = false;
				
				my_packet->anim_num = anim_num;
				m_pShadow->GetMesh()->SetAnimation(my_packet->anim_num);
				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
			}
			break;
		case 'S':
			if (!bCharaterRunBack)
			{
				anim_num = back_run;
				m_pPlayer->GetMesh()->SetAnimation(anim_num);
				bCharaterRunFront = false;
				bCharaterRunBack = true;
				bCharaterRunLeft = false;
				bCharaterRunRight = false;
				bCharaterAttackFireStorm = false;

				my_packet->anim_num = anim_num;
				m_pShadow->GetMesh()->SetAnimation(my_packet->anim_num);
				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
			}
			break;
		case 'A':
			if (!bCharaterRunLeft)
			{
				anim_num = left_run;
				m_pPlayer->GetMesh()->SetAnimation(anim_num);
				bCharaterRunFront = false;
				bCharaterRunBack = false;
				bCharaterRunLeft = true;
				bCharaterRunRight = false;
				bCharaterAttackFireStorm = false;
				
				my_packet->anim_num = anim_num;
				m_pShadow->GetMesh()->SetAnimation(my_packet->anim_num);
				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
			}
			break;
		case 'D':
			if (!bCharaterRunRight)
			{
				anim_num = right_run;
				m_pPlayer->GetMesh()->SetAnimation(anim_num);
				bCharaterRunFront = false;
				bCharaterRunBack = false;
				bCharaterRunLeft = false;
				bCharaterRunRight = true;
				bCharaterAttackFireStorm = false;

				my_packet->anim_num = anim_num;
				m_pShadow->GetMesh()->SetAnimation(my_packet->anim_num);
				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
			}
			break;
		case VK_SPACE:
			if (!bCharaterAttackFireStorm)
			{
				anim_num = 5;
				m_pPlayer->GetMesh()->SetAnimation(anim_num);
				m_pPlayer->isFire = true;

				bCharaterRunFront = false;
				bCharaterRunBack = false;
				bCharaterRunLeft = false;
				bCharaterRunRight = false;
				bCharaterAttackFireStorm = true;

				my_packet->anim_num = anim_num;
				m_pShadow->GetMesh()->SetAnimation(my_packet->anim_num);
				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
			}
			break;
		default:
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case 'W':
			if (bCharaterRunFront)
			{
				anim_num = player_idle;
				m_pPlayer->GetMesh()->SetAnimation(anim_num);

				bCharaterRunFront = false;
				bCharaterRunBack = false;
				bCharaterRunLeft = false;
				bCharaterRunRight = false;

				my_packet->anim_num = anim_num;
				m_pShadow->GetMesh()->SetAnimation(my_packet->anim_num);
				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
			}
			break;
		case 'S':
			if (bCharaterRunBack)
			{
				anim_num = player_idle;
				m_pPlayer->GetMesh()->SetAnimation(anim_num);
				bCharaterRunFront = false;
				bCharaterRunBack = false;
				bCharaterRunLeft = false;
				bCharaterRunRight = false;

				my_packet->anim_num = anim_num;
				m_pShadow->GetMesh()->SetAnimation(my_packet->anim_num);
				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
			}
			break;
		case 'A':
			if (bCharaterRunLeft)
			{
				anim_num = player_idle;
				m_pPlayer->GetMesh()->SetAnimation(anim_num);
				bCharaterRunFront = false;
				bCharaterRunBack = false;
				bCharaterRunLeft = false;
				bCharaterRunRight = false;
				my_packet->anim_num = anim_num;
				m_pShadow->GetMesh()->SetAnimation(my_packet->anim_num);
				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
			}
			break;
		case 'D':
			if (bCharaterRunRight)
			{
				anim_num = player_idle;
				m_pPlayer->GetMesh()->SetAnimation(anim_num);
				bCharaterRunFront = false;
				bCharaterRunBack = false;
				bCharaterRunLeft = false;
				bCharaterRunRight = false;

				my_packet->anim_num = anim_num;
				m_pShadow->GetMesh()->SetAnimation(my_packet->anim_num);
				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
			}
			break;
		case VK_SPACE:
			if (bCharaterAttackFireStorm)
			{
				anim_num = player_idle;
				m_pPlayer->GetMesh()->SetAnimation(anim_num);
				bCharaterRunFront = false;
				bCharaterRunBack = false;
				bCharaterRunLeft = false;
				bCharaterRunRight = false;

				bCharaterAttackFireStorm = false;
				my_packet->anim_num = anim_num;
				m_pShadow->GetMesh()->SetAnimation(my_packet->anim_num);
				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
			}
			break;
				case VK_ESCAPE:
					::PostQuitMessage(0);
					break;
				case VK_RETURN:
					break;
				case VK_F1:
				case VK_F2:
				case VK_F3:
					if(m_pCurrScene != m_pTitleScene)
					m_pCamera = m_pPlayer->ChangeCamera((DWORD)(wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
					break;
				case VK_F9:
				{
					BOOL bFullScreenState = FALSE;
					m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
					m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

					DXGI_MODE_DESC dxgiTargetParameters;
					dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					dxgiTargetParameters.Width = m_nWndClientWidth;
					dxgiTargetParameters.Height = m_nWndClientHeight;
					dxgiTargetParameters.RefreshRate.Numerator = 60;
					dxgiTargetParameters.RefreshRate.Denominator = 1;
					dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
					dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
					m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

					OnResizeBackBuffers();

					break;
				}
				case VK_F10:
					break;
				default:
					break;
	}
			break;
		default:
			m_pCurrScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
			break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_ACTIVATE:
		{
			if (LOWORD(wParam) == WA_INACTIVE)
				m_GameTimer.Stop();
			else
				m_GameTimer.Start();
			break;
		}
		case WM_SIZE:
		{
			m_nWndClientWidth = LOWORD(lParam);
			m_nWndClientHeight = HIWORD(lParam);

			OnResizeBackBuffers();
			break;
		}
		case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MOUSEMOVE:
			OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
            break;
        case WM_KEYDOWN:
        case WM_KEYUP:
			OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
			break;
		case WM_SOCKET:
			Server(wParam, lParam);
	}
	return(0);
}


void CGameFramework::OnDestroy()
{
    ReleaseObjects();

	::CloseHandle(m_hFenceEvent);

#if defined(_DEBUG)
	if (m_pd3dDebugController) m_pd3dDebugController->Release();
#endif

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();
	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();

	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();

	if (m_pd3dFence) m_pd3dFence->Release();

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
    if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pdxgiFactory) m_pdxgiFactory->Release();
}

void CGameFramework::BuildObjects()
{

	soundMgr = new SoundMgr();
	soundMgr->sound_bgm();

	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	//-------------------------------------
	m_pTitleScene = new CTitleScene();
	m_pTitleScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);
	m_pTitlePlayer = new CTitlePlayer(m_pd3dDevice, m_pd3dCommandList, m_pTitleScene->GetGraphicsRootSignature(), m_pTitleScene->GetTerrain(), 1);

	m_pStageOneScene = new CStageOneScene();
	m_pStageOneScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

	m_pStageOnePlayer = new CAirplanePlayer(m_pd3dDevice, m_pd3dCommandList, m_pStageOneScene->GetGraphicsRootSignature(), m_pStageOneScene->GetTerrain());

	m_pSwordPlayer = new CAirplanePlayer(m_pd3dDevice, m_pd3dCommandList, m_pStageOneScene->GetGraphicsRootSignature(), m_pStageOneScene->GetTerrain(), 1);

	m_pSelectScene = new CSelectScene();
	m_pSelectScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);
	m_pStageTwoScene = new CStageTwoScene();
	m_pStageTwoScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

	//m_pStageThreeScene = new CStageThreeScene();
	//m_pStageThreeScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

	m_pUIShader = new UIShader();
	m_pUIShader->CreateGraphicsRootSignature(m_pd3dDevice);
	m_pUIShader->CreateShader(m_pd3dDevice, 1);
	m_pUIShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList);
	//-------------------------------------------UI
	//------------------------------------

	m_pCurrScene = m_pTitleScene;

	m_pPlayer = new CAirplanePlayer(m_pd3dDevice, m_pd3dCommandList, m_pStageOneScene->GetGraphicsRootSignature(), m_pStageOneScene->GetTerrain(), 1);
	m_pCurrScene->m_pPlayer = m_pPlayer;
	m_pCamera = m_pPlayer->GetCamera();


	//---------초기 TitleScene Player로 설정--------------

	m_pCurrScene = m_pTitleScene;
	m_pPlayer = m_pTitlePlayer;
	m_pCurrScene->m_pPlayer = m_pPlayer;

	m_pPlayer->SetPosition(XMFLOAT3(0.0f, -170.0, 0.0f));
	m_pCamera = m_pPlayer->GetCamera();


	//--------------------------------------------------

	//--------------------------------------------
	// Create OtherPlayer for rendering
	//-------------------------------------------
	//waterTerrain 쓰기위한 변수 --------------------------------------------
	XMFLOAT3 xmf3WaterScale(20.0f, 2.0f, 20.0f);
	XMFLOAT4 xmf4WaterColor(0.9f, 0.9f, 0.1f, 0.0f);
	m_pWaterHeight = new CWaterHeightmap(m_pd3dDevice, m_pd3dCommandList, m_pCurrScene->GetGraphicsRootSignature(), _T("LAssets/Image/Terrain/WaterMap.raw"), 257, 257, 257, 257, xmf3WaterScale, xmf4WaterColor);
	m_pWaterHeight->SetPosition(850, -275, 1300);



	m_ppOtherPlayers = new CPlayer*[MAX_USER];
	m_ppMonsters = new CGameObject*[MAX_OBJECT_INDEX - MAX_USER];
	m_ppMonstersShadow = new CMonsterShadow*[MAX_OBJECT_INDEX - MAX_USER];

	CHeightMapTerrain *tmpterrain = m_pStageOneScene->GetTerrain();
	//tmpterrain->GetHeightMapLength
	//성벽

	m_nWall = 1;
	m_ppWalls = new CGameObject*[m_nWall];

	int looopnum = 0;
	int looppnum2 = 0;
	int looppnum3 = 0;
	int	looppnum4 = 0;
	int Terrianwidth = tmpterrain->GetWidth();
	int Terrianlength = tmpterrain->GetLength();
	for (int i = 0; i < m_nWall; ++i)
	{
		int x = tmpterrain->GetLength();
		int y = tmpterrain->GetWidth();

		++looopnum;

		m_ppWalls[i] = new CWall(m_pd3dDevice, m_pd3dCommandList, m_pCurrScene->GetGraphicsRootSignature());

		if ((m_nWall / 4) * 3 < looopnum)
		{

			++looppnum4;
			int posY = (Terrianwidth / 10) *looppnum4;
			m_ppWalls[i]->SetPosition(XMFLOAT3(x - Terrianwidth, tmpterrain->GetHeight(x, posY), posY - 200));
			m_ppWalls[i]->RotateWorldM(-90.f, 270.0f, 0.0f);
		}
		else if (m_nWall / 2 < looopnum)
		{

			++looppnum3;
			int posX = ((Terrianlength / 10) *looppnum3);
			m_ppWalls[i]->SetPosition(XMFLOAT3(posX, tmpterrain->GetHeight(posX, y + Terrianlength), y - Terrianlength));
			m_ppWalls[i]->RotateWorldM(-90.f, 180.0f, 0.0f);
		}
		else if (m_nWall / 4 < looopnum)
		{

			++looppnum2;
			int posY = (Terrianlength / 10) *looppnum2;
			m_ppWalls[i]->SetPosition(XMFLOAT3(x, tmpterrain->GetHeight(x, posY), posY - 200));
			m_ppWalls[i]->RotateWorldM(-90.f, 90.0f, 0.0f);
		}

		else
		{
			int posX = (Terrianwidth / 10)*looopnum;
			m_ppWalls[i]->SetPosition(XMFLOAT3(posX, tmpterrain->GetHeight(posX, y), y));
			m_ppWalls[i]->RotateWorldM(-90.f, 0.0f, 0.0f);
		}
	}


	for (int i = 0; i < MAX_USER; ++i) {

		m_ppOtherPlayers[i] = new COtherPlayer(m_pd3dDevice, m_pd3dCommandList, m_pCurrScene->GetGraphicsRootSignature(), m_pCurrScene->GetTerrain(), 1);
		m_ppOtherPlayers[i]->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_ppOtherPlayers[i]->SetVisible(false);
	}
	for (int i = 0; i < MAX_OBJECT_INDEX - MAX_USER; ++i) {
		m_ppMonsters[i] = new CDinosour(m_pd3dDevice, m_pd3dCommandList, m_pStageOneScene->GetGraphicsRootSignature(), i);
		if (i == 0)
			m_ppMonsters[i]->SetPosition(XMFLOAT3(0.0f, 96.0f, 0.0f));
		m_ppMonsters[i]->SetVisible(false);
		m_ppMonstersShadow[i] = new CMonsterShadow(m_pd3dDevice, m_pd3dCommandList, m_pStageOneScene->GetGraphicsRootSignature(), i);

	}
	//////
	//XMFLOAT3 t_pos;

	//m_pEffectMgr = new CEffectMgr(m_pd3dDevice, m_pd3dCommandList, m_pStageOneScene->GetGraphicsRootSignature());
	m_pCBall = new CBall(m_pd3dDevice, m_pd3dCommandList, m_pStageOneScene->GetGraphicsRootSignature());

	m_pShadow = new CShadow(m_pd3dDevice, m_pd3dCommandList, m_pStageOneScene->GetGraphicsRootSignature(), m_pStageOneScene->GetTerrain(), 1);
	//m_pShadow->Setscale
	//m_pShadow->Rotate1(0, 0, 90);
	//=====
	m_pd3dCommandList->Close();
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();


	//if (m_pEffectMgr) m_pEffectMgr->ReleaseUploadBuffer();
	if (m_pPlayer) m_pPlayer->ReleaseUploadBuffers();
	if (m_pShadow) m_pShadow->ReleaseUploadBuffers();


	for (int i = 0; i < m_nWall; ++i)
	{
		m_ppWalls[i]->ReleaseUploadBuffers();
	}
	if (m_pWaterHeight) m_pWaterHeight->ReleaseUploadBuffers();

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_ppOtherPlayers)
			m_ppOtherPlayers[i]->ReleaseUploadBuffers();
	}
	for (int i = 0; i < MAX_OBJECT_INDEX - MAX_USER; ++i)
	{
		if (m_ppMonsters)
			m_ppMonsters[i]->ReleaseUploadBuffers();
		if (m_ppMonstersShadow)
			m_ppMonstersShadow[i]->ReleaseUploadBuffers();
	}

	if (m_pCurrScene) m_pCurrScene->ReleaseUploadBuffers();


	m_GameTimer.Reset();
}

void CGameFramework::ReleaseObjects()
{
	//if (m_pEffectMgr) delete m_pEffectMgr;
	if (m_pPlayer) delete m_pPlayer;
	for (int i = 0; i < MAX_USER; ++i) {
		if (m_ppOtherPlayers)
			delete m_ppOtherPlayers[i];
	}
	if (m_pWaterHeight) m_pWaterHeight->Release();
	//if (m_pWaterHeight) delete m_pWaterHeight;
	for (int i = 0; i < MAX_OBJECT_INDEX - MAX_USER; ++i)
	{
		if (m_ppMonsters)
			delete m_ppMonsters[i];
		if (m_ppMonstersShadow)
			delete m_ppMonstersShadow[i];
	} 

	if (m_pCurrScene) m_pCurrScene->ReleaseObjects();
	if (m_pCurrScene) delete m_pCurrScene;
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	if (GetKeyboardState(pKeysBuffer) && m_pCurrScene) bProcessedByScene = m_pCurrScene->ProcessInput(pKeysBuffer);
	if (!bProcessedByScene)
	{
		DWORD dwDirection = 0;
		int x = 0, y = 0;
		//if (pKeysBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_FORWARD;
		//if (pKeysBuffer[VK_UP] & 0xF0) dwDirection |= DIR_BACKWARD;
		//if (pKeysBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_LEFT;
		//if (pKeysBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_RIGHT;
		//if (pKeysBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		//if (pKeysBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;

		if (pKeysBuffer['D'] & 0xF0) {
			x -= 1;
		}
		if (pKeysBuffer['A'] & 0xF0) {
			x += 1;
		}
		if (pKeysBuffer['W'] & 0xF0) {
			y += 1;
		}
		if (pKeysBuffer['S'] & 0xF0) {
			y -= 1;
		}
		//if (pKeysBuffer[VK_SPACE] & 0xF0)
		//	teleport = true;
		if (pKeysBuffer['1'] & 0xF0 && m_pCurrScene == m_pTitleScene) {
			m_pCurrScene = m_pSelectScene;

			m_pCurrScene->m_pPlayer = m_pPlayer;
			m_pPlayer->SetPosition(XMFLOAT3(0.0f, -170.0, 0.0f));

			m_pCamera = m_pPlayer->GetCamera();

		}

		if (pKeysBuffer['S'] & 0xF0 && m_pCurrScene == m_pSelectScene)
		{
			InitServer(m_ip);

			m_pCurrScene = m_pStageOneScene;
			if (g_myid == 0 || g_myid % 2 == 0)
				m_pPlayer = m_pStageOnePlayer;
			else
				m_pPlayer = m_pSwordPlayer;

			m_pCurrScene->m_pPlayer = m_pPlayer;
			m_pPlayer->SetPosition(XMFLOAT3(512.0f, m_pCurrScene->GetTerrain()->GetHeight(512.f, 512.f), 512.0f));
			for (int i = 0; i < MAX_USER; ++i)
				m_ppOtherPlayers[i]->SetPlayerUpdatedContext(m_pCurrScene->GetTerrain());
			m_pCamera = m_pPlayer->GetCamera();
		}



		cs_packet_move *my_packet = reinterpret_cast<cs_packet_move*>(send_buffer);
		my_packet->size = sizeof(my_packet);
		send_wsabuf.len = sizeof(my_packet);

		DWORD iobyte;
		float tempx, tempz;
		tempx = (roundf(m_pPlayer->GetLookVector().x * 100) / 100) * 100;
		tempz = (roundf(m_pPlayer->GetLookVector().z * 100) / 100) * 100;

		my_packet->dir_x = tempx;
		my_packet->dir_z = tempz;

		if (0 != x) {
			if (1 == x) my_packet->type = CS_RIGHT;
			else my_packet->type = CS_LEFT;

			int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
			if (ret) {
				int error_code = WSAGetLastError();
				printf("Error while sending packet [%d]", error_code);
			}
		}
		if (0 != y) {
			if (1 == y) my_packet->type = CS_DOWN;
			else my_packet->type = CS_UP;

			int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
			if (ret) {
				int error_code = WSAGetLastError();
				printf("Error while sending packet [%d]", error_code);
			}
		}

		//if (teleport) {
		//	my_packet->type = CS_TELEPORT;

		//	int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		//	if (ret) {
		//		int error_code = WSAGetLastError();
		//		printf("Error while sending packet [%d]", error_code);
		//	}
		//}

		if (pKeysBuffer['F'] & 0xF0)
		{
//			my_packet->type = CS_ATTACK;

			int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
			if (ret) {
				int error_code = WSAGetLastError();
				printf("Error while sending packet [%d]", error_code);
			}
		}

		// 2번키를 누른다면 다음 스테이지로?
		if (pKeysBuffer['2'] & 0xF0) {
			my_packet->type = CS_NEXTSTAGE;

			int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
			if (ret) {
				int error_code = WSAGetLastError();
				printf("Error while sending packet [%d]", error_code);
			}
		}
		if (pKeysBuffer['3'] & 0xF0) {
			my_packet->type = CS_THREESTAGE;

			int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
			if (ret) {
				int error_code = WSAGetLastError();
				printf("Error while sending packet [%d]", error_code);
			}
		}
		if (pKeysBuffer['4'] & 0xF0) {
			my_packet->type = CS_ONESTAGE;

			int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
			if (ret) {
				int error_code = WSAGetLastError();
				printf("Error while sending packet [%d]", error_code);
			}
		}

		//if (pKeysBuffer['1'] & 0xF0) {
		//	InitServer();
		//	m_pCurrScene = m_pTownScene;
		//	m_pPlayer = m_TownPlayer;
		//m_pScene->m_pPlayer = m_pPlayer;
		//m_pPlayer->SetPosition(XMFLOAT3(512.0f, m_pScene->GetTerrain()->GetHeight(512.f, 512.f), 512.0f));
		//for (int i = 0; i < MAX_USER; ++i)
		//	m_ppOtherPlayers[i]->SetPlayerUpdatedContext(m_pScene->GetTerrain());
		//m_pCamera = m_pPlayer->GetCamera();
		//}

		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		if (GetCapture() == m_hWnd)
		{
			SetCursor(NULL);
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}

		if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta)
			{
				if (pKeysBuffer[VK_RBUTTON] & 0xF0)
					m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
				else
					m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			}
			if (dwDirection) m_pPlayer->Move(dwDirection, 50.0f * m_GameTimer.GetTimeElapsed(), true);

			cs_packet_sight  *my_packet = reinterpret_cast<cs_packet_sight*>(send_buffer);
			my_packet->size = sizeof(my_packet);
			send_wsabuf.len = sizeof(my_packet);
			DWORD iobyte;
			float tempx, tempz;
			tempx = (roundf(m_pPlayer->GetLookVector().x * 100) / 100) * 100;
			tempz = (roundf(m_pPlayer->GetLookVector().z * 100) / 100) * 100;

			my_packet->type = CS_SIGHT;
			my_packet->dir_x = tempx;
			my_packet->dir_z = tempz;

			int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
			if (ret) {
				int error_code = WSAGetLastError();
				printf("Error while sending packet [%d]", error_code);
			}
		}
	}
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_ppOtherPlayers[i]->Getvisible() && (g_myid != i))
			m_ppOtherPlayers[i]->Update(m_GameTimer.GetTimeElapsed());
	}

	if (m_fireball)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		xmf3Shift = Vector3::Add(xmf3Shift, m_pPlayer->GetLook(), m_GameTimer.GetTimeElapsed()*(-50));
		m_fireball->Move(xmf3Shift, TRUE);
		m_fireball->Update(m_GameTimer.GetTimeElapsed());
	}

	//for (int i = 0; i < MAX_USER; ++i)
	//{
	//	if (m_ppMonsters[i]->Getvisible() && (g_myid != i))
	//		m_ppMonsters[i]->Update(m_GameTimer.GetTimeElapsed());
	//}
}

void CGameFramework::AnimateObjects()
{
	float fTimeElapsed = m_GameTimer.GetTimeElapsed(); 
	static bool isRotate = true;
	if (m_pPlayer)
	{
		m_pPlayer->Animate(fTimeElapsed);
		m_pPlayer->GetMesh()->FBXFrameAdvance(fTimeElapsed);

		m_pShadow->Animate(fTimeElapsed);
		m_pShadow->GetMesh()->FBXFrameAdvance(fTimeElapsed);
		//XMFLOAT3 temp_pos = { m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y, m_pPlayer->GetPosition().z};
		m_pShadow->CGameObject::SetPosition(m_pPlayer->GetPosition().x , m_pPlayer->GetPosition().y  - 20.f, m_pPlayer->GetPosition().z );
		
		m_pShadow->Rotate1(-250.f, m_pPlayer->GetYaw(), m_pPlayer->GetRoll()-20.f);
		

		
		if (m_pPlayer->isFireMove)
		{
		}
		else if (m_pPlayer->isFire)
		{
			XMFLOAT3 tmp = m_pPlayer->GetPosition();
			m_pCBall->SetPosition(tmp.x,tmp.y+100.f,tmp.z);
			m_pCBall->SetVisible(true);
			m_pPlayer->isFire = false;
		}
	}
	if(m_pCBall->Getvisible())
		m_pCBall->Animate(fTimeElapsed);

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_ppOtherPlayers[i]->Getvisible() && (g_myid != i))
		{
			m_ppOtherPlayers[i]->Animate(fTimeElapsed);
			m_ppOtherPlayers[i]->GetMesh()->FBXFrameAdvance(fTimeElapsed);
		}
	}
	for (int i = 0; i < MAX_OBJECT_INDEX - MAX_USER; ++i)
	{
		if (m_ppMonsters[i]->Getvisible()) 
		{
			m_ppMonsters[i]->Animate(fTimeElapsed);
			m_ppMonsters[i]->GetMesh()->FBXFrameAdvance(fTimeElapsed);

			if (m_ppMonstersShadow)
			{
				m_ppMonstersShadow[i]->Animate(fTimeElapsed);
				m_ppMonstersShadow[i]->GetMesh()->FBXFrameAdvance(fTimeElapsed);
				if (m_pCurrScene == m_pStageOneScene)
					m_ppMonstersShadow[i]->CGameObject::SetPosition(m_ppMonsters[i]->GetPosition().x, m_ppMonsters[i]->GetPosition().y - 200.f, m_ppMonsters[i]->GetPosition().z);
				else
					m_ppMonstersShadow[i]->CGameObject::SetPosition(m_ppMonsters[i]->GetPosition().x, m_ppMonsters[i]->GetPosition().y - 70.f, m_ppMonsters[i]->GetPosition().z);
				//m_ppMonstersShadow[i]->Rotate(0, m_ppMonsters[i]->GetYaw(), m_ppMonsters[i]->GetRoll());
				//m_ppMonstersShadow[i]->RotateWorldM(-250.f, m_pPlayer->GetYaw(), m_pPlayer->GetRoll() - 35.f);
				//m_ppMonstersShadow[i]->GetLook();
				//XMFLOAT3 currlookvector = m_ppMonsters[i]->GetLook();
				//XMFLOAT3 newlookvector = m_ppMonstersShadow[i]->GetLook();
				//float degree = GetAngleBetweenTwoVectors2(newlookvector.x, newlookvector.z, currlookvector.x, currlookvector.z);
				//m_ppMonstersShadow[i]->Rotate(0.f, degree, 0.f);
			}
		}
	}
					
	//for (int i = 0; i < m_pPlayer->m_vBullets.size(); ++i)
	//	m_pPlayer->m_vBullets[i]->Animate(fTimeElapsed);
	if (m_pCurrScene){
		m_pCurrScene->AnimateObjects(fTimeElapsed, m_pCamera);
	}
	
	//if (m_pCurrScene != m_pTitleScene)
		//m_pEffectMgr->Animate(fTimeElapsed);

}

void CGameFramework::WaitForGpuComplete()
{
	const UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	//m_nSwapChainBufferIndex = (m_nSwapChainBufferIndex + 1) % m_nSwapChainBuffers;

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

bool CGameFramework::SendLogInPack()
{
	DWORD iobyte;
	cs_packet_signin *my_packet = reinterpret_cast<cs_packet_signin*>(send_buffer);
	ZeroMemory(my_packet->id, MAX_STR_SIZE);
	ZeroMemory(my_packet->password, MAX_STR_SIZE);

	my_packet->size = (sizeof(BYTE) + MAX_STR_SIZE) * 2;;
	send_wsabuf.len = (sizeof(BYTE) + MAX_STR_SIZE) * 2;;
	my_packet->type = CS_SIGNIN;
	strcpy_s(my_packet->id, MAX_STR_SIZE, g_id);
	strcpy_s(my_packet->password, MAX_STR_SIZE, g_password);

	int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
	if (ret) { 
		int error_code = WSAGetLastError();
		printf("Error while sending packet [%d]", error_code);
	}
	return true;
}


/*==============================================================*/
//#define _WITH_PLAYER_TOP
void CGameFramework::FrameAdvance()
{    
	m_GameTimer.Tick(0.0f);
	
	ProcessInput();

    AnimateObjects();

	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);

	float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor/*Colors::Azure*/, 0, NULL);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);

	m_pCurrScene->Render(m_pd3dCommandList, m_pCamera);

#ifdef _WITH_PLAYER_TOP
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif

	//==========//
	if (m_pPlayer != NULL) {
		m_pPlayer->UpdateTransform(NULL);
		m_pPlayer->Render(m_pd3dCommandList, m_pCamera);
	}
	if (m_pShadow != NULL) {

		
		if(m_pCurrScene== m_pStageOneScene || m_pCurrScene == m_pStageTwoScene)
		{
			m_pShadow->UpdateTransform(NULL);
			m_pShadow->Render(m_pd3dCommandList, m_pCamera);

		}
	}

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_ppOtherPlayers[i]->Getvisible() && (g_myid != i))
		{
			m_ppOtherPlayers[i]->UpdateTransform(NULL);
			m_ppOtherPlayers[i]->Render(m_pd3dCommandList, m_pCamera);
		}
	}

	for (int i = 0; i < MAX_OBJECT_INDEX - MAX_USER; ++i)
	{

		
		{
			if (m_ppMonsters[i]->Getvisible())
			{

					m_ppMonsters[i]->UpdateTransform(NULL);
					m_ppMonsters[i]->Render(m_pd3dCommandList, m_pCamera);
				if (m_pCurrScene == m_pStageOneScene || m_pCurrScene == m_pStageTwoScene)
				{
					m_ppMonstersShadow[i]->UpdateTransform(NULL);
					m_ppMonstersShadow[i]->Render(m_pd3dCommandList, m_pCamera);
				}
			}
		}
	}

	if (m_ppWalls)
	{

		if (m_pCurrScene == m_pStageOneScene || m_pCurrScene == m_pStageTwoScene)
		{
			for (int i = 0; i < m_nWall; ++i) {
				m_ppWalls[i]->Render(m_pd3dCommandList);
			}
		}

	}

	if (m_pWaterHeight != NULL)
	{
		
		if (m_pCurrScene == m_pStageThreeScene)
		{
			m_pWaterHeight->UpdateShaderVariables(m_pd3dCommandList);
			m_pWaterHeight->Render(m_pd3dCommandList, m_pCamera);
		}
	}

	if (m_pCBall->Getvisible())
		m_pCBall->Render(m_pd3dCommandList);
	if (m_pCurrScene != m_pTitleScene)
		//m_pEffectMgr->Render(m_pd3dCommandList);
	if (m_pCurrScene != m_pTitleScene && m_pCurrScene != m_pSelectScene)
		m_pUIShader->Render(m_pd3dCommandList, m_pCamera);
	//==========//

	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	hResult = m_pd3dCommandList->Close();
	
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();

#ifdef _WITH_PRESENT_PARAMETERS
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;
	m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);
#else
#ifdef _WITH_SYNCH_SWAPCHAIN
	m_pdxgiSwapChain->Present(1, 0);
#else
	m_pdxgiSwapChain->Present(0, 0);
#endif
#endif

   // m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	MoveToNextFrame();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}


////////// Game Server


bool CGameFramework::InitServer(const char* ip)
{
	WSAStartup(MAKEWORD(2, 2), &wsa);

	g_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr(ip);

	int Result = WSAConnect(g_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);

	WSAAsyncSelect(g_mysocket, m_hWnd, WM_SOCKET, FD_CLOSE | FD_READ);
	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUF_SIZE;

	return Result;
}

void CGameFramework::ShutDownServer()
{
	WSACleanup();
}

void CGameFramework::ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;

	int ret = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (ret) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != iobyte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (iobyte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			iobyte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}

void CGameFramework::ProcessPacket(char * ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_PUT_PLAYER:
	{
		sc_packet_put_player *my_packet = reinterpret_cast<sc_packet_put_player *>(ptr);
		int id = my_packet->id;
		if (first_time) {
			first_time = false;
			g_myid = id;
		}
		if (id == g_myid) {
			XMFLOAT3 temp = { (float)(my_packet->x)*0.1f, 0.0, (float)my_packet->y*0.1f };
			
			temp.y = m_pCurrScene->GetTerrain()->GetHeight(temp.x, temp.z);
			m_pPlayer->SetPosition(temp);
			m_pPlayer->SetVisible(true);
			m_pPlayer->SetScene(STAGE_ONE);
			m_pPlayer->hp = my_packet->hp;
			m_pUIShader->updateMYHP(m_pPlayer->hp);
		}
		else if (id < MAX_USER)
		{
			XMFLOAT3 temp = { (float)my_packet->x*0.1f, 0.0, (float)my_packet->y*0.1f };
			XMFLOAT3 newlookvector = { (float)my_packet->sight_x*0.01f, 0.0, (float)my_packet->sight_z*0.01f };
			XMFLOAT3 currlookvector = m_ppOtherPlayers[id]->GetLookVector();
			temp.y = m_pCurrScene->GetTerrain()->GetHeight(temp.x, temp.z);

			float degree = GetAngleBetweenTwoVectors2(newlookvector.x, newlookvector.z, currlookvector.x, currlookvector.z);

			m_ppOtherPlayers[id]->Rotate(0.0, degree, 0.0);
			m_ppOtherPlayers[id]->SetVisible(true);
			m_ppOtherPlayers[id]->SetPosition(temp);
			m_ppOtherPlayers[id]->hp = my_packet->hp;
		}
		else
		{
			XMFLOAT3 temp = { (float)my_packet->x*0.1f, 0.0, (float)my_packet->y*0.1f };
			//XMFLOAT3 newlookvector = { (float)my_packet->sight_x*0.01f, 0.0, (float)my_packet->sight_z*0.01f };
			//XMFLOAT3 currlookvector = m_ppMonsters[id - MAX_USER]->GetLook();
			temp.y = m_pCurrScene->GetTerrain()->GetHeight(temp.x, temp.z);
			
			//float degree = GetAngleBetweenTwoVectors2(newlookvector.x, newlookvector.z, currlookvector.x, currlookvector.z);
			
			m_ppMonsters[id - MAX_USER]->Rotate(0.0, 180.0, 0.0);
			m_ppMonsters[id - MAX_USER]->SetVisible(true);
			m_ppMonsters[id - MAX_USER]->SetPosition(temp);
			m_ppMonsters[id - MAX_USER]->hp = my_packet->hp;
			m_pUIShader->updateBOSSHP(m_ppMonsters[id - MAX_USER]->hp);
		}
		break;
	}
	case SC_POS:
	{
		sc_packet_pos *my_packet = reinterpret_cast<sc_packet_pos *>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			XMFLOAT3 temp = { (float)my_packet->x*0.1f, 0.0,(float)my_packet->y*0.1f };
			temp.y = m_pCurrScene->GetTerrain()->GetHeight(temp.x, temp.z);
			m_pPlayer->SetPosition(temp);
			soundMgr->pChannel[1]->isPlaying(&soundMgr->IsEffectPlaying);
			if (!soundMgr->IsEffectPlaying)
				soundMgr->pSystem->playSound(FMOD_CHANNEL_FREE, soundMgr->pSound[1], false, &soundMgr->pChannel[1]);
		}
		else if (other_id < MAX_USER)
		{
			XMFLOAT3 temp = { (float)my_packet->x*0.1f, 0.0, (float)my_packet->y*0.1f };
			XMFLOAT3 newlookvector = { (float)my_packet->sight_x*0.01f, 0.0, (float)my_packet->sight_z*0.01f };
			XMFLOAT3 currlookvector = m_ppOtherPlayers[other_id]->GetLookVector();
			temp.y = m_pCurrScene->GetTerrain()->GetHeight(temp.x, temp.z);

			float degree = GetAngleBetweenTwoVectors2(newlookvector.x, newlookvector.z, currlookvector.x, currlookvector.z);

			m_ppOtherPlayers[other_id]->Rotate(0.0, degree, 0.0);
			m_ppOtherPlayers[other_id]->SetPosition(temp);
		}
		else
		{
			XMFLOAT3 temp = { (float)my_packet->x*0.1f, 0.0, (float)my_packet->y*0.1f };
			XMFLOAT3 newlookvector = { (float)my_packet->sight_x*0.01f, 0.0, (float)my_packet->sight_z*0.01f };
			XMFLOAT3 currlookvector = m_ppMonsters[other_id - MAX_USER]->GetLook();
			float degree = GetAngleBetweenTwoVectors2(newlookvector.x, newlookvector.z, currlookvector.x, currlookvector.z);
			temp.y = m_pCurrScene->GetTerrain()->GetHeight(temp.x, temp.z);
			if (m_pCurrScene == m_pStageOneScene)
				temp.y += 18;
			
			if ((other_id - MAX_USER) == 0) {
				m_ppMonsters[other_id - MAX_USER]->Rotate(0.0, degree, 0.0);
				if (m_pCurrScene == m_pStageOneScene)
					m_ppMonstersShadow[other_id - MAX_USER]->Rotate(0.0, -degree, 0.0f);
				else
					m_ppMonstersShadow[other_id - MAX_USER]->Rotate(0.0, degree, 0.0f);
				temp.y += 80;
				m_ppMonsters[other_id - MAX_USER]->SetPosition(temp);
			}
			else {
				m_ppMonsters[other_id - MAX_USER]->Rotate(0.0, degree - 180, 0.0);
				if (m_pCurrScene == m_pStageOneScene)
					m_ppMonstersShadow[other_id - MAX_USER]->Rotate(0.0, -degree - 180, 0.f);
				else
					m_ppMonstersShadow[other_id - MAX_USER]->Rotate(0.0, degree + 180, 0.0f);
				m_ppMonsters[other_id - MAX_USER]->SetPosition(temp);
			}
			m_ppMonsters[other_id - MAX_USER]->SetVisible(true);
		}
		break;
	}
	case SC_REMOVE_PLAYER:
	{
		sc_packet_remove_player *my_packet = reinterpret_cast<sc_packet_remove_player *>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid)
		{
			m_pPlayer->SetVisible(false);
		}
		else if (other_id < MAX_USER)
			m_ppOtherPlayers[other_id]->SetVisible(false);
		else
			m_ppMonsters[other_id - MAX_USER]->SetVisible(false);
		break;
	}
	case SC_CHAT:
	{
		sc_packet_chat *my_packet = reinterpret_cast<sc_packet_chat *>(ptr);
		my_packet->message;
		break;
	}
	case SC_LOGIN:
	{
		cs_packet_login *my_packet = reinterpret_cast<cs_packet_login *>(ptr);
		if (my_packet->success)
			m_LoginSuccess = true;
		else
			closesocket(g_mysocket);

		break;
	}
	case SC_SCENE:
	{
		sc_packet_scene_player * my_packet = reinterpret_cast<sc_packet_scene_player *>(ptr);
		m_pPlayer->SetPlayerUpdatedContext(m_pCurrScene->GetTerrain());
		m_pPlayer->SetCameraUpdatedContext(m_pCurrScene->GetTerrain());

		if (my_packet->scene == STAGE_ONE)
		{
			m_pPlayer->SetScene(STAGE_ONE);
			
			m_pCurrScene = m_pStageOneScene;
			m_pPlayer = m_pStageOnePlayer;
			m_pCurrScene->m_pPlayer = m_pPlayer;
			m_pPlayer->SetPosition(XMFLOAT3(512.0f, m_pCurrScene->GetTerrain()->GetHeight(512.f, 900.f), 900.0f));
		
			for (int i = 0; i < MAX_USER; ++i)
				m_ppOtherPlayers[i]->SetPlayerUpdatedContext(m_pCurrScene->GetTerrain());
			m_ppMonsters[0]->SetVisible(true);
			m_pCamera = m_pPlayer->GetCamera();

			//m_pEffectMgr->SceneOneSet();
		}

		if (my_packet->scene == STAGE_TWO)
		{
			m_pPlayer->SetScene(STAGE_TWO);
			m_pCurrScene = m_pStageTwoScene;
			m_pPlayer = m_pStageOnePlayer;
			m_pCurrScene->m_pPlayer = m_pPlayer;
			m_pPlayer->SetPosition(XMFLOAT3(512.0f, m_pCurrScene->GetTerrain()->GetHeight(512.f, 512.f), 512.f));
			
			for (int i = 0; i < MAX_USER; ++i)
				m_ppOtherPlayers[i]->SetPlayerUpdatedContext(m_pCurrScene->GetTerrain());
			m_ppMonsters[0]->SetVisible(false);
			m_ppMonsters[1]->SetVisible(true);
			m_pCamera = m_pPlayer->GetCamera();

			//m_pEffectMgr->SceneTwoSet();
		}

		if (my_packet->scene == STAGE_THREE)
		{
			m_pPlayer->SetScene(STAGE_THREE);
			m_pCurrScene = m_pStageThreeScene;
			m_pPlayer = m_pStageOnePlayer;
			m_pCurrScene->m_pPlayer = m_pPlayer;
			m_pPlayer->SetPosition(XMFLOAT3(512.0f, m_pCurrScene->GetTerrain()->GetHeight(512.f, 512.f), 512.f));

			for (int i = 0; i < MAX_USER; ++i)
				m_ppOtherPlayers[i]->SetPlayerUpdatedContext(m_pCurrScene->GetTerrain());
			m_ppMonsters[1]->SetVisible(false);
			m_ppMonsters[2]->SetVisible(true);
			m_pCamera = m_pPlayer->GetCamera();

			//m_pEffectMgr->SceneThreeSet();
		}
		break;
	}
	case SC_ANIM:
	{
		cs_packet_npc_anim *my_packet = reinterpret_cast<cs_packet_npc_anim *>(ptr);
		 
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			m_pPlayer->GetMesh()->SetAnimation(my_packet->anim_num);
			//m_pPlayer->hp = my_packet->hp;
		}
		else if (other_id < MAX_USER)
		{
			m_ppOtherPlayers[other_id]->GetMesh()->SetAnimation(my_packet->anim_num);
		}
		else
		{
			m_ppMonsters[other_id - MAX_USER]->GetMesh()->SetAnimation(my_packet->anim_num);
			m_ppMonstersShadow[other_id - MAX_USER]->GetMesh()->SetAnimation(my_packet->anim_num);
		}
		break;
	}

	case SC_CONDITION:
	{
		cs_packet_npc_condition *my_packet = reinterpret_cast<cs_packet_npc_condition *>(ptr);

		int other_id = my_packet->id;
		if (other_id == g_myid) {
			m_pPlayer->hp = my_packet->hp;
			m_pUIShader->updateMYHP(m_pPlayer->hp);
			soundMgr->pChannel[1]->isPlaying(&soundMgr->IsEffectPlaying);
			if (!soundMgr->IsEffectPlaying)
				soundMgr->pSystem->playSound(FMOD_CHANNEL_FREE, soundMgr->pSound[2], false, &soundMgr->pChannel[1]);
		}
		else if (other_id < MAX_USER)
		{
			m_ppOtherPlayers[other_id]->hp = my_packet->hp;
		}
		else
		{
			m_ppMonsters[other_id - MAX_USER]->hp = my_packet->hp;
			m_pUIShader->updateBOSSHP(m_ppMonsters[other_id - MAX_USER]->hp);

			soundMgr->pChannel[1]->isPlaying(&soundMgr->IsEffectPlaying);
			if (!soundMgr->IsEffectPlaying)
				soundMgr->pSystem->playSound(FMOD_CHANNEL_FREE, soundMgr->pSound[3], false, &soundMgr->pChannel[1]);
			
		}
		break;

	}
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}

float CGameFramework::GetAngleBetweenTwoVectors2(float dVec1X, float dVec1Y, float dVec2X, float dVec2Y)
{
	float dAngle1 = RAD2DEG(atan2(dVec1X, dVec1Y));

	float dAngle2 = RAD2DEG(atan2(dVec2X, dVec2Y));

	float dDiffAngles = dAngle1 - dAngle2;


	if (dDiffAngles < 0)

		dDiffAngles = 360 + dDiffAngles;

	return dDiffAngles;
}

void CGameFramework::Server(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam)) {
		closesocket((SOCKET)wParam);
		exit(1);
	}
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
		ReadPacket((SOCKET)wParam);
		break;
	case FD_CLOSE:
		closesocket((SOCKET)wParam);
		exit(1);
		break;
	}
}

