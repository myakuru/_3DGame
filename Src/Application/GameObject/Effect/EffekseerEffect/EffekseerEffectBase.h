﻿#pragma once
class Player;
class Enemy;
class EffekseerEffectBase : public KdGameObject
{
public:

	EffekseerEffectBase() = default;
	~EffekseerEffectBase() override = default;

	void SetPlayEffect(bool once) { m_load = once; }

	// エフェクトが再生中か
	bool IsEffectPlaying() const { return m_isEffectPlaying; }

	// エフェクトが再生終了したか
	bool IsEffectEnd() const;

protected:

	void Init() override;
	void Update() override;
	void DrawEffect() override;
	void DrawToon() override;

	virtual void EffectUpdate();

	std::weak_ptr<Player> m_player;
	std::weak_ptr<Enemy> m_enemy;

	void ImGuiInspector() override;
	void JsonSave(nlohmann::json& _json) const override;
	void JsonInput(const nlohmann::json& _json) override;

	bool ModelLoad(std::string _path) override;

	bool m_once = false;	// エフェクトを１回だけ再生する
	bool m_load = false; // エフェクトがロードされたか

	// 現在再生中のエフェクト参照
	std::weak_ptr<KdEffekseerObject> m_wpEffect;

	// 再生状態
	bool m_isEffectPlaying = false;

	float m_distance = 0.0f;
	float m_effectSpeed = 0.0f;

};