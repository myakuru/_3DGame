#include "SceneManager.h"

// シーンのインクルード
#include "BaseScene/BaseScene.h"
#include "GameScene/GameScene.h"
#include "TitleScene/TitleScene.h"
#include "ResultScene/ResultScene.h"
#include "TestStage/TestStage.h"

// フレームワークのインクルード
#include"../../Framework/RegisterObject/RegisterObject.h"
#include"../main.h"
#include"../../Framework/Json/Json.h"
#include"../GameObject/Utility/Time.h"

// ゲームオブジェクトのインクルード
#include"../GameObject/Camera/TPSCamera/TPSCamera.h"
#include"../GameObject/Camera/FPSCamera/FPSCamera.h"
#include"../GameObject/SkySphere/SkySphere.h"
#include"../GameObject/HUD/NormalUI/NormalUI.h"
#include"../GameObject/HUD/AlpfaMoving/AlpfaMoving.h"
#include"../GameObject/Map/Map.h"
#include"../GameObject/Character/Player/Player.h"
#include"../GameObject/Weapon/Katana/Katana.h"
#include"../GameObject/Camera/PlayerCamera/PlayerCamera.h"
#include"../GameObject/Character/Enemy/Enemy.h"
#include"../GameObject/HUD/ScaleMoving/ScaleMoving.h"
#include"../GameObject/HUD/Scoring/Scoring.h"
#include"../GameObject/HUD/Timer/Timer.h"
#include"../GameObject/HUD/HpBar/HpBar.h"
#include"../GameObject/HUD/Number/NumBer.h"
#include"../GameObject/HUD/NowHp/NowHp.h"
#include"../GameObject/HUD/SpecialMove/SpecialMove.h"
#include"../GameObject/Winner/Winner.h"
#include"../GameObject/HUD/InvestigationComplete/InvestigationComplete.h"
#include"../GameObject/HUD/BlackBanner/BlackBanner.h"
#include"../GameObject/ResultScore/ResultScore.h"
#include"../GameObject/Collition/Collition.h"

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
	RegisterObject::GetInstance().Register<AlpfaMoving>();
	RegisterObject::GetInstance().Register<Map>();
	RegisterObject::GetInstance().Register<Player>();
	RegisterObject::GetInstance().Register<Katana>();
	RegisterObject::GetInstance().Register<Enemy>();
	RegisterObject::GetInstance().Register<ScaleMoving>();
	RegisterObject::GetInstance().Register<Scoring>();
	RegisterObject::GetInstance().Register<Timer>();
	RegisterObject::GetInstance().Register<HpBar>();
	RegisterObject::GetInstance().Register<NumBer>();
	RegisterObject::GetInstance().Register<NowHp>();
	RegisterObject::GetInstance().Register<SpecialMove>();
	RegisterObject::GetInstance().Register<Winner>();
	RegisterObject::GetInstance().Register<InvestigationComplete>();
	RegisterObject::GetInstance().Register<BlackBanner>();
	RegisterObject::GetInstance().Register<ResultScore>();
	RegisterObject::GetInstance().Register<Collision>();
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
	case SceneType::Result:
		m_currentScene = std::make_shared<ResultScene>();
		break;
	case SceneType::Test:
		m_currentScene = std::make_shared<TestScene>();
		break;
	}

	Time::Instance().Reset(); // 時間をリセット

	// 現在のシーン情報を更新
	m_currentSceneType = _sceneType;

	if (m_currentScene)
	{
		// ここでJsonからオブジェクトを追加
		JSON_MANAGER.JsonToObj();
	}
}
