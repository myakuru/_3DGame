#include "KdGameObject.h"
#include"../../Application/main.h"
#include"../../Framework/Json/Json.h"

void KdGameObject::Init()
{
	ModelLoad(m_path);
}

void KdGameObject::DrawDebug()
{
	// 早期リターン
	if (!m_pDebugWire)return;

	m_pDebugWire->Draw();
}

void KdGameObject::SetScale(float scalar)
{
	Math::Vector3 scale(scalar);

	SetScale(scale);
}

void KdGameObject::SetScale(const Math::Vector3& scale)
{
	Math::Vector3 vecX = m_mWorld.Right();
	Math::Vector3 vecY = m_mWorld.Up();
	Math::Vector3 vecZ = m_mWorld.Backward(); 
	
	vecX.Normalize();
	vecY.Normalize();
	vecZ.Normalize();

	m_mWorld.Right(vecX * scale.x);
	m_mWorld.Up(vecY * scale.y);
	m_mWorld.Backward(vecZ * scale.z);
}

Math::Vector3 KdGameObject::GetScale() const
{
	return Math::Vector3(m_mWorld.Right().Length(), m_mWorld.Up().Length(), m_mWorld.Backward().Length());
}

void KdGameObject::CalcDistSqrFromCamera(const Math::Vector3& camPos)
{
	m_distSqrFromCamera = (m_mWorld.Translation() - camPos).LengthSquared();
}

bool KdGameObject::Intersects(const KdCollider::SphereInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults)
{
	if (!m_pCollider) { return false; }

	return m_pCollider->Intersects(targetShape, m_mWorld, pResults);
}

bool KdGameObject::Intersects(const KdCollider::BoxInfo& targetBox, std::list<KdCollider::CollisionResult>* pResults)
{
	if (!m_pCollider) { return false; }

	return m_pCollider->Intersects(targetBox, m_mWorld, pResults);
}

bool KdGameObject::Intersects(const KdCollider::RayInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults)
{
	if (!m_pCollider) { return false; }

	return m_pCollider->Intersects(targetShape, m_mWorld, pResults);
}

void KdGameObject::JsonInput(const nlohmann::json& _json)
{
	if (_json.contains("path")) m_path = _json["path"];
	if (_json.contains("pos")) m_pos = JSON_MANAGER.JsonToVector(_json["pos"]);
	if (_json.contains("scale")) m_scale = JSON_MANAGER.JsonToVector(_json["scale"]);
	if (_json.contains("deg")) m_deg = JSON_MANAGER.JsonToVector(_json["deg"]);
}

void KdGameObject::JsonSave(nlohmann::json& _json) const
{
	std::string className = typeid(*this).name(); // クラス名の所得(this)にすると基底クラスの名前が取得されるので自分自身のポインタを使用
	
	_json["Name"] = className;
	_json["path"] = m_path;
	_json["pos"] = JSON_MANAGER.VectorToJson(m_pos);
	_json["scale"] = JSON_MANAGER.VectorToJson(m_scale);
	_json["deg"] = JSON_MANAGER.VectorToJson(m_deg);
}

bool KdGameObject::ModelLoad(std::string_view _path)
{
	std::string path(_path);
	// .~ 以降の拡張子を識別するために部分文字列を取得
	std::string ext = path.substr(path.find_last_of('.') + 1);

	m_path = path; // どちらでもパスを保存

	if (ext == "png" || ext == "PNG") {
		// テクスチャ読み込み
		m_texture->Load(path);
		m_polygon->SetMaterial(path);
		return true;
	}

	// モデル読み込み
	m_model = std::make_shared<KdModelData>();
	m_model = KdAssets::Instance().m_modeldatas.GetData(path);

    return false;
}

void KdGameObject::ImGuiInspector()
{
	// 0.1ずつのステップでドラッグできるようにする
	ImGui::DragFloat3("pos", &m_pos.x, 0.1f);
	ImGui::DragFloat3("m_scale", &m_scale.x, 0.1f);
	ImGui::DragFloat3("m_rot", &m_deg.x, 0.1f);

	if (ImGui::Button(("LoadPath: %s", m_path.data())))
	{
		if (Application::Instance().GetWindow().OpenFileDialog(m_path))
		{
			ModelLoad(m_path);
		}
	}

	ImGui::ColorEdit4("color", &m_color.x);

}
