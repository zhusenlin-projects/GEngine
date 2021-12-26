#include "GWindowApplication.h"
using namespace GEngine::GApp;

//Ӧ�ó������
//**********************************************************************************************************************************************

// ������� [Windows Application]

int WINAPI WinMain(
	HINSTANCE hInst,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	GEngine::GGraphic::GMain::Initialize();
	if (GApplication::GetApplication() != nullptr)
	{
		if (!GApplication::GetApplication()->Main(hInst, hPrevInstance, lpCmdLine, nCmdShow))
			GASSERT(0);
	}
	GEngine::GGraphic::GMain::Terminate();
	return 1;
}

#ifdef WINDOWS
//[Windows Application]  Windows��Ϣ��
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (GWindowApplication::GetApplication())
		return GWindowApplication::GetApplication()->MsgProc(hwnd, msg, wParam, lParam);
	else
		return -1;
}
#endif



bool GEngine::GApp::GWindowApplication::Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	m_hInstance = hInstance;
	m_hPreInstance = hPrevInstance;
	m_pCommand = GNEW GCommand(pCmdLine);
	if (m_pCommand == nullptr)
		return false;
	m_cmdShow = nCmdShow;

	m_pGlobalTimer = &GTimer::GetTimer();
	GASSERT(m_pGlobalTimer != nullptr);

	//*�����ʼ������*-------------------------
	if (!PreInitial())       return false;
	if (!InitialWindow())    return false;
	if (!CreateGEngine())    return false;
	if (!OnInitial())        return false;

	//*��Ϸѭ������*---------------------------
	MSG  msg = { 0 };
	bool bError = false;
	while (msg.message!=WM_QUIT)
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (m_bIsActive)//���ڴ��ڽ���
		{
			if (!Run())
			{
				m_bIsRunning = false;
				bError = true;
			}
			else
			{
				//����������Ҫ���µ�����
			}
		}
	}

	//*�������ٹ���*---------------------------
	if (!OnTerminal())       bError = true;
	if (!ReleaseGEngine())   bError = true;
	if (!TerminalWindow())   bError = true;

	GSAFE_DELETE(m_pCommand);
	return !bError;
}

LRESULT GEngine::GApp::GWindowApplication::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:                    //Ӧ�ó��򴴽�һ������
		return 0;
	case WM_MOVE:                      //�ƶ�һ������
		return 0;
	case WM_SIZE:                      //�ı�һ�����ڵĴ�С
		return 0;
	case WM_ACTIVATE:                  //һ�����ڱ������ʧȥ����
		return 0;
	case WM_SETFOCUS:                  //���ڻ�ý���
		return 0;
	case WM_KILLFOCUS:                 //����ʧȥ����
		return 0;
	case WM_ENABLE:                    //�ı�Enable״̬
		return 0;


	//*Input System Msg Receive*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	case WM_KEYDOWN:      ms_pApplication->GEngineInputProc(GInputDevices::ID_Keyboard, (KeyCode)wParam, GMouseButton::None, GInputAction::Down, 0, 0, 0); return 0;
	case WM_KEYUP:        ms_pApplication->GEngineInputProc(GInputDevices::ID_Keyboard, (KeyCode)wParam, GMouseButton::None, GInputAction::Up, 0, 0, 0); return 0;
	case WM_MOUSEMOVE:    ms_pApplication->GEngineInputProc(GInputDevices::ID_Mouse, KeyCode::NONE, GMouseButton::None, GInputAction::Up, LOWORD(lParam), HIWORD(lParam), 0); return 0;
	case WM_LBUTTONDOWN:  ms_pApplication->GEngineInputProc(GInputDevices::ID_Mouse, KeyCode::NONE, GMouseButton::LButton, GInputAction::Down, LOWORD(lParam), HIWORD(lParam), 0); return 0;
	case WM_LBUTTONUP:    ms_pApplication->GEngineInputProc(GInputDevices::ID_Mouse, KeyCode::NONE, GMouseButton::LButton, GInputAction::Up, LOWORD(lParam), HIWORD(lParam), 0); return 0;
	case WM_LBUTTONDBLCLK:ms_pApplication->GEngineInputProc(GInputDevices::ID_Mouse, KeyCode::NONE, GMouseButton::LButton, GInputAction::DBClick, LOWORD(lParam), HIWORD(lParam), 0); return 0;
	case WM_MBUTTONDOWN:  ms_pApplication->GEngineInputProc(GInputDevices::ID_Mouse, KeyCode::NONE, GMouseButton::MButton, GInputAction::Down, LOWORD(lParam), HIWORD(lParam), 0); return 0;
	case WM_MBUTTONUP:    ms_pApplication->GEngineInputProc(GInputDevices::ID_Mouse, KeyCode::NONE, GMouseButton::MButton, GInputAction::Up, LOWORD(lParam), HIWORD(lParam), 0); return 0;
	case WM_MBUTTONDBLCLK:ms_pApplication->GEngineInputProc(GInputDevices::ID_Mouse, KeyCode::NONE, GMouseButton::MButton, GInputAction::DBClick, LOWORD(lParam), HIWORD(lParam), 0); return 0;
	case WM_RBUTTONDOWN:  ms_pApplication->GEngineInputProc(GInputDevices::ID_Mouse, KeyCode::NONE, GMouseButton::RButton, GInputAction::Down, LOWORD(lParam), HIWORD(lParam), 0); return 0;
	case WM_RBUTTONUP:    ms_pApplication->GEngineInputProc(GInputDevices::ID_Mouse, KeyCode::NONE, GMouseButton::RButton, GInputAction::Up, LOWORD(lParam), HIWORD(lParam), 0); return 0;
	case WM_RBUTTONDBLCLK:ms_pApplication->GEngineInputProc(GInputDevices::ID_Mouse, KeyCode::NONE, GMouseButton::RButton, GInputAction::DBClick, LOWORD(lParam), HIWORD(lParam), 0); return 0;
	case WM_MOUSEWHEEL:   ms_pApplication->GEngineInputProc(GInputDevices::ID_Mouse, KeyCode::NONE, GMouseButton::Wheel, GInputAction::Down, LOWORD(lParam), HIWORD(lParam), HIWORD(wParam)); return 0;
	//*Input System Msg Receive*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------



	case WM_DESTROY:                   //һ�����ڱ�����
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool GEngine::GApp::GWindowApplication::Run()
{
	/*PreUpdate();
	OnDraw();
	PostUpdate();*/
	return true;
}

bool GEngine::GApp::GWindowApplication::InitialWindow()
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MainWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = m_WndClassName.c_str();
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassExW(&wcex))
	{
		MessageBox(NULL,
			L"Call to RegisterClassEx failed!",
			L"Windows Desktop Guided Tour",
			NULL);
		return false;
	}

	m_hwnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		m_WndClassName.c_str(),
		m_ApplicationTitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		m_uiScreenWidth,
		m_uiScreenHeight,
		NULL,
		NULL,
		m_hInstance,
		NULL
	);

	if (!m_hwnd)
	{
		MessageBox(NULL,
			L"Call to CreateWindowEx failed!",
			L"Windows Desktop Guided Tour",
			NULL);
		return false;
	}

	ShowWindow(m_hwnd, m_cmdShow);
	UpdateWindow(m_hwnd);
	return true;
}

bool GEngine::GApp::GWindowApplication::TerminalWindow()
{
	return UnregisterClass(L"BasicWndClassName", m_hInstance);
}

void GEngine::GApp::GWindowApplication::GEngineInputProc(GInputDevices dt, KeyCode key, GMouseButton mb, GInputAction action, int xPos, int yPos, int zDet)
{
	//���ü����¼�����
	if (dt == GInputDevices::ID_Keyboard)
	{
		if (action == GInputAction::Down) 
		{
			OnKeyDown(key);
			return;
		}

		if (action == GInputAction::Up) 
		{
			OnKeyUp(key);
			return;
		}
	}

	else if (dt == GInputDevices::ID_Mouse)
	{
		if (mb == GMouseButton::Wheel)
		{
			OnMouseWheel(xPos, yPos, zDet);
			return;
		}

		if (action == GInputAction::Down) 
		{
			if (mb == GMouseButton::LButton) 
			{
				OnLButtonDown(xPos, yPos);
				return;
			}
			if (mb == GMouseButton::MButton) 
			{
				OnMButtonDown(xPos, yPos);
				return;
			}
			if (mb == GMouseButton::RButton) 
			{
				OnRButtonDown(xPos, yPos);
				return;
			}
		}

		if (action == GInputAction::Up)
		{
			if (mb == GMouseButton::LButton) 
			{
				OnLButtonUp(xPos, yPos);
				return;
			}
			if (mb == GMouseButton::MButton) 
			{
				OnMButtonUp(xPos, yPos);
				return;
			}
			if (mb == GMouseButton::RButton) 
			{
				OnRButtonUp(xPos, yPos);
				return;
			}
		}

		if (action == GInputAction::Up)
		{
			if (mb == GMouseButton::LButton)
			{
				OnLButtonUp(xPos, yPos);
				return;
			}
			if (mb == GMouseButton::MButton) 
			{
				OnMButtonUp(xPos, yPos);
				return;
			}
			if (mb == GMouseButton::RButton) 
			{
				OnRButtonUp(xPos, yPos);
				return;
			}
		}
	}

}