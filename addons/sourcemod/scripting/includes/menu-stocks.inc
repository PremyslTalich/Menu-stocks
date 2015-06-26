#if defined _downloadtableconfig_included_
	#endinput
#endif
#define _downloadtableconfig_included_ "0.8"

#include <sdktools>
#include <regex>

#define DTC_MAX_LINE_LEN 4096
#define DTC_MAX_PREFIX_LEN 1024
#define DTC_MAX_NAME_LEN 256
#define DTC_MAX_ARGS 32
#define DTC_MAX_ARG_LEN 256
#define DTC_DEFAULT_VALUE "_"

functag public DTC_OnFile(String:sFile[], String:sPrefixName[DTC_MAX_NAME_LEN], Handle:hArgs);
functag public DTC_OnFileNotFound(String:sFile[], String:sPrefixName[DTC_MAX_NAME_LEN], Handle:hArgs);
functag public DTC_OnCreateConfig(String:sConfigPath[], Handle:hConfigFile);

static Handle:g_hRegEx;

stock bool:DTC_CreateConfig(String:sConfigPath[], DTC_OnCreateConfig:OnCreateConfig = INVALID_FUNCTION, bool:bInsertHelperText = false)
{
	if (FileExists(sConfigPath)) {
		return false;
	}
	
	new Handle:hConfig = OpenFile(sConfigPath, "w");
	
	if (hConfig == INVALID_HANDLE) {
		LogError("Cannot create config '%s' !", sConfigPath);
		return false;
	}
	
	decl String:sPluginName[256];
	GetPluginFilename(INVALID_HANDLE, sPluginName, sizeof(sPluginName));
	WriteFileLine(hConfig, "// Downloads config for plugin \"%s\"\n", sPluginName);
	
	if (bInsertHelperText) {
		WriteFileLine(hConfig, "// comment");
		WriteFileLine(hConfig, "# comment");
		WriteFileLine(hConfig, "; comment\n");
		WriteFileLine(hConfig, "// Prefix description.");
		WriteFileLine(hConfig, "// Arg1 = \"default value\": Arg1 description");
		WriteFileLine(hConfig, "// Arg2 = \"default value\": Arg2 description");
		WriteFileLine(hConfig, "// Arg3 = \"default value\": Arg3 description");
		WriteFileLine(hConfig, "// -");
		WriteFileLine(hConfig, "// [Prefix_name <Arg1> <Arg2> <Arg3>]\n");
		WriteFileLine(hConfig, "// ----------------------------------------------------------------------------- //");
		WriteFileLine(hConfig, "// ----------------------------------------------------------------------------- //\n");
	}
	
	if (OnCreateConfig != INVALID_FUNCTION) {
		Call_StartFunction(INVALID_HANDLE, OnCreateConfig);
		Call_PushString(sConfigPath);
		Call_PushCell(hConfig);
		Call_Finish();
	}
	
	CloseHandle(hConfig);
	return true;
}

stock bool:DTC_LoadConfig(String:sConfigPath[], DTC_OnFile:OnFile = INVALID_FUNCTION, DTC_OnFileNotFound:OnFileNotFound = INVALID_FUNCTION, bool:bCallBackForPrefixesOnly = true, bool:bAutoDownload = true)
{
	new Handle:hConfig = OpenFile(sConfigPath, "r");
	
	if (hConfig == INVALID_HANDLE) {
		LogError("Config '%s' not found!", sConfigPath);
		return false;
	}
	
	new String:sFile[PLATFORM_MAX_PATH], String:sPrefix[DTC_MAX_PREFIX_LEN];
	new String:sPrefixName[DTC_MAX_NAME_LEN];
	new Handle:hArgs;
	new iCommentIndex;
	
	while (!IsEndOfFile(hConfig) && ReadFileLine(hConfig, sFile, sizeof (sFile)))
	{
		strcopy(sPrefix, 1, "");
		strcopy(sPrefixName, 1, "");
		
		if ((iCommentIndex = StrContains(sFile, "//")) != -1) {
			sFile[iCommentIndex] = '\0';
		}
		
		if ((iCommentIndex = StrContains(sFile, "#")) != -1) {
			sFile[iCommentIndex] = '\0';
		}
		
		if ((iCommentIndex = StrContains(sFile, ";")) != -1) {
			sFile[iCommentIndex] = '\0';
		}
		
		TrimString(sFile);
		
		if (StrEqual(sFile, "", false))
			continue;
		
		if (DTC_SplitString(sFile, sPrefix, sizeof(sPrefix), sFile, sizeof(sFile))) {
			hArgs = DTC_ParsePrefix(sPrefix, sPrefixName, sizeof(sPrefixName));
		}
		
		if (!FileExists(sFile, false) && !FileExists(sFile, true)) {
			if (OnFileNotFound == INVALID_FUNCTION) {
				LogError("File '%s' (prefix '%s') not found!", sFile, sPrefix);
			} else {
				Call_StartFunction(INVALID_HANDLE, OnFileNotFound);
				Call_PushString(sFile);
				Call_PushString(sPrefixName);
				Call_PushCell(hArgs);
				Call_Finish();
			}
		} else {
			if (bAutoDownload)
				AddFileToDownloadsTable(sFile);
			
			if (OnFile != INVALID_FUNCTION && (!bCallBackForPrefixesOnly || (bCallBackForPrefixesOnly && sPrefixName[0] != '\0'))) {
				Call_StartFunction(INVALID_HANDLE, OnFile);
				Call_PushString(sFile);
				Call_PushString(sPrefixName);
				Call_PushCell(hArgs);
				Call_Finish();
			}
		}
		
		if (hArgs != INVALID_HANDLE) {
			CloseHandle(hArgs);
			hArgs = INVALID_HANDLE;
		}
	}
	
	CloseHandle(hConfig);
	return true;
}

stock bool:DTC_CreateAndLoadConfig(String:sConfigPath[], DTC_OnCreateConfig:OnCreateConfig = INVALID_FUNCTION, DTC_OnFile:OnFile = INVALID_FUNCTION, DTC_OnFileNotFound:OnFileNotFound = INVALID_FUNCTION)
{
	return DTC_CreateConfig(sConfigPath, OnCreateConfig) && DTC_LoadConfig(sConfigPath, OnFile, OnFileNotFound);
}

stock bool:DTC_ParseString(const String:sLine[], String:sFile[], iMaxFileSize, String:sPrefixName[], iMaxPrefixNameSize, &Handle:hArgs = INVALID_HANDLE)
{
	new String:sPrefix[DTC_MAX_PREFIX_LEN];
	
	if (!DTC_SplitString(sLine, sPrefix, sizeof(sPrefix), sFile, iMaxFileSize)) {
		return false;
	} else {
		hArgs = DTC_ParsePrefix(sPrefix, sPrefixName, iMaxPrefixNameSize);
		return true;
	}
}

stock bool:DTC_SplitString(const String:sLine[], String:sPrefix[], iMaxPrefixSize, String:sFile[], iMaxFileSize)
{
	CreateRegEx();
	
	if (sLine[0] == '[' && MatchRegex(g_hRegEx, sLine) > 0) {
		GetRegexSubString(g_hRegEx, 0, sPrefix, iMaxPrefixSize);
		strcopy(sFile, iMaxFileSize, sLine[strlen(sPrefix)]);
		
		TrimString(sPrefix);
		TrimString(sFile);
		return true;
	} else {
		strcopy(sPrefix, iMaxPrefixSize, "");
		strcopy(sFile, iMaxFileSize, sLine);
		return false;
	}
}

stock bool:DTC_GetPrefix(const String:sLine[], String:sPrefix[], iMaxPrefixSize)
{
	CreateRegEx();
	
	if (sLine[0] == '[' && MatchRegex(g_hRegEx, sLine) > 0) {
		GetRegexSubString(g_hRegEx, 0, sPrefix, iMaxPrefixSize);
		TrimString(sPrefix);
		return true;
	} else {
		strcopy(sPrefix, iMaxPrefixSize, "");
		return false;
	}
}

stock bool:DTC_GetFile(const String:sLine[], String:sFile[], iMaxFileSize)
{
	decl String:sPrefix[DTC_MAX_PREFIX_LEN];
	if (DTC_GetPrefix(sLine, sPrefix, sizeof(sPrefix))) {
		strcopy(sFile, iMaxFileSize, sLine[strlen(sPrefix)]);
		TrimString(sFile);
		return true;
	} else {
		strcopy(sFile, iMaxFileSize, "");
		return false;
	}
}

stock Handle:DTC_ParsePrefix(const String:sLine[], String:sPrefixName[], iMaxPrefixNameSize)
{
	new String:sPrefix[DTC_MAX_PREFIX_LEN];
	strcopy(sPrefix, sizeof(sPrefix), sLine);
	strcopy(sPrefixName, iMaxPrefixNameSize, "");
	TrimString(sPrefix);
	
	if (sPrefix[0] != '[' || sPrefix[strlen(sPrefix) - 1] != ']') {
		if (!DTC_GetPrefix(sLine, sPrefix, sizeof(sPrefix))) {
			return INVALID_HANDLE;
		}
	}
	
	new String:sArgument[DTC_MAX_ARG_LEN];
	new iPos, iChangePos;
	new Handle:hArgs = CreateArray(DTC_MAX_ARG_LEN);
	
	for (new i = 0; i < DTC_MAX_ARGS; i++) {
		iChangePos = BreakString(sPrefix[iPos], sArgument, DTC_MAX_ARG_LEN);
		
		if (iChangePos == -1) {
			new iClosingBracket = FindCharInString(sArgument, ']', true);
			
			if (iClosingBracket != -1) {
				sArgument[iClosingBracket] = '\0';
			}			
			TrimString(sArgument);
		}
		
		if (i == 0) {
			strcopy(sPrefixName, iMaxPrefixNameSize, sArgument);
		} else {
			PushArrayString(hArgs, sArgument);
		}
		
		if (iChangePos == -1) {
			break;
		}
		
		iPos += iChangePos;
	}
	
	if (sPrefixName[0] != '\0') {
		ReplaceStringEx(sPrefixName, iMaxPrefixNameSize, "[", "");
		TrimString(sPrefixName);
	} else {
		CloseHandle(hArgs);
		return INVALID_HANDLE;
	}
	
	if (GetArraySize(hArgs) == 0) {
		CloseHandle(hArgs);
		return INVALID_HANDLE;
	}
	
	return hArgs;
}

stock bool:DTC_GetPrefixName(const String:sLine[], String:sPrefixName[], iMaxPrefixNameSize)
{
	CreateRegEx();
	
	if (sLine[0] == '[' && MatchRegex(g_hRegEx, sLine) > 0) {
		BreakString(sLine, sPrefixName, iMaxPrefixNameSize);
		TrimString(sPrefixName);
		ReplaceStringEx(sPrefixName, iMaxPrefixNameSize, "[", "");
		
		new iLastChar = strlen(sPrefixName) - 1;
		if (sPrefixName[iLastChar] == ']') {
			sPrefixName[iLastChar] = '\0';
		}
		return true;
	} else {
		strcopy(sPrefixName, iMaxPrefixNameSize, "");
		return false;
	}
}

stock Handle:DTC_GetArgs(const String:sLine[])
{
	new String:sPrefix[DTC_MAX_PREFIX_LEN];
	
	if (DTC_GetPrefix(sLine, sPrefix, sizeof(sPrefix))) {
		new Handle:hArgs = CreateArray(DTC_MAX_ARG_LEN);
		new String:sArgument[DTC_MAX_ARG_LEN];
		new iPos, iChangePos;
		
		for (new i = 0; i < DTC_MAX_ARGS; i++) {
			iChangePos = BreakString(sPrefix[iPos], sArgument, DTC_MAX_ARG_LEN);
			
			if (iChangePos == -1) {
				new iClosingBracket = FindCharInString(sArgument, ']', true);
				
				if (iClosingBracket != -1) {
					sArgument[iClosingBracket] = '\0';
				}
				TrimString(sArgument);
			}
			
			if (i > 0) {
				PushArrayString(hArgs, sArgument);
			}
			
			if (iChangePos == -1) {
				break;
			}
			
			iPos += iChangePos;
		}
		
		if (GetArraySize(hArgs) == 0) {
			CloseHandle(hArgs);
			hArgs = INVALID_HANDLE;
		}
		
		return hArgs;
	} else {
		return INVALID_HANDLE;
	}
}

stock bool:DTC_StrHasPrefix(const String:sLine[], const String:sPrefixName[], bCaseSensitive = true)
{
	if (sLine[0] != '[')
		return false;
	
	new String:sRegEx[DTC_MAX_PREFIX_LEN + 24];
	new flags = !bCaseSensitive ? PCRE_CASELESS : 0;
	Format(sRegEx, sizeof(sRegEx), "^\\[%s (?:[^\\\\\\]]|\\\\.)*\\]", sPrefixName);
	
	return (SimpleRegexMatch(sLine, sRegEx, flags) > 0 );
}

stock DTC_GetArgsCount(Handle:hArgs)
{
	return hArgs != INVALID_HANDLE ? GetArraySize(hArgs) : -1;
}

stock bool:DTC_GetArg(Handle:hArgs, iArg, String:sValue[], iValueSize, const String:sDefaultValue[] = "")
{
	iArg--;
	
	if (hArgs == INVALID_HANDLE || iArg >= GetArraySize(hArgs) || iArg < 0) {
		strcopy(sValue, iValueSize, sDefaultValue);
		return false;
	} else {
		new String:sArg[DTC_MAX_ARG_LEN];
		GetArrayString(hArgs, iArg, sArg, sizeof(sArg));
		
		if (StrEqual(sArg, DTC_DEFAULT_VALUE)) {
			strcopy(sValue, iValueSize, sDefaultValue);
			return false;
		} else {
			strcopy(sValue, iValueSize, sArg);
			return true;
		}
	}
}

stock DTC_GetArgNum(Handle:hArgs, iArg, iDefaultValue = 0)
{
	new String:sArg[DTC_MAX_ARG_LEN];
	if (!DTC_GetArg(hArgs, iArg, sArg, sizeof(sArg))) {
		return iDefaultValue;
	} else {
		return StringToInt(sArg);
	}
}

stock Float:DTC_GetArgFloat(Handle:hArgs, iArg, Float:fDefaultValue = 0.0)
{
	new String:sArg[DTC_MAX_ARG_LEN];
	if (!DTC_GetArg(hArgs, iArg, sArg, sizeof(sArg))) {
		return fDefaultValue;
	} else {
		return StringToFloat(sArg);
	}
}

stock bool:DTC_ExistArgValue(Handle:hArgs, const String:sSearchedValue[], bool:bCaseSensitive = true)
{
	new iSize;	
	if ((iSize = GetArraySize(hArgs)) < 1) {
		return false;
	} else {
		new String:sArg[DTC_MAX_ARG_LEN];
		for (new i = 0; i < iSize; i++) {
			GetArrayString(hArgs, i, sArg, sizeof(sArg));
			
			if (StrEqual(sArg, sSearchedValue, bCaseSensitive)) {
				return true;
			}
		}		
		return false;
	}
}

static stock CreateRegEx()
{
	if (g_hRegEx == INVALID_HANDLE) {
		g_hRegEx = CompileRegex("^\\[(?:[^\\\\\\]]|\\\\.)*\\]");
	}
}

static stock bool:CopySubStr(const String:sSource[], iFrom, iLength, String:sTarget[], iMaxTargetSize)
{
	if (iMaxTargetSize < iLength) {
		return false;
	}
	
	new iSourceLength = strlen(sSource);
	new i;
	
	for (i = 0; i < iLength; i++) {
		if (i < iMaxTargetSize && (iFrom + i) < iSourceLength) {
			sTarget[i] = sSource[iFrom + i]
		} else {
			return false;
		}
	}
	
	if (i > 0 && (i + 1) < iMaxTargetSize) {
		sTarget[i + 1] = '\0';
		return true;
	} else {
		return false;
	}	
}
