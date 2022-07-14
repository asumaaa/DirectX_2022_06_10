#pragma once
#include "Windows.h"
#include "cstdint"

class WinApp
{
public:
	//�E�B���h�E�T�C�Y
	static const int window_width = 1280;	//����
	static const int window_height = 720;	//����

	//���O
	static const wchar_t  windowClassName[];

	//�E�B���h�E�̃T�C�Y��ς����邩
	enum class SizeChangeMode {
		None,        //!< �T�C�Y�ύX�s��
		Normal,      //!< ���R�ύX
		FixedAspect, //!< �A�X�y�N�g����
	};

public:
	WinApp* GetInstance();

	//�E�B���h�E�v���V�[�W��
	LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	//�E�B���h�E����
	void CreateGameWindow(
		const char* title = "DirectXGame", UINT windowStyle = WS_OVERLAPPEDWINDOW,
		int32_t clientWidth = window_width, int32_t clientHeight = window_height);

	//�E�B���h�E�j��
	void TerminateGameWindow();

	//�I�����ǂ���
	bool ProcessMessage();

	//�Q�b�^�[
	HWND GetHwnd() const { return hwnd_; }
	HINSTANCE GetHInstance() const { return wndClass_.hInstance; }

	//�t���X�N���[���ݒ�
	void SetFullscreen(bool fullscreen);
	//�t���X�N���[�����ǂ���
	bool IsFullscreen() const;
	//�T�C�Y�ύX���[�h�̐ݒ�
	void SetSizeChangeMode(SizeChangeMode sizeChangeMode);
	//�T�C�Y�ύX���[�h���ǂ���
	SizeChangeMode GetSizeChangeMode() const;

private: // �����o�֐�
	WinApp() = default;
	~WinApp() = default;
	WinApp(const WinApp&) = delete;
	const WinApp& operator=(const WinApp&) = delete;

private: // �����o�ϐ�
// Window�֘A
	HWND hwnd_ = nullptr;   // �E�B���h�E�n���h��
	WNDCLASSEX wndClass_{}; // �E�B���h�E�N���X
	UINT windowStyle_;
	bool isFullscreen_ = false;
	RECT windowRect_;
	SizeChangeMode sizeChangeMode_ = SizeChangeMode::Normal;
	float aspectRatio_;
};

