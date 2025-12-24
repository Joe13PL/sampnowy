/*
 * =============================================================================
 *  DIALOGS - System dialog??w
 * =============================================================================
 *
 *  Modu??: ui/dialogs.inc
 *  Opis: Dynamiczne dialogi z obs??ug?? stan??w i walidacji
 *
 *  ZMIANY W REFAKTORZE:
 *  - Dodano system informacyjnych GUI
 *  - Centralne zarz??dzanie dialogami
 *  - Obs??uga stan??w dialog??w gracza
 *
 * =============================================================================
 */

#if defined _dialogs_included
    #endinput
#endif
#define _dialogs_included

// ===========================================================================
// STA??E
// ===========================================================================

// ID Dialog??w (zdefiniowane w defines.inc)
// DIALOG_NONE = 0
// DIALOG_LOGIN = 1
// itd...

// Stany dialog??w
enum E_DIALOG_STATE
{
    DIALOG_STATE_NONE,
    DIALOG_STATE_WAITING,
    DIALOG_STATE_PROCESSING
};

// ===========================================================================
// ZMIENNE
// ===========================================================================

static E_DIALOG_STATE:g_DialogState[MAX_PLAYERS];
static g_CurrentDialog[MAX_PLAYERS];
static g_DialogData[MAX_PLAYERS][256]; // Dodatkowe dane dialogu

// ===========================================================================
// FUNKCJE G????WNE
// ===========================================================================

// SendGuiInformation jest zdefiniowana w utils/strings.inc

/**
 * Wyświetla dialog potwierdzenia
 * @param playerid ID gracza
 * @param dialogId ID dialogu
 * @param title Tytu??
 * @param message Tre????
 * @param confirmBtn Tekst przycisku potwierdzenia
 * @param cancelBtn Tekst przycisku anulowania
 */
stock ShowConfirmDialog(playerid, dialogId, const title[], const message[],
                        const confirmBtn[] = "Yes", const cancelBtn[] = "No")
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    g_CurrentDialog[playerid] = dialogId;
    g_DialogState[playerid] = DIALOG_STATE_WAITING;

    ShowPlayerDialog(playerid, dialogId, DIALOG_STYLE_MSGBOX,
        title, message, confirmBtn, cancelBtn);

    return 1;
}

/**
 * Wy??wietla dialog z polem tekstowym
 * @param playerid ID gracza
 * @param dialogId ID dialogu
 * @param title Tytu??
 * @param message Tre????
 * @param isPassword Czy pole has??a
 */
stock ShowInputDialog(playerid, dialogId, const title[], const message[], bool:isPassword = false)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    g_CurrentDialog[playerid] = dialogId;
    g_DialogState[playerid] = DIALOG_STATE_WAITING;

    new style = isPassword ? DIALOG_STYLE_PASSWORD : DIALOG_STYLE_INPUT;

    ShowPlayerDialog(playerid, dialogId, style,
        title, message, "Confirm", "Cancel");

    return 1;
}

/**
 * Wy??wietla dialog z list??
 * @param playerid ID gracza
 * @param dialogId ID dialogu
 * @param title Tytu??
 * @param items Lista element??w (oddzielonych \n)
 * @param selectBtn Tekst przycisku wyboru
 * @param cancelBtn Tekst przycisku anulowania
 */
stock ShowListDialog(playerid, dialogId, const title[], const items[],
                     const selectBtn[] = "Select", const cancelBtn[] = "Cancel")
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    g_CurrentDialog[playerid] = dialogId;
    g_DialogState[playerid] = DIALOG_STATE_WAITING;

    ShowPlayerDialog(playerid, dialogId, DIALOG_STYLE_LIST,
        title, items, selectBtn, cancelBtn);

    return 1;
}

/**
 * Wy??wietla dialog z tabel?? (tablist)
 * @param playerid ID gracza
 * @param dialogId ID dialogu
 * @param title Tytu??
 * @param headers Nag????wki kolumn (oddzielone \t)
 * @param rows Wiersze (kolumny \t, wiersze \n)
 */
stock ShowTablistDialog(playerid, dialogId, const title[], const headers[], const rows[],
                        const selectBtn[] = "Select", const cancelBtn[] = "Cancel")
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    g_CurrentDialog[playerid] = dialogId;
    g_DialogState[playerid] = DIALOG_STATE_WAITING;

    new content[2048];
    format(content, sizeof(content), "%s\n%s", headers, rows);

    ShowPlayerDialog(playerid, dialogId, DIALOG_STYLE_TABLIST_HEADERS,
        title, content, selectBtn, cancelBtn);

    return 1;
}

// ===========================================================================
// DIALOGI EKWIPUNKU
// ===========================================================================

/**
 * Wy??wietla dialog ekwipunku gracza
 * @param playerid ID gracza
 */
stock Dialog_ShowInventory(playerid)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return 0;
    }

    new content[2048];
    new itemCount = 0;

    // Headers
    strcopy(content, "Item\tType\tValue\n", sizeof(content));

    for(new i = 0; i < MAX_PLAYER_ITEMS; i++)
    {
        if(pItem[playerid][i][item_id] > 0)
        {
            new typeName[32];
            Item_GetDefaultName(E_ITEM_TYPE:pItem[playerid][i][item_type], typeName);

            new line[128];
            format(line, sizeof(line), "%s\t%s\t%d\n",
                pItem[playerid][i][item_name], typeName, pItem[playerid][i][item_value]);

            strcat(content, line);
            itemCount++;
        }
    }

    if(itemCount == 0)
    {
        strcopy(content, "Your inventory is empty.", sizeof(content));
        ShowPlayerDialog(playerid, DIALOG_INVENTORY, DIALOG_STYLE_MSGBOX,
            "Inventory", content, "OK", "");
    }
    else
    {
        new title[64];
        format(title, sizeof(title), "Inventory (%d/%d)", itemCount, MAX_PLAYER_ITEMS);

        ShowPlayerDialog(playerid, DIALOG_INVENTORY, DIALOG_STYLE_TABLIST_HEADERS,
            title, content, "Use", "Close");
    }

    return 1;
}

/**
 * Wy??wietla opcje dla wybranego przedmiotu
 * @param playerid ID gracza
 * @param slot Slot przedmiotu
 */
stock Dialog_ShowItemOptions(playerid, slot)
{
    if(slot < 0 || slot >= MAX_PLAYER_ITEMS || pItem[playerid][slot][item_id] == 0)
    {
        return 0;
    }

    // Zapisz slot do danych dialogu
    format(g_DialogData[playerid], sizeof(g_DialogData[]), "%d", slot);

    new title[64];
    format(title, sizeof(title), "%s - Options", pItem[playerid][slot][item_name]);

    new content[256] = "Use\nGive to player\nDrop\nThrow away";

    ShowPlayerDialog(playerid, DIALOG_ITEM_OPTIONS, DIALOG_STYLE_LIST,
        title, content, "Select", "Back");

    return 1;
}

// ===========================================================================
// DIALOGI GRUP
// ===========================================================================

/**
 * Wy??wietla list?? grup gracza
 * @param playerid ID gracza
 */
stock Dialog_ShowPlayerGroups(playerid)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return 0;
    }

    new content[1024];
    new groupCount = 0;

    strcopy(content, "Group Name\tRank\tSalary\n", sizeof(content));

    for(new i = 0; i < MAX_PLAYER_GROUPS; i++)
    {
        new gid = pInfo[playerid][player_group][i];

        if(gid > 0 && gid < MAX_GROUPS && gInfo[gid][group_created])
        {
            new rankName[32];
            Player_GetGroupRankName(playerid, gid, rankName);

            new line[128];
            format(line, sizeof(line), "{%06x}%s\t%s\t%s$\n",
                gInfo[gid][group_color] >>> 8,
                gInfo[gid][group_name],
                rankName,
                FormatMoney(pInfo[playerid][player_group_salary][i]));

            strcat(content, line);
            groupCount++;
        }
    }

    if(groupCount == 0)
    {
        SendGuiInformation(playerid, "Groups", "You are not a member of any groups.");
        return 0;
    }

    ShowPlayerDialog(playerid, DIALOG_GROUPS, DIALOG_STYLE_TABLIST_HEADERS,
        "Your Groups", content, "Options", "Close");

    return 1;
}

// ===========================================================================
// DIALOGI POJAZD??W
// ===========================================================================

/**
 * Wy??wietla list?? pojazd??w gracza
 * @param playerid ID gracza
 */
stock Dialog_ShowPlayerVehicles(playerid)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return 0;
    }

    new vehicles[MAX_VEHICLES];
    new vehicleCount = Vehicle_GetPlayerVehicles(playerid, vehicles);

    if(vehicleCount == 0)
    {
        SendGuiInformation(playerid, "Vehicles", "You do not own any vehicles.");
        return 0;
    }

    new content[2048];
    strcopy(content, "Model\tFuel\tCondition\n", sizeof(content));

    for(new i = 0; i < vehicleCount; i++)
    {
        new slot = vehicles[i];
        new modelName[32];
        GetVehicleModelName(vInfo[slot][veh_model], modelName);

        new line[128];
        format(line, sizeof(line), "%s\t%d%%\t%.0f HP\n",
            modelName,
            vInfo[slot][veh_fuel],
            vInfo[slot][veh_health]);

        strcat(content, line);
    }

    ShowPlayerDialog(playerid, DIALOG_VEHICLES, DIALOG_STYLE_TABLIST_HEADERS,
        "Your Vehicles", content, "Locate", "Close");

    return 1;
}

/**
 * Pobiera nazw?? modelu pojazdu
 * @param modelid ID modelu
 * @param output Bufor wyj??ciowy
 */
stock GetVehicleModelName(modelid, output[])
{
    static const vehicleNames[][] = {
        "Landstalker", "Bravura", "Buffalo", "Linerunner", "Perennial", "Sentinel", "Dumper", "Firetruck",
        "Trashmaster", "Stretch", "Manana", "Infernus", "Voodoo", "Pony", "Mule", "Cheetah", "Ambulance",
        "Leviathan", "Moonbeam", "Esperanto", "Taxi", "Washington", "Bobcat", "Mr Whoopee", "BF Injection",
        "Hunter", "Premier", "Enforcer", "Securicar", "Banshee", "Predator", "Bus", "Rhino", "Barracks",
        "Hotknife", "Trailer", "Previon", "Coach", "Cabbie", "Stallion", "Rumpo", "RC Bandit", "Romero",
        "Packer", "Monster", "Admiral", "Squalo", "Seasparrow", "Pizzaboy", "Tram", "Trailer", "Turismo",
        "Speeder", "Reefer", "Tropic", "Flatbed", "Yankee", "Caddy", "Solair", "Berkley's RC Van", "Skimmer",
        "PCJ-600", "Faggio", "Freeway", "RC Baron", "RC Raider", "Glendale", "Oceanic", "Sanchez", "Sparrow",
        "Patriot", "Quad", "Coastguard", "Dinghy", "Hermes", "Sabre", "Rustler", "ZR-350", "Walton", "Regina",
        "Comet", "BMX", "Burrito", "Camper", "Marquis", "Baggage", "Dozer", "Maverick", "News Chopper",
        "Rancher", "FBI Rancher", "Virgo", "Greenwood", "Jetmax", "Hotring Racer", "Sandking", "Blista Compact",
        "Police Maverick", "Boxville", "Benson", "Mesa", "RC Goblin", "Hotring Racer", "Hotring Racer",
        "Bloodring Banger", "Rancher", "Super GT", "Elegant", "Journey", "Bike", "Mountain Bike", "Beagle",
        "Cropduster", "Stuntplane", "Tanker", "RoadTrain", "Nebula", "Majestic", "Buccaneer", "Shamal",
        "Hydra", "FCR-900", "NRG-500", "HPV1000", "Cement Truck", "Tow Truck", "Fortune", "Cadrona", "FBI Truck",
        "Willard", "Forklift", "Tractor", "Combine", "Feltzer", "Remington", "Slamvan", "Blade", "Freight",
        "Streak", "Vortex", "Vincent", "Bullet", "Clover", "Sadler", "Firetruck", "Hustler", "Intruder", "Primo",
        "Cargobob", "Tampa", "Sunrise", "Merit", "Utility Van", "Nevada", "Yosemite", "Windsor", "Monster",
        "Monster", "Uranus", "Jester", "Sultan", "Stratum", "Elegy", "Raindance", "RC Tiger", "Flash", "Tahoma",
        "Savanna", "Bandito", "Freight", "Trailer", "Kart", "Mower", "Dune", "Sweeper", "Broadway", "Tornado",
        "AT-400", "DFT-30", "Huntley", "Stafford", "BF-400", "Newsvan", "Tug", "Trailer", "Emperor", "Wayfarer",
        "Euros", "Hotdog", "Club", "Trailer", "Trailer", "Andromada", "Dodo", "RC Cam", "Launch", "Police",
        "Police", "Police", "Police Ranger", "Picador", "S.W.A.T.", "Alpha", "Phoenix", "Glendale", "Sadler",
        "Luggage", "Luggage", "Stairs", "Boxville", "Tiller", "Utility Trailer"
    };

    if(modelid >= 400 && modelid <= 611)
    {
        strcopy(output, vehicleNames[modelid - 400], 32);
    }
    else
    {
        strcopy(output, "Unknown", 32);
    }
}

// ===========================================================================
// FUNKCJE POMOCNICZE
// ===========================================================================

/**
 * Pobiera aktualny dialog gracza
 * @param playerid ID gracza
 * @return ID dialogu
 */
stock Dialog_GetCurrent(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return DIALOG_NONE;
    }

    return g_CurrentDialog[playerid];
}

/**
 * Pobiera dane dialogu gracza
 * @param playerid ID gracza
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 */
stock Dialog_GetData(playerid, output[], size = sizeof(output))
{
    if(!IsValidPlayerId(playerid))
    {
        output[0] = EOS;
        return 0;
    }

    strcopy(output, g_DialogData[playerid], size);
    return 1;
}

/**
 * Ustawia dane dialogu gracza
 * @param playerid ID gracza
 * @param data Dane do zapisania
 */
stock Dialog_SetData(playerid, const data[])
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    strcopy(g_DialogData[playerid], data, sizeof(g_DialogData[]));
    return 1;
}

/**
 * Zamyka aktualny dialog gracza
 * @param playerid ID gracza
 */
stock Dialog_Close(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    ShowPlayerDialog(playerid, -1, DIALOG_STYLE_MSGBOX, " ", " ", " ", "");

    g_CurrentDialog[playerid] = DIALOG_NONE;
    g_DialogState[playerid] = DIALOG_STATE_NONE;
    g_DialogData[playerid][0] = EOS;

    return 1;
}

// ===========================================================================
// HOOKI
// ===========================================================================

hook OnPlayerConnect@Dialogs(playerid)
{
    g_CurrentDialog[playerid] = DIALOG_NONE;
    g_DialogState[playerid] = DIALOG_STATE_NONE;
    g_DialogData[playerid][0] = EOS;

    return 1;
}

// ===========================================================================
// EOF
// ===========================================================================

