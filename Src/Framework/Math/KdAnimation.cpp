#include "KdAnimation.h"
#include "../Direct3D/KdModel.h"

// 二分探索で、指定時間から次の配列要素のKeyIndexを求める関数
// list		… キー配列
// time		… 時間
// 戻り値	… 次の配列要素のIndex
template<class T>
int BinarySearchNextAnimKey(const std::vector<T>& list, float time)
{
	int low = 0;
	int high = (int)list.size();
	while (low < high)
	{
		int mid = (low + high) / 2;
		float midTime = list[mid].m_time;

		if (midTime <= time) low = mid + 1;
		else high = mid;
	}
	return low;
}

bool KdAnimationData::Node::InterpolateTranslations(Math::Vector3& result, float time)
{
	if (m_translations.size() == 0)return false;

	// キー位置検索
	UINT keyIdx = BinarySearchNextAnimKey(m_translations, time);

	// 先頭のキーなら、先頭のデータを返す
	if (keyIdx == 0) {
		result = m_translations.front().m_vec;
		return true;
	}
	// 配列外のキーなら、最後のデータを返す
	else if (keyIdx >= m_translations.size()) {
		result = m_translations.back().m_vec;
		return true;
	}
	// それ以外(中間の時間)なら、その時間の値を補間計算で求める
	else {
		auto& prev = m_translations[keyIdx - 1];	// 前のキー
		auto& next = m_translations[keyIdx];		// 次のキー
		// 前のキーと次のキーの時間から、0～1間の時間を求める
		float f = (time - prev.m_time) / (next.m_time - prev.m_time);
		// 補間
		result = DirectX::XMVectorLerp(
			prev.m_vec,
			next.m_vec,
			f
		);
	}

	return true;
}

bool KdAnimationData::Node::InterpolateRotations(Math::Quaternion& result, float time)
{
	if (m_rotations.size() == 0)return false;

	// キー位置検索
	UINT keyIdx = BinarySearchNextAnimKey(m_rotations, time);
	// 先頭のキーなら、先頭のデータを返す
	if (keyIdx == 0) {
		result = m_rotations.front().m_quat;
	}
	// 配列外のキーなら、最後のデータを返す
	else if (keyIdx >= m_rotations.size()) {
		result = m_rotations.back().m_quat;
	}
	// それ以外(中間の時間)なら、その時間の値を補間計算で求める
	else {
		auto& prev = m_rotations[keyIdx - 1];	// 前のキー
		auto& next = m_rotations[keyIdx];		// 次のキー
		// 前のキーと次のキーの時間から、0～1間の時間を求める
		float f = (time - prev.m_time) / (next.m_time - prev.m_time);
		// 補間
		result = DirectX::XMQuaternionSlerp(
			prev.m_quat,
			next.m_quat,
			f
		);
	}

	return true;
}

bool KdAnimationData::Node::InterpolateScales(Math::Vector3& result, float time)
{
	if (m_scales.size() == 0)return false;

	// キー位置検索
	UINT keyIdx = BinarySearchNextAnimKey(m_scales, time);

	// 先頭のキーなら、先頭のデータを返す
	if (keyIdx == 0) {
		result = m_scales.front().m_vec;
		return true;
	}
	// 配列外のキーなら、最後のデータを返す
	else if (keyIdx >= m_scales.size()) {
		result = m_scales.back().m_vec;
		return true;
	}
	// それ以外(中間の時間)なら、その時間の値を補間計算で求める
	else {
		auto& prev = m_scales[keyIdx - 1];	// 前のキー
		auto& next = m_scales[keyIdx];		// 次のキー
		// 前のキーと次のキーの時間から、0～1間の時間を求める
		float f = (time - prev.m_time) / (next.m_time - prev.m_time);
		// 補間
		result = DirectX::XMVectorLerp(
			prev.m_vec,
			next.m_vec,
			f
		);
	}

	return true;
}

void KdAnimationData::Node::Interpolate(Math::Matrix& rDst, float time)
{
	// ベクターによる拡縮補間
	bool isChange = false;
	Math::Matrix scale;
	Math::Vector3 resultVec;
	if (InterpolateScales(resultVec, time))
	{
		scale = scale.CreateScale(resultVec);
		isChange = true;
	}

	// クォタニオンによる回転補間
	Math::Matrix rotate;
	Math::Quaternion resultQuat;
	if (InterpolateRotations(resultQuat, time))
	{
		rotate = rotate.CreateFromQuaternion(resultQuat);
		isChange = true;
	}

	// ベクターによる座標補間
	Math::Matrix trans;
	if (InterpolateTranslations(resultVec, time))
	{
		trans = trans.CreateTranslation(resultVec);
		isChange = true;
	}

	if (isChange)
	{
		rDst = scale * rotate * trans;
	}
}

void KdAnimator::AdvanceTime(std::vector<KdModelWork::Node>& rNodes, float speed)
{
	if (!m_spAnimation) { return; }

	if (m_isBlending && m_spNextAnimation)
	{
		float t = m_blendTime / m_blendDuration;
		if (t > 1.0f) t = 1.0f;

		for (size_t i = 0; i < rNodes.size(); ++i)
		{
			Math::Matrix lastMatrix, nextFirstMatrix, lerpedMatrix;

			// 現在アニメーションの最後
			if (i < m_spAnimation->m_nodes.size())
				m_spAnimation->m_nodes[i].Interpolate(lastMatrix, m_spAnimation->m_maxLength);

			// 次アニメーションの最初
			if (i < m_spNextAnimation->m_nodes.size())
				m_spNextAnimation->m_nodes[i].Interpolate(nextFirstMatrix, 0.0f);

			Math::Vector3 scale, translation;
			Math::Quaternion rotation;

			Math::Vector3 resultScale, resultTranslation;
			Math::Quaternion resultRotation;

			lastMatrix.Decompose(scale, rotation, translation);
			nextFirstMatrix.Decompose(resultScale, resultRotation, resultTranslation);

			Math::Vector3 lerpScale = DirectX::XMVectorLerp(scale, resultScale, t);
			Math::Quaternion lerpRotation = DirectX::XMQuaternionSlerp(rotation, resultRotation, t);
			Math::Vector3 lerpTranslation = DirectX::XMVectorLerp(translation, resultTranslation, t);

			Math::Matrix transMat = Math::Matrix::CreateTranslation(lerpTranslation);
			Math::Matrix scaleMat = Math::Matrix::CreateScale(lerpScale);
			Math::Matrix rotateMat = Math::Matrix::CreateFromQuaternion(lerpRotation);

			lerpedMatrix = scaleMat * rotateMat * transMat;

			rNodes[i].m_localTransform = lerpedMatrix;
		}

		m_blendTime += speed;
		if (m_blendTime >= m_blendDuration)
		{
			// 補間終了
			m_spAnimation = m_spNextAnimation;
			m_time = 0.0f;
			m_isBlending = false;
			m_spNextAnimation = nullptr;
		}
		return;
	}

	// 通常のアニメーション進行
	for (auto& rAnimNode : m_spAnimation->m_nodes)
	{
		UINT idx = rAnimNode.m_nodeOffset;
		rAnimNode.Interpolate(rNodes[idx].m_localTransform, m_time);
	}

	m_time += speed;

	if (m_time >= m_spAnimation->m_maxLength)
	{
		if (m_isLoop)
		{
			m_time = 0.0f;
		}
		else
		{
			m_time = m_spAnimation->m_maxLength;
		}
	}
}

void KdAnimator::AnimationBlend(const std::shared_ptr<KdAnimationData>& nextAnim, float duration)
{
	m_spNextAnimation = nextAnim;
	m_blendTime = 0.0f;
	m_blendDuration = duration;
	m_isBlending = true;
}
