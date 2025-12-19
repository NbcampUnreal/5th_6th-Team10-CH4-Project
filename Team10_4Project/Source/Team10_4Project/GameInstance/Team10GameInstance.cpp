// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/Team10GameInstance.h"


// 서버에 접속 전 임시로 닉네임을 저장하고 서버에 접속 후 gamemode에서 playerstate의 닉네임 재설정
void UTeam10GameInstance::SaveNickName(FString InNickName)
{
	NickName = InNickName;
}
