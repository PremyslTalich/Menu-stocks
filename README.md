# Menu-stocks

This include allows you to pass a value (cell, float or string) to menu callback.<br/>
Just look at the example ;-) [Download](https://raw.githubusercontent.com/KissLick/Menu-stocks/master/addons/sourcemod/scripting/includes/menu-stocks.inc)

  ```SourcePawn
ShowMyMenu(iClient, iSecretValue)
{
	new Handle:hMenu = CreateMenu(MyMenu_Handler);

	SetMenuTitle(hMenu, "Choose your weapon:");
	AddMenuItem(hMenu, "weapon_m4a1", "M4A1");
	AddMenuItem(hMenu, "weapon_ak47", "AK-47");
	AddMenuItem(hMenu, "weapon_scout", "Scout");
	
	// here is the magic
	PushMenuCell(hMenu, "-MySecretValue-", iSecretValue);
	
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
			
			// here is the magic
			new iSecretValue = GetMenuCell(hMenu, "-MySecretValue-");		
			
			PrintToConsole("Player %N has secret value = %d.", iClient, iSecretValue);
		}
		case MenuAction_End: {
			CloseHandle(hMenu);
		}
	}
}
  ```

Let's say, you need to pass a value from one menu to another
  ```SourcePawn
public MyMenu_Handler(Handle:hMenu, MenuAction:iAction, iClient, iKey)
{
    switch (iAction) {
        case MenuAction_Select: {
            new String:sWeapon[64];
            GetMenuItem(hMenu, iKey, sWeapon, sizeof(sWeapon));
            GivePlayerItem(iClient, sWeapon);

			new Handle:hSubMenu = CreateMenu(MySubMenu_Handler);
			SetMenuTitle(hSubMenu, "Choose your HP:");
			AddMenuItem(hSubMenu, "35", "35 HP");
			AddMenuItem(hSubMenu, "100", "100 HP");
			AddMenuItem(hSubMenu, "300", "300 HP");
			
			// here is the magic
			CopyMenuAny(hMenu, hSubMenu, "-MySecretValue-");

			SetMenuExitBackButton(hSubMenu, true);
			DisplayMenu(hSubMenu, iClient, 30);
        }
        case MenuAction_End: {
            CloseHandle(hMenu);
        }
    }
}
  ```

---

Also notice function `AddMenuItemFormat`, which allows you to do
  ```SourcePawn
AddMenuItemFormat(hMenu, "weapon_ak47", _, "AK-47 + %d ammo", 90);

// instead of

new String:sDisplay[64];
Format(sDisplay, sizeof(sDisplay), "AK-47 + %d ammo", 90);
AddMenuItem(hMenu, "weapon_ak47", sDisplay);
  ```
