#include "SceneManager.h"

// シーンのインクルード
#include "BaseScene/BaseScene.h"
#include "GameScene/GameScene.h"
#include "TitleScene/TitleScene.h"

// フレームワークのインクルード
#include"../../Framework/RegisterObject/RegisterObject.h"
#include"../main.h"
#include"../../Framework/Json/Json.h"

// ゲームオブジェクトのインクルード
#include"../GameObject/Camera/TPSCamera/TPSCamera.h"
#include"../GameObject/Camera/FPSCamera/FPSCamera.h"
#include"../GameObject/SkySphere/SkySphere.h"
#include"../GameObject/HUD/NormalUI/NormalUI.h"
#include"../GameObject/Map/Map.h"
#include"../GameObject/Character/Player/Player.h"
#include"../GameObject/Weapon/Katana/Katana.h"
#include"../GameObject/Camera/PlayerCamera/PlayerCamera.h"
#include"../GameObject/Character/Enemy/Enemy.h"

void SceneManager::Init()
{
	Register();
	// 開始シーンに切り替え
	ChangeScene(m_currentSceneType);
}

void SceneManager::Register() const
{
	RegisterObject::GetInstance().Register<TPSCamera>();
	RegisterObject::GetInstance().Register<FPSCamera>();
	RegisterObject::GetInstance().Register<PlayerCamera>();
	RegisterObject::GetInstance().Register<SkySphere>();
	RegisterObject::GetInstance().Register<NormalUI>();
	RegisterObject::GetInstance().Register<Map>();
	RegisterObject::GetInstance().Register<Player>();
	RegisterObject::GetInstance().Register<Katana>();
	RegisterObject::GetInstance().Register<Enemy>();
}

void SceneManager::PreUpdate()
{
	// シーン切替
	if (m_currentSceneType != m_nextSceneType)
	{
		ChangeScene(m_nextSceneType);
	}

	m_currentScene->PreUpdate();
}

void SceneManager::Update()
{
	m_currentScene->Update();
}

void SceneManager::PostUpdate()
{
	m_currentScene->PostUpdate();
}

void SceneManager::PreDraw()
{
	m_currentScene->PreDraw();
}

void SceneManager::Draw()
{
	m_currentScene->Draw();
}

void SceneManager::DrawSprite()
{
	m_currentScene->DrawSprite();
}

void SceneManager::DrawDebug()
{
	m_currentScene->DrawDebug();
}

std::list<std::shared_ptr<KdGameObject>>& SceneManager::GetObjList()
{
	return m_currentScene->GetObjList();
}

std::list<std::shared_ptr<KdGameObject>>& SceneManager::GetCameraList()
{
	return m_currentScene->GetCameraObjList();
}

std::list<std::shared_ptr<KdGameObject>>& SceneManager::GetMapList()
{
	return m_currentScene->GetMapObjectList();
}

void SceneManager::AddObject(const std::shared_ptr<KdGameObject>& _obj)
{
	m_currentScene->AddObject(_obj);
}

void SceneManager::ChangeScene(SceneType _sceneType)
{
	// 次のシーンを作成し、現在のシーンにする
	switch (_sceneType)
	{
	case SceneType::Title:
		m_currentScene = std::make_shared<TitleScene>();
		break;
	case SceneType::Game:
		m_currentScene = std::make_shared<GameScene>();
		break;
	}

	// 現在のシーン情報を更新
	m_currentSceneType = _sceneType;

	if (m_currentScene)
	{
		// ここでJsonからオブジェクトを追加
		JSON_MANAGER.JsonToObj();
	}
}
