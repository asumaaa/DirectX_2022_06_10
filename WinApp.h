#pragma once
#include "Windows.h"
#include <cstdint>

class WinApp
{
public:
	//�����o�ϐ�	
	//�E�B���h�E�T�C�Y
	static const int window_width = 1280;	//����
	static const int window_height = 720;	//����
	// �E�B���h�E�N���X��
	static const wchar_t WindowClassName[];
	//�E�B���h�E�v���V�[�W��
	LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	static WinApp* Initialize();
	//�E�B���h�E�𐶐�
	void CreateGameWindow(
		const char* title = "DirectXGame", UINT windowStyle = WS_OVERLAPPEDWINDOW,
		int32_t clientWidth = window_width, int32_t clientHeight = window_height);
};

