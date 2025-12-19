
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
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Civilian, "GamePlayTags.PlayerRole.Civilian", "Tag for Player Role");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Infected, "GamePlayTags.PlayerRole.Infected", "Tag for Player Role");
	}
	
	namespace CivilianAbility
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "GamePlayTags.CivilianAbility.Primary", "Tag for Civilian Primary");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Seconary, "GamePlayTags.CivilianAbility.Secondary", "Tag for Civilian Seconary");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Shoot, "GamePlayTags.CivilianAbility.Shoot", "Tag for Civilian Shoot");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Reload, "GamePlayTags.CivilianAbility.Reload", "Tag for Civilian Reload");
	}
	
	namespace InfectedAbility
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "GamePlayTags.InfectedAbility.Primary", "Tag for Infected Primary");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Seconary, "GamePlayTags.InfectedAbility.Secondary", "Tag for Infected Seconary");
	}
}



