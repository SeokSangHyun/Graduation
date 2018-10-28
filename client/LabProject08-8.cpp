// LabProject08-8.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "LabProject08-8.h"
#include "GameFramework.h"

#define MAX_LOADSTRING 100

HINSTANCE						ghAppInstance;
TCHAR							szTitle[MAX_LOADSTRING];
TCHAR							szWindowClass[MAX_LOADSTRING];
CGameFramework					gGameFramework;

char							g_ip[MAX_STR_SIZE];
char g_id[MAX_STR_SIZE];
char g_password[MAX_STR_SIZE];

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
void ConvertMtoC(HWND hDlg);
LRESULT DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

HWND currWnd;
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	::LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	::LoadString(hInstance, IDC_LABPROJECT088, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow)) return(FALSE);

	hAccelTable = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LABPROJECT088));

	bool firstTime = false;
	bool secondTime = false;
	while (1)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		else
		{
			gGameFramework.FrameAdvance();
			if (!firstTime)
			{
				if (secondTime) {
					DialogBox(ghAppInstance, MAKEINTRESOURCE(IDD_DIALOG), currWnd, DlgProc);
					firstTime = true;
					secondTime = false;
				}
				secondTime = true;
			}
		}
	}
	gGameFramework.OnDestroy();

	return((int)msg.wParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LABPROJECT088));
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;//MAKEINTRESOURCE(IDC_LABPROJECT088);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = ::LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return ::RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	ghAppInstance = hInstance;

	RECT rc ={ 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&rc, dwStyle, FALSE);
	HWND hMainWnd = CreateWindow(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

	if (!hMainWnd) return(FALSE);

	gGameFramework.OnCreate(hInstance, hMainWnd);

	::ShowWindow(hMainWnd, nCmdShow);
	::UpdateWindow(hMainWnd);

	currWnd = hMainWnd;
	return(TRUE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SOCKET:
		gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDM_ABOUT:
			::DialogBox(ghAppInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			::DestroyWindow(hWnd);
			break;
		default:
			return(::DefWindowProc(hWnd, message, wParam, lParam));
		}
		break;
	case WM_PAINT:
		hdc = ::BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return(::DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return((INT_PTR)TRUE);
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			::EndDialog(hDlg, LOWORD(wParam));
			return((INT_PTR)TRUE);
		}
		break;
	}
	return((INT_PTR)FALSE);
}


LRESULT DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	char temp[258] = { '\0', };
	SetWindowPos(hDlg, HWND_TOP, 500, 400, 0, 0, SWP_NOSIZE);

	char id[MAX_STR_SIZE] = { '\0', };
	char password[MAX_STR_SIZE] = { '\0', };

		SetTimer(hDlg, 0, 500, NULL);
	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 0:
			if (gGameFramework.GetSucessLogin()) {
				EndDialog(hDlg, LOWORD(wParam));
				return 0;
			}
			else {
				char str_temp[MAX_STR_SIZE];
				strcpy_s(str_temp, MAX_STR_SIZE, "다시 입력...");
				SetDlgItemTextA(hDlg, IDC_EDIT_INFO, (LPCSTR)str_temp);
			}
			break;
		}
		return 0;
	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK)
		{
			ConvertMtoC(hDlg);
			char str_temp[MAX_STR_SIZE];
			EndDialog(hDlg, LOWORD(wParam));
			strcpy_s(gGameFramework.m_ip, MAX_STR_SIZE, g_ip);

			return 0;
			//if (!gGameFramework.InitServer(g_ip)) {
			//	gGameFramework.SendLogInPack();
			//	strcpy_s(str_temp, MAX_STR_SIZE,"로그인 중...");
			//	SetDlgItemTextA(hDlg, IDC_EDIT_INFO, (LPCSTR)str_temp);
			//}
			//else {
			//	strcpy_s(str_temp, MAX_STR_SIZE, "올바른 정보를 입력하세요");
			//	SetDlgItemTextA(hDlg, IDC_EDIT_INFO, (LPCSTR)str_temp);
			//}
			
			break;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			//((CLogIn*)g_manager)->FileSave();
			EndDialog(hDlg, LOWORD(wParam));
			return 0;
		}
		else if (LOWORD(wParam) == IDB_PASSWORD_CHANGE) {


			break;
		}
		else if (LOWORD(wParam) == IDB_SIGNUP) {

			break;
		}
		break;
	}
	return 0;
}

void ConvertMtoC(HWND hDlg) {
	ZeroMemory(g_id, sizeof(char)*MAX_STR_SIZE);
	ZeroMemory(g_password, sizeof(char)*MAX_STR_SIZE);
	ZeroMemory(g_ip, sizeof(char)*MAX_STR_SIZE);

	GetDlgItemText(hDlg, IDC_EDIT_ID, (LPWSTR)g_id, MAX_STR_SIZE);
	GetDlgItemText(hDlg, IDC_EDIT_PASSWORD, (LPWSTR)g_password, MAX_STR_SIZE);
	GetDlgItemText(hDlg, IDC_EDIT_IP, (LPWSTR)g_ip, MAX_STR_SIZE);
	char buffer;
	int id_num = 0, password_num = 0, ip_num = 0;
	for (int i = 0; i < MAX_STR_SIZE; ++i) {
		if (g_id[i] != '\0') {
			buffer = g_id[i];
			g_id[i] = '\0';
			g_id[id_num++] = buffer;
		}
		if (g_password[i] != '\0') {
			buffer = g_password[i];
			g_password[i] = '\0';
			g_password[password_num++] = buffer;
		}
		if (g_ip[i] != '\0') {
			buffer = g_ip[i];
			g_ip[i] = '\0';
			g_ip[ip_num++] = buffer;
		}
	}
}
