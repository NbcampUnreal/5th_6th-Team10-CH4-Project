
#include "GamePlayTag/GamePlayTags.h"

namespace GamePlayTags
{
	namespace AreaTag
	{
		UE_DEFINE_GAMEPLAY_TAG(Area_Area1, "Area.Area1");
		UE_DEFINE_GAMEPLAY_TAG(Area_Area2, "Area.Area2");
		UE_DEFINE_GAMEPLAY_TAG(Area_Area3, "Area.Area3");
	}
	
	namespace PlayerRole
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Civilian, "PlayerRole.Civilian", "Tag for Player Role");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Infected, "PlayerRole.Infected", "Tag for Player Role");
	}
	
	namespace CivilianState
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Idle, "CivilianState.Idle", "Tag for Character CivilianState Info");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stun, "CivilianState.Stun", "Tag for Character CivilianState Info");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dead, "CivilianState.Dead", "Tag for Character CivilianState Info");
	}
	
	namespace CivilianAbility
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "CivilianAbility.Primary", "Tag for Civilian MeleeAttack");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Seconary, "CivilianAbility.Secondary", "Tag for Civilian MeleeAttack");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Shoot, "CivilianAbility.Shoot", "Tag for Civilian Shoot");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Reload, "CivilianAbility.Reload", "Tag for Civilian Reload");
	}
	
	namespace InfectedState
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Idle, "InfectedState.Idle", "Tag for Character InfectedState Info");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Morphing, "InfectedState.Morphing", "Tag for Character InfectedState Info");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Transformed, "InfectedState.Transformed", "Tag for Character InfectedState Info");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stun, "InfectedState.Stun", "Tag for Character InfectedState Info");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dead, "InfectedState.Dead", "Tag for Character InfectedState Info");
	}
	
	namespace InfectedAbility
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "InfectedAbility.Primary", "Tag for Infected MeleeAttack");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Seconary, "InfectedAbility.Secondary", "Tag for Infected MeleeAttack");
	}
	
	namespace Data
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage, "Data.Damage", "Tag for Data Format");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HealAmount, "Data.HealAmount", "Tag for Data Format");
	}
	
	namespace Effect
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Role, "Effect.Type.Role", "Tag for Effect Type");
	}
	
	namespace Ability
	{
		namespace Civilian
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stun, "Ability.Civilian.Stun", "Ability for Civilian Player");
		}
		namespace Infected
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Morph, "Ability.Infected.Morph", "Ability for Infected Player");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stun, "Ability.Infected.Stun", "Ability for Infected Player");
		}
	}

	namespace Gimmick {
		UE_DEFINE_GAMEPLAY_TAG(Item_Fuse, "Gimmick,.Item_Fuse");
		UE_DEFINE_GAMEPLAY_TAG(Status_HasFuse, "Gimmick.Status_HasFuse");
	}
}



