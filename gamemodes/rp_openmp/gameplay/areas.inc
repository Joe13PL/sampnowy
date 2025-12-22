/*
 * =============================================================================
 *  AREAS - System stref/obszar??w
 * =============================================================================
 *  
 *  Modu??: gameplay/areas.inc
 *  Opis: Zarz??dzanie strefami (bezpieczne, grupowe, specjalne)
 *
 *  ZMIANY W REFAKTORZE:
 *  - Wykorzystano streamer do dynamicznych stref
 *  - Dodano system wykrywania stref
 *  - Zoptymalizowano sprawdzanie pozycji
 *
 * =============================================================================
 */

#if defined _areas_included
    #endinput
#endif
#define _areas_included

// ===========================================================================
// TYPY STREF
// ===========================================================================

enum E_AREA_TYPE
{
    AREA_TYPE_NONE = 0,
    AREA_TYPE_SAFE,             // Strefa bezpieczna (brak obra??e??)
    AREA_TYPE_BUSINESS,         // Biznes
    AREA_TYPE_GROUP,            // Strefa grupowa
    AREA_TYPE_POLICE,           // Posterunek policji
    AREA_TYPE_HOSPITAL,         // Szpital
    AREA_TYPE_SHOP,             // Sklep
    AREA_TYPE_GAS_STATION,      // Stacja benzynowa
    AREA_TYPE_BANK,             // Bank
    AREA_TYPE_ATM,              // Bankomat
    AREA_TYPE_GYM,              // Si??ownia
    AREA_TYPE_FISHING,          // Strefa ??owienia
    AREA_TYPE_WORK,             // Strefa pracy
    AREA_TYPE_NO_WEAPONS,       // Zakaz broni
    AREA_TYPE_RESTRICTED,       // Strefa zastrze??ona
    AREA_TYPE_WARZONE           // Strefa wojenna (PvP)
};

// ===========================================================================
// STA??E
// ===========================================================================

#define AREA_LABEL_DISTANCE     15.0

// ===========================================================================
// ITERATOR - uzywany z variables.inc
// ===========================================================================

// Iterator Areas jest juz zdefiniowany w core/variables.inc

// ===========================================================================
// DYNAMICZNE STREFY
// ===========================================================================

static g_AreaDynamic[MAX_AREAS];
static Text3D:g_AreaLabel[MAX_AREAS];

// ===========================================================================
// DANE GRACZA
// ===========================================================================

static g_PlayerCurrentArea[MAX_PLAYERS];

// ===========================================================================
// FUNKCJE WCZYTYWANIA
// ===========================================================================

/**
 * Wczytuje strefy z bazy danych
 */
stock Areas_Load()
{
    new query[256];
    mysql_format(mySQLconnection, query, sizeof(query),
           "SELECT * FROM `areas` ORDER BY `area_id` ASC"
    );
    
    mysql_tquery(mySQLconnection, query, "Areas_OnLoad");
}

forward Areas_OnLoad();
public Areas_OnLoad()
{
    new rows = cache_num_rows();
    new loaded = 0;
    
    for(new i = 0; i < rows; i++)
    {
        new areaId;
        cache_get_value_int(i, "area_id", areaId);
        
        if(areaId <= 0 || areaId >= MAX_AREAS)
        {
            continue;
        }
        
        aInfo[areaId][area_created] = true;
        
        cache_get_value(i, "area_name", aInfo[areaId][area_name], 64);
        cache_get_value_int(i, "area_type", aInfo[areaId][area_type]);
        cache_get_value_int(i, "area_owner", aInfo[areaId][area_owner]);
        
        cache_get_value_float(i, "area_min_x", aInfo[areaId][area_min_x]);
        cache_get_value_float(i, "area_min_y", aInfo[areaId][area_min_y]);
        cache_get_value_float(i, "area_max_x", aInfo[areaId][area_max_x]);
        cache_get_value_float(i, "area_max_y", aInfo[areaId][area_max_y]);
        cache_get_value_float(i, "area_min_z", aInfo[areaId][area_min_z]);
        cache_get_value_float(i, "area_max_z", aInfo[areaId][area_max_z]);
        
        cache_get_value_int(i, "area_interior", aInfo[areaId][area_interior]);
        cache_get_value_int(i, "area_vw", aInfo[areaId][area_vw]);
        
        // Utw??rz dynamiczn?? stref??
        Area_CreateDynamic(areaId);
        
        Iter_Add(Areas, areaId);
        loaded++;
    }
    
    printf("[Areas] Wczytano %d stref", loaded);
    return 1;
}

/**
 * Tworzy dynamiczn?? stref?? (streamer)
 * @param areaId ID strefy
 */
stock Area_CreateDynamic(areaId)
{
    if(areaId <= 0 || areaId >= MAX_AREAS || !aInfo[areaId][area_created])
    {
        return 0;
    }
    
    // Utw??rz stref?? prostok??tn??
    g_AreaDynamic[areaId] = CreateDynamicCuboid(
        aInfo[areaId][area_min_x], aInfo[areaId][area_min_y], aInfo[areaId][area_min_z],
        aInfo[areaId][area_max_x], aInfo[areaId][area_max_y], aInfo[areaId][area_max_z],
        aInfo[areaId][area_vw], aInfo[areaId][area_interior]
    );
    
    // Do????cz extra dane do strefy
    Streamer_SetIntData(STREAMER_TYPE_AREA, g_AreaDynamic[areaId], E_STREAMER_EXTRA_ID, areaId);
    
    // Utw??rz label (opcjonalnie)
    if(aInfo[areaId][area_type] != _:AREA_TYPE_NONE)
    {
        new Float:centerX = (aInfo[areaId][area_min_x] + aInfo[areaId][area_max_x]) / 2.0;
        new Float:centerY = (aInfo[areaId][area_min_y] + aInfo[areaId][area_max_y]) / 2.0;
        new Float:centerZ = aInfo[areaId][area_max_z];
        
        new labelText[128];
        format(labelText, sizeof(labelText), "{FFFFFF}%s", aInfo[areaId][area_name]);
        
        g_AreaLabel[areaId] = CreateDynamic3DTextLabel(
            labelText, COLOR_WHITE,
            centerX, centerY, centerZ,
            AREA_LABEL_DISTANCE, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0,
            aInfo[areaId][area_vw], aInfo[areaId][area_interior]
        );
    }
    
    return 1;
}

/**
 * Usuwa dynamiczn?? stref??
 * @param areaId ID strefy
 */
stock Area_DestroyDynamic(areaId)
{
    if(areaId <= 0 || areaId >= MAX_AREAS)
    {
        return 0;
    }
    
    if(g_AreaDynamic[areaId] != -1)
    {
        DestroyDynamicArea(g_AreaDynamic[areaId]);
        g_AreaDynamic[areaId] = -1;
    }
    
    if(g_AreaLabel[areaId] != Text3D:INVALID_3DTEXT_ID)
    {
        DestroyDynamic3DTextLabel(g_AreaLabel[areaId]);
        g_AreaLabel[areaId] = Text3D:INVALID_3DTEXT_ID;
    }
    
    return 1;
}

// ===========================================================================
// FUNKCJE SPRAWDZANIA
// ===========================================================================

/**
 * Pobiera aktualn?? stref?? gracza
 * @param playerid ID gracza
 * @return ID strefy lub 0
 */
stock Area_GetPlayerArea(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    return g_PlayerCurrentArea[playerid];
}

/**
 * Pobiera typ aktualnej strefy gracza
 * @param playerid ID gracza
 * @return Typ strefy
 */
stock E_AREA_TYPE:Area_GetPlayerAreaType(playerid)
{
    new areaId = Area_GetPlayerArea(playerid);
    
    if(areaId <= 0 || areaId >= MAX_AREAS)
    {
        return AREA_TYPE_NONE;
    }
    
    return E_AREA_TYPE:aInfo[areaId][area_type];
}

/**
 * Sprawdza czy gracz jest w strefie okre??lonego typu
 * @param playerid ID gracza
 * @param areaType Typ strefy
 * @return true je??li jest
 */
stock bool:Area_IsPlayerInType(playerid, E_AREA_TYPE:areaType)
{
    return Area_GetPlayerAreaType(playerid) == areaType;
}

/**
 * Sprawdza czy gracz jest w strefie bezpiecznej
 * @param playerid ID gracza
 * @return true je??li jest
 */
stock bool:Area_IsPlayerSafe(playerid)
{
    new areaType = Area_GetPlayerAreaType(playerid);
    
    switch(areaType)
    {
        case AREA_TYPE_SAFE, AREA_TYPE_HOSPITAL, AREA_TYPE_BANK, AREA_TYPE_SHOP:
            return true;
    }
    
    return false;
}

/**
 * Sprawdza czy w strefie obowi??zuje zakaz broni
 * @param playerid ID gracza
 * @return true je??li zakaz broni
 */
stock bool:Area_IsNoWeapons(playerid)
{
    new areaType = Area_GetPlayerAreaType(playerid);
    
    switch(areaType)
    {
        case AREA_TYPE_SAFE, AREA_TYPE_HOSPITAL, AREA_TYPE_BANK, 
             AREA_TYPE_SHOP, AREA_TYPE_NO_WEAPONS:
            return true;
    }
    
    return false;
}

// ===========================================================================
// FUNKCJE AKCJI
// ===========================================================================

/**
 * Wykonuje akcj?? przy wej??ciu do strefy
 * @param playerid ID gracza
 * @param areaId ID strefy
 */
stock Area_OnPlayerEnter(playerid, areaId)
{
    if(!IsValidPlayerId(playerid) || areaId <= 0 || areaId >= MAX_AREAS)
    {
        return 0;
    }
    
    g_PlayerCurrentArea[playerid] = areaId;
    
    new areaType = aInfo[areaId][area_type];
    
    // Komunikat
    new msg[128];
    format(msg, sizeof(msg), "~w~Wchodzisz do: ~y~%s", aInfo[areaId][area_name]);
    GameTextForPlayer(playerid, msg, 3000, 4);
    
    // Akcje specyficzne dla typu
    switch(E_AREA_TYPE:areaType)
    {
        case AREA_TYPE_SAFE:
        {
            SendClientMessage(playerid, COLOR_GREEN, 
                "Jeste?? w strefie bezpiecznej. Nie mo??esz tutaj u??ywa?? broni.");
        }
        
        case AREA_TYPE_GAS_STATION:
        {
            if(IsPlayerInAnyVehicle(playerid))
            {
                SendClientMessage(playerid, COLOR_YELLOW, 
                    "U??yj /zatankuj aby zatankowa?? pojazd.");
            }
        }
        
        case AREA_TYPE_GYM:
        {
            SendClientMessage(playerid, COLOR_YELLOW, 
                "U??yj /trenuj aby ??wiczy?? na si??owni.");
        }
        
        case AREA_TYPE_FISHING:
        {
            SendClientMessage(playerid, COLOR_YELLOW, 
                "U??yj /lowienie aby rozpocz???? ??owienie ryb.");
        }
        
        case AREA_TYPE_NO_WEAPONS:
        {
            // Ukryj bro??
            SetPlayerArmedWeapon(playerid, 0);
        }
        
        case AREA_TYPE_RESTRICTED:
        {
            new groupId = aInfo[areaId][area_owner];
            
            if(groupId > 0 && !Player_IsInGroup(playerid, groupId))
            {
                SendClientMessage(playerid, COLOR_RED, 
                    "Uwaga! To jest strefa zastrze??ona. Mo??esz zosta?? aresztowany.");
            }
        }
    }
    
    return 1;
}

/**
 * Wykonuje akcj?? przy wyj??ciu ze strefy
 * @param playerid ID gracza
 * @param areaId ID strefy
 */
stock Area_OnPlayerLeave(playerid, areaId)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    if(g_PlayerCurrentArea[playerid] == areaId)
    {
        g_PlayerCurrentArea[playerid] = 0;
    }
    
    return 1;
}

// ===========================================================================
// HOOKI
// ===========================================================================

hook OnPlayerEnterDynamicArea@Areas(playerid, areaid)
{
    // Pobierz ID strefy z extra data
    new areaId = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID);
    
    if(areaId > 0 && areaId < MAX_AREAS && aInfo[areaId][area_created])
    {
        Area_OnPlayerEnter(playerid, areaId);
    }
    
    return 1;
}

hook OnPlayerLeaveDynamicArea@Areas(playerid, areaid)
{
    new areaId = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID);
    
    if(areaId > 0 && areaId < MAX_AREAS)
    {
        Area_OnPlayerLeave(playerid, areaId);
    }
    
    return 1;
}

hook OnPlayerConnect@Areas(playerid)
{
    g_PlayerCurrentArea[playerid] = 0;
    return 1;
}

// ===========================================================================
// FUNKCJE ADMINISTRACYJNE
// ===========================================================================

/**
 * Tworzy now?? stref??
 * @param name Nazwa strefy
 * @param type Typ strefy
 * @param minX, minY, minZ Minimalne wsp????rz??dne
 * @param maxX, maxY, maxZ Maksymalne wsp????rz??dne
 * @param interior Interior
 * @param vw Virtual World
 * @return ID strefy lub -1
 */
stock Area_Create(const name[], E_AREA_TYPE:type, 
                  Float:minX, Float:minY, Float:minZ,
                  Float:maxX, Float:maxY, Float:maxZ,
                  interior = 0, vw = 0)
{
    // Znajd?? wolne ID
    new areaId = -1;
    for(new i = 1; i < MAX_AREAS; i++)
    {
        if(!aInfo[i][area_created])
        {
            areaId = i;
            break;
        }
    }
    
    if(areaId == -1)
    {
        return -1;
    }
    
    // Ustaw dane
    aInfo[areaId][area_created] = true;
    strcopy(aInfo[areaId][area_name], name, 64);
    aInfo[areaId][area_type] = _:type;
    aInfo[areaId][area_min_x] = minX;
    aInfo[areaId][area_min_y] = minY;
    aInfo[areaId][area_min_z] = minZ;
    aInfo[areaId][area_max_x] = maxX;
    aInfo[areaId][area_max_y] = maxY;
    aInfo[areaId][area_max_z] = maxZ;
    aInfo[areaId][area_interior] = interior;
    aInfo[areaId][area_vw] = vw;
    
    // Utw??rz dynamiczn?? stref??
    Area_CreateDynamic(areaId);
    
    // Dodaj do iteratora
    Iter_Add(Areas, areaId);
    
    // Zapisz do bazy
    Area_Save(areaId);
    
    return areaId;
}

/**
 * Zapisuje stref?? do bazy
 * @param areaId ID strefy
 */
stock Area_Save(areaId)
{
    if(areaId <= 0 || areaId >= MAX_AREAS || !aInfo[areaId][area_created])
    {
        return 0;
    }
    
    new query[512];
    new escapedName[129];
    mysql_escape_string(aInfo[areaId][area_name], escapedName, mySQLconnection, sizeof(escapedName));
    
    mysql_format(mySQLconnection, query, sizeof(query),
        "REPLACE INTO `areas` (`area_id`, `area_name`, `area_type`, `area_owner`, `area_min_x`, `area_min_y`, `area_min_z`, `area_max_x`, `area_max_y`, `area_max_z`, `area_interior`, `area_vw`) VALUES (%d, '%s', %d, %d, %f, %f, %f, %f, %f, %f, %d, %d)",
        areaId, escapedName, aInfo[areaId][area_type], aInfo[areaId][area_owner],
        aInfo[areaId][area_min_x], aInfo[areaId][area_min_y], aInfo[areaId][area_min_z],
        aInfo[areaId][area_max_x], aInfo[areaId][area_max_y], aInfo[areaId][area_max_z],
        aInfo[areaId][area_interior], aInfo[areaId][area_vw]
    );
    
    mysql_tquery(mySQLconnection, query);
    return 1;
}

// ===========================================================================
// EOF
// ===========================================================================

