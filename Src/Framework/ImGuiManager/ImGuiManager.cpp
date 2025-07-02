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
