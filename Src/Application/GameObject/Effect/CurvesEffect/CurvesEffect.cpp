#include "CurvesEffect.h"
#include"../../Character/Player/Player.h"
#include"../../../Scene/SceneManager.h"
#include"../../../main.h"

const uint32_t CurvesEffect::TypeID = KdGameObject::GenerateTypeID();

void CurvesEffect::Init()
{
	m_trailPolygon = std::make_shared<KdTrailPolygon>();
	m_trailPolygon2 = std::make_shared<KdTrailPolygon>();
	m_trailPolygon3 = std::make_shared<KdTrailPolygon>();

	m_trailPolygon->ClearPoints();
	m_trailPolygon2->ClearPoints();
	m_trailPolygon3->ClearPoints();

	m_trailPolygon->SetLength(60);
	m_trailPolygon2->SetLength(60);
	m_trailPolygon3->SetLength(60);

	m_trailTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/NA_ball of light_006.png");

	m_trailPolygon->SetMaterial(m_trailTex);
	m_trailPolygon2->SetMaterial(m_trailTex);
	m_trailPolygon3->SetMaterial(m_trailTex);


	m_trailPolygon->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
	m_trailPolygon2->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
	m_trailPolygon3->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
}

void CurvesEffect::Update()
{
	SceneManager::Instance().GetObjectWeakPtr(m_player);
	auto player = m_player.lock();
	if (!player) return;

	if (KeyboardManager::GetInstance().IsKeyJustPressed('R'))
	{
		m_isTrailActive = !m_isTrailActive;
	}

	float deltaTime = Application::Instance().GetDeltaTime();

	static float circleAngle = 0.0f;
	const float circleSpeed = 200.0f;
	static Math::Vector3 circleNormal1, circleNormal2, circleNormal3;
	static bool wasActive = false;
	static float elapsedTime = 0.0f; // ランダム方向用
	static float radiusElapsedTime = 0.0f; // radius用

	bool isActive = m_isTrailActive;

	if (isActive)
	{
		// アクティブになった瞬間だけリセット
		if (!wasActive)
		{
			m_trailPolygon->ClearPoints();
			m_trailPolygon2->ClearPoints();
			m_trailPolygon3->ClearPoints();
			circleAngle = 90.0f;
			elapsedTime = 0.0f;
			radiusElapsedTime = 0.1f; // ここでリセット

			auto randomNormal = []() {
				float theta = DirectX::XMConvertToRadians((float)(rand() % 60 + 20));
				float phi = DirectX::XMConvertToRadians((float)(rand() % 360));
				Math::Vector3 n(
					std::sin(theta) * std::cos(phi),
					std::cos(theta),
					std::sin(theta) * std::sin(phi)
				);
				n.Normalize();
				return n;
				};
			circleNormal1 = randomNormal();
			circleNormal2 = randomNormal();
			circleNormal3 = randomNormal();
		}

		// radius用の経過時間を加算
		radiusElapsedTime += deltaTime;

		// radiusを0から4へ徐々に変化（例：0.5秒で最大値）
		float lerpDuration = 0.5f;
		float t = std::min(radiusElapsedTime / lerpDuration, 1.0f);
		float radius = 4.0f * t;

		// ランダム方向用の経過時間を加算
		elapsedTime += deltaTime;

		// 0.2秒ごとにランダム方向を再決定
		if (elapsedTime >= 0.2f)
		{
			auto randomNormal = []() {
				float theta = DirectX::XMConvertToRadians((float)(rand() % 60 + 20));
				float phi = DirectX::XMConvertToRadians((float)(rand() % 360));
				Math::Vector3 n(
					std::sin(theta) * std::cos(phi),
					std::cos(theta),
					std::sin(theta) * std::sin(phi)
				);
				n.Normalize();
				return n;
				};
			circleNormal1 = randomNormal();
			circleNormal2 = randomNormal();
			circleNormal3 = randomNormal();
			elapsedTime = 0.0f;
		}

		circleAngle += circleSpeed * 0.5f;

		Math::Vector3 center = player->GetPos();

		auto calcCirclePos = [](const Math::Vector3& center, const Math::Vector3& normal, float angle, float radius) {
			Math::Vector3 baseVec = Math::Vector3::Forward;
			if (std::abs(normal.Dot(baseVec)) > 0.99f) baseVec = Math::Vector3::Right;

			Math::Vector3 tangent = normal.Cross(baseVec);
			tangent.Normalize();

			Math::Vector3 bitangent = normal.Cross(tangent);
			bitangent.Normalize();

			Math::Vector3 pos = center;
			pos += tangent * (std::cos(angle) * radius);
			pos += bitangent * (std::sin(angle) * radius);
			pos += normal * 0.5f;

			return pos;
			};

		Math::Matrix scaleMat = Math::Matrix::CreateScale(0.05f);

		Math::Matrix mat1 = scaleMat * Math::Matrix::CreateTranslation(calcCirclePos(center, circleNormal1, circleAngle, radius));
		Math::Matrix mat2 = scaleMat * Math::Matrix::CreateTranslation(calcCirclePos(center, circleNormal2, circleAngle + DirectX::XM_2PI / 3, radius));
		Math::Matrix mat3 = scaleMat * Math::Matrix::CreateTranslation(calcCirclePos(center, circleNormal3, circleAngle + DirectX::XM_2PI * 2 / 3, radius));

		m_trailPolygon->AddPoint(mat1);
		m_trailPolygon2->AddPoint(mat2);
		m_trailPolygon3->AddPoint(mat3);
	}
	else
	{
		m_trailPolygon->ClearPoints();
		m_trailPolygon2->ClearPoints();
		m_trailPolygon3->ClearPoints();
	}

	wasActive = isActive;
}

void CurvesEffect::DrawBright()
{
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon, Math::Matrix::Identity, { 0.51f, 0.31f, 0.97f, 1.f });
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon2, Math::Matrix::Identity, { 0.2f, 0.38f, 0.94f, 1.f });
	//KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon3, Math::Matrix::Identity, { 1.0f,1.0f,1.0f,0.7f });
}
