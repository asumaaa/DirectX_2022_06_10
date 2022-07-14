#pragma once
#include "dxgidebug.h"
#include <d3d12.h>

class Masage
{
public:
	static Masage* GetInstance();
	void Update();
	//Xボタンで終了メッセ時が来たらゲームループを抜ける 
	bool ExitGameloop();
public:
	MSG msg{};
};

