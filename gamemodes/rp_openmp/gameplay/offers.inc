/*
 * =============================================================================
 *  OFFERS - System ofert
 * =============================================================================
 *
 *  Moduł: gameplay/offers.inc
 *  Opis: System ofert między graczami (sprzedaż przedmiotów, usługi, etc.)
 *
 *  ZMIANY W REFAKTORZE:
 *  - Timer-based expiration zamiast polling
 *  - Lepsze zarządzanie pamięcią
 *  - Walidacja ofert
 *
 * =============================================================================
 */

#if defined _offers_included
    #endinput
#endif
#define _offers_included

// ===========================================================================
// FORWARD DECLARATIONS dla funkcji z vehicles.inc
// Uwaga: te funkcje musza byc zdefiniowane PRZED tym plikiem w kolejnosci includow
// ===========================================================================

// ===========================================================================
// TYPY OFERT
// ===========================================================================

enum E_OFFER_TYPE
{
    OFFER_NONE = 0,
    OFFER_ITEM,         // Przedmiot
    OFFER_VEHICLE,      // Pojazd
    OFFER_PROPERTY,     // Nieruchomo????
    OFFER_REPAIR,       // Naprawa pojazdu
    OFFER_HEAL,         // Leczenie
    OFFER_WEAPON,       // Bro??
    OFFER_DRUGS,        // Narkotyki
    OFFER_HANDSHAKE,    // Powitanie
    OFFER_TAXI,         // Kurs taks??wk??
    OFFER_MECHANIC,     // Us??uga mechanika
    OFFER_ADVERTISE,    // Reklama
    E_OFFER_MAX
};

// ===========================================================================
// STRUKTURA OFERTY
// ===========================================================================

enum e_offer
{
    offer_active,
    E_OFFER_TYPE:offer_type,
    offer_from,
    offer_to,
    offer_price,
    offer_item_id,
    offer_item_value,
    offer_expire_time,
    offer_timer
};

// ===========================================================================
// ZMIENNE
// ===========================================================================

static g_PlayerOffer[MAX_PLAYERS][e_offer];

// Czas wa??no??ci oferty (sekundy)
#define OFFER_EXPIRE_TIME   30

// ===========================================================================
// FUNKCJE G????WNE
// ===========================================================================

/**
 * Tworzy now?? ofert??
 * @param playerid ID gracza sk??adaj??cego ofert??
 * @param targetid ID gracza otrzymuj??cego ofert??
 * @param offerType Typ oferty
 * @param price Cena
 * @param itemId ID przedmiotu (opcjonalne)
 * @param itemValue Warto???? przedmiotu (opcjonalne)
 */
stock Offer_Create(playerid, targetid, E_OFFER_TYPE:offerType, price = 0, itemId = 0, itemValue = 0)
{
    if(!IsValidPlayerId(playerid) || !IsValidPlayerId(targetid))
    {
        return 0;
    }

    if(!pInfo[playerid][player_logged] || !pInfo[targetid][player_logged])
    {
        SendClientMessage(playerid, COLOR_ERROR, "Player is not logged in.");
        return 0;
    }

    if(playerid == targetid)
    {
        SendClientMessage(playerid, COLOR_ERROR, "You cannot make an offer to yourself.");
        return 0;
    }

    // Check distance
    new Float:dist = GetPlayerDistanceFromPlayer(playerid, targetid);
    if(dist > 5.0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Player is too far away.");
        return 0;
    }

    // Check if the player already has an active offer
    if(g_PlayerOffer[targetid][offer_active])
    {
        SendClientMessage(playerid, COLOR_ERROR, "This player already has a pending offer.");
        return 0;
    }

    // Check if the offerer has enough money (for certain types)
    if(price < 0 && Player_GetMoney(playerid) < -price)
    {
        SendClientMessage(playerid, COLOR_ERROR, "You do not have enough money.");
        return 0;
    }

    // Create offer
    g_PlayerOffer[targetid][offer_active] = 1;
    g_PlayerOffer[targetid][offer_type] = offerType;
    g_PlayerOffer[targetid][offer_from] = playerid;
    g_PlayerOffer[targetid][offer_to] = targetid;
    g_PlayerOffer[targetid][offer_price] = price;
    g_PlayerOffer[targetid][offer_item_id] = itemId;
    g_PlayerOffer[targetid][offer_item_value] = itemValue;
    g_PlayerOffer[targetid][offer_expire_time] = gettime() + OFFER_EXPIRE_TIME;

    // Timer wyga??ni??cia
    g_PlayerOffer[targetid][offer_timer] = SetTimerEx("OnOfferExpire", OFFER_EXPIRE_TIME * 1000, false, "i", targetid);

    // Wy??lij wiadomo??ci
    new msg[256];
    new typeName[32];
    Offer_GetTypeName(offerType, typeName);

    format(msg, sizeof(msg),
        "%s offers you: {FFFFFF}%s\n{3498DB}Price: {FFFFFF}%s$\n\n{2ECC71}/accept{FFFFFF} to accept\n{E74C3C}/reject{FFFFFF} to decline",
        Player_RPName(playerid), typeName, FormatMoney(price));

    ShowPlayerDialog(targetid, DIALOG_OFFER, DIALOG_STYLE_MSGBOX,
        "Offer", msg, "Accept", "Reject");
    format(msg, sizeof(msg), "You have made an offer to %s: %s for %s$",
        Player_RPName(targetid), typeName, FormatMoney(price));
    SendClientMessage(playerid, COLOR_INFO, msg);

    return 1;
}

/**
 * Akceptuje aktywn?? ofert??
 * @param playerid ID gracza akceptuj??cego
 */
stock Offer_Accept(playerid)
{
    if(!IsValidPlayerId(playerid) || !g_PlayerOffer[playerid][offer_active])
    {
        SendClientMessage(playerid, COLOR_ERROR, "You do not have any pending offers.");
        return 0;
    }

    new fromid = g_PlayerOffer[playerid][offer_from];
    new price = g_PlayerOffer[playerid][offer_price];

    // Sprawd?? czy sk??adaj??cy jest online
    if(!IsValidPlayerId(fromid) || !pInfo[fromid][player_logged])
    {
        SendClientMessage(playerid, COLOR_ERROR, "The player who made the offer is offline.");
        Offer_Cancel(playerid);
        return 0;
    }

    // Check distance
    new Float:dist = GetPlayerDistanceFromPlayer(playerid, fromid);
    if(dist > 10.0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Player is too far away.");
        return 0;
    }

    // Check if the acceptor has enough money
    if(price > 0 && Player_GetMoney(playerid) < price)
    {
        SendClientMessage(playerid, COLOR_ERROR, "You do not have enough money.");
        return 0;
    }

    // Perform action depending on type
    new success = 0;

    switch(g_PlayerOffer[playerid][offer_type])
    {
        case OFFER_ITEM:
        {
            success = Offer_ProcessItem(playerid, fromid);
        }

        case OFFER_VEHICLE:
        {
            success = Offer_ProcessVehicle(playerid, fromid);
        }

        case OFFER_REPAIR:
        {
            success = Offer_ProcessRepair(playerid, fromid);
        }

        case OFFER_HEAL:
        {
            success = Offer_ProcessHeal(playerid, fromid);
        }

        case OFFER_HANDSHAKE:
        {
            success = Offer_ProcessHandshake(playerid, fromid);
        }

        case OFFER_TAXI:
        {
            success = Offer_ProcessTaxi(playerid, fromid);
        }

        default:
        {
            success = 1; // Default acceptance
        }
    }

    if(success)
    {
        // Przeka?? pieni??dze
        if(price > 0)
        {
            Player_GiveMoney(playerid, -price);
            Player_GiveMoney(fromid, price);
        }
        else if(price < 0)
        {
            Player_GiveMoney(playerid, -price);
            Player_GiveMoney(fromid, price);
        }

        // RP messages
        new msg[128];
        format(msg, sizeof(msg), "* %s accepts an offer from %s.",
            Player_RPName(playerid), Player_RPName(fromid));
        SendLocalMessage(playerid, COLOR_ME, msg, 10.0);

        SendClientMessage(playerid, COLOR_SUCCESS, "You have accepted the offer.");
        SendClientMessage(fromid, COLOR_SUCCESS, "Your offer has been accepted.");
    }

    // Clear offer
    Offer_Cancel(playerid);

    return success;
}

/**
 * Odrzuca aktywn?? ofert??
 * @param playerid ID gracza odrzucaj??cego
 */
stock Offer_Reject(playerid)
{
    if(!IsValidPlayerId(playerid) || !g_PlayerOffer[playerid][offer_active])
    {
        SendClientMessage(playerid, COLOR_ERROR, "You do not have any pending offers.");
        return 0;
    }

    new fromid = g_PlayerOffer[playerid][offer_from];

    SendClientMessage(playerid, COLOR_WARNING, "You have rejected the offer.");

    if(IsValidPlayerId(fromid) && pInfo[fromid][player_logged])
    {
        SendClientMessage(fromid, COLOR_WARNING, "Your offer has been rejected.");
    }

    Offer_Cancel(playerid);

    return 1;
}

/**
 * Anuluje ofert?? (wewn??trzna)
 * @param playerid ID gracza
 */
stock Offer_Cancel(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    // Anuluj timer
    if(g_PlayerOffer[playerid][offer_timer] != 0)
    {
        KillTimer(g_PlayerOffer[playerid][offer_timer]);
        g_PlayerOffer[playerid][offer_timer] = 0;
    }

    // Resetuj ofert??
    g_PlayerOffer[playerid][offer_active] = 0;
    g_PlayerOffer[playerid][offer_type] = OFFER_NONE;
    g_PlayerOffer[playerid][offer_from] = INVALID_PLAYER_ID;
    g_PlayerOffer[playerid][offer_to] = INVALID_PLAYER_ID;
    g_PlayerOffer[playerid][offer_price] = 0;
    g_PlayerOffer[playerid][offer_item_id] = 0;
    g_PlayerOffer[playerid][offer_item_value] = 0;

    return 1;
}

// ===========================================================================
// PRZETWARZANIE TYP??W OFERT
// ===========================================================================

/**
 * Przetwarza ofert?? przedmiotu
 */
static stock Offer_ProcessItem(playerid, fromid)
{
    new itemSlot = g_PlayerOffer[playerid][offer_item_id];

    // Check if item exists
    if(itemSlot < 0 || itemSlot >= MAX_PLAYER_ITEMS || pItem[fromid][itemSlot][item_id] == 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Item does not exist.");
        return 0;
    }

    // Transfer item
    if(Item_Transfer(fromid, playerid, itemSlot))
    {
        new msg[128];
        format(msg, sizeof(msg), "You have received an item: %s", pItem[playerid][itemSlot][item_name]);
        SendClientMessage(playerid, COLOR_INFO, msg);

        return 1;
    }

    return 0;
}

/**
 * Przetwarza ofert?? pojazdu
 */
static stock Offer_ProcessVehicle(playerid, fromid)
{
    new vehicleSlot = g_PlayerOffer[playerid][offer_item_id];

    // Sprawdź czy pojazd istnieje i należy do składającego
    if(!Vehicle_IsOwner(fromid, vehicleSlot))
    {
        SendClientMessage(playerid, COLOR_ERROR, "The vehicle does not belong to the seller.");
        return 0;
    }

    // Transfer ownership
    Vehicle_SetOwner(vehicleSlot, playerid);
    Vehicle_Save(vehicleSlot);

    new msg[128];
    format(msg, sizeof(msg), "You have purchased a vehicle from %s.", Player_RPName(fromid));
    SendClientMessage(playerid, COLOR_SUCCESS, msg);

    return 1;
}

/**
 * Przetwarza ofert?? naprawy
 */
static stock Offer_ProcessRepair(playerid, fromid)
{
    new vehicleid = GetPlayerVehicleID(playerid);

    if(vehicleid == 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "You must be in a vehicle.");
        return 0;
    }

    RepairVehicle(vehicleid);
    SetVehicleHealth(vehicleid, 1000.0);

    // RP action
    new msg[128];
    format(msg, sizeof(msg), "* %s repairs the vehicle of %s.",
        Player_RPName(fromid), Player_RPName(playerid));
    SendLocalMessage(playerid, COLOR_ME, msg, 15.0);

    return 1;
}

/**
 * Przetwarza ofert?? leczenia
 */
static stock Offer_ProcessHeal(playerid, fromid)
{
    new healAmount = g_PlayerOffer[playerid][offer_item_value];

    if(healAmount <= 0) healAmount = 100;

    new Float:health;
    GetPlayerHealth(playerid, health);

    health = (health + float(healAmount) > 100.0) ? 100.0 : (health + float(healAmount));
    SetPlayerHealth(playerid, health);
    Player_SetHealth(playerid, health);

    // RP action
    new msg[128];
    format(msg, sizeof(msg), "* %s heals the wounds of %s.",
        Player_RPName(fromid), Player_RPName(playerid));
    SendLocalMessage(playerid, COLOR_ME, msg, 15.0);

    return 1;
}

/**
 * Przetwarza u??cisk d??oni
 */
static stock Offer_ProcessHandshake(playerid, fromid)
{
    // RP action
    new msg[128];
    format(msg, sizeof(msg), "* %s shakes the hand of %s.",
        Player_RPName(fromid), Player_RPName(playerid));
    SendLocalMessage(playerid, COLOR_ME, msg, 15.0);

    format(msg, sizeof(msg), "* %s shakes the hand of %s.",
        Player_RPName(playerid), Player_RPName(fromid));
    SendLocalMessage(playerid, COLOR_ME, msg, 15.0);

    return 1;
}

/**
 * Przetwarza ofert?? taks??wki
 */
static stock Offer_ProcessTaxi(playerid, fromid)
{
    // Gracz wsiada do taks??wki
    new vehicleid = GetPlayerVehicleID(fromid);

    if(vehicleid == 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "The taxi driver is not in a vehicle.");
        return 0;
    }

    PutPlayerInVehicle(playerid, vehicleid, 1); // As a passenger

    SendClientMessage(playerid, COLOR_INFO, "You have entered the taxi.");
    SendClientMessage(fromid, COLOR_INFO, "A passenger has entered the taxi.");

    return 1;
}

// ===========================================================================
// FUNKCJE POMOCNICZE
// ===========================================================================

/**
 * Pobiera nazw?? typu oferty
 */
stock Offer_GetTypeName(E_OFFER_TYPE:offerType, output[])
{
    static const names[][] = {
        "None",
        "Item",
        "Vehicle",
        "Property",
        "Vehicle Repair",
        "Healing",
        "Gun",
        "Drugs",
        "Handshake",
        "Taxi Ride",
        "Mechanic Service",
        "Advertisement"
    };

    if(offerType >= OFFER_NONE && offerType < E_OFFER_MAX)
    {
        strcopy(output, names[_:offerType], 32);
    }
    else
    {
        strcopy(output, "Unknown", 32);
    }
}

/**
 * Sprawdza czy gracz ma aktywn?? ofert??
 */
stock bool:Offer_HasPending(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return false;
    }

    return g_PlayerOffer[playerid][offer_active] != 0;
}

/**
 * Pobiera dystans mi??dzy graczami
 */
stock Float:GetPlayerDistanceFromPlayer(playerid, targetid)
{
    new Float:x1, Float:y1, Float:z1;
    new Float:x2, Float:y2, Float:z2;

    GetPlayerPos(playerid, x1, y1, z1);
    GetPlayerPos(targetid, x2, y2, z2);

    return GetDistanceBetweenPoints3D(x1, y1, z1, x2, y2, z2);
}

// ===========================================================================
// CALLBACK WYGA??NI??CIA
// ===========================================================================

forward OnOfferExpire(playerid);
public OnOfferExpire(playerid)
{
    if(!IsValidPlayerId(playerid) || !g_PlayerOffer[playerid][offer_active])
    {
        return 1;
    }

    new fromid = g_PlayerOffer[playerid][offer_from];

    SendClientMessage(playerid, COLOR_WARNING, "Offer has expired.");

    if(IsValidPlayerId(fromid) && pInfo[fromid][player_logged])
    {
        SendClientMessage(fromid, COLOR_WARNING, "Your offer has expired.");
    }

    g_PlayerOffer[playerid][offer_timer] = 0;
    Offer_Cancel(playerid);

    return 1;
}

// ===========================================================================
// KOMENDY
// ===========================================================================

YCMD:accept(playerid, params[], help)
{
    return Offer_Accept(playerid);
}

YCMD:reject(playerid, params[], help)
{
    return Offer_Reject(playerid);
}

YCMD:shakehands(playerid, params[], help)
{
    if(!pInfo[playerid][player_logged])
    {
        return SendClientMessage(playerid, COLOR_ERROR, "You must be logged in!");
    }

    new targetid;
    if(sscanf(params, "u", targetid))
    {
        return SendClientMessage(playerid, COLOR_GREY, "Usage: /shakehands [id/nick]");
    }

    return Offer_Create(playerid, targetid, OFFER_HANDSHAKE, 0);
}

// ===========================================================================
// HOOKI
// ===========================================================================

hook OnPlayerConnect@Offers(playerid)
{
    Offer_Cancel(playerid);
    return 1;
}

hook OnPlayerDisconnect@Offers(playerid, reason)
{
    Offer_Cancel(playerid);
    return 1;
}

hook OnDialogResponse@Offers(playerid, dialogid, response, listitem, inputtext[])
{
    if(dialogid == DIALOG_OFFER)
    {
        if(response)
        {
            Offer_Accept(playerid);
        }
        else
        {
            Offer_Reject(playerid);
        }

        return Y_HOOKS_BREAK_RETURN_1;
    }

    return 1;
}

// ===========================================================================
// EOF
// ===========================================================================


