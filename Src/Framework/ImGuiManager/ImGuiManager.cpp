#include "ImGuiManager.h"
#include"../Json/Json.h"
#include"../../Application/Scene/SceneManager.h"
#include"../../Application/main.h"
#include"../../Application/GameObject/Camera/CameraBase.h"
#include"../../Application/Scene/BaseScene/BaseScene.h"

void ImGuiManager::ImGuiUpdate()
{
	// 上部のメニューバーを表示
	MainMenuBar();

	// ヒエラルキーウィンドウを表示
	Hierarchy();
}

void ImGuiManager::Hierarchy()
{
	if (ImGui::Begin("Hierarchy"))
	{
		std::string name = SceneManager::GetInstance().ImSelectClass();

		if (ImGui::Button("AddObject"))
		{
			JSON_MANAGER.AddJsonObject(name);
		}

		// ツリー構造のオブジェクトを表示
		TreeNode();

	}
	ImGui::End();
}

void ImGuiManager::MainMenuBar() const
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
    for (auto& obj : SceneManager::GetInstance().GetObjList())
    {
        const auto& objectName = *obj;

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

            // ボタンでオブジェクトを削除できる
            if (ImGui::SmallButton("Delete"))
            {
                obj->SetExpired(true);
            }

            // 各オブジェクトのプロパティを表示
            obj->ImGuiInspector();

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
	// 画面上で,click場合のみ当たり判定
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		// マウスの座標を取得
		ImVec2 mousePos = ImGui::GetMousePos();

		// マウスを取得
		POINT mouse;
		mouse.x = static_cast<LONG>(mousePos.x);
		mouse.y = static_cast<LONG>(mousePos.y);

		// コライダーからレイを取得
		KdCollider::RayInfo rayInfo;

		//　CameraBaseからカメラの情報を取得
		//auto cameraBase = FindObjectOfType<CameraBase>();
		//if (cameraBase)
		//{
		//	// CameraBaseのm_spCameraを取得
		//	auto spCamera = cameraBase->GetCamera();
		//	if (spCamera)
		//	{
		//		spCamera->GenerateRayInfoFromClientPos(mouse, rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range);
		//	}
		//}

		// KdColliderのレイ情報を設定
		rayInfo.m_type = KdCollider::TypeEvent;

		std::list<KdCollider::CollisionResult> results;
		for (auto& it : SceneManager::GetInstance().GetObjList())
		{
			it->Intersects(rayInfo, &results);
		}

		KdCollider::CollisionResult resultObject;

		if (resultObject.m_resultObject)
		{
			SceneManager::GetInstance().m_selectObject = resultObject.m_resultObject;
		}

	}
}

void ImGuiManager::listSwap(std::shared_ptr<KdGameObject> _obj1, std::shared_ptr<KdGameObject> _obj2)
{
	auto& _list = SceneManager::GetInstance().GetCurrentScene()->WorkObjList();

	auto it1 = std::find(_list.begin(), _list.end(), _obj1);
	if (it1 == _list.end())return;
	auto it2 = std::ranges::find(_list, _obj2);
	if (it2 == _list.end())return;

	std::swap(*it1, *it2);
}
