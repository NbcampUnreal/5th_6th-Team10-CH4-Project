# 프로젝트 빌드 설정 및 시작 가이드

## 1. 프로젝트 생성 (Unreal Engine 5.6)

### 프로젝트 설정
```
템플릿: Third Person (C++)
프로젝트 이름: SurvivalGame
경로: 원하는 위치
```

### Build.cs 설정 (SurvivalGame.Build.cs)

```csharp
// SurvivalGame.Build.cs (UE 5.6)
using UnrealBuildTool;

public class SurvivalGame : ModuleRules
{
    public SurvivalGame(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // UE 5.6 설정
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
        CppStandard = CppStandardVersion.Cpp20;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",

            // GAS 모듈
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",

            // State Tree (UE 5.6 정식)
            "StateTreeModule",

            // 네트워킹
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "Sockets",
            "Networking",

            // UI
            "UMG",
            "Slate",
            "SlateCore",

            // UE 5.6 권장
            "CommonUI",
            "AIModule",
            "NavigationSystem"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks"
        });

        // Voice Chat (UE 5.6 - EOSVoiceChat 권장)
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicDependencyModuleNames.Add("EOSVoiceChat");
        }
    }
}
```

## 2. 데디서버 타겟 파일 생성

### SurvivalGameServer.Target.cs

```csharp
// Source/SurvivalGameServer.Target.cs (UE 5.6)
using UnrealBuildTool;
using System.Collections.Generic;

public class SurvivalGameServerTarget : TargetRules
{
    public SurvivalGameServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V5; // UE 5.6
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
        ExtraModuleNames.Add("SurvivalGame");

        // 서버 최적화
        bUseLoggingInShipping = true;
        bCompileWithPluginSupport = true;
        bUseUnityBuild = true;
        bUsePCHFiles = true;
    }
}
```

## 3. DefaultEngine.ini 전체 설정

```ini
[/Script/EngineSettings.GameMapsSettings]
GameDefaultMap=/Game/Maps/MainMenu
EditorStartupMap=/Game/Maps/MainMenu
GlobalDefaultGameMode=/Script/SurvivalGame.SurvivalGameMode
ServerDefaultMap=/Game/Maps/GameMap

[/Script/Engine.Engine]
+ActiveGameNameRedirects=(OldGameName="TP_ThirdPerson",NewGameName="/Script/SurvivalGame")
+ActiveGameNameRedirects=(OldGameName="/Script/TP_ThirdPerson",NewGameName="/Script/SurvivalGame")
+NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="/Script/OnlineSubsystemUtils.IpNetDriver",DriverClassNameFallback="/Script/OnlineSubsystemUtils.IpNetDriver")

[/Script/OnlineSubsystemUtils.IpNetDriver]
NetServerMaxTickRate=60
MaxNetTickRate=60
MaxInternetClientRate=25000
MaxClientRate=25000
InitialConnectTimeout=30.0
ConnectionTimeout=30.0

[OnlineSubsystem]
DefaultPlatformService=Null
bHasVoiceEnabled=true

[OnlineSubsystemNull]
bEnabled=true

[Voice]
bEnabled=true

[/Script/Engine.GameEngine]
+NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="/Script/OnlineSubsystemUtils.IpNetDriver")

; GAS 설정
[/Script/GameplayAbilities.AbilitySystemGlobals]
AbilitySystemGlobalsClassName="/Script/SurvivalGame.SurvivalAbilitySystemGlobals"
GlobalGameplayCueManagerClass="/Script/GameplayAbilities.GameplayCueManager"
bPreloadGameplayAbilities=true

; Replication
[/Script/Engine.Player]
ConfiguredInternetSpeed=25000
ConfiguredLanSpeed=50000
```

## 4. DefaultGame.ini 설정

```ini
[/Script/EngineSettings.GeneralProjectSettings]
ProjectID=YOUR_PROJECT_ID
ProjectName=Survival Game
CompanyName=Your Team
CopyrightNotice=Copyright 2024

[/Script/UnrealEd.ProjectPackagingSettings]
Build=IfProjectHasCode
BuildConfiguration=PPBC_Shipping
StagingDirectory=(Path="D:/Builds/SurvivalGame")
FullRebuild=False
ForDistribution=True
IncludeDebugFiles=False
BlueprintNativizationMethod=Disabled
bIncludeNativizedAssetsInProjectGeneration=False
bExcludeMonolithicEngineHeadersInNativizedCode=False
UsePakFile=True
bUseIoStore=True
bMakeBinaryConfig=False
bGenerateChunks=False
bGenerateNoChunks=False
bChunkHardReferencesOnly=False
bForceOneChunkPerFile=False
MaxChunkSize=0
bBuildHttpChunkInstallData=False
HttpChunkInstallDataDirectory=(Path="")
bCompressed=True
PakFileCompressionFormats=Oodle
bForceUseProjectCompressionFormatIgnoreHardwareOverride=False
PakFileAdditionalCompressionOptions=-compressionblocksize=1MB
PakFileCompressionMethod=Kraken
PakFileCompressionLevel_DebugDevelopment=4
PakFileCompressionLevel_TestShipping=5
PakFileCompressionLevel_Distribution=7
HttpChunkInstallDataVersion=
IncludePrerequisites=True
IncludeAppLocalPrerequisites=False
bShareMaterialShaderCode=True
bDeterministicShaderCodeOrder=False
bSharedMaterialNativeLibraries=True
ApplocalPrerequisitesDirectory=(Path="")
IncludeCrashReporter=False
InternationalizationPreset=English
-CulturesToStage=en
+CulturesToStage=en
LocalizationTargetCatchAllChunkId=0
bCookAll=False
bCookMapsOnly=True
bSkipEditorContent=True
bSkipMovies=True
+MapsToCook=(FilePath="/Game/Maps/MainMenu")
+MapsToCook=(FilePath="/Game/Maps/GameMap")
+MapsToCook=(FilePath="/Game/Maps/ResultMap")
```

## 5. GameplayTags.ini 설정

```ini
; Config/DefaultGameplayTags.ini
[/Script/GameplayTags.GameplayTagsSettings]
ImportTagsFromConfig=True
WarnOnInvalidTags=True
ClearInvalidTags=False
FastReplication=True
InvalidTagCharacters="\"\',"
NumBitsForContainerSize=6
NetIndexFirstBitSegment=16

; 캐릭터 상태
+GameplayTagList=(Tag="Character.State.Alive",DevComment="살아있음")
+GameplayTagList=(Tag="Character.State.Dead",DevComment="사망")
+GameplayTagList=(Tag="Character.State.Infected",DevComment="감염됨")
+GameplayTagList=(Tag="Character.State.Citizen",DevComment="시민")

; 어빌리티
+GameplayTagList=(Tag="Ability.Attack",DevComment="공격")
+GameplayTagList=(Tag="Ability.Heal",DevComment="치료")
+GameplayTagList=(Tag="Ability.UseItem",DevComment="아이템 사용")
+GameplayTagList=(Tag="Ability.Interact",DevComment="상호작용")

; 효과
+GameplayTagList=(Tag="Effect.Damage",DevComment="데미지")
+GameplayTagList=(Tag="Effect.Heal",DevComment="치료")
+GameplayTagList=(Tag="Effect.Buff",DevComment="버프")
+GameplayTagList=(Tag="Effect.Debuff",DevComment="디버프")
+GameplayTagList=(Tag="Effect.Infection",DevComment="감염")

; 아이템
+GameplayTagList=(Tag="Item.Weapon.Melee",DevComment="근접 무기")
+GameplayTagList=(Tag="Item.Weapon.Ranged",DevComment="원거리 무기")
+GameplayTagList=(Tag="Item.Consumable.HealthPack",DevComment="힐팩")

; 게임 페이즈
+GameplayTagList=(Tag="Game.Phase.Lobby",DevComment="로비")
+GameplayTagList=(Tag="Game.Phase.Phase1",DevComment="1단계")
+GameplayTagList=(Tag="Game.Phase.Phase2",DevComment="2단계")
+GameplayTagList=(Tag="Game.Phase.Phase3",DevComment="3단계")
+GameplayTagList=(Tag="Game.Phase.Vote",DevComment="투표")
+GameplayTagList=(Tag="Game.Phase.End",DevComment="종료")

; 데이터 전달
+GameplayTagList=(Tag="Data.Damage",DevComment="데미지 값")
+GameplayTagList=(Tag="Data.Healing",DevComment="치료 값")
```

## 6. Git 설정 (.gitignore)

```gitignore
# Visual Studio
.vs/
*.sln
*.suo
*.user
*.userosscache
*.sln.docstates

# Unreal Engine
Binaries/
Build/
DerivedDataCache/
Intermediate/
Saved/
*.VC.db
*.opensdf
*.opendb
*.sdf
*.suo
*.xcodeproj
*.xcworkspace

# 예외 (커밋해야 할 것들)
!Content/**/*.uasset
!Content/**/*.umap

# 플러그인 Binaries/Intermediate
Plugins/*/Binaries/
Plugins/*/Intermediate/

# 에셋 대용량 파일
*.fbx
*.obj
*.mp3
*.wav
*.tga
*.psd

# Starter Content는 제외
Content/StarterContent/
```

## 7. 프로젝트 초기 설정 체크리스트

### 필수 플러그인 활성화
```
1. Gameplay Abilities (GAS) - 활성화
2. State Tree - 활성화
3. Online Subsystem - 활성화
4. Online Subsystem NULL - 활성화 (테스트용)
5. Voice Chat (선택사항)
```

### 에디터 설정
```
Edit > Project Settings:

1. Maps & Modes:
   - Default GameMode: SurvivalGameMode
   - Default Pawn Class: SurvivalCharacter

2. Input:
   - Enhanced Input 사용

3. Engine - Network:
   - Max Players: 16
   - Use UPnP: True (테스트용)

4. Gameplay Tags:
   - Config 파일에서 Import: True
```

## 8. 각 팀원별 시작 작업

### 강태웅 (캐릭터)
```
1. Source/SurvivalGame/Characters/ 폴더 생성
2. SurvivalCharacter.h/cpp 구현
3. CharacterAttributeSet.h/cpp 구현
4. InteractionComponent.h/cpp 구현
5. Blueprint: BP_SurvivalCharacter 생성
```

### 서원준 (게임모드)
```
1. Source/SurvivalGame/GameModes/ 폴더 생성
2. SurvivalGameMode.h/cpp 구현
3. SurvivalGameState.h/cpp 구현
4. Content/StateTrees/ 폴더 생성
5. StateTree Asset: ST_GameFlow 생성
```

### 김세훈, 조기혁 (InGame UI)
```
1. Source/SurvivalGame/UI/InGame/ 폴더 생성
2. InGameHUD.h/cpp 구현
3. InventoryWidget.h/cpp 구현
4. ChatWidget.h/cpp 구현
5. Content/UI/InGame/ 폴더에 WBP 생성
```

### 김현수 (OutGame UI)
```
1. Source/SurvivalGame/UI/OutGame/ 폴더 생성
2. MainMenuWidget.h/cpp 구현
3. LobbyWidget.h/cpp 구현
4. Content/UI/OutGame/ 폴더에 WBP 생성
5. Content/Maps/MainMenu 맵 생성
```

### 한금성 (Gimmick)
```
1. Source/SurvivalGame/Gimmicks/ 폴더 생성
2. InteractableActor.h/cpp 구현
3. HealthPackActor.h/cpp 구현
4. MissionObjectActor.h/cpp 구현
5. SafeZoneActor.h/cpp 구현
6. Content/StateTrees/Gimmicks/ 생성
```

## 9. 빌드 및 테스트 순서

### 1단계: 로컬 빌드 확인
```bash
# Visual Studio에서 빌드
1. Development Editor 빌드
2. 에디터 실행 확인
3. 기본 GameMode/Character 동작 확인
```

### 2단계: 네트워크 테스트
```bash
# 에디터에서 멀티플레이어 테스트
1. Play Settings:
   - Number of Players: 2
   - Net Mode: Play As Listen Server
2. 기본 이동/상호작용 동기화 확인
```

### 3단계: 데디서버 빌드
```bash
# 1. 서버 빌드
Right Click .uproject > Build > SurvivalGameServer (Development)

# 2. 클라이언트 빌드
Right Click .uproject > Build > SurvivalGame (Development)

# 3. 서버 실행
SurvivalGameServer.exe /Game/Maps/GameMap -log

# 4. 클라이언트 연결
SurvivalGame.exe 127.0.0.1:7777
```

## 10. 디버깅 팁

### 네트워크 디버깅
```cpp
// 로그 활성화
LogNet: Log
LogNetPlayerMovement: Log
LogNetTraffic: Log
LogOnline: Log

// 콘솔 명령어
stat net - 네트워크 통계
stat game - 게임 통계
displayall SurvivalCharacter Health - 모든 캐릭터 HP 표시
```

### GAS 디버깅
```cpp
// 콘솔 명령어
showdebug abilitysystem - GAS 디버그 정보
AbilitySystem.Debug.NextCategory - 다음 카테고리
```

---

## 다음 단계

1. **1주차**: 기본 프로젝트 구조 및 GAS 셋업
2. **2주차**: 캐릭터 이동/상호작용 구현 + 네트워크 동기화
3. **3주차**: 게임모드 페이즈 시스템 + UI 연동
4. **4주차**: 기믹 시스템 + 미션 오브젝트
5. **5주차**: 투표 시스템 + 채팅
6. **6주차**: 밸런싱 + 버그 수정 + 최적화
7. **최종주**: 발표 준비 + 시연 영상

화이팅! 🔥
