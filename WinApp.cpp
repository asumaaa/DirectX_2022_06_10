#include "WinApp.h"
#include <string>

//WinApp* WinApp::GetInstance()
//{
//    return nullptr;
//}
//
//LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
//{
//    return LRESULT();
//}
//
//void WinApp::CreateGameWindow(const char* title, UINT windowStyle, int32_t clientWidth, int32_t clientHeight)
//{
//}
//
//void WinApp::TerminateGameWindow()
//{
//}
//
//bool WinApp::ProcessMessage()
//{
//    return false;
//}
//
//void WinApp::SetFullscreen(bool fullscreen)
//{
//}
//
//bool WinApp::IsFullscreen() const
//{
//    return false;
//}
//
//void WinApp::SetSizeChangeMode(SizeChangeMode sizeChangeMode)
//{
//}
//
//WinApp::SizeChangeMode WinApp::GetSizeChangeMode() const
//{
//    return SizeChangeMode();
//}

LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//���b�Z�[�W�ɉ����ăQ�[���ŗL�̏������s��
	switch (msg)
	{
		//�E�B���h�E���j�����ꂽ
	case WM_DESTROY:
		//OS�ɑ΂��āA�A�v���̏I����`����
		PostQuitMessage(0);
		return 1;
	}
	//�W���̃��b�Z�[�W�������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

WinApp* WinApp::GetInstance()
{
	static WinApp instance;
	return &instance;
}

void WinApp::CreateWindow_()
{
	// COM������
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;	//�E�B���h�E�v���V�[�W����ݒ�
	w.lpszClassName = L"DirectXGame";		//�E�B���h�E�N���X��
	w.hInstance = GetModuleHandle(nullptr);	//�E�B���h�E�n���h��
	w.hCursor = LoadCursor(NULL, IDC_ARROW);//�J�[�\���w��

	//�E�B���h�E�N���X��OS�ɓo�^����
	RegisterClassEx(&w);
	//�E�B���h�E�T�C�Y��
	RECT wrc = { 0,0,window_width,window_height };
	//�����ŃT�C�Y��␳����
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�E�I�u�W�F�N�g�̐���
	hwnd = CreateWindow(
		w.lpszClassName,		//�N���X��
		L"DirectXGame",			//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,	//�W���I�ȃE�B���h�E�X�^�C��
		CW_USEDEFAULT,			//�\��X���W
		CW_USEDEFAULT,			//�\��Y���W
		wrc.right - wrc.left,	//�E�B���h�E����
		wrc.bottom - wrc.top,	//�E�B���h�E�c��
		nullptr,				//�e�E�B���h�E�n���h��
		nullptr,				//���j���[�n���h��
		w.hInstance,			//�Ăяo���A�v���P�[�V�����n���h��
		nullptr					//�I�v�V����
	);

	//�E�B���h�E��\����Ԃɂ���
	ShowWindow(hwnd, SW_SHOW);
}

void WinApp::deleteWindow()
{
	UnregisterClass(w.lpszClassName, w.hInstance);
}
