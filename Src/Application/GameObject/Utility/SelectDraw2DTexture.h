#pragma once
class SelectDraw2DTexture : public KdGameObject
{
public:
	SelectDraw2DTexture() = default;
	~SelectDraw2DTexture() override = default;

	void DrawSprite() override
	{
		if (m_bDrawTexture)
			KdShaderManager::Instance().m_spriteShader.DrawTex(
				m_texture,
				static_cast<int>(m_pos.x),
				static_cast<int>(m_pos.y),
				nullptr,
				&m_color);
	}

	// ImGuiのインスペクターでDrawを変更できるようにする。
	void ImGuiInspector() override
	{
		KdGameObject::ImGuiInspector();

		ImGui::Checkbox("Draw Texture", &m_bDrawTexture);
	}

	// jsonに、DrawFlagsを保存する
	void JsonSave(nlohmann::json& _json) const override
	{
		KdGameObject::JsonSave(_json);
		_json["drawFlags"] = m_bDrawTexture;
	}

	// jsonから、DrawFlagsを読み込む
	void JsonInput(const nlohmann::json& _json) override
	{
		KdGameObject::JsonInput(_json);

		if (_json.contains("drawFlags"))
		{
			const auto& flagsJson = _json["drawFlags"];
			if (flagsJson.contains("texture")) m_bDrawTexture = flagsJson["texture"].get<bool>();
		}
	}

private:

	bool m_bDrawTexture = false;

};