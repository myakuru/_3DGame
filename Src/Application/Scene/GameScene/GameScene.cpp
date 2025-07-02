#include "GameScene.h"
#include"../SceneManager.h"

void GameScene::Event()
{
	if (GetAsyncKeyState('T') & 0x8000)
	{
		SceneManager::GetInstance().SetNextScene
		(
			SceneManager::SceneType::Title
		);
	}
}

void GameScene::Init()
{
}
