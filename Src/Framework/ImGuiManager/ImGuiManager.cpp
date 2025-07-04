#include "ImGuiManager.h"
#include"../Json/Json.h"
#include"../../Application/Scene/SceneManager.h"
#include"../../Application/main.h"

void ImGuiManager::ImGuiUpdate() const
{
	// 上部のメニューバーを表示
	MainMenuBar();

	// ヒエラルキーウィンドウを表示
	Hierarchy();
}

void ImGuiManager::Hierarchy() const
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

void ImGuiManager::TreeNode() const
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

        // ここからツリー形式で詳細情報を表示
        if (std::string className = typeid(objectName).name(); //<- C++17から追加された書き方
            ImGui::TreeNode(className.data())) //className + "##" + std::to_stirng((int)obj->get()) <=こういう書き方もあるよ
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
