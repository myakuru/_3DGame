#include "SceneManager.h"

#include "BaseScene/BaseScene.h"
#include "GameScene/GameScene.h"
#include "TitleScene/TitleScene.h"
#include"../../Framework/RegisterObject/RegisterObject.h"
#include"../main.h"
#include"../../Framework/Json/Json.h"
#include"../GameObject/Camera/TPSCamera/TPSCamera.h"

void SceneManager::Init()
{
	Register();
	// 開始シーンに切り替え
	ChangeScene(m_currentSceneType);
}

void SceneManager::Register() const
{
	RegisterObject::GetInstance().Register<KdGameObject>();
	RegisterObject::GetInstance().Register<TPSCamera>();
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

void SceneManager::ImGuiUpdate()
{
	if (ImGui::Begin("Hierarchy"))
	{
		std::string name = ImSelectClass();
		if (ImGui::Button("AddObject"))
		{
			JSON_MANAGER.AddJsonObject(name);
		}
	}
	ImGui::End();

	if (ImGui::Begin("Save"))
	{
		if (ImGui::Button("Savs"))
		{
			JSON_MANAGER.AllSave();
		}
	}
	ImGui::End();

}

std::string SceneManager::ImSelectClass() const
{
	static std::string name = "KdGameObject";
	if (ImGui::BeginCombo("##Class", name.data())) // c_str()も、data()も同じように使える
	{
		for (auto& it : RegisterObject::GetInstance().GetRegisterObject())
		{
			const char* nowName = it.first.data();
			bool selected = (name == nowName);
			if (ImGui::Selectable(nowName, selected))
			{
				name = nowName;
			}
			if (selected)
			ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	return name;
}

const std::list<std::shared_ptr<KdGameObject>>& SceneManager::GetObjList()
{
	return m_currentScene->GetObjList();
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
