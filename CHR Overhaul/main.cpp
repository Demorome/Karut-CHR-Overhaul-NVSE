#include "main.h"

IDebugLog		gLog("CHROverhaul.log");


bool NVSEPlugin_Query(const NVSEInterface* nvse, PluginInfo* info)
{
	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = "Charisma Overhaul";
	info->version = 1;

	if (nvse->isEditor)
	{
		return false;
	}
	gLog.Open("CHROverhaul.log");
	if (nvse->runtimeVersion != RUNTIME_VERSION_1_4_0_525)
	{
		_MESSAGE("ERROR: Unsupported runtime version (%08X).", nvse->runtimeVersion);
		return false;
	}
	_MESSAGE("successful load");
	return true;
}


bool NVSEPlugin_Load(const NVSEInterface* nvse)
{
	kOriginalGetActorValueInt = *(uintptr_t*) 0x108A97C;
	kOriginalGetActorValueF = *(uintptr_t*)0x108A980;
	SafeWrite32(0x108A97C,(uintptr_t) hk_GetActorValueInt);
	SafeWrite32(0x108A980,(uintptr_t) hk_GetActorValueFloat);

	char iniDir[MAX_PATH];
	GetModuleFileNameA(GetModuleHandle(NULL), iniDir, MAX_PATH);
	strcpy((char*)(strrchr(iniDir, '\\') + 1), "Data\\NVSE\\Plugins\\CHROverhaul.ini");
	g_bAffectAllSkills = (bool) GetPrivateProfileInt("Main", "bAffectAllSkills", 0, iniDir);
	g_iMode = (bool)GetPrivateProfileInt("Main", "iMode", 0, iniDir);
	g_bUseNPCIntelligence = (bool)GetPrivateProfileInt("Main", "bUseNPCIntelligence", 0, iniDir);

	return true;
}







