/*
 * =============================================================================
 *  VEHICLES - System pojazd??w
 * =============================================================================
 *
 *  Modu??: gameplay/vehicles.inc
 *  Opis: Zarz??dzanie pojazdami serwera (tworzenie, w??asno????, parking)
 *
 *  ZMIANY W REFAKTORZE:
 *  - Zastosowano iterator do przechowywania pojazd??w
 *  - Zoptymalizowano wyszukiwanie pojazd??w
 *  - Dodano cache dla cz??sto u??ywanych danych
 *  - Poprawiono obs??ug?? respawnu
 *
 * =============================================================================
 */

// Include guard to avoid double inclusion
#if defined _rg_vehicles_included
    #endinput
#endif
#define _rg_vehicles_included


#define VEHICLE_RESPAWN_DELAY       300000      // 5 minut do respawnu
#define VEHICLE_FUEL_MAX            100         // Max paliwa
#define VEHICLE_MILEAGE_FUEL_RATE   0.01        // Zu??ycie paliwa na metr

// Typy w??asno??ci
enum E_VEHICLE_OWNER_TYPE
{
    OWNER_NONE,
    OWNER_PLAYER,
    OWNER_GROUP,
    OWNER_SERVER
};

// ===========================================================================
// ITERATOR
// ===========================================================================
// Iterator is declared globally in core/variables.inc

// ===========================================================================
// FUNKCJE TWORZENIA
// ===========================================================================

/**
 * Tworzy pojazd serwera
 * @param modelid Model pojazdu
 * @param x, y, z, angle Pozycja
 * @param color1, color2 Kolory
 * @param ownerType Typ w??a??ciciela
 * @param ownerId ID w??a??ciciela
 * @return ID pojazdu lub INVALID_VEHICLE_ID
 */
Vehicle_Create(modelid, Float:x, Float:y, Float:z, Float:angle,
                     color1, color2, E_VEHICLE_OWNER_TYPE:ownerType = OWNER_SERVER, ownerId = 0)
{
    // Znajd?? wolne ID w tablicy
    new vehicleSlot = Vehicle_FindFreeSlot();

    if(vehicleSlot == -1)
    {
        printf("[Vehicles] B????D: Brak wolnych slot??w dla pojazd??w!");
        return INVALID_VEHICLE_ID;
    }

    // Utw??rz pojazd w grze
    new vehicleid = CreateVehicle(modelid, x, y, z, angle, color1, color2, VEHICLE_RESPAWN_DELAY / 1000);

    if(vehicleid == INVALID_VEHICLE_ID)
    {
        printf("[Vehicles] B????D: Nie mo??na utworzy?? pojazdu model %d", modelid);
        return INVALID_VEHICLE_ID;
    }

    // Zapisz dane
    vInfo[vehicleSlot][veh_created] = true;
    vInfo[vehicleSlot][veh_gameid] = vehicleid;
    vInfo[vehicleSlot][veh_model] = modelid;

    vInfo[vehicleSlot][veh_spawn_x] = x;
    vInfo[vehicleSlot][veh_spawn_y] = y;
    vInfo[vehicleSlot][veh_spawn_z] = z;
    vInfo[vehicleSlot][veh_spawn_a] = angle;

    vInfo[vehicleSlot][veh_color1] = color1;
    vInfo[vehicleSlot][veh_color2] = color2;

    vInfo[vehicleSlot][veh_owner_type] = _:ownerType;
    vInfo[vehicleSlot][veh_owner] = ownerId;

    vInfo[vehicleSlot][veh_fuel] = VEHICLE_FUEL_MAX;
    vInfo[vehicleSlot][veh_mileage] = 0.0;
    vInfo[vehicleSlot][veh_health] = 1000.0;
    vInfo[vehicleSlot][veh_locked] = false;

    // Dodaj do iteratora
    Iter_Add(Vehicles, vehicleSlot);

    return vehicleid;
}

/**
 * Znajduje wolny slot dla pojazdu
 * @return ID slotu lub -1 je??li brak
 */
Vehicle_FindFreeSlot()
{
    for(new i = 1; i < MAX_VEHICLES; i++)
    {
        if(!vInfo[i][veh_created])
        {
            return i;
        }
    }
    return -1;
}

/**
 * Usuwa pojazd
 * @param vehicleSlot Slot pojazdu w tablicy
 * @return 1 je??li sukces
 */
Vehicle_Destroy(vehicleSlot)
{
    if(vehicleSlot <= 0 || vehicleSlot >= MAX_VEHICLES || !vInfo[vehicleSlot][veh_created])
    {
        return 0;
    }

    // Usuń pojazd z gry
    if(vInfo[vehicleSlot][veh_gameid] != INVALID_VEHICLE_ID)
    {
        DestroyVehicle(vInfo[vehicleSlot][veh_gameid]);
    }

    // Wyczyść dane
    static const emptyVeh[e_vehicles];
    vInfo[vehicleSlot] = emptyVeh;

    // Usuń z iteratora
    Iter_Remove(Vehicles, vehicleSlot);

    return 1;
}

// ===========================================================================
// WYSZUKIWANIE
// ===========================================================================

/**
 * Znajduje slot pojazdu po jego ID w grze
 * @param vehicleid ID pojazdu w grze
 * @return Slot lub -1 je??li nie znaleziono
 */
Vehicle_GetSlot(vehicleid)
{
    foreach(new i : Vehicles)
    {
        if(vInfo[i][veh_gameid] == vehicleid)
        {
            return i;
        }
    }
    return -1;
}

/**
 * Sprawdza czy pojazd jest pojazdem serwera
 * @param vehicleid ID pojazdu w grze
 * @return true je??li pojazd serwera
 */
bool:Vehicle_IsServerVehicle(vehicleid)
{
    return Vehicle_GetSlot(vehicleid) != -1;
}

/**
 * Pobiera pojazdy gracza
 * @param playerid ID gracza
 * @param output Tablica wyj??ciowa
 * @param maxOutput Max wynik??w
 * @return Liczba znalezionych pojazd??w
 */
Vehicle_GetPlayerVehicles(playerid, output[], maxOutput = sizeof(output))
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return 0;
    }

    new uid = pInfo[playerid][player_id];
    new count = 0;

    foreach(new i : Vehicles)
    {
        if(vInfo[i][veh_owner_type] == _:OWNER_PLAYER && vInfo[i][veh_owner] == uid)
        {
            if(count < maxOutput)
            {
                output[count] = i;
                count++;
            }
        }
    }

    return count;
}

/**
 * Pobiera pojazdy grupy
 * @param groupId ID grupy
 * @param output Tablica wyj??ciowa
 * @param maxOutput Max wynik??w
 * @return Liczba znalezionych pojazd??w
 */
Vehicle_GetGroupVehicles(groupId, output[], maxOutput = sizeof(output))
{
    new count = 0;

    foreach(new i : Vehicles)
    {
        if(vInfo[i][veh_owner_type] == _:OWNER_GROUP && vInfo[i][veh_owner] == groupId)
        {
            if(count < maxOutput)
            {
                output[count] = i;
                count++;
            }
        }
    }

    return count;
}

// ===========================================================================
// FUNKCJE PALIWA
// ===========================================================================

/**
 * Pobiera ilo???? paliwa pojazdu
 * @param vehicleid ID pojazdu
 * @return Ilo???? paliwa (0-100) lub -1 je??li b????d
 */
Vehicle_GetFuel(vehicleid)
{
    new slot = Vehicle_GetSlot(vehicleid);

    if(slot == -1)
    {
        return -1;
    }

    return vInfo[slot][veh_fuel];
}

/**
 * Ustawia paliwo pojazdu
 * @param vehicleid ID pojazdu
 * @param fuel Ilo???? paliwa
 */
Vehicle_SetFuel(vehicleid, fuel)
{
    new slot = Vehicle_GetSlot(vehicleid);

    if(slot == -1)
    {
        return 0;
    }

    vInfo[slot][veh_fuel] = clamp(fuel, 0, VEHICLE_FUEL_MAX);
    return 1;
}

/**
 * Tankuje pojazd
 * @param vehicleid ID pojazdu
 * @param amount Ilo???? paliwa do zatankowania
 * @return Ilo???? faktycznie zatankowanego paliwa
 */
Vehicle_Refuel(vehicleid, amount)
{
    new slot = Vehicle_GetSlot(vehicleid);

    if(slot == -1)
    {
        return 0;
    }

    new currentFuel = vInfo[slot][veh_fuel];
    new newFuel = clamp(currentFuel + amount, 0, VEHICLE_FUEL_MAX);
    new refueled = newFuel - currentFuel;

    vInfo[slot][veh_fuel] = newFuel;

    return refueled;
}

/**
 * Zu??ywa paliwo podczas jazdy
 * @param vehicleid ID pojazdu
 * @param distance Przejechany dystans
 */
Vehicle_ConsumeFuel(vehicleid, Float:distance)
{
    new slot = Vehicle_GetSlot(vehicleid);

    if(slot == -1)
    {
        return 0;
    }

    // Oblicz zu??ycie
    new Float:consumption = distance * VEHICLE_MILEAGE_FUEL_RATE;
    vInfo[slot][veh_fuel] -= floatround(consumption, floatround_ceil);

    if(vInfo[slot][veh_fuel] < 0)
    {
        vInfo[slot][veh_fuel] = 0;
    }

    // Aktualizuj przebieg
    vInfo[slot][veh_mileage] += distance / 1000.0; // km

    return 1;
}

// ===========================================================================
// FUNKCJE BLOKADY
// ===========================================================================

/**
 * Blokuje/odblokowuje pojazd
 * @param vehicleid ID pojazdu
 * @param locked Stan blokady
 */
Vehicle_SetLocked(vehicleid, bool:locked)
{
    new slot = Vehicle_GetSlot(vehicleid);

    if(slot != -1)
    {
        vInfo[slot][veh_locked] = locked;
    }

    // Aktualizuj parametry drzwi
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vehicleid, engine, lights, alarm, locked ? 1 : 0, bonnet, boot, objective);

    return 1;
}

/**
 * Zmienia zapisane miejsce parkowania pojazdu
 * @param vehicleid ID pojazdu
 * @param x, y, z, angle Nowa pozycja i kierunek
 */
Vehicle_SetSpawnPos(vehicleid, Float:x, Float:y, Float:z, Float:angle)
{
    new slot = Vehicle_GetSlot(vehicleid);

    if(slot == -1)
    {
        return 0;
    }

    vInfo[slot][veh_spawn_x] = x;
    vInfo[slot][veh_spawn_y] = y;
    vInfo[slot][veh_spawn_z] = z;
    vInfo[slot][veh_spawn_a] = angle;

    return 1;
}

/**
 * Sprawdza czy pojazd jest zablokowany
 * @param vehicleid ID pojazdu
 * @return true je??li zablokowany
 */
bool:Vehicle_IsLocked(vehicleid)
{
    new slot = Vehicle_GetSlot(vehicleid);

    if(slot == -1)
    {
        return false;
    }

    return vInfo[slot][veh_locked];
}

/**
 * Prze??cza stan blokady pojazdu i zwraca nowy stan
 * @param vehicleid ID pojazdu
 * @return 1 gdy zablokowany, 0 gdy odblokowany, -1 gdy b??d
 */
Vehicle_ToggleLock(vehicleid)
{
    new slot = Vehicle_GetSlot(vehicleid);

    if(slot == -1)
    {
        return -1;
    }

    new bool:newState = !vInfo[slot][veh_locked];
    Vehicle_SetLocked(vehicleid, newState);

    return newState;
}

// ===========================================================================
// FUNKCJE SILNIKA
// ===========================================================================

/**
 * W????cza/wy????cza silnik pojazdu
 * @param vehicleid ID pojazdu
 * @param state Stan silnika
 * @return 1 je??li sukces, 0 je??li brak paliwa
 */
Vehicle_SetEngine(vehicleid, bool:engineState)
{
    new slot = Vehicle_GetSlot(vehicleid);

    // Sprawd?? paliwo
    if(engineState && slot != -1 && vInfo[slot][veh_fuel] <= 0)
    {
        return 0; // Brak paliwa
    }

    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);

    printf("[VDBG] Vehicle_SetEngine vehicle=%d slot=%d nativeEngine=%d -> setEngine=%d\n", vehicleid, slot, engine, engineState);

    SetVehicleParamsEx(vehicleid, engineState ? 1 : 0, lights, alarm, doors, bonnet, boot, objective);

    // Keep server-side cache in sync
    if(slot != -1)
    {
        vInfo[slot][veh_engine] = engineState;
        printf("[VDBG] Vehicle_SetEngine updated vInfo slot=%d veh_engine=%d\n", slot, vInfo[slot][veh_engine]);
    }

    return 1;
}

/**
 * Sprawdza stan silnika
 * @param vehicleid ID pojazdu
 * @return true je??li silnik w????czony
 */
bool:Vehicle_IsEngineOn(vehicleid)
{
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    return engine == 1;
}

/**
 * Ustawia stan świateł pojazdu
 * @param vehicleid ID pojazdu
 * @param lightsState Stan świateł
 * @return 1 jeżeli sukces
 */
Vehicle_SetLights(vehicleid, bool:lightsState)
{
    new slot = Vehicle_GetSlot(vehicleid);

    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);

    printf("[VDBG] Vehicle_SetLights vehicle=%d slot=%d nativeLights=%d -> setLights=%d\n", vehicleid, slot, lights, lightsState);

    SetVehicleParamsEx(vehicleid, engine, lightsState ? 1 : 0, alarm, doors, bonnet, boot, objective);

    // Keep server-side cache in sync
    if(slot != -1)
    {
        vInfo[slot][veh_lights] = lightsState;
        printf("[VDBG] Vehicle_SetLights updated vInfo slot=%d veh_lights=%d\n", slot, vInfo[slot][veh_lights]);
    }

    return 1;
}

// ===========================================================================
// FUNKCJE W??ASNO??CI
// ===========================================================================

/** * Ustawia właściciela pojazdu
 * @param slot Slot pojazdu
 * @param playerid ID gracza (nowy właściciel)
 */
Vehicle_SetOwner(slot, playerid)
{
    if(slot <= 0 || slot >= MAX_VEHICLES || !vInfo[slot][veh_created])
    {
        return 0;
    }

    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    vInfo[slot][veh_owner_type] = _:OWNER_PLAYER;
    vInfo[slot][veh_owner] = pInfo[playerid][player_id];

    return 1;
}

/** * Sprawdza czy gracz jest w??a??cicielem pojazdu
 * @param playerid ID gracza
 * @param vehicleid ID pojazdu
 * @return true je??li w??a??ciciel
 */
bool:Vehicle_IsOwner(playerid, vehicleid)
{
    if(!IsValidPlayerId(playerid))
    {
        return false;
    }

    new slot = Vehicle_GetSlot(vehicleid);

    if(slot == -1)
    {
        return false;
    }

    // Sprawd?? w??asno???? gracza
    if(vInfo[slot][veh_owner_type] == _:OWNER_PLAYER)
    {
        return vInfo[slot][veh_owner] == pInfo[playerid][player_id];
    }

    // Sprawd?? w??asno???? grupy
    if(vInfo[slot][veh_owner_type] == _:OWNER_GROUP)
    {
        return Player_IsInGroup(playerid, vInfo[slot][veh_owner]);
    }

    return false;
}

/**
 * Sprawdza czy gracz mo??e u??ywa?? pojazdu
 * @param playerid ID gracza
 * @param vehicleid ID pojazdu
 * @return true je??li mo??e u??ywa??
 */
bool:Vehicle_CanUse(playerid, vehicleid)
{
    if(!IsValidPlayerId(playerid))
    {
        return false;
    }

    new slot = Vehicle_GetSlot(vehicleid);

    // Pojazd nierejestrowany - ka??dy mo??e
    if(slot == -1)
    {
        return true;
    }

    // Pojazd serwera - ka??dy mo??e
    if(vInfo[slot][veh_owner_type] == _:OWNER_SERVER ||
       vInfo[slot][veh_owner_type] == _:OWNER_NONE)
    {
        return true;
    }

    // W??a??ciciel mo??e zawsze
    if(Vehicle_IsOwner(playerid, vehicleid))
    {
        return true;
    }

    // Pojazd nie jest zablokowany
    if(!vInfo[slot][veh_locked])
    {
        return true;
    }

    return false;
}

// ===========================================================================
// RESPAWN
// ===========================================================================

/**
 * Respawnuje pojazd na oryginalnej pozycji
 * @param vehicleid ID pojazdu
 */
Vehicle_Respawn(vehicleid)
{
    new slot = Vehicle_GetSlot(vehicleid);

    if(slot != -1)
    {
        // Przywr???? paliwo i stan
        vInfo[slot][veh_fuel] = VEHICLE_FUEL_MAX;
        vInfo[slot][veh_health] = 1000.0;
    }

    SetVehicleToRespawn(vehicleid);
    return 1;
}

/**
 * Naprawia pojazd
 * @param vehicleid ID pojazdu
 */
Vehicle_Repair(vehicleid)
{
    new slot = Vehicle_GetSlot(vehicleid);

    if(slot != -1)
    {
        vInfo[slot][veh_health] = 1000.0;
    }

    RepairVehicle(vehicleid);
    return 1;
}

// ===========================================================================
// WCZYTYWANIE Z BAZY
// ===========================================================================

/**
 * Wczytuje pojazdy z bazy danych
 */
Vehicles_Load()
{
    new query[256];
    mysql_format(mySQLconnection, query, sizeof(query),
        "SELECT * FROM `vehicles` ORDER BY `veh_id` ASC"
    );

    mysql_tquery(mySQLconnection, query, "Vehicles_OnLoad");
}

forward Vehicles_OnLoad();
public Vehicles_OnLoad()
{
    new rows = cache_num_rows();
    new loaded = 0;

    for(new i = 0; i < rows; i++)
    {
        new slot;
        cache_get_value_int(i, "veh_id", slot);

        if(slot <= 0 || slot >= MAX_VEHICLES)
        {
            continue;
        }

        new modelid, color1, color2;
        new Float:x, Float:y, Float:z, Float:a;

        cache_get_value_int(i, "veh_model", modelid);
        cache_get_value_float(i, "veh_pos_x", x);
        cache_get_value_float(i, "veh_pos_y", y);
        cache_get_value_float(i, "veh_pos_z", z);
        cache_get_value_float(i, "veh_pos_a", a);
        cache_get_value_int(i, "veh_color1", color1);
        cache_get_value_int(i, "veh_color2", color2);

        // Utw??rz pojazd w grze
        new vehicleid = CreateVehicle(modelid, x, y, z, a, color1, color2, VEHICLE_RESPAWN_DELAY / 1000);

        if(vehicleid == INVALID_VEHICLE_ID)
        {
            continue;
        }

        // Wczytaj pozosta??e dane
        vInfo[slot][veh_created] = true;
        vInfo[slot][veh_gameid] = vehicleid;
        vInfo[slot][veh_model] = modelid;
        vInfo[slot][veh_spawn_x] = x;
        vInfo[slot][veh_spawn_y] = y;
        vInfo[slot][veh_spawn_z] = z;
        vInfo[slot][veh_spawn_a] = a;
        vInfo[slot][veh_color1] = color1;
        vInfo[slot][veh_color2] = color2;

        cache_get_value_int(i, "veh_owner_type", vInfo[slot][veh_owner_type]);
        cache_get_value_int(i, "veh_owner", vInfo[slot][veh_owner]);
        cache_get_value_int(i, "veh_fuel", vInfo[slot][veh_fuel]);
        cache_get_value_float(i, "veh_mileage", vInfo[slot][veh_mileage]);

        // Dodaj do iteratora
        Iter_Add(Vehicles, slot);
        loaded++;
    }

    printf("[Vehicles] Wczytano %d pojazd??w", loaded);
    return 1;
}

/**
 * Zapisuje pojazd do bazy
 * @param slot Slot pojazdu
 */
Vehicle_Save(slot)
{
    if(slot <= 0 || slot >= MAX_VEHICLES || !vInfo[slot][veh_created])
    {
        return 0;
    }

    new query[512];
    mysql_format(mySQLconnection, query, sizeof(query),
        "REPLACE INTO `vehicles` \
         (`veh_id`, `veh_model`, `veh_pos_x`, `veh_pos_y`, `veh_pos_z`, `veh_pos_a`, \
          `veh_color1`, `veh_color2`, `veh_owner_type`, `veh_owner`, `veh_fuel`, `veh_mileage`) \
         VALUES (%d, %d, %f, %f, %f, %f, %d, %d, %d, %d, %d, %f)",
        slot, vInfo[slot][veh_model],
        vInfo[slot][veh_spawn_x], vInfo[slot][veh_spawn_y],
        vInfo[slot][veh_spawn_z], vInfo[slot][veh_spawn_a],
        vInfo[slot][veh_color1], vInfo[slot][veh_color2],
        vInfo[slot][veh_owner_type], vInfo[slot][veh_owner],
        vInfo[slot][veh_fuel], vInfo[slot][veh_mileage]
    );

    mysql_tquery(mySQLconnection, query);
    return 1;
}

// ===========================================================================
// HOOKI
// ===========================================================================

hook OnVehicleSpawn@Vehicles(vehicleid)
{
    new slot = Vehicle_GetSlot(vehicleid);

    if(slot != -1)
    {
        // Resetuj stan przy respawnie
        vInfo[slot][veh_fuel] = VEHICLE_FUEL_MAX;
        vInfo[slot][veh_health] = 1000.0;
        vInfo[slot][veh_locked] = false;
    }

    return 1;
}

hook OnVehicleDeath@Vehicles(vehicleid, killerid)
{
    // Log zniszczenia pojazdu
    new slot = Vehicle_GetSlot(vehicleid);

    if(slot != -1)
    {
        printf("[Vehicles] Pojazd #%d (slot %d) zosta?? zniszczony", vehicleid, slot);
    }

    return 1;
}

// ===========================================================================
// EOF
// ===========================================================================

