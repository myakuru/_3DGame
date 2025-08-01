﻿#include "ImGuiManager.h"
#include"../Json/Json.h"
#include"../../Application/Scene/SceneManager.h"
#include"../../Application/main.h"
#include"../../Application/GameObject/Camera/TPSCamera/TPSCamera.h"
#include"../../Application/GameObject/Camera/FPSCamera/FPSCamera.h"
#include"../../Application/Scene/BaseScene/BaseScene.h"
#include"../RegisterObject/RegisterObject.h"

void ImGuiManager::ImGuiUpdate()
{
	// ゲームシーンを表示
	ShowGameScene();

	// 上部のメニューバーを表示
	MainMenuBar();

	// ヒエラルキーウィンドウを表示
	Hierarchy();

	// インスペクターウィンドウを表示
	ShowInspector();
}

void ImGuiManager::Hierarchy()
{
	if (ImGui::Begin("Hierarchy"))
	{
		std::string name = ImSelectClass();

		if (ImGui::Button("AddObject"))
		{
			JSON_MANAGER.AddJsonObject(name);
		}

		// ツリー構造のオブジェクトを表示
		TreeNode();

	}
	ImGui::End();
}

void ImGuiManager::MainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		// FileやEditなどのメニュー
		if (ImGui::BeginMenu("File"))
		{
			// メニュー項目
			ImGui::MenuItem("ObjectSave");
			JSON_MANAGER.AllSave();
			ImGui::EndMenu();
		}

		ImGui::SameLine();

		InGuiSceneSelect();

		ImGui::SameLine(1280/2);
		if (U8("実行"))
		{
			ImGuiSelecetCamera();
		}

		// シーンセレクタをメニューバーに配置

		ImGui::EndMainMenuBar();
	}
}

void ImGuiManager::TreeNode()
{
	// ヒエラルキーウィンドウの子ウィンドウを作成
if (ImGui::BeginChild("Objects"))
{
    ImGui::Separator();
    // オブジェクトリストからオブジェクトを取得し、ツリーで表示
    for (auto& obj : SceneManager::Instance().GetObjList())
    {
        const auto& objectName = *obj;

		// 1. 開いているノードだけ開く
		if (obj == m_openObject)
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Always);
		}
		else
		{
			ImGui::SetNextItemOpen(false, ImGuiCond_Always);
		}

		ImGui::PushID(obj.get());
		std::string className = typeid(objectName).name();

		if (className.starts_with("class "))
		{
			className.erase(0, 6); // "class"の部分を削除
		}

		if (IsDroped("GameObjectInstance"))
		{
			std::shared_ptr<KdGameObject>* temp = nullptr;
			GetDragData("GameObjectInstance", temp);
			listSwap(obj, *temp);
			ImGui::ClearDragDrop(); // 自作のドロップ処理のため自力で初期化
		}

		bool TreeNode = ImGui::TreeNode(className.data());
		DragDropSource("GameObjectInstance", &obj);	//ゲームオブジェクトのポインターをドラック出来るようになる

        // ここからツリー形式で詳細情報を表示
        if (TreeNode) //className + "##" + std::to_string((int)obj.get()) <=こういう書き方もあるよ
        {
            ImGui::SameLine(120);

			// クリックされたらこのノードを開く
			if (ImGui::IsItemClicked())
			{
				m_openObject = obj; // 選択したオブジェクトを保存
			}

            // ボタンでオブジェクトを削除できる
            if (ImGui::SmallButton("Delete"))
            {
				obj->SetExpired(true);
            }
            //ツリーの終了処理
            ImGui::TreePop();
        }

        ImGui::Separator();
		ImGui::PopID();
    }

	for (auto& obj : SceneManager::Instance().GetCurrentScene()->GetCameraObjList())
	{
		const auto& objectName = *obj;

		// 1. 開いているノードだけ開く
		if (obj == m_openObject)
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Always);
		}
		else
		{
			ImGui::SetNextItemOpen(false, ImGuiCond_Always);
		}

		ImGui::PushID(obj.get());
		std::string className = typeid(objectName).name();

		if (className.starts_with("class "))
		{
			className.erase(0, 6); // "class"の部分を削除
		}

		if (IsDroped("GameObjectInstance"))
		{
			std::shared_ptr<KdGameObject>* temp = nullptr;
			GetDragData("GameObjectInstance", temp);
			listSwap(obj, *temp);
			ImGui::ClearDragDrop(); // 自作のドロップ処理のため自力で初期化
		}

		bool TreeNode = ImGui::TreeNode(className.data());
		DragDropSource("GameObjectInstance", &obj);	//ゲームオブジェクトのポインターをドラック出来るようになる

		// ここからツリー形式で詳細情報を表示
		if (TreeNode) //className + "##" + std::to_string((int)obj.get()) <=こういう書き方もあるよ
		{
			ImGui::SameLine(270);

			// クリックされたらこのノードを開く
			if (ImGui::IsItemClicked())
			{
				m_openObject = obj; // 選択したオブジェクトを保存
			}

			// ボタンでオブジェクトを削除できる
			if (ImGui::SmallButton("Delete"))
			{
				obj->SetExpired(true);
			}

			obj->ImGuiSelectGltf(); // GLTFの選択を表示

			//ツリーの終了処理
			ImGui::TreePop();
		}

		ImGui::Separator();
		ImGui::PopID();
	}
	ImGui::EndChild();
}
}

void ImGuiManager::ImGuiSelectObject()
{
	ImGui::Begin("Game");
	ImVec2 imagePos = ImGui::GetCursorScreenPos();
	ImVec2 imageSize = { m_gameSceneSize.x, m_gameSceneSize.y };

	// 画面上で,click場合のみ当たり判定
	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		// マウスの座標を取得
		ImVec2 mousePos = ImGui::GetIO().MousePos;

		// Image内のクリック位置を計算
		float offsetX = mousePos.x - imagePos.x;
		float offsetY = mousePos.y - imagePos.y;

		// マウスを取得
		POINT mouse;

		if (offsetX >= 0 && offsetY >= 0 && offsetX < imageSize.x && offsetY < imageSize.y)
		{
			// ゲーム画面の解像度に基づいてマウス座標を合わせる
			float gameX = offsetX * (1280.0f / imageSize.x);
			float gameY = offsetY * (720.0f / imageSize.y);

	
			mouse.x = static_cast<LONG>(gameX);
			mouse.y = static_cast<LONG>(gameY);

			KdDebugGUI::Instance().AddLog("MousePosX:%d,MousePosY:%d \n", mouse.x, mouse.y);

		}

		// コライダーからレイを取得
		KdCollider::RayInfo rayInfo;

		auto spCamera = GetActiveCamera();

		if (!spCamera)
		{
			ImGui::End();
			return;
		}

		spCamera->GenerateRayInfoFromClientPos(mouse, rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range);

		// KdColliderのレイ情報を設定
		rayInfo.m_type = KdCollider::TypeEvent;

		std::list<KdCollider::CollisionResult> results;
		for (const auto& it : SceneManager::Instance().GetObjList())
		{
			// 当たったオブジェクトの自身のポインタが返り値になる
			it->SelectObjectIntersects(rayInfo, &results);
		}

		KdCollider::CollisionResult resultObject;

		float length = rayInfo.m_range;
		for (auto& it : results)
		{
			if (length > (it.m_hitPos - spCamera->GetCameraMatrix().Translation()).Length())
			{
				resultObject = it;
				length = (it.m_hitPos - spCamera->GetCameraMatrix().Translation()).Length();
			}
		}

		// 当たったオブジェクトがある場合
		if (resultObject.m_resultObject)
		{
			SceneManager::Instance().m_selectObject = resultObject.m_resultObject;
			m_openObject = resultObject.m_resultObject;	// 選択したオブジェクトを保存
		}
	}
	ImGui::End();
}

void ImGuiManager::ShowInspector()
{
	if (ImGui::Begin("Inspector"))
	{
		//選択されたオブジェクト
		auto selectPbject = SceneManager::Instance().m_selectObject;
		// 開きたいオブジェクト
		auto openObject = m_openObject;

		if (openObject && openObject != selectPbject)
		{
			openObject->ImGuiInspector(); // 選択したオブジェクトのインスペクターを表示
		}
		else if (openObject)
		{
			selectPbject->ImGuiInspector();
		}
	}
	ImGui::End();
}

void ImGuiManager::ShowGameScene()
{
	if (ImGui::Begin("Game"))
	{
		if (!SceneManager::Instance().GetCurrentScene())
		{
			ImGui::End();
			return;
		}

		m_winSize = ImGui::GetWindowSize();

		auto texID = (ImTextureID)(SceneManager::Instance().GetCurrentScene()->GetRenderTargetPack().m_RTTexture->WorkSRView());

		m_width = m_winSize.x;
		m_height = m_width * 9.0f / 16.0f;
		m_gameSceneSize.x = m_width - 20.f;
		m_gameSceneSize.y = m_height;

		ImGui::Image(texID, { m_gameSceneSize.x ,m_gameSceneSize.y });

		// 画像の矩形取得
		ImVec2 imageMin = ImGui::GetItemRectMin();
		ImVec2 imageMax = ImGui::GetItemRectMax();

		// 画像上でクリックされたか判定
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			ImVec2 mousePos = ImGui::GetIO().MousePos;
			float offsetX = mousePos.x - imageMin.x;
			float offsetY = mousePos.y - imageMin.y;

			POINT mouse;
			if (offsetX >= 0 && offsetY >= 0 && offsetX < m_gameSceneSize.x && offsetY < m_gameSceneSize.y)
			{
				float gameX = offsetX * (1280.0f / m_gameSceneSize.x);
				float gameY = offsetY * (720.0f / m_gameSceneSize.y);

				mouse.x = static_cast<LONG>(gameX);
				mouse.y = static_cast<LONG>(gameY);

				KdDebugGUI::Instance().AddLog("MousePosX:%d,MousePosY:%d \n", mouse.x, mouse.y);

				// コライダーからレイを取得
				KdCollider::RayInfo rayInfo;
				auto spCamera = GetActiveCamera();
				if (spCamera)
				{
					spCamera->GenerateRayInfoFromClientPos(mouse, rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range);
					rayInfo.m_type = KdCollider::TypeEvent;

					std::list<KdCollider::CollisionResult> results;
					for (const auto& it : SceneManager::Instance().GetObjList())
					{
						it->SelectObjectIntersects(rayInfo, &results);
					}

					KdCollider::CollisionResult resultObject;
					float length = rayInfo.m_range;
					for (auto& it : results)
					{
						if (length > (it.m_hitPos - spCamera->GetCameraMatrix().Translation()).Length())
						{
							resultObject = it;
							length = (it.m_hitPos - spCamera->GetCameraMatrix().Translation()).Length();
						}
					}

					if (resultObject.m_resultObject)
					{
						SceneManager::Instance().m_selectObject = resultObject.m_resultObject;
						m_openObject = resultObject.m_resultObject;
					}
				}
			}
		}

		ImGui::End();
	}
}

std::string ImGuiManager::ImSelectClass() const
{
	static std::string currentName = "KdGameObject";
	// ImGuiのコンボボックスを作成
	if (ImGui::BeginCombo("##Class", currentName.data()))
	{
		for (const auto& [name, id] : RegisterObject::GetInstance().m_ClassNameToID)
		{
			bool selected = (currentName == name);
			if (ImGui::Selectable(name.c_str(), selected)) currentName = name;
			if (selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	return currentName;
}

void ImGuiManager::InGuiSceneSelect() const
{
	// シーンの種類を定義するマップを作成＜－こいつ増やせばシーンを増やせる
	std::map<std::string,SceneManager::SceneType,std::less<>> sceneType =
	{
		{"Title",SceneManager::SceneType::Title},
		{"Game",SceneManager::SceneType::Game}
	};

	// こいつと比較して、シーンが異なるかどうかを確認する
	std::string nowSceneName = SceneManager::Instance().GetCurrentScene()->GetSceneName();

	// シーンを選択するボタンを作成
	for (const auto& [key,value]: sceneType)
	{
		// ボタンと現在のシーン名が異なる場合のみ、シーンを切り替える
		if (ImGui::Button(key.data()) && nowSceneName != key.data())
		{
			SceneManager::Instance().SetNextScene(value);
		}
	}
}

void ImGuiManager::ImGuiSelecetCamera()
{
	
	if (!SceneManager::Instance().m_sceneCamera)
	{
		if (ImGui::Button(U8("スタート", ImVec2(150, 20)))) SceneManager::Instance().m_sceneCamera = true;
	}
	else
	{
		if (ImGui::Button(U8("ストップ", ImVec2(150, 20)))) SceneManager::Instance().m_sceneCamera = false;
	}
}

void ImGuiManager::listSwap(std::shared_ptr<KdGameObject> _obj1, std::shared_ptr<KdGameObject> _obj2)
{
	auto& _list = SceneManager::Instance().GetCurrentScene()->WorkObjList();

	auto it1 = std::find(_list.begin(), _list.end(), _obj1);
	if (it1 == _list.end())return;
	auto it2 = std::ranges::find(_list, _obj2);
	if (it2 == _list.end())return;

	std::swap(*it1, *it2);
}

std::shared_ptr<KdCamera> ImGuiManager::GetActiveCamera()
{
	if (SceneManager::Instance().m_sceneCamera)
	{
		SceneManager::Instance().GetCameraWeakPtr(m_fpsCamera);
		if (!m_fpsCamera.expired()) return m_fpsCamera.lock()->GetCamera();
	}
	return nullptr;
}
