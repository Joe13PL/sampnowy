/*
 * =============================================================================
 *  ITEMS - System przedmiot??w
 * =============================================================================
 *
 *  Modu??: gameplay/items.inc
 *  Opis: Zarz??dzanie przedmiotami (tworzenie, u??ywanie, transfer)
 *
 *  ZMIANY W REFAKTORZE:
 *  - Zdefiniowano typy przedmiotu jako enum
 *  - Dodano obs??ug?? stosu przedmiot??w
 *  - Zoptymalizowano wyszukiwanie w ekwipunku
 *  - Dodano system u??ycia przedmiot??w
 *
 * =============================================================================
 */

// Wyciszenie warningów tag mismatch
#pragma warning disable 213

#if defined _items_included
    #endinput
#endif
#define _items_included

// ===========================================================================
// TYPY PRZEDMIOT??W
// ===========================================================================

enum E_ITEM_TYPE
{
    ITEM_TYPE_NONE = 0,

    // Dokumenty
    ITEM_TYPE_ID_CARD = 1,          // Dow??d osobisty
    ITEM_TYPE_DRIVING_LICENSE,      // Prawo jazdy
    ITEM_TYPE_GUN_LICENSE,          // Pozwolenie na bro??
    ITEM_TYPE_MEDICAL_CARD,         // Karta zdrowia

    // Jedzenie i picie
    ITEM_TYPE_FOOD = 10,            // Jedzenie
    ITEM_TYPE_DRINK,                // Nap??j
    ITEM_TYPE_ALCOHOL,              // Alkohol
    ITEM_TYPE_DRUG,                 // Narkotyki

    // Medyczne
    ITEM_TYPE_BANDAGE = 20,         // Banda??
    ITEM_TYPE_MEDKIT,               // Apteczka
    ITEM_TYPE_PAINKILLER,           // Leki przeciwb??lowe

    // Narz??dzia
    ITEM_TYPE_PHONE = 30,
    ITEM_TYPE_SIMCARD,              // SIM Card
    ITEM_TYPE_RADIO,                // Radio
    ITEM_TYPE_GPS,                  // GPS
    ITEM_TYPE_CAMERA,               // Aparat
    ITEM_TYPE_FISHING_ROD,          // W??dka
    ITEM_TYPE_ROPE,                 // Lina
    ITEM_TYPE_LOCKPICK,             // Wytrych

    // Bro??
    ITEM_TYPE_WEAPON = 50,          // Bro??
    ITEM_TYPE_AMMO,                 // Amunicja
    ITEM_TYPE_ARMOR,                // Kamizelka

    // Pojazdy
    ITEM_TYPE_CAR_KEY = 60,         // Kluczyki do auta
    ITEM_TYPE_FUEL_CAN,             // Kanister z paliwem
    ITEM_TYPE_REPAIR_KIT,           // Zestaw naprawczy

    // Klucze
    ITEM_TYPE_DOOR_KEY = 70,        // Klucz do drzwi
    ITEM_TYPE_SAFE_KEY,             // Klucz do sejfu

    // Materia??y
    ITEM_TYPE_MATERIALS = 80,       // Materia??y
    ITEM_TYPE_DRUGS_PACKAGE,        // Paczka narkotyk??w

    // Inne
    ITEM_TYPE_MASK = 90,            // Maska
    ITEM_TYPE_SPRAYPAINT,           // Farba w sprayu
    ITEM_TYPE_MONEY_BAG,            // Worek z pieni??dzmi
    ITEM_TYPE_BOOMBOX,              // Boombox

    ITEM_TYPE_MAX
};

// ===========================================================================
// STA??E
// ===========================================================================

#define ITEM_NAME_MAX               32
#define ITEM_DROP_RANGE             3.0
#define ITEM_PICKUP_RANGE           2.0

// ===========================================================================
// FUNKCJE TWORZENIA
// ===========================================================================

/**
 * Tworzy przedmiot dla gracza
 * @param playerid ID gracza
 * @param itemType Typ przedmiotu
 * @param value1 Warto???? 1 (zale??na od typu)
 * @param value2 Warto???? 2 (zale??na od typu)
 * @param name Nazwa przedmiotu (opcjonalna)
 * @return Slot przedmiotu lub -1 je??li b????d
 */
stock Item_CreateForPlayer(playerid, E_ITEM_TYPE:itemType, value1 = 0, value2 = 0, const name[] = "")
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return -1;
    }

    // Znajdź wolny slot
    new freeSlot = -1;
    for(new i = 0; i < MAX_PLAYER_ITEMS; i++)
    {
        if(pItem[playerid][i][item_id] == 0)
        {
            freeSlot = i;
            break;
        }
    }

    if(freeSlot == -1)
    {
        SendClientMessage(playerid, COLOR_RED, "Your inventory is full!");
        return -1;
    }

    // Ustaw dane przedmiotu
    new num; new found;
    pItem[playerid][freeSlot][item_type] = _:itemType;
    pItem[playerid][freeSlot][item_value] = value1;
    pItem[playerid][freeSlot][item_value2] = value2;

    // Ustaw nazw??
    if(strlen(name) > 0)
    {
        strcopy(pItem[playerid][freeSlot][item_name], name, ITEM_NAME_MAX);
    }
    else
    {
        Item_GetDefaultName(itemType, pItem[playerid][freeSlot][item_name], ITEM_NAME_MAX);
    }

    pInfo[playerid][player_item_count]++;
    Player_MarkDataModified(playerid);

    // Zapisz do bazy
    Item_SaveToDatabase(playerid, freeSlot);

    return freeSlot;
}



stock Item_Drop(playerid, slot)
{
    return Item_Remove(playerid, slot);
}

stock Item_Destroy(playerid, slot)
{
    return Item_Remove(playerid, slot);
}




/**
 * Usuwa przedmiot gracza
 * @param playerid ID gracza
 * @param slot Slot przedmiotu
 * @return 1 je??li sukces
 */
stock Item_Remove(playerid, slot)
{
    if(!IsValidPlayerId(playerid) || slot < 0 || slot >= MAX_PLAYER_ITEMS)
    {
        return 0;
    }

    if(pItem[playerid][slot][item_id] == 0)
    {
        return 0;
    }

    // Usu?? z bazy
    new query[128];
        mysql_format(mySQLconnection, query, sizeof(query),
            "DELETE FROM `items` WHERE `item_id` = %d",
            pItem[playerid][slot][item_id]
    );
    mysql_tquery(mySQLconnection, query);

    // Wyczy???? dane
    pItem[playerid][slot][item_id] = 0;
    pItem[playerid][slot][item_type] = 0;
    pItem[playerid][slot][item_value] = 0;
    pItem[playerid][slot][item_value2] = 0;
    pItem[playerid][slot][item_name][0] = EOS;

    pInfo[playerid][player_item_count]--;
    Player_MarkDataModified(playerid);

    return 1;
}

// ===========================================================================
// FUNKCJE WYSZUKIWANIA
// ===========================================================================

/**
 * Szuka przedmiotu okre??lonego typu w ekwipunku gracza
 * @param playerid ID gracza
 * @param itemType Typ przedmiotu
 * @return Slot przedmiotu lub -1 je??li nie znaleziono
 */
stock Item_FindByType(playerid, E_ITEM_TYPE:itemType)
{
    if(!IsValidPlayerId(playerid))
    {
        return -1;
    }

    for(new i = 0; i < MAX_PLAYER_ITEMS; i++)
    {
        if(pItem[playerid][i][item_type] == _:itemType)
        {
            return i;
        }
    }

    return -1;
}

/**
 * Szuka przedmiotu po ID bazy danych
 * @param playerid ID gracza
 * @param itemDbId ID przedmiotu w bazie
 * @return Slot przedmiotu lub -1 je??li nie znaleziono
 */
stock Item_FindById(playerid, itemDbId)
{
    if(!IsValidPlayerId(playerid))
    {
        return -1;
    }

    for(new i = 0; i < MAX_PLAYER_ITEMS; i++)
    {
        if(pItem[playerid][i][item_id] == itemDbId)
        {
            return i;
        }
    }

    return -1;
}

/**
 * Sprawdza czy gracz ma przedmiot okre??lonego typu
 * @param playerid ID gracza
 * @param itemType Typ przedmiotu
 * @return true je??li ma
 */
stock bool:Item_Has(playerid, E_ITEM_TYPE:itemType)
{
    return Item_FindByType(playerid, itemType) != -1;
}

/**
 * Liczy przedmioty okre??lonego typu
 * @param playerid ID gracza
 * @param itemType Typ przedmiotu
 * @return Liczba przedmiot??w
 */
stock Item_Count(playerid, E_ITEM_TYPE:itemType)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    new count = 0;

    for(new i = 0; i < MAX_PLAYER_ITEMS; i++)
    {
        if(pItem[playerid][i][item_type] == _:itemType)
        {
            count++;
        }
    }

    return count;
}

// ===========================================================================
// FUNKCJE U??YCIA
// ===========================================================================

/**
 * U??ywa przedmiotu
 * @param playerid ID gracza
 * @param slot Slot przedmiotu
 * @return 1 je??li sukces
 */
stock Item_Use(playerid, slot)
{
    if(!IsValidPlayerId(playerid) || slot < 0 || slot >= MAX_PLAYER_ITEMS)
    {
        return 0;
    }

    if(pItem[playerid][slot][item_id] == 0)
    {
        return 0;
    }

    new itemType = pItem[playerid][slot][item_type];
    new value1 = pItem[playerid][slot][item_value];
    new value2 = pItem[playerid][slot][item_value2];
    new itemName[ITEM_NAME_MAX];
    strcopy(itemName, pItem[playerid][slot][item_name], ITEM_NAME_MAX);

    new bool:consumed = false;

    switch(E_ITEM_TYPE:itemType)
    {
        // === DOKUMENTY ===
        case ITEM_TYPE_ID_CARD:
        {
            // Poka?? dow??d
            new msg[256];
            format(msg, sizeof(msg),
                "* %s shows their ID card.",
                pInfo[playerid][player_name]);
            SendLocalMessage(playerid, COLOR_PURPLE, msg, 10.0);

            // TODO: Display dialog with data
        }

        case ITEM_TYPE_DRIVING_LICENSE:
        {
            new msg[256];
            format(msg, sizeof(msg),
                "* %s shows their driving license.",
                pInfo[playerid][player_name]);
            SendLocalMessage(playerid, COLOR_PURPLE, msg, 10.0);
        }

        // === JEDZENIE ===
        case ITEM_TYPE_FOOD:
        {
            // Warto???? = ilo???? g??odu do regeneracji
            pInfo[playerid][player_hunger] += value1;
            if(pInfo[playerid][player_hunger] > 100)
            {
                pInfo[playerid][player_hunger] = 100;
            }

            new msg[128];
            format(msg, sizeof(msg),
                "* %s je %s.",
                pInfo[playerid][player_name], itemName);
            SendLocalMessage(playerid, COLOR_PURPLE, msg, 10.0);

            consumed = true;
        }

        case ITEM_TYPE_DRINK:
        {
            pInfo[playerid][player_thirst] += value1;
            if(pInfo[playerid][player_thirst] > 100)
            {
                pInfo[playerid][player_thirst] = 100;
            }

            new msg[128];
            format(msg, sizeof(msg),
                "* %s drinks %s.",
                pInfo[playerid][player_name], itemName);
            SendLocalMessage(playerid, COLOR_PURPLE, msg, 10.0);

            consumed = true;
        }

        case ITEM_TYPE_PHONE:
        {
            // If phone is turned off, prevent usage
            if(pItem[playerid][slot][item_value] == 0)
            {
                SendClientMessage(playerid, COLOR_ERROR, "Phone is turned off.");
                return 1;
            }

            // Store active slot for dialog handlers and open phone menu
            pInfo[playerid][player_dialog_tmp1] = slot;
            Dialog_ShowPhoneMenu(playerid);
            return 1;
        }

        case ITEM_TYPE_ALCOHOL:
        {
            pInfo[playerid][player_thirst] += value1;
            pInfo[playerid][player_drunk] += value2;

            if(pInfo[playerid][player_thirst] > 100)
                pInfo[playerid][player_thirst] = 100;

            new msg[128];
            format(msg, sizeof(msg),
                "* %s drinks %s.",
                pInfo[playerid][player_name], itemName);
            SendLocalMessage(playerid, COLOR_PURPLE, msg, 10.0);

            // Effect of drunkenness
            if(pInfo[playerid][player_drunk] > 2000)
            {
                SetPlayerDrunkLevel(playerid, pInfo[playerid][player_drunk]);
            }

            consumed = true;
        }

        // === MEDYCZNE ===
        case ITEM_TYPE_BANDAGE:
        {
            new Float:health;
            GetPlayerHealth(playerid, health);

            if(health >= 100.0)
            {
                SendClientMessage(playerid, COLOR_RED, "You don't need a bandage!");
                return 0;
            }

            health += float(value1);
            if(health > 100.0) health = 100.0;
            SetPlayerHealth(playerid, health);

            new msg[128];
            format(msg, sizeof(msg),
                "* %s uses a bandage.",
                pInfo[playerid][player_name]);
            SendLocalMessage(playerid, COLOR_PURPLE, msg, 10.0);

            consumed = true;
        }

        case ITEM_TYPE_MEDKIT:
        {
            SetPlayerHealth(playerid, 100.0);

            new msg[128];
            format(msg, sizeof(msg),
                "* %s uses a medkit.",
                pInfo[playerid][player_name]);
            SendLocalMessage(playerid, COLOR_PURPLE, msg, 10.0);

            consumed = true;
        }

        // === MASKA ===
        case ITEM_TYPE_MASK:
        {
            if(pInfo[playerid][player_mask])
            {
                pInfo[playerid][player_mask] = false;
                SendClientMessage(playerid, COLOR_GREEN, "You removed your mask.");
            }
            else
            {
                pInfo[playerid][player_mask] = true;
                SendClientMessage(playerid, COLOR_GREEN, "You put on a mask. Your identity is hidden.");
            }
        }

        // === KAMIZELKA ===
        case ITEM_TYPE_ARMOR:
        {
            SetPlayerArmour(playerid, float(value1));

            new msg[128];
            format(msg, sizeof(msg),
                "* %s puts on a bulletproof vest.",
                pInfo[playerid][player_name]);
            SendLocalMessage(playerid, COLOR_PURPLE, msg, 10.0);

            consumed = true;
        }

        // === KANISTER ===
        case ITEM_TYPE_FUEL_CAN:
        {
            // Sprawd?? czy jest w poje??dzie lub obok
            new vehicleid = GetPlayerVehicleID(playerid);

            if(vehicleid == 0)
            {
                vehicleid = GetClosestVehicle(playerid, 5.0);
            }

            if(vehicleid == 0 || vehicleid == INVALID_VEHICLE_ID)
            {
                SendClientMessage(playerid, COLOR_RED, "There is no vehicle nearby!");
                return 0;
            }

            new refueled = Vehicle_Refuel(vehicleid, value1);

            if(refueled > 0)
            {
                new msg[128];
                format(msg, sizeof(msg),
                    "You refueled %d liters of fuel.", refueled);
                SendClientMessage(playerid, COLOR_GREEN, msg);
                consumed = true;
            }
            else
            {
                SendClientMessage(playerid, COLOR_RED, "The tank is full!");
                return 0;
            }
        }

        default:
        {
            SendClientMessage(playerid, COLOR_RED, "You can't use this item!");
            return 0;
        }
    }

    // Remove item if consumed
    if(consumed)
    {
        Item_Remove(playerid, slot);
    }

    return 1;
}

// ===========================================================================
// TRANSFER PRZEDMIOT??W
// ===========================================================================

/**
 * Przekazuje przedmiot innemu graczowi
 * @param fromPlayerid ID daj??cego
 * @param toPlayerid ID odbieraj??cego
 * @param slot Slot przedmiotu
 * @return 1 je??li sukces
 */
stock Item_Transfer(fromPlayerid, toPlayerid, slot)
{
    if(!IsValidPlayerId(fromPlayerid) || !IsValidPlayerId(toPlayerid))
    {
        return 0;
    }

    if(slot < 0 || slot >= MAX_PLAYER_ITEMS)
    {
        return 0;
    }

    if(pItem[fromPlayerid][slot][item_id] == 0)
    {
        return 0;
    }

    // Sprawd?? czy odbiorca ma miejsce
    if(pInfo[toPlayerid][player_item_count] >= MAX_PLAYER_ITEMS)
    {
        SendClientMessage(fromPlayerid, COLOR_RED, "The player has no space in their inventory!");
        return 0;
    }

    // Create item for the recipient
    new newSlot = Item_CreateForPlayer(
        toPlayerid,
        E_ITEM_TYPE:pItem[fromPlayerid][slot][item_type],
        pItem[fromPlayerid][slot][item_value],
        pItem[fromPlayerid][slot][item_value2],
        pItem[fromPlayerid][slot][item_name]
    );

    if(newSlot == -1)
    {
        return 0;
    }

    // Remove from the giver
    Item_Remove(fromPlayerid, slot);

    // Messages
    new msg[128];
    format(msg, sizeof(msg), "You have given %s to player %s.",
        pItem[toPlayerid][newSlot][item_name], pInfo[toPlayerid][player_name]);
    SendClientMessage(fromPlayerid, COLOR_GREEN, msg);

    format(msg, sizeof(msg), "You have received %s from player %s.",
        pItem[toPlayerid][newSlot][item_name], pInfo[fromPlayerid][player_name]);
    SendClientMessage(toPlayerid, COLOR_GREEN, msg);

    return 1;
}

// ===========================================================================
// FUNKCJE POMOCNICZE
// ===========================================================================

/**
 * Pobiera domy??ln?? nazw?? przedmiotu
 * @param itemType Typ przedmiotu
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 */
stock Item_GetDefaultName(E_ITEM_TYPE:itemType, output[], size = ITEM_NAME_MAX)
{
    switch(itemType)
    {
        case ITEM_TYPE_ID_CARD: strcopy(output, "ID Card", size);
        case ITEM_TYPE_DRIVING_LICENSE: strcopy(output, "Driving License", size);
        case ITEM_TYPE_GUN_LICENSE: strcopy(output, "Gun License", size);
        case ITEM_TYPE_MEDICAL_CARD: strcopy(output, "Medical Card", size);
        case ITEM_TYPE_FOOD: strcopy(output, "Food", size);
        case ITEM_TYPE_DRINK: strcopy(output, "Drink", size);
        case ITEM_TYPE_PHONE: strcopy(output, "Phone", size);
        case ITEM_TYPE_SIMCARD: strcopy(output, "SIM Card", size);
        case ITEM_TYPE_ALCOHOL: strcopy(output, "Alcohol", size);
        case ITEM_TYPE_BANDAGE: strcopy(output, "Bandage", size);
        case ITEM_TYPE_MEDKIT: strcopy(output, "Medkit", size);
        case ITEM_TYPE_RADIO: strcopy(output, "Radio", size);
        case ITEM_TYPE_GPS: strcopy(output, "GPS", size);
        case ITEM_TYPE_WEAPON: strcopy(output, "Weapon", size);
        case ITEM_TYPE_AMMO: strcopy(output, "Ammo", size);
        case ITEM_TYPE_ARMOR: strcopy(output, "Armor", size);
        case ITEM_TYPE_CAR_KEY: strcopy(output, "Car Keys", size);
        case ITEM_TYPE_FUEL_CAN: strcopy(output, "Fuel Can", size);
        case ITEM_TYPE_DOOR_KEY: strcopy(output, "Key", size);
        case ITEM_TYPE_MASK: strcopy(output, "Mask", size);
        case ITEM_TYPE_DRUG: strcopy(output, "Drugs", size);
        default: strcopy(output, "Item", size);
    }
}

/**
 * Zapisuje przedmiot do bazy danych
 * @param playerid ID gracza
 * @param slot Slot przedmiotu
 */
stock Item_SaveToDatabase(playerid, slot)
{
    if(pItem[playerid][slot][item_id] == 0)
    {
        // Nowy przedmiot - INSERT
        new query[512];
        new escapedName[65];
        mysql_escape_string(pItem[playerid][slot][item_name], escapedName, mySQLconnection, sizeof(escapedName));

        // If this is a phone and it has no number yet, generate one and assign to the item
        if(pItem[playerid][slot][item_type] == ITEM_TYPE_PHONE && pItem[playerid][slot][item_value2] == 0)
        {
            new owner_uid = pInfo[playerid][player_uid];
            new phone = Phone_GenerateFreeNumber(owner_uid);
            if(phone != 0)
            {
                pItem[playerid][slot][item_value2] = phone;
                printf("[PHONE] Generated phone number %d for player uid=%d slot=%d\n", phone, owner_uid, slot);
            }
            else
            {
                printf("[PHONE] Failed to generate phone number for player uid=%d slot=%d\n", owner_uid, slot);
            }
        }

        mysql_format(mySQLconnection, query, sizeof(query),
            "INSERT INTO `items` (`item_owner`, `item_owner_type`, `item_type`, `item_value`, `item_value2`, `item_phone`, `item_name`) VALUES (%d, 0, %d, %d, %d, %d, '%s')",
            pInfo[playerid][player_id],
            pItem[playerid][slot][item_type],
            pItem[playerid][slot][item_value],
            pItem[playerid][slot][item_value2],
            (pItem[playerid][slot][item_type] == ITEM_TYPE_PHONE) ? pItem[playerid][slot][item_value2] : 0,
            escapedName
        );

        mysql_tquery(mySQLconnection, query, "Item_OnInsert", "dd", playerid, slot);
    }
    else
    {
        // Istniejacy przedmiot - UPDATE
        new query[512];
        new escapedName[65];
        mysql_escape_string(pItem[playerid][slot][item_name], escapedName, mySQLconnection, sizeof(escapedName));

        mysql_format(mySQLconnection, query, sizeof(query),
            "UPDATE `items` SET `item_type` = %d, `item_value` = %d, `item_value2` = %d, `item_phone` = %d, `item_name` = '%s' WHERE `item_id` = %d",
            pItem[playerid][slot][item_type],
            pItem[playerid][slot][item_value],
            pItem[playerid][slot][item_value2],
            (pItem[playerid][slot][item_type] == ITEM_TYPE_PHONE) ? pItem[playerid][slot][item_value2] : 0,
            escapedName,
            pItem[playerid][slot][item_id]
        );

        mysql_tquery(mySQLconnection, query);
        // If this is a phone, synchronously update/create phone_devices record so DB mapping stays current
        if(pItem[playerid][slot][item_type] == ITEM_TYPE_PHONE)
        {
            // Ensure phone_numbers row exists and use the shared helper to insert/update device to avoid FK errors
            Phone_CreateOrUpdateDeviceForItem(pItem[playerid][slot][item_id], pItem[playerid][slot][item_value2], pInfo[playerid][player_uid], 0);
        }
    }
}

forward Item_OnInsert(playerid, slot);
forward Phone_CreateOrUpdateDeviceForItem(p_item_id, phone_number, owner_uid, owner_type);
forward Phone_GenerateFreeNumber(owner_uid);
public Item_OnInsert(playerid, slot)
{
    if(IsPlayerConnected(playerid) && slot >= 0 && slot < MAX_PLAYER_ITEMS)
    {
        pItem[playerid][slot][item_id] = cache_insert_id();
        // If this is a phone, ensure phone_devices entry is created/updated
        if(pItem[playerid][slot][item_type] == ITEM_TYPE_PHONE)
        {
            new created_item_id = pItem[playerid][slot][item_id];
            new phone_num = pItem[playerid][slot][item_value2];
            new owner_uid = pInfo[playerid][player_uid];
            Phone_CreateOrUpdateDeviceForItem(created_item_id, phone_num, owner_uid, 0);
        }
    }
    return 1;
}

/**
 * Wysy??a wiadomo???? lokaln??
 * @param playerid ID gracza
 * @param color Kolor
 * @param message Wiadomo????
 * @param range Zasi??g
 */
stock SendLocalMessage(playerid, color, const message[], Float:range = 15.0)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);

    new vw = GetPlayerVirtualWorld(playerid);
    new interior = GetPlayerInterior(playerid);

    foreach(new i : Player)
    {
        if(GetPlayerVirtualWorld(i) != vw) continue;
        if(GetPlayerInterior(i) != interior) continue;

        if(IsPlayerInRangeOfPoint(i, range, px, py, pz))
        {
            SendClientMessage(i, color, message);
        }
    }

    return 1;
}

/**
 * Pobiera najbli??szy pojazd
 * @param playerid ID gracza
 * @param range Zasi??g
 * @return ID pojazdu lub 0
 */
stock GetClosestVehicle(playerid, Float:range)
{
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);

    new closestVeh = 0;
    new Float:closestDist = range;

    foreach(new vehSlot : Vehicles)
    {
        new vehicleid = vInfo[vehSlot][veh_gameid];

        if(vehicleid != INVALID_VEHICLE_ID)
        {
            new Float:vx, Float:vy, Float:vz;
            GetVehiclePos(vehicleid, vx, vy, vz);

            new Float:dist = GetDistanceBetweenPoints3D(px, py, pz, vx, vy, vz);

            if(dist < closestDist)
            {
                closestDist = dist;
                closestVeh = vehicleid;
            }
        }
    }

    return closestVeh;
}

// ===========================================================================
// EOF
// ===========================================================================


