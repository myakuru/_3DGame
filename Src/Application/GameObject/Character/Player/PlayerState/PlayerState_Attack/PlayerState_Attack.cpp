#include "PlayerState_Attack.h"

void PlayerState_Attack::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Attack");
	m_player->GetAnimator()->AnimationBlend(anime, 1.0f);
	m_player->AnimeSetFlg() = true;
}

void PlayerState_Attack::StateUpdate()
{
}

void PlayerState_Attack::StateEnd()
{
}
