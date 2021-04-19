#pragma once
#include "nvse/GameRTTI.h"
#include "nvse/PluginAPI.h"
#include "nvse/GameObjects.h"
#include "nvse/GameEffects.h"
#include "nvse/GameData.h"
#include "nvse/GameProcess.h"
#include "nvse/GameSettings.h"
#include "nvse/GameTasks.h"
#include "nvse/GameOSDepend.h"
#include "nvse/GameUI.h"
#include "nvse/SafeWrite.h"
#include "internal/decoding.h"
#include "nvse/Utilities.h"
//IDebugLog gLog;

uintptr_t kOriginalGetActorValueInt = 0;
uintptr_t kOriginalGetActorValueF = 0;
DialogMenu** g_DialogMenu = (DialogMenu**)0x11D9510;
bool g_bAffectAllSkills = 0;
unsigned int g_iMode = 0;
bool g_bUseNPCIntelligence = 0;
bool ShouldModifyForDialogueSpeechAndBarter(UInt32 avCode)
{
	switch (avCode)
	{
	case kAVCode_Barter:
	case kAVCode_Speech:
		return true;
	default:
		return false;
	}

}




bool ShouldModifyForDialogueALL(UInt32 avCode)
{
	switch (avCode)
	{
	case kAVCode_Strength:
	case kAVCode_Perception:
	case kAVCode_Endurance:
	case kAVCode_Intelligence:
	case kAVCode_Agility:
	case kAVCode_Luck:
	case kAVCode_Barter:
	case kAVCode_BigGuns:
	case kAVCode_EnergyWeapons:
	case kAVCode_Explosives:
	case kAVCode_Lockpick:
	case kAVCode_Medicine:
	case kAVCode_MeleeWeapons:
	case kAVCode_Repair:
	case kAVCode_Science:
	case kAVCode_Guns:
	case kAVCode_Speech:
	case kAVCode_Sneak:
	case kAVCode_Survival:
	case kAVCode_Unarmed:
		return true;
	default:
		return false;
	}

}


bool ShouldModifyForDialogue_wrap(UInt32 avCode)
{
	if (!*g_DialogMenu) return false;
	if (g_bAffectAllSkills) return ShouldModifyForDialogueALL(avCode);
	else  return ShouldModifyForDialogueSpeechAndBarter(avCode);
}


void GetPlayerAndTargetCHR(float& PlayerCHR, float& TargetCHR)
{
	PlayerCHR = 5;
	TargetCHR = 5;
	if (auto ThePlayerS = PlayerCharacter::GetSingleton()) {
		if (auto dMenu = *g_DialogMenu)
		{
			Actor* menuRef = (Actor*)(dMenu->partnerRef);
			if (menuRef && IS_TYPE(((TESObjectREFR*)menuRef)->baseForm, BGSTalkingActivator))
			{
				ExtraTalkingActor* xTalker = GetExtraType(((TESObjectREFR*)menuRef)->extraDataList, TalkingActor);
				if (xTalker) menuRef = xTalker->actor;
				else menuRef = NULL;

			}			
			if (menuRef)
			{
				PlayerCHR = ThePlayerS->avOwner.GetActorValue(kAVCode_Charisma);
				if (g_bUseNPCIntelligence) 	TargetCHR = menuRef->avOwner.GetActorValue(kAVCode_Intelligence);
				else TargetCHR = menuRef->avOwner.GetActorValue(kAVCode_Charisma);
			}
			//return (1 + (ThePlayerS->avOwner.GetActorValue(kAVCode_Charisma) - menuRef->avOwner.GetActorValue(kAVCode_Charisma))*0.05);
		}

	}
}


double ReturnAVScale(UInt32 avCode)
{

	switch (avCode)
	{
	case kAVCode_Strength:
	case kAVCode_Perception:
	case kAVCode_Endurance:
	case kAVCode_Charisma:
	case kAVCode_Intelligence:
	case kAVCode_Agility:
	case kAVCode_Luck:
		return 10;
	default:
		return 100;
	}
}

double ModifyAVByTargetCHR(float AVBase, float PlayerCHR, float TargetCHR, UInt32 avCode)
{
	double toReturn = AVBase;
	switch (g_iMode)
	{
	case 0:
		toReturn *= 1 + (PlayerCHR - TargetCHR) * 0.05;
		break;
	case 1:
		toReturn += ((PlayerCHR - TargetCHR) * (0.05 * ReturnAVScale(avCode)));
		break;
	default:
		break;
	}
	if (toReturn < 0 && AVBase >= 0) toReturn = 0;
	return toReturn;
}










int __fastcall hk_GetActorValueInt(ActorValueOwner* avOwner, void* edx, UInt32 avCode)
{
	int returnValueOriginal = ThisStdCall<int>(kOriginalGetActorValueInt, avOwner, avCode); 
	auto returnValue = returnValueOriginal;
	if (ShouldModifyForDialogue_wrap(avCode))
	{
		float PlayerCHR = 5, TargetCHR = 5;
		GetPlayerAndTargetCHR(PlayerCHR, TargetCHR);
		returnValue = ModifyAVByTargetCHR(returnValueOriginal, PlayerCHR, TargetCHR, avCode);

	}
	return returnValue;

}



double __fastcall hk_GetActorValueFloat(ActorValueOwner* avOwner, void* edx, UInt32 avCode)
{
	double returnValueOriginal = ((double(__thiscall*)(ActorValueOwner*, UInt32))kOriginalGetActorValueF)(avOwner, avCode);
	auto returnValue = returnValueOriginal;
	if (ShouldModifyForDialogue_wrap(avCode))
	{
		float PlayerCHR = 5, TargetCHR = 5;
		GetPlayerAndTargetCHR(PlayerCHR, TargetCHR);
		returnValue = ModifyAVByTargetCHR(returnValueOriginal,PlayerCHR, TargetCHR, avCode);
	}
	return returnValue;

}