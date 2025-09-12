#include "EffekseerEffectBase.h"
#include"../../Character/Player/Player.h"
#include"../../../Scene/SceneManager.h"

void EffekseerEffectBase::Init()
{
	KdGameObject::Init();

	// 画面の大きさが変わっても問題ないようにする
	KdCSVData windowData("Asset/Data/WindowSettings.csv");
	const std::vector<std::string>& sizeData = windowData.GetLine(0);
	KdEffekseerManager::GetInstance().Create(atoi(sizeData[0].data()), atoi(sizeData[1].data()));

	// 初回は起動しないようにする。
	m_once = false;
}

void EffekseerEffectBase::Update()
{

	KdEffekseerManager::GetInstance().Update();

	if (KeyboardManager::GetInstance().IsKeyJustPressed('F'))
	{
		m_once = true;
	}

	SceneManager::Instance().GetObjectWeakPtr(m_player);
	auto player = m_player.lock();
	if (!player) return;

	// プレイヤーの前方ベクトル
	Math::Vector3 forward = Math::Vector3::TransformNormal(Math::Vector3::Forward, Math::Matrix::CreateFromQuaternion(player->GetRotationQuaternion()));
	forward.Normalize();

	m_mWorld = Math::Matrix::CreateScale(m_scale);
	m_mWorld *= Math::Matrix::CreateFromYawPitchRoll
	(
		DirectX::XMConvertToRadians(m_degree.y),
		DirectX::XMConvertToRadians(m_degree.x),
		DirectX::XMConvertToRadians(m_degree.z)
	) * Math::Matrix::CreateFromQuaternion(player->GetRotationQuaternion());

	m_mWorld.Translation(m_position + player->GetPos() + forward * m_distance);

	EffectUpdate();
}

void EffekseerEffectBase::EffectUpdate()
{
	if (m_once)
	{
		KdEffekseerManager::GetInstance().Play(m_path, m_mWorld, m_effectSpeed);
		m_once = false;
	}
}

void EffekseerEffectBase::DrawEffect()
{
	// Effekseerの描画
	KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
	KdEffekseerManager::GetInstance().Draw();
	KdShaderManager::Instance().UndoRasterizerState();
}

void EffekseerEffectBase::ImGuiInspector()
{
	KdGameObject::ImGuiInspector();

	ImGui::DragFloat(U8("エフェクトの出現位置"), &m_distance, 0.1f);
	ImGui::DragFloat(U8("エフェクトの再生速度"), &m_effectSpeed, 0.1f);
}

void EffekseerEffectBase::JsonSave(nlohmann::json& _json) const
{
	KdGameObject::JsonSave(_json);
	_json["distance"] = m_distance;
	_json["EffectSpeed"] = m_effectSpeed;
}

void EffekseerEffectBase::JsonInput(const nlohmann::json& _json)
{
	KdGameObject::JsonInput(_json);
	if (_json.contains("distance")) m_distance = _json["distance"].get<float>();
	if (_json.contains("EffectSpeed")) m_effectSpeed = _json["EffectSpeed"].get<float>();
}

bool EffekseerEffectBase::ModelLoad(std::string _path)
{
	// .~ 以降の拡張子を識別するために部分文字列を取得
	std::string ext = _path.substr(_path.find_last_of('.') + 1);

	if (ext == "efkefc")
	{
		m_path = _path;
		return true;
	}

	return false;
}
