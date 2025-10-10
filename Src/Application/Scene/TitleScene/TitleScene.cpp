#include "TitleScene.h"
#include "../SceneManager.h"

#include"../../../Framework/Json/Json.h"
#include"../../main.h"

void TitleScene::Event()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Test
		);
	}
}

void TitleScene::Init()
{
}
