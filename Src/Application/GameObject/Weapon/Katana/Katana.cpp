#include "Katana.h"
#include"../../../main.h"
#include"../../../../Framework/Json/Json.h"
#include"../../../Scene/SceneManager.h"

// TypeIDの定義と初期化
const uint32_t Katana::TypeID = KdGameObject::GenerateTypeID();

void Katana::Init()
{
	WeaponBase::Init();
	m_trailPolygon = std::make_shared<KdTrailPolygon>();
	m_trailPolygon2 = std::make_shared<KdTrailPolygon>();
	m_trailPolygon3 = std::make_shared<KdTrailPolygon>();

	m_trailPolygon->ClearPoints();
	m_trailPolygon2->ClearPoints();
	m_trailPolygon3->ClearPoints();

	m_trailPolygon->SetLength(100);
	m_trailPolygon2->SetLength(100);
	m_trailPolygon3->SetLength(100);

	m_trailModel->Load("Asset/Models/Goast/nani.gltf");

	m_trailTex->Load("Asset/Textures/NA_ball of light_006.png");
	m_trailTex2->Load("Asset/Textures/NA_Basic hit_006.png");
	m_trailTex3->Load("Asset/Textures/nailGlow.png");

	m_trailPolygon->SetMaterial(m_trailTex);
	m_trailPolygon2->SetMaterial(m_trailTex);
	m_trailPolygon3->SetMaterial(m_trailTex3);

	m_trailPolygon->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
	m_trailPolygon2->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
	m_trailPolygon3->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
}

void Katana::Update()
{
	// 剣の行列計算（元の処理そのまま）
	if (m_swordHandData.m_weaponTranslationMatrix != Math::Matrix::Identity)
	{
		UpdateHand();
	}
	else
	{
		m_swordData.m_weaponRotationMatrix = Math::Matrix::CreateFromYawPitchRoll(
			DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.y),
			DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.x),
			DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.z)
		);

		Math::Vector3 playerHipPos = m_swordData.m_weaponTranslationMatrix.Translation();
		m_swordData.m_weaponTranslationMatrix.Translation(m_katanaOffset + playerHipPos);
		m_swordData.m_weaponScaleMatrix = Math::Matrix::CreateScale(m_swordData.m_scale);
		m_swordData.m_weaponMatrix = m_swordData.m_weaponScaleMatrix * m_swordData.m_weaponRotationMatrix * m_swordData.m_weaponTranslationMatrix * m_swordData.m_playerTranslationMatrix;
		m_trailOffset = m_swordData.m_weaponScaleMatrix * m_swordData.m_weaponRotationMatrix * m_swordData.m_playerTranslationMatrix;
	}

	// 軌跡の先端位置を計算
	Math::Vector3 tip1, tip2, tip3;
	if (m_swordHandData.m_weaponTranslationMatrix != Math::Matrix::Identity)
	{
		tip1 = m_swordData.m_weaponMatrix.Translation() + m_swordData.m_weaponMatrix.Up() * 4.0f;
		tip2 = m_swordData.m_weaponMatrix.Translation() + m_swordData.m_weaponMatrix.Up() * 4.0f;
		tip3 = m_swordData.m_weaponMatrix.Translation() + m_swordData.m_weaponMatrix.Up() * 4.0f;
	}
	else
	{
		//tip1 = m_swordData.m_weaponMatrix.Translation() + m_swordData.m_weaponMatrix.Up();
		//tip2 = m_swordData.m_weaponMatrix.Translation() + m_swordData.m_weaponMatrix.Up();
		//tip3 = m_swordData.m_weaponMatrix.Translation() + m_swordData.m_weaponMatrix.Up();

		// 軌跡をクリア
		m_trailPolygon->ClearPoints();
		m_trailPolygon2->ClearPoints();
		m_trailPolygon3->ClearPoints();
	}

	// 軌跡点リスト（staticでフレーム間保持）
	static std::deque<Math::Vector3> trailPoints1, trailPoints2, trailPoints3;
	trailPoints1.push_back(tip1);
	trailPoints2.push_back(tip2);
	trailPoints3.push_back(tip3);

	// 最大数制限
	const size_t maxTrailLen = 100;
	if (trailPoints1.size() > maxTrailLen) trailPoints1.pop_front();
	if (trailPoints2.size() > maxTrailLen) trailPoints2.pop_front();
	if (trailPoints3.size() > maxTrailLen) trailPoints3.pop_front();

	// Catmull-Rom補間で滑らかな軌跡を生成
	auto addCatmullTrail = [&](std::deque<Math::Vector3>& points, std::shared_ptr<KdTrailPolygon>& trail, float scaleVal)
		{
			Math::Matrix inv = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(90.0f));
			Math::Matrix scale = Math::Matrix::CreateScale(scaleVal);

			if (points.size() >= 4)
			{
				size_t start = points.size() - 4;
				for (float t = 0.0f; t < 1.0f; t += 0.25f)
				{
					Math::Vector3 interp = CatmullRom(
						points[start], points[start + 1], points[start + 2], points[start + 3], t
					);
					Math::Matrix mat = Math::Matrix::CreateTranslation(interp);
					m_finalMat = scale * inv * (m_swordData.m_weaponRotationMatrix * mat);
					trail->AddPoint(m_finalMat);
				}
			}
			else
			{
				// 点が足りない場合はそのまま追加
				Math::Matrix mat = Math::Matrix::CreateTranslation(points.back());
				m_finalMat = scale * inv * (m_swordData.m_weaponRotationMatrix * mat);
				trail->AddPoint(m_finalMat);
			}
		};

	addCatmullTrail(trailPoints1, m_trailPolygon, 0.4f);
	addCatmullTrail(trailPoints2, m_trailPolygon2, 0.4f);
	addCatmullTrail(trailPoints3, m_trailPolygon3, 0.4f);
}

void Katana::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_trailModel);
}

void Katana::DrawUnLit()
{
	//KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon3, Math::Matrix::Identity, { 1.0f,1.0f,1.0f,0.7f });
}

void Katana::DrawBright()
{
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon, Math::Matrix::Identity, { 0.0f,1.0f,1.0f,0.5f });
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon2, Math::Matrix::Identity, { 1.0f,1.0f,1.0f,0.7f });
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon3, Math::Matrix::Identity, { 1.0f,1.0f,1.0f,0.7f });
}

void Katana::UpdateHand()
{
	m_swordHandData.m_weaponRotationMatrix = Math::Matrix::CreateFromYawPitchRoll
	(
		DirectX::XMConvertToRadians(m_swordHandData.m_weaponDeg.y),
		DirectX::XMConvertToRadians(m_swordHandData.m_weaponDeg.x),
		DirectX::XMConvertToRadians(m_swordHandData.m_weaponDeg.z)
	);

	m_swordHandData.m_weaponTranslationMatrix.Translation(m_swordHandData.m_weaponTranslationMatrix.Translation());
	m_swordData.m_weaponMatrix = m_swordData.m_weaponScaleMatrix * m_swordHandData.m_weaponRotationMatrix *m_swordHandData.m_weaponTranslationMatrix * m_swordData.m_playerTranslationMatrix; 

}

void Katana::ImGuiInspector()
{
	KdGameObject::ImGuiInspector();

	ImGui::Text("Katana Inspector");

	ImGui::Text(U8("刀の角度を変更"));
	ImGui::DragFloat3("deg", &m_swordData.m_weaponDeg.x, 0.1f);
	ImGui::Text(U8("刀の位置を変更"));
	ImGui::DragFloat3("offset", &m_katanaOffset.x, 0.01f);
	ImGui::Text(U8("刀のスケールを変更"));
	ImGui::DragFloat3("scale", &m_swordData.m_scale.x, 0.01f);

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Hand Katana"))
	{
		ImGui::Text(U8("手持ちの刀の角度を変更"));
		ImGui::DragFloat3("handDeg", &m_swordHandData.m_weaponDeg.x, 0.1f);
	}

	UpdateHand();
}

void Katana::JsonSave(nlohmann::json& _json) const
{
	WeaponBase::JsonSave(_json);
	_json["weaponDeg"] = JSON_MANAGER.VectorToJson(m_swordData.m_weaponDeg);
	_json["katanaOffset"] = JSON_MANAGER.VectorToJson(m_katanaOffset);
	_json["scale"] = JSON_MANAGER.VectorToJson(m_swordData.m_scale);
	_json["handWeaponDeg"] = JSON_MANAGER.VectorToJson(m_swordHandData.m_weaponDeg);
}

void Katana::JsonInput(const nlohmann::json& _json)
{
	WeaponBase::JsonInput(_json);
	if (_json.contains("weaponDeg")) m_swordData.m_weaponDeg = JSON_MANAGER.JsonToVector(_json["weaponDeg"]);
	if (_json.contains("katanaOffset")) m_katanaOffset = JSON_MANAGER.JsonToVector(_json["katanaOffset"]);
	if (_json.contains("scale")) m_swordData.m_scale = JSON_MANAGER.JsonToVector(_json["scale"]);
	if (_json.contains("handWeaponDeg")) m_swordHandData.m_weaponDeg = JSON_MANAGER.JsonToVector(_json["handWeaponDeg"]);
}
