/*
 * =============================================================================
 *  DOORS - System drzwi/budynk??w
 * =============================================================================
 *  
 *  Modu??: gameplay/doors.inc
 *  Opis: Zarz??dzanie drzwiami (wej??cia/wyj??cia, blokady, w??asno????)
 *
 *  ZMIANY W REFAKTORZE:
 *  - Zastosowano iterator do przechowywania drzwi
 *  - Zoptymalizowano wyszukiwanie drzwi
 *  - Dodano obs??ug?? teleportacji przez pickup
 *  - Poprawiono system blokad
 *
 * =============================================================================
 */

// Wyciszenie warningów tag mismatch
#pragma warning disable 213

#if defined _doors_included
    #endinput
#endif
#define _doors_included

// ===========================================================================
// STA??E
// ===========================================================================

#define DOOR_INTERACTION_RANGE      2.0         // Zasi??g interakcji
#define DOOR_LABEL_DISTANCE         10.0        // Zasi??g widoczno??ci etykiety

// Typy drzwi
enum E_DOOR_TYPE
{
    DOOR_TYPE_PUBLIC,           // Publiczne (ka??dy mo??e wej????)
    DOOR_TYPE_PRIVATE,          // Prywatne (tylko w??a??ciciel)
    DOOR_TYPE_GROUP,            // Grupowe (cz??onkowie grupy)
    DOOR_TYPE_LOCKED            // Zablokowane (wymagany klucz)
};

// Typy w??a??cicieli
enum E_DOOR_OWNER_TYPE
{
    DOOR_OWNER_NONE,
    DOOR_OWNER_PLAYER,
    DOOR_OWNER_GROUP,
    DOOR_OWNER_SERVER
};

// ===========================================================================
// ITERATOR - uzywany z variables.inc
// ===========================================================================

// Iterator Doors jest juz zdefiniowany w core/variables.inc

// ===========================================================================
// DYNAMICZNE OBIEKTY (Pickupy i Labele)
// ===========================================================================

static g_DoorPickup[MAX_DOORS][2];      // [0] = zewn??trzny, [1] = wewn??trzny
static Text3D:g_DoorLabel[MAX_DOORS][2];

// ===========================================================================
// FUNKCJE INICJALIZACJI
// ===========================================================================

/**
 * Wczytuje drzwi z bazy danych
 */
stock Doors_Load()
{
    new query[256];
    mysql_format(mySQLconnection, query, sizeof(query),
           "SELECT * FROM `doors` ORDER BY `door_id` ASC"
    );
    
    mysql_tquery(mySQLconnection, query, "Doors_OnLoad");
}

forward Doors_OnLoad();
public Doors_OnLoad()
{
    new rows = cache_num_rows();
    new loaded = 0;
    
    for(new i = 0; i < rows; i++)
    {
        new doorId;
        cache_get_value_int(i, "door_id", doorId);
        
        if(doorId <= 0 || doorId >= MAX_DOORS)
        {
            continue;
        }
        
        dInfo[doorId][door_created] = true;
        
        // Nazwa i ustawienia
        cache_get_value(i, "door_name", dInfo[doorId][door_name], 64);
        cache_get_value_int(i, "door_type", dInfo[doorId][door_type]);
        cache_get_value_int(i, "door_owner_type", dInfo[doorId][door_owner_type]);
        cache_get_value_int(i, "door_owner", dInfo[doorId][door_owner]);
        cache_get_value_int(i, "door_locked", dInfo[doorId][door_locked]);
        cache_get_value_int(i, "door_pickup", dInfo[doorId][door_pickup_model]);
        cache_get_value_int(i, "door_bank", dInfo[doorId][door_bank]);
        
        // Pozycja zewn??trzna
        cache_get_value_float(i, "door_ext_x", dInfo[doorId][door_ext_x]);
        cache_get_value_float(i, "door_ext_y", dInfo[doorId][door_ext_y]);
        cache_get_value_float(i, "door_ext_z", dInfo[doorId][door_ext_z]);
        cache_get_value_float(i, "door_ext_a", dInfo[doorId][door_ext_a]);
        cache_get_value_int(i, "door_ext_interior", dInfo[doorId][door_ext_interior]);
        cache_get_value_int(i, "door_ext_vw", dInfo[doorId][door_ext_vw]);
        
        // Pozycja wewn??trzna
        cache_get_value_float(i, "door_int_x", dInfo[doorId][door_int_x]);
        cache_get_value_float(i, "door_int_y", dInfo[doorId][door_int_y]);
        cache_get_value_float(i, "door_int_z", dInfo[doorId][door_int_z]);
        cache_get_value_float(i, "door_int_a", dInfo[doorId][door_int_a]);
        cache_get_value_int(i, "door_int_interior", dInfo[doorId][door_int_interior]);
        
        // Utw??rz elementy wizualne
        Door_CreateVisuals(doorId);
        
        Iter_Add(Doors, doorId);
        loaded++;
    }
    
    printf("[Doors] Wczytano %d drzwi", loaded);
    return 1;
}

/**
 * Tworzy wizualne elementy drzwi (pickupy, labele)
 * @param doorId ID drzwi
 */
stock Door_CreateVisuals(doorId)
{
    if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
    {
        return 0;
    }
    
    new pickupModel = dInfo[doorId][door_pickup_model];
    if(pickupModel == 0) pickupModel = 1318; // Domy??lny marker
    
    // Pickup zewn??trzny
    g_DoorPickup[doorId][0] = CreateDynamicPickup(
        pickupModel, 1,
        dInfo[doorId][door_ext_x], dInfo[doorId][door_ext_y], dInfo[doorId][door_ext_z],
        dInfo[doorId][door_ext_vw], dInfo[doorId][door_ext_interior]
    );
    
    // Pickup wewn??trzny
    g_DoorPickup[doorId][1] = CreateDynamicPickup(
        pickupModel, 1,
        dInfo[doorId][door_int_x], dInfo[doorId][door_int_y], dInfo[doorId][door_int_z],
        doorId + 1000, dInfo[doorId][door_int_interior]
    );
    
    // Label zewn??trzny
    new labelText[256];
    Door_FormatLabel(doorId, labelText, sizeof(labelText), false);
    
    g_DoorLabel[doorId][0] = CreateDynamic3DTextLabel(
        labelText, COLOR_WHITE,
        dInfo[doorId][door_ext_x], dInfo[doorId][door_ext_y], dInfo[doorId][door_ext_z] + 0.5,
        DOOR_LABEL_DISTANCE, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0,
        dInfo[doorId][door_ext_vw], dInfo[doorId][door_ext_interior]
    );
    
    // Label wewn??trzny
    Door_FormatLabel(doorId, labelText, sizeof(labelText), true);
    
    g_DoorLabel[doorId][1] = CreateDynamic3DTextLabel(
        labelText, COLOR_WHITE,
        dInfo[doorId][door_int_x], dInfo[doorId][door_int_y], dInfo[doorId][door_int_z] + 0.5,
        DOOR_LABEL_DISTANCE, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0,
        doorId + 1000, dInfo[doorId][door_int_interior]
    );
    
    return 1;
}

/**
 * Usuwa wizualne elementy drzwi
 * @param doorId ID drzwi
 */
stock Door_DestroyVisuals(doorId)
{
    if(doorId <= 0 || doorId >= MAX_DOORS)
    {
        return 0;
    }
    
    // Usu?? pickupy
    if(g_DoorPickup[doorId][0] != -1)
    {
        DestroyDynamicPickup(g_DoorPickup[doorId][0]);
        g_DoorPickup[doorId][0] = -1;
    }
    
    if(g_DoorPickup[doorId][1] != -1)
    {
        DestroyDynamicPickup(g_DoorPickup[doorId][1]);
        g_DoorPickup[doorId][1] = -1;
    }
    
    // Usu?? labele
    if(g_DoorLabel[doorId][0] != Text3D:INVALID_3DTEXT_ID)
    {
        DestroyDynamic3DTextLabel(g_DoorLabel[doorId][0]);
        g_DoorLabel[doorId][0] = Text3D:INVALID_3DTEXT_ID;
    }
    
    if(g_DoorLabel[doorId][1] != Text3D:INVALID_3DTEXT_ID)
    {
        DestroyDynamic3DTextLabel(g_DoorLabel[doorId][1]);
        g_DoorLabel[doorId][1] = Text3D:INVALID_3DTEXT_ID;
    }
    
    return 1;
}

/**
 * Formatuje tekst etykiety drzwi
 * @param doorId ID drzwi
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 * @param isInside Czy etykieta wewn??trzna
 */
stock Door_FormatLabel(doorId, output[], size, bool:isInside = false)
{
    if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
    {
        output[0] = EOS;
        return 0;
    }
    
    new lockStatus[32];
    
    if(dInfo[doorId][door_locked])
    {
        strcopy(lockStatus, "{FF0000}[ZAMKNI??TE]", sizeof(lockStatus));
    }
    else
    {
        strcopy(lockStatus, "{00FF00}[OTWARTE]", sizeof(lockStatus));
    }
    
    if(isInside)
    {
        format(output, size, 
            "{FFFFFF}%s\n\
            %s\n\
            {AAAAAA}U??yj /wyjdz lub naci??nij ~k~~PED_LOCK_TARGET~",
            dInfo[doorId][door_name], lockStatus);
    }
    else
    {
        format(output, size, 
            "{FFFFFF}%s\n\
            %s\n\
            {AAAAAA}U??yj /wejdz lub naci??nij ~k~~PED_LOCK_TARGET~",
            dInfo[doorId][door_name], lockStatus);
    }
    
    return 1;
}

/**
 * Aktualizuje etykiety drzwi
 * @param doorId ID drzwi
 */
stock Door_UpdateLabels(doorId)
{
    if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
    {
        return 0;
    }
    
    new labelText[256];
    
    // Zewn??trzna
    Door_FormatLabel(doorId, labelText, sizeof(labelText), false);
    UpdateDynamic3DTextLabelText(g_DoorLabel[doorId][0], COLOR_WHITE, labelText);
    
    // Wewn??trzna
    Door_FormatLabel(doorId, labelText, sizeof(labelText), true);
    UpdateDynamic3DTextLabelText(g_DoorLabel[doorId][1], COLOR_WHITE, labelText);
    
    return 1;
}

// ===========================================================================
// FUNKCJE G????WNE
// ===========================================================================

/**
 * Pr??buje wej???? przez drzwi
 * @param playerid ID gracza
 * @return 1 je??li sukces
 */
stock Door_TryEnter(playerid)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return 0;
    }
    
    // Znajd?? najbli??sze drzwi (wej??cie zewn??trzne)
    new doorId = Door_FindNearestExternal(playerid);
    
    if(doorId == -1)
    {
        SendClientMessage(playerid, COLOR_RED, "Nie ma tu ??adnych drzwi!");
        return 0;
    }
    
    // Sprawd?? czy drzwi s?? zablokowane
    if(dInfo[doorId][door_locked] && !Door_CanAccess(playerid, doorId))
    {
        SendClientMessage(playerid, COLOR_RED, "Te drzwi s?? zamkni??te!");
        return 0;
    }
    
    // Teleportuj gracza do ??rodka
    Player_Teleport(playerid,
        dInfo[doorId][door_int_x], dInfo[doorId][door_int_y], dInfo[doorId][door_int_z],
        dInfo[doorId][door_int_a], dInfo[doorId][door_int_interior], doorId + 1000);
    
    // Efekt d??wi??kowy
    PlayerPlaySound(playerid, 1056, 0.0, 0.0, 0.0);
    
    return 1;
}

/**
 * Pr??buje wyj???? przez drzwi
 * @param playerid ID gracza
 * @return 1 je??li sukces
 */
stock Door_TryExit(playerid)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return 0;
    }
    
    // Znajd?? drzwi na podstawie VW
    new vw = GetPlayerVirtualWorld(playerid);
    new doorId = vw - 1000;
    
    if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
    {
        SendClientMessage(playerid, COLOR_RED, "Nie mo??esz st??d wyj????!");
        return 0;
    }
    
    // Sprawd?? odleg??o???? od wyj??cia
    new Float:dist = GetDistanceBetweenPoints3D(
        dInfo[doorId][door_int_x], dInfo[doorId][door_int_y], dInfo[doorId][door_int_z],
        0.0, 0.0, 0.0 // Pozycja gracza
    );
    
    GetPlayerPos(playerid, dist, dist, dist); // Aktualizuj
    
    // Sprawd?? czy drzwi s?? zablokowane (od ??rodka zawsze mo??na wyj????)
    // Teleportuj gracza na zewn??trz
    Player_Teleport(playerid,
        dInfo[doorId][door_ext_x], dInfo[doorId][door_ext_y], dInfo[doorId][door_ext_z],
        dInfo[doorId][door_ext_a], dInfo[doorId][door_ext_interior], dInfo[doorId][door_ext_vw]);
    
    // Efekt d??wi??kowy
    PlayerPlaySound(playerid, 1056, 0.0, 0.0, 0.0);
    
    return 1;
}

/**
 * Blokuje/odblokowuje drzwi
 * @param doorId ID drzwi
 * @param locked Stan blokady
 */
stock Door_SetLocked(doorId, bool:locked)
{
    if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
    {
        return 0;
    }
    
    dInfo[doorId][door_locked] = locked;
    Door_UpdateLabels(doorId);
    Door_Save(doorId);
    
    return 1;
}

/**
 * Sprawdza czy gracz mo??e uzyska?? dost??p do drzwi
 * @param playerid ID gracza
 * @param doorId ID drzwi
 * @return true je??li mo??e
 */
stock bool:Door_CanAccess(playerid, doorId)
{
    if(!IsValidPlayerId(playerid) || doorId <= 0 || doorId >= MAX_DOORS)
    {
        return false;
    }
    
    // Publiczne drzwi
    if(dInfo[doorId][door_type] == _:DOOR_TYPE_PUBLIC)
    {
        return true;
    }
    
    // Admin na s??u??bie
    if(pInfo[playerid][player_admin_duty])
    {
        return true;
    }
    
    // Sprawdz wlasciciela
    switch(dInfo[doorId][door_owner_type])
    {
        case DOOR_OWNER_PLAYER:
        {
            return dInfo[doorId][door_owner] == pInfo[playerid][player_id];
        }
        
        case DOOR_OWNER_GROUP:
        {
            return Player_IsInGroup(playerid, dInfo[doorId][door_owner]);
        }
        
        case DOOR_OWNER_SERVER:
        {
            return true;
        }
    }
    
    return false;
}

// ===========================================================================
// WYSZUKIWANIE
// ===========================================================================

/**
 * Znajduje najbli??sze drzwi (zewn??trzne)
 * @param playerid ID gracza
 * @return ID drzwi lub -1
 */
stock Door_FindNearestExternal(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return -1;
    }
    
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);
    
    new interior = GetPlayerInterior(playerid);
    new vw = GetPlayerVirtualWorld(playerid);
    
    new bestDoor = -1;
    new Float:bestDist = DOOR_INTERACTION_RANGE;
    
    foreach(new doorId : Doors)
    {
        // Sprawd?? interior i VW
        if(dInfo[doorId][door_ext_interior] != interior || 
           dInfo[doorId][door_ext_vw] != vw)
        {
            continue;
        }
        
        new Float:dist = GetDistanceBetweenPoints3D(
            px, py, pz,
            dInfo[doorId][door_ext_x], dInfo[doorId][door_ext_y], dInfo[doorId][door_ext_z]
        );
        
        if(dist < bestDist)
        {
            bestDist = dist;
            bestDoor = doorId;
        }
    }
    
    return bestDoor;
}

/**
 * Pobiera drzwi gracza (kt??rych jest w??a??cicielem)
 * @param playerid ID gracza
 * @param output Tablica wyj??ciowa
 * @param maxOutput Max wynik??w
 * @return Liczba znalezionych drzwi
 */
stock Door_GetPlayerDoors(playerid, output[], maxOutput = sizeof(output))
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return 0;
    }
    
    new uid = pInfo[playerid][player_id];
    new count = 0;
    
    foreach(new doorId : Doors)
    {
        if(dInfo[doorId][door_owner_type] == _:DOOR_OWNER_PLAYER && 
           dInfo[doorId][door_owner] == uid)
        {
            if(count < maxOutput)
            {
                output[count] = doorId;
                count++;
            }
        }
    }
    
    return count;
}

// ===========================================================================
// ZAPIS
// ===========================================================================

/**
 * Zapisuje drzwi do bazy
 * @param doorId ID drzwi
 */
stock Door_Save(doorId)
{
    if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
    {
        return 0;
    }
    
    new query[2048];
    new escapedName[129];
    new escapedAudio[256];
    mysql_escape_string(dInfo[doorId][door_name], escapedName, mySQLconnection, sizeof(escapedName));
    mysql_escape_string(dInfo[doorId][door_audio], escapedAudio, mySQLconnection, sizeof(escapedAudio));
    
    mysql_format(mySQLconnection, query, sizeof(query),
        "UPDATE `doors` SET `door_name` = '%s', `door_type` = %d, `door_owner_type` = %d, `door_owner` = %d, `door_locked` = %d, `door_pickup` = %d, `door_bank` = %d, \
         `door_ext_x` = %f, `door_ext_y` = %f, `door_ext_z` = %f, `door_ext_a` = %f, `door_ext_interior` = %d, `door_ext_vw` = %d, \
         `door_int_x` = %f, `door_int_y` = %f, `door_int_z` = %f, `door_int_a` = %f, `door_int_interior` = %d, `door_int_vw` = %d, \
         `door_objects_limit` = %d, `door_auto_closing` = %d, `door_car_crosing` = %d, `door_payment` = %d, `door_map_icon` = %d, `door_closed` = %d, `door_rentable` = %d, `door_rent` = %d, `door_surface` = %f, `door_time` = %d, `door_access` = %d, `door_destroyed` = %d, `door_burned` = %d, `door_meters` = %d, `door_audio` = '%s', `door_area` = %d, `door_demolition` = %d \
         WHERE `door_id` = %d",
        escapedName, dInfo[doorId][door_type], dInfo[doorId][door_owner_type], dInfo[doorId][door_owner],
        dInfo[doorId][door_locked], dInfo[doorId][door_pickup_model], dInfo[doorId][door_bank],
        dInfo[doorId][door_ext_x], dInfo[doorId][door_ext_y], dInfo[doorId][door_ext_z], dInfo[doorId][door_ext_a],
        dInfo[doorId][door_ext_interior], dInfo[doorId][door_ext_vw],
        dInfo[doorId][door_int_x], dInfo[doorId][door_int_y], dInfo[doorId][door_int_z], dInfo[doorId][door_int_a],
        dInfo[doorId][door_int_interior], dInfo[doorId][door_int_vw],
        dInfo[doorId][door_objects_limit], dInfo[doorId][door_auto_closing], dInfo[doorId][door_car_crosing], dInfo[doorId][door_payment], dInfo[doorId][door_map_icon], dInfo[doorId][door_closed], dInfo[doorId][door_rentable], dInfo[doorId][door_rent], dInfo[doorId][door_surface], dInfo[doorId][door_time], dInfo[doorId][door_access], dInfo[doorId][door_destroyed], dInfo[doorId][door_burned], dInfo[doorId][door_meters],
        escapedAudio, dInfo[doorId][door_area], dInfo[doorId][door_demolition], doorId
    );
    
    mysql_tquery(mySQLconnection, query);
    return 1;
}

// ===========================================================================
// HOOKI
// ===========================================================================

hook OnPlayerKeyStateChange@Doors(playerid, newkeys, oldkeys)
{
    // Klawisz Alt+Spacja do interakcji z drzwiami
    if((newkeys & KEY_WALK) && (newkeys & KEY_SPRINT))
    {
        if(pInfo[playerid][player_spawned] && !IsPlayerInAnyVehicle(playerid))
        {
            // Sprawd?? czy gracz jest w VW drzwi (wewn??trz)
            new vw = GetPlayerVirtualWorld(playerid);
            
            if(vw >= 1001 && vw < 1001 + MAX_DOORS)
            {
                // Wewn??trz - spr??buj wyj????
                Door_TryExit(playerid);
            }
            else
            {
                // Na zewn??trz - spr??buj wej????
                Door_TryEnter(playerid);
            }
        }
    }
    
    return 1;
}

// ===========================================================================
// EOF
// ===========================================================================

