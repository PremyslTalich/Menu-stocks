#if defined _menu_stocks_included_
  #endinput
#endif
#define _menu_stocks_included_ "1.0"

//Include by KissLick
//Syntax update by Keith Warren (Drixevel) (redwerewolf on Allied Mods)

#include <menus>

stock void PushMenuString(Handle hndl, const char[] id, const char[] data)
{
	AddMenuItem(hndl, id, data, ITEMDRAW_IGNORE);
}

stock void PushMenuCell(Handle hndl, const char[] id, int data)
{
	char DataString[64];
	IntToString(data, DataString, sizeof(DataString));
	AddMenuItem(hndl, id, DataString, ITEMDRAW_IGNORE);
}

stock void PushMenuFloat(Handle hndl, const char[] id, float data)
{
	char DataString[64];
	FloatToString(data, DataString, sizeof(DataString));
	AddMenuItem(hndl, id, DataString, ITEMDRAW_IGNORE);
}

stock bool GetMenuString(Handle hndl, const char[] id, char[] Buffer, int size)
{
	int ItemCount = GetMenuItemCount(hndl);
	char info[64]; char data[64];
	
	for (int i = 0; i < ItemCount; i++) {
		GetMenuItem(hndl, i, info, sizeof(info), _, data, sizeof(data));
		
		if (StrEqual(info, id)) {	
			strcopy(Buffer, size, data);
			return true;
		}
	}	
	return false;
}

stock int GetMenuCell(Handle hndl, const char[] id, int DefaultValue = 0)
{
	int ItemCount = GetMenuItemCount(hndl);
	char info[64]; char data[64];
	
	for (int i = 0; i < ItemCount; i++) {
		GetMenuItem(hndl, i, info, sizeof(info), _, data, sizeof(data));
		
		if (StrEqual(info, id))
			return StringToInt(data);
	}	
	return DefaultValue;
}

stock float GetMenuFloat(Handle hndl, const char[] id, float DefaultValue = 0.0)
{
	int ItemCount = GetMenuItemCount(hndl);
	char info[64]; char data[64];
	
	for (int i = 0; i < ItemCount; i++) {
		GetMenuItem(hndl, i, info, sizeof(info), _, data, sizeof(data));
		
		if (StrEqual(info, id))
			return StringToFloat(data);
	}	
	return DefaultValue;
}

stock void CopyMenuAny(Handle source, Handle dest, const char[] id)
{
	int ItemCount = GetMenuItemCount(source);
	char info[64]; char data[64];
	
	for (int i = 0; i < ItemCount; i++) {
		GetMenuItem(source, i, info, sizeof(info), _, data, sizeof(data));
		
		if (StrEqual(info, id))
			AddMenuItem(dest, id, data, ITEMDRAW_IGNORE);
	}
}

stock bool AddMenuItemFormat(Handle& menu, const char[] info, int style = ITEMDRAW_DEFAULT, const char[] format, any ...)
{
	char display[128];
	VFormat(display, sizeof(display), format, 5);
	
	return view_as<bool>(AddMenuItem(menu, info, display, style));
}
