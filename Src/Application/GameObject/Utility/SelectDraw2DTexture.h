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
				static_cast<int>(m_position.x),
				static_cast<int>(m_position.y),
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
		
		if (_json.contains("drawFlags")) m_bDrawTexture = _json["drawFlags"].get<bool>();
	}

private:

	bool ModelLoad(std::string _path) override
	{
		// .~ 以降の拡張子を識別するために部分文字列を取得
		std::string ext = _path.substr(_path.find_last_of('.') + 1);

		if (ext == "png" || ext == "PNG")
		{
			_path = m_path;
			// テクスチャ読み込み
			m_texture = KdAssets::Instance().m_textures.GetData(m_path);
			return true;
		}

		return false;
	}

	bool m_bDrawTexture = false;

};