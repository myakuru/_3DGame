﻿#pragma once
class TPSCamera;
class FPSCamera;
class ImGuiManager : public KdGameObject
{
public:
	ImGuiManager() = default;
	~ImGuiManager() override = default;

	void ImGuiUpdate();

	void Hierarchy();

	void MainMenuBar();

	void TreeNode();

	// ImGuiでオブジェクトを選択できるようにするための関数
	void ImGuiSelectObject();

	// インスペクターの表示のみ
	void ShowInspector();

	void ShowGameScene();

	std::string ImSelectClass() const;

	void InGuiSceneSelect() const;

	void ImGuiSelecetCamera();

	template<class T>
	inline bool DragDropSource(std::string_view _tag, T _payload)
	{
		bool flg = false;
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload(_tag.data(), &_payload, sizeof(T), ImGuiCond_Once);
			ImGui::Text(_tag.data());

			flg = true;

			ImGui::EndDragDropSource();
		}
		return flg;
	}

	template<class T>
	inline bool DragDropTarget(std::string_view _tag, T& _payload)
	{
		bool flg = false;
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(_tag.data()))
			{
				IM_ASSERT(payload->DataSize == sizeof(T));
				_payload = *(T*)payload->Data;
				flg = true;
			}
			ImGui::EndDragDropTarget();
		}
		return flg;
	}

	template<class T>
	bool GetDragData(std::string_view _id, T& _result)
	{
		auto payload = ImGui::GetDragDropPayload();
		if (!payload)return false;
		if (payload->IsDataType(_id.data())) {
			IM_ASSERT(payload->DataSize == sizeof(T));
			_result = *(T*)payload->Data;
			return true;
		}
		return false;
	}

	bool IsDroped(std::string_view _id)
	{
		// ドラック中か
		auto payload = ImGui::GetDragDropPayload();
		if (!payload)return false;
		if (!payload->IsDataType(_id.data()))return false;

		// 前のTreeアイテムの境界を取得
		float previousItemBottom = ImGui::GetItemRectMax().y;
		// 現在のTreeアイテム座標
		ImVec2 currentItem = ImGui::GetCursorScreenPos();
		// Item間のRect
		ImRect betweenSpace = { { currentItem.x,previousItemBottom }, { ImGui::GetWindowSize().x,currentItem.y } };

		auto& g = *ImGui::GetCurrentContext();
		if (ImGui::IsMouseHoveringRect(betweenSpace.Min, betweenSpace.Max)          // ドラッグ中の要素がある場合
			&& g.DragDropPayload.SourceId != g.LastItemData.ID              // ソースへのドロップ禁止
			)
		{
			// 挿入位置を示すラインの描画
			float lineY = (previousItemBottom + currentItem.y) * 0.5f;
			ImGui::GetWindowDrawList()->AddLine(
				ImVec2(currentItem.x, lineY),
				ImVec2(ImGui::GetWindowSize().x, lineY),
				ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_DragDropTarget]),
				2.0f
			);
			return !ImGui::IsMouseDown(g.DragDropMouseButton);
		}

		return false;
	}

	void listSwap(std::shared_ptr<KdGameObject> _obj1, std::shared_ptr<KdGameObject> _obj2);

	//std::shared_ptr<KdTexture> GetTexture() const { return m_texture; }

	KdRenderTargetPack GetRenderTargetChanger() { return m_renderTargetChanger; }

private:

	std::weak_ptr<TPSCamera> m_tpsCamera;
	std::weak_ptr<FPSCamera> m_fpsCamera;

	// FPSカメラのウィークポインタを取得する。
	std::shared_ptr<KdCamera> GetActiveCamera();

	// 開きたいオブジェクトを記録する変数
	std::shared_ptr<KdGameObject> m_openObject = nullptr;

	std::shared_ptr<KdTexture> m_texture = nullptr;

	KdRenderTargetPack m_renderTargetChanger;

	// 本画像のを半分にする
	ImVec2 m_gameSceneSize = { 1280.0f / 2, 720.0f / 2 };

	ImVec2 m_winSize{};
	float m_width = 0.0f;
	float m_height = 0.0f;

	std::string m_modelPath = "Asset/";

};