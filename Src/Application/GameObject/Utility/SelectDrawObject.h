#pragma once
class SelectDrawObject:public KdGameObject
{
public:

	void GenerateDepthMapFromLight() override
	{
		if (m_bGenerateDepthMapFromLight) KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld, m_color);
	}

	void DrawLit() override
	{
		if(m_bDrawLit) KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld, m_color);
	}

	void DrawUnLit() override
	{
		if(m_bDrawUnLit) KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld, m_color);
	}

	void DrawBright() override { if (m_bDrawBright)KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld, m_color); }

	void DrawSprite() override {
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

		ImGui::BeginTable("DrawFlags", 2); // 3列
		int col = 0;
		for (const auto& [name, flg] : m_drawFlg)
		{
			ImGui::TableNextColumn();
			ImGui::Checkbox(name.data(), flg);
			col++;
		}
		ImGui::EndTable();
	}

	// jsonに、DrawFlagsを保存する
	void JsonSave(nlohmann::json& _json) const override
	{
		KdGameObject::JsonSave(_json);

		nlohmann::json flagsJson;
		for (const auto& [name, ptr] : m_drawFlg)
		{
			flagsJson[name] = *ptr;
		}
		_json["drawFlags"] = flagsJson;
	}

	// jsonから、DrawFlagsを読み込む
	void JsonInput(const nlohmann::json& _json) override
	{
		KdGameObject::JsonInput(_json);

		if (_json.contains("drawFlags"))
		{
			const auto& flagsJson = _json["drawFlags"];
			for (const auto& [name, ptr] : m_drawFlg)
			{
				if (flagsJson.contains(name)) *ptr = flagsJson[name].get<bool>();
			}
		}
	}

private:

	bool m_bGenerateDepthMapFromLight = false;	// ライトからの深度マップを生成するかどうか
	bool m_bDrawLit = false;					// ライトの影響を受ける描画を行うかどうか
	bool m_bDrawUnLit = false;					// ライトの影響を受けない描画を行うかどうか
	bool m_bDrawBright = false;					// ブライト描画を行うかどうか
	bool m_bDrawTexture = false;				//2DTextureを描画するかどうか

	std::map<std::string, bool*, std::less<void>> m_drawFlg =
	{
			{"GenerateDepthMapFromLight", &m_bGenerateDepthMapFromLight},
			{"DrawLit", &m_bDrawLit},
			{"DrawUnLit", &m_bDrawUnLit},
			{"DrawBright", &m_bDrawBright},
			{"DrawSprite",&m_bDrawTexture},
	};
};