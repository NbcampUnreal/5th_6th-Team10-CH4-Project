#pragma once

#include "CoreMinimal.h"
#include "MyAttributeSet.h"

// 최종 클래스 이름으로 교체필요
typedef UMyAttributeSet FAttributeSetClass;
// 최종 헤더 파일 경로로 교체필요
#define ATTRIBUTE_SET_HEADER_PATH "MyAttributeSet.h" 

// UI 업데이트에 필요한 Attribute 접근자 함수 정의 (UI 코드에서 사용)
#define GET_HEALTH_ATTRIBUTE FAttributeSetClass::GetHealthAttribute()
#define GET_MAX_HEALTH_ATTRIBUTE FAttributeSetClass::GetMaxHealthAttribute()
#define GET_STAMINA_ATTRIBUTE FAttributeSetClass::GetStaminaAttribute()
#define GET_MAX_STAMINA_ATTRIBUTE FAttributeSetClass::GetMaxStaminaAttribute()