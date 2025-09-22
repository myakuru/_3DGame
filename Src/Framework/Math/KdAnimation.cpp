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

	// ブレンド中
	if (m_isBlending && m_spNextAnimation)
	{
		float t = (m_blendDuration > 0.0f) ? (m_blendTime / m_blendDuration) : 1.0f;
		if (t > 1.0f) t = 1.0f;

		// ブレンド開始時に一度だけ現行ポーズをキャプチャ
		if (!m_blendCaptured)
		{
			m_blendBasePoses.resize(rNodes.size());
			for (auto& rAnimNode : m_spAnimation->m_nodes)
			{
				// 対応するモデルノードのインデックス
				UINT idx = rAnimNode.m_nodeOffset;
				m_blendBasePoses[idx] = rNodes[idx].m_localTransform;
			}
			m_blendCaptured = true;
		}

		// 次アニメ側の再生時間（0 から進行）
		float nextTime = m_blendTime;

		// 各アニメノード（nodeOffset がモデル側インデックス）
		for (auto& nextAnimNode : m_spNextAnimation->m_nodes)
		{
			UINT nodeIdx = nextAnimNode.m_nodeOffset;
			if (nodeIdx >= rNodes.size()) continue;

			// 現行側：キャプチャ済みポーズ
			Math::Matrix& fromMat = m_blendBasePoses[nodeIdx];

			// 次アニメ側：現在の nextTime でサンプル
			Math::Matrix toMat;
			nextAnimNode.Interpolate(toMat, nextTime);

			// Decompose
			Math::Vector3 fromS, fromT;
			Math::Quaternion fromR;
			fromMat.Decompose(fromS, fromR, fromT);

			Math::Vector3 toS, toT;
			Math::Quaternion toR;
			toMat.Decompose(toS, toR, toT);

			// 補間
			Math::Vector3 blendS = DirectX::XMVectorLerp(fromS, toS, t);
			Math::Quaternion blendR = DirectX::XMQuaternionSlerp(fromR, toR, t);
			Math::Vector3 blendT = DirectX::XMVectorLerp(fromT, toT, t);

			// 再構築
			Math::Matrix m = Math::Matrix::CreateScale(blendS)
				* Math::Matrix::CreateFromQuaternion(blendR)
				* Math::Matrix::CreateTranslation(blendT);

			rNodes[nodeIdx].m_localTransform = m;
		}

		m_blendTime += speed;
		if (m_blendTime >= m_blendDuration)
		{
			// 完了：次を現行へ
			m_spAnimation = m_spNextAnimation;
			m_spNextAnimation = nullptr;
			m_time = 0.0f;
			m_isBlending = false;
			m_blendCaptured = false;
			m_blendBasePoses.clear();
			m_isLoop = m_nextIsLoop;
		}
		return;
	}

	// 通常進行（既存）
	for (auto& rAnimNode : m_spAnimation->m_nodes)
	{
		UINT idx = rAnimNode.m_nodeOffset;
		if (idx >= rNodes.size()) continue;
		rAnimNode.Interpolate(rNodes[idx].m_localTransform, m_time);
	}

	m_time += speed;

	if (m_time >= m_spAnimation->m_maxLength)
	{
		if (m_isLoop)
		{
			m_time = 0.0f;
			m_loopCount++;
			if (m_maxLoopCount > 0 && m_loopCount >= m_maxLoopCount)
			{
				m_isLoop = false;
			}
		}
		else
		{
			m_time = m_spAnimation->m_maxLength;
		}
	}
}

void KdAnimator::AnimationBlend(const std::shared_ptr<KdAnimationData>& nextAnim, float duration, bool nextIsLoop, int maxLoopCount)
{
	m_spNextAnimation = nextAnim;
	m_blendTime = 0.0f;
	m_time = 0.0f;
	m_blendDuration = duration;
	m_isBlending = true;
	m_nextIsLoop = nextIsLoop;
	m_maxLoopCount = maxLoopCount;
	m_blendCaptured = false; // 次フレームでキャプチャ
}

bool KdAnimator::GetRootMotion(const std::shared_ptr<KdAnimationData>& animData, const std::vector<KdModelData::Node>& modelNodes, const std::string& rootBoneName, float time, Math::Vector3& outTranslation) const
{
	if (!animData) return false;

	// ノードインデックス取得
	int boneIndex = -1;
	for (size_t i = 0; i < modelNodes.size(); ++i) {
		if (modelNodes[i].m_name == rootBoneName) {
			boneIndex = static_cast<int>(i);
			break;
		}
	}
	if (boneIndex == -1) return false;

	// アニメーションノード取得
	for (auto& node : animData->m_nodes) {
		if (node.m_nodeOffset == boneIndex) {
			return node.InterpolateTranslations(outTranslation, time);
		}
	}
	return false;
}
