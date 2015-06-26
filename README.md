# Menu-stocks

This include allows you to pass a value to menu callback. Just look at the examples ;-)

  ```SourcePawn
ShowMyMenu(iClient, iSecretValue)
{
	new Handle:hMenu = CreateMenu(MyMenu_Handler);

	SetMenuTitle(hMenu, "Choose your weapon:");
	AddMenuItem(hMenu, "weapon_m4a1", "M4A1");
	AddMenuItem(hMenu, "weapon_ak47", "AK-47");
	AddMenuItem(hMenu, "weapon_scout", "Scout");
	
	PushMenuString(hMenu, "-MySecretValue-", iSecretValue);
	
	SetMenuExitBackButton(hMenu, true);
	DisplayMenu(hMenu, iClient, 30);
}

public MyMenu_Handler(Handle:hMenu, MenuAction:iAction, iClient, iKey)
{
	switch (iAction) {
		case MenuAction_Select: {
			new String:sWeapon[64];
			GetMenuItem(hMenu, iKey, sWeapon, sizeof(sWeapon));
			GivePlayerItem(iClient, sWeapon);
			
			new iSecretValue = GetMenuCell(hMenu, "-MySecretValue-");		
			
			PrintToConsole("Player %N has secret value = %d.", iClient, iSecretValue);
		}
		case MenuAction_End: {
			CloseHandle(hMenu);
		}
	}
}
  ```
