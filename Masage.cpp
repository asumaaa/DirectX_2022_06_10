#include "Masage.h"


Masage* Masage::GetInstance()
{
	static Masage instance;
	return &instance;
}

void Masage::Update()
{
	//メッセージがある？
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);	//キー入力メッセージの処理
		DispatchMessage(&msg);	//プロシージャにメッセージを送る
	}
}

bool Masage::ExitGameloop()
{
	//Xボタンで終了メッセ時が来たらゲームループを抜ける 
	if (msg.message == WM_QUIT)
	{
		IDXGIDebug* giDebugInterface = nullptr;

		if (giDebugInterface == nullptr)
		{
			//作成
			typedef HRESULT(__stdcall* fPtr)(const IID&, void**);
			HMODULE hDll = GetModuleHandleW(L"dxgidebug.dll");
			if (hDll != 0)
			{
				fPtr DXGIGetDebugInterface =
					(fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface");

				DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&giDebugInterface);

				//出力
				giDebugInterface->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_DETAIL);
			}
		}
		return 1;
	}
}
