/*
 * =============================================================================
 *  PLAYER COMMANDS - Komendy gracza
 * =============================================================================
 *
 *  Modu??: player/player_commands.inc
 *  Opis: Podstawowe komendy RP dla graczy
 *
 *  ZMIANY W REFAKTORZE:
 *  - Zast??piono zcmd na y_commands (YCMD)
 *  - Dodano obs??ug?? b????d??w i walidacj??
 *  - Centralne zarz??dzanie uprawnieniami
 *
 * =============================================================================
 */

#if defined _player_commands_included
    #endinput
#endif
#define _player_commands_included

// ===========================================================================
// MAKRA POMOCNICZE
// ===========================================================================

// Sprawdza czy gracz może używać komend
#define CMD_GUARDS(%0) \
    if(!pInfo[playerid][player_logged]) return SendClientMessage(playerid, COLOR_ERROR, "Musisz byc zalogowany!"); \
    if(pInfo[playerid][player_bw] > 0) return SendClientMessage(playerid, COLOR_ERROR, "Nie mozesz tego zrobic podczas BW."); \
    if(pInfo[playerid][player_aj] > 0) return SendClientMessage(playerid, COLOR_ERROR, "Nie mozesz tego zrobic w areszcie.")

// Wymaga znajdowania si?? w poje??dzie
#define CMD_IN_VEHICLE(%0) \
    if(!IsPlayerInAnyVehicle(playerid)) return SendClientMessage(playerid, COLOR_ERROR, "Musisz by?? w poje??dzie!")

// Wymaga bycia kierowcą
#define CMD_IS_DRIVER(%0) \
    if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return SendClientMessage(playerid, COLOR_ERROR, "Musisz być kierowcą!")

// ===========================================================================
// KOMENDY ROLEPLAY
// ===========================================================================

/**
 * /cmdtest - Komenda testowa do debugowania systemu komend
 */
YCMD:cmdtest(playerid, params[], help)
{
    new msg[128];
    format(msg, sizeof(msg), "[CMDTEST] Komenda dziala! logged=%d, bw=%d, aj=%d",
        pInfo[playerid][player_logged],
        pInfo[playerid][player_bw],
        pInfo[playerid][player_aj]);
    SendClientMessage(playerid, 0x00FF00FF, msg);
    return 1;
}

/**
 * /phone - Otwiera menu telefonu (kontakty, historia)
 */
YCMD:phone(playerid, params[], help)
{
    CMD_GUARDS();

    if(!isnull(params))
    {
        new token[32];
        if(sscanf(params, "s", token) == 1)
        {
            if((strcmp(token, "fixnumbers", true) == 0 || strcmp(token, "fix", true) == 0))
            {
                if(!IsPlayerAdmin(playerid)) return SendClientMessage(playerid, COLOR_ERROR, "You do not have permission to run this command.");
                Phone_ReassignDuplicateNumbers(playerid);
                return 1;
            }
        }
    }

    // Phone system is independent of having a phone item in inventory
    // Player can use phone GUI even without a phone item.
    Dialog_ShowPhoneMenu(playerid);
    return 1;
}

// /o - quick offer command
YCMD:o(playerid, params[], help)
{
    CMD_GUARDS();
    if(isnull(params) || strlen(params) == 0) return SendClientMessage(playerid, COLOR_GRAY, "Uzycie: /o <targetid> <item|vcard|repair|heal|taxi|mechanic|advertise|vehicle> [slot|amount|vehicleid] [price]");

    // tokenize up to 5 tokens (manual to avoid multi-dim array issues)
    new tokens0[64]; new tokens1[64]; new tokens2[64]; new tokens3[64]; new tokens4[64];
    new tn = 0; new p = 0; new len = strlen(params);
    while(p < len && tn < 5)
    {
        while(p < len && params[p] == ' ') p++;
        if(p >= len) break;
        new ti = 0;
        if(tn == 0)
        {
            while(p < len && params[p] != ' ' && ti < sizeof(tokens0)-1) tokens0[ti++] = params[p++];
            tokens0[ti] = '\0';
        }
        else if(tn == 1)
        {
            while(p < len && params[p] != ' ' && ti < sizeof(tokens1)-1) tokens1[ti++] = params[p++];
            tokens1[ti] = '\0';
        }
        else if(tn == 2)
        {
            while(p < len && params[p] != ' ' && ti < sizeof(tokens2)-1) tokens2[ti++] = params[p++];
            tokens2[ti] = '\0';
        }
        else if(tn == 3)
        {
            while(p < len && params[p] != ' ' && ti < sizeof(tokens3)-1) tokens3[ti++] = params[p++];
            tokens3[ti] = '\0';
        }
        else if(tn == 4)
        {
            while(p < len && params[p] != ' ' && ti < sizeof(tokens4)-1) tokens4[ti++] = params[p++];
            tokens4[ti] = '\0';
        }
        tn++;
    }

    if(tn < 2) return SendClientMessage(playerid, COLOR_GRAY, "Uzycie: /o <targetid> <type> ...");

    // parse target id
    new targetid = 0;
    if(sscanf(tokens0, "%d", targetid) != 1) return SendClientMessage(playerid, COLOR_ERROR, "Invalid target id.");
    if(!IsValidPlayerId(targetid) || !pInfo[targetid][player_logged]) return SendClientMessage(playerid, COLOR_ERROR, "Target not found or offline.");

    // Determine type
    new typeStr[64]; strcopy(typeStr, tokens1, sizeof(typeStr));
    if(strcmp(typeStr, "item", true) == 0 || (tokens1[0] >= '0' && tokens1[0] <= '9'))
    {
        // item offer: allow forms: /o <target> <slot> <price>  OR /o <target> item <slot> <price>
        new slot = 0; new price = 0;
        if(strcmp(typeStr, "item", true) == 0)
        {
            if(tn < 3) return SendClientMessage(playerid, COLOR_GRAY, "Uzycie: /o <target> item <slot> <price>");
            if(sscanf(tokens2, "%d", slot) != 1) return SendClientMessage(playerid, COLOR_ERROR, "Invalid slot number.");
            if(tn >= 4) sscanf(tokens3, "%d", price);
        }
        else
        {
            // second token is slot
            if(sscanf(tokens1, "%d", slot) != 1) return SendClientMessage(playerid, COLOR_ERROR, "Invalid slot number.");
            if(tn >= 3) sscanf(tokens2, "%d", price);
        }

        if(slot < 0 || slot >= MAX_PLAYER_ITEMS || pItem[playerid][slot][item_id] == 0) return SendClientMessage(playerid, COLOR_ERROR, "Invalid item slot.");

        Offer_Create(playerid, targetid, OFFER_ITEM, price, slot, 0);
        return 1;
    }

    // other types
    if(strcmp(typeStr, "vcard", true) == 0 || strcmp(typeStr, "handshake", true) == 0)
    {
        Offer_Create(playerid, targetid, OFFER_HANDSHAKE, 0, 0, 0);
        return 1;
    }

    if(strcmp(typeStr, "repair", true) == 0)
    {
        // /o <target> repair <price>
        new price = 0; if(tn >= 3) sscanf(tokens2, "%d", price);
        Offer_Create(playerid, targetid, OFFER_REPAIR, price, 0, 0);
        return 1;
    }

    if(strcmp(typeStr, "heal", true) == 0)
    {
        // /o <target> heal <amount> <price?>
        new amount = 0; new price = 0; if(tn >= 3) sscanf(tokens2, "%d", amount); if(tn >= 4) sscanf(tokens3, "%d", price);
        Offer_Create(playerid, targetid, OFFER_HEAL, price, amount, 0);
        return 1;
    }

    if(strcmp(typeStr, "taxi", true) == 0)
    {
        new price = 0; if(tn >= 3) sscanf(tokens2, "%d", price);
        Offer_Create(playerid, targetid, OFFER_TAXI, price, 0, 0);
        return 1;
    }

    if(strcmp(typeStr, "mechanic", true) == 0)
    {
        new price = 0; if(tn >= 3) sscanf(tokens2, "%d", price);
        Offer_Create(playerid, targetid, OFFER_MECHANIC, price, 0, 0);
        return 1;
    }

    if(strcmp(typeStr, "advertise", true) == 0 || strcmp(typeStr, "ad", true) == 0)
    {
        new price = 0; if(tn >= 3) sscanf(tokens2, "%d", price);
        Offer_Create(playerid, targetid, OFFER_ADVERTISE, price, 0, 0);
        return 1;
    }

    if(strcmp(typeStr, "vehicle", true) == 0)
    {
        // /o <target> vehicle <vehicleid> <price>
        new vehicleid = 0; new price = 0;
        if(tn >= 3) sscanf(tokens2, "%d", vehicleid);
        if(tn >= 4) sscanf(tokens3, "%d", price);
        if(vehicleid == 0) return SendClientMessage(playerid, COLOR_ERROR, "Invalid vehicle id.");
        Offer_Create(playerid, targetid, OFFER_VEHICLE, price, vehicleid, 0);
        return 1;
    }

    return SendClientMessage(playerid, COLOR_ERROR, "Unknown offer type. Supported: item,vcard,repair,heal,taxi,mechanic,advertise,vehicle");
}

/**
 * /zadzwon <number|playerid> - quick call by phone number or player id
 */
YCMD:zadzwon(playerid, params[], help)
{
    CMD_GUARDS();
    if(isnull(params)) { printf("[DBG] YCMD:zadzwon called with null params by player=%d\n", playerid); return SendClientMessage(playerid, COLOR_GRAY, "Uzycie: /zadzwon <numer|id>"); }
    printf("[DBG] YCMD:zadzwon params raw='%s' len=%d\n", params, strlen(params));

    new num;
    if(!Phone_ParseNumberString(params, num)) { printf("[DBG] YCMD:zadzwon parse failed player=%d params='%s' len=%d\n", playerid, params, strlen(params)); return SendClientMessage(playerid, COLOR_ERROR, "Provide a phone number (e.g. 12345 or 012345)."); }

    if(num < PHONE_MIN_NUMBER || num > PHONE_MAX_NUMBER) return SendClientMessage(playerid, COLOR_ERROR, "Invalid phone number (use numbers 1000-99999).");

    new targetid = -1;
    foreach(new i : Player)
    {
        new pslot = Item_FindByType(i, ITEM_TYPE_PHONE);
        if(pslot >= 0 && pItem[i][pslot][item_value2] == num && pInfo[i][player_logged]) { targetid = i; break; }
    }

    if(targetid == -1) return SendClientMessage(playerid, COLOR_ERROR, "Phone is unreachable.");
    if(targetid == playerid) {
        // allow self-call for testing
    }
    Phone_Dial(playerid, targetid);
    return 1;
}

// English alias: /call
YCMD:call(playerid, params[], help)
{
    CMD_GUARDS();
    if(isnull(params)) { printf("[DBG] YCMD:call called with null params by player=%d\n", playerid); return SendClientMessage(playerid, COLOR_GRAY, "Usage: /call <number|id>"); }
    printf("[DBG] YCMD:call params raw='%s' len=%d\n", params, strlen(params));

    new num;
    if(!Phone_ParseNumberString(params, num)) { printf("[DBG] YCMD:call parse failed player=%d params='%s' len=%d\n", playerid, params, strlen(params)); return SendClientMessage(playerid, COLOR_ERROR, "Provide a phone number (e.g. 12345 or 012345)."); }

    if(num < PHONE_MIN_NUMBER || num > PHONE_MAX_NUMBER) return SendClientMessage(playerid, COLOR_ERROR, "Invalid phone number (use numbers 1000-99999).");

    new targetid = -1;
    foreach(new i : Player)
    {
        new pslot = Item_FindByType(i, ITEM_TYPE_PHONE);
        if(pslot >= 0 && pItem[i][pslot][item_value2] == num && pInfo[i][player_logged]) { targetid = i; break; }
    }

    if(targetid == -1) return SendClientMessage(playerid, COLOR_ERROR, "Phone is unreachable.");
    Phone_Dial(playerid, targetid);
    return 1;
}

/**
 * /sms <number|playerid> <message> - quick SMS by number or player id
 */
YCMD:sms(playerid, params[], help)
{
    CMD_GUARDS();
    if(isnull(params)) return SendClientMessage(playerid, COLOR_GRAY, "Usage: /sms <number|id> <message>");

    // Parse first token as target (manual to avoid sscanf buffer overflow)
    new paramsLenDbg = strlen(params);

    new targetStr[32]; new message[256];
    // Parse first token as target (manual to avoid sscanf buffer overflow)
    {
        new p = 0; new ti = 0;
        while(params[p] != '\0' && params[p] != ' ' && ti < sizeof(targetStr)-1)
        {
            targetStr[ti++] = params[p++];
        }
        targetStr[ti] = '\0';
        if(ti == 0) return SendClientMessage(playerid, COLOR_ERROR, "Uzycie: /sms <numer> <tresc>");
    }
    // Find start of message after target
    new pos = strlen(targetStr);
    new paramsLen = strlen(params);
    if(pos >= paramsLen) return SendClientMessage(playerid, COLOR_ERROR, "Uzycie: /sms <numer> <tresc>");
    // Skip leading spaces
    while(pos < paramsLen && params[pos] == ' ') pos++;
    if(pos >= paramsLen) return SendClientMessage(playerid, COLOR_ERROR, "Wiadomosc nie moze byc pusta.");
    // Copy remainder safely into message buffer
    new mi = 0;
    for(new p = pos; p < paramsLen && mi < sizeof(message)-1; p++)
    {
        message[mi++] = params[p];
    }
    message[mi] = '\0';

    new targetNum;
    // Check if targetStr is numeric (only digits)
    new isNum = 1; new tlen = strlen(targetStr);
    for(new ti = 0; ti < tlen; ti++) if(targetStr[ti] < '0' || targetStr[ti] > '9') { isNum = 0; break; }


    if(isNum)
    {
        // Parse number manually to avoid sscanf edge-cases
        targetNum = 0;
        for(new di = 0; di < tlen; di++)
        {
            targetNum = targetNum * 10 + (targetStr[di] - '0');
        }

        if(targetNum < PHONE_MIN_NUMBER || targetNum > PHONE_MAX_NUMBER) return SendClientMessage(playerid, COLOR_ERROR, "Nieprawidlowy numer telefonu (uzyj numeru 1000-99999).");

        // find online player with that phone
        new targetid = -1;
        foreach(new i : Player)
        {
            new pslot = Item_FindByType(i, ITEM_TYPE_PHONE);
            if(pslot >= 0 && pItem[i][pslot][item_value2] == targetNum && pInfo[i][player_logged]) { targetid = i; break; }
        }

        if(targetid != -1)
        {
            Phone_SendSMS(playerid, targetid, message);
            return 1;
        }

        Phone_SendSMSToUID(playerid, targetNum, "", message);
        return 1;
    }

    // Non-numeric target: try parsing as player id or player name
    new targetid = -1;
    if(sscanf(targetStr, "%d", targetid) == 1)
    {
        if(!IsValidPlayerId(targetid) || !pInfo[targetid][player_logged]) return SendClientMessage(playerid, COLOR_ERROR, "Nieprawidlowy gracz.");
        Phone_SendSMS(playerid, targetid, message);
        return 1;
    }

    // Search by exact player name (case-insensitive)
    new name[64];
    new lname[64]; new ltarget[64];
    foreach(new i : Player)
    {
        GetPlayerName(i, name, sizeof(name));
        strcopy(lname, name, sizeof(lname));
        strcopy(ltarget, targetStr, sizeof(ltarget));
        for(new k = 0; lname[k] != '\0'; k++) if(lname[k] >= 'A' && lname[k] <= 'Z') lname[k] += 32;
        for(new k = 0; ltarget[k] != '\0'; k++) if(ltarget[k] >= 'A' && ltarget[k] <= 'Z') ltarget[k] += 32;
        if(strcmp(lname, ltarget) == 0 && pInfo[i][player_logged]) { targetid = i; break; }
    }

    if(targetid != -1)
    {
        Phone_SendSMS(playerid, targetid, message);
        return 1;
    }

    return SendClientMessage(playerid, COLOR_ERROR, "Recipient not recognized.");
}

/**
 * /me [akcja] - Opisuje akcje wykonywana przez postac
 */
YCMD:me(playerid, params[], help)
{
    CMD_GUARDS();

    if(isnull(params))
    {
        return SendClientMessage(playerid, COLOR_GREY, "U??ycie: /me [akcja]");
    }

    if(strlen(params) < 3 || strlen(params) > 128)
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Akcja musi mie?? od 3 do 128 znak??w.");
    }

    // U??yj systemu nieznajomych - ka??dy widzi nazw?? jak?? nada??
    Stranger_SendMeMessage(playerid, params, 15.0, COLOR_ME);

    return 1;
}

/**
 * /do [opis], /opis [opis] - Opisuje otoczenie lub efekt akcji
 */
YCMD:opis(playerid, params[], help)
{
    CMD_GUARDS();

    if(isnull(params))
    {
        return SendClientMessage(playerid, COLOR_GREY, "Użycie: /do [opis sytuacji]");
    }

    if(strlen(params) < 3 || strlen(params) > 128)
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Opis musi mieć od 3 do 128 znaków.");
    }

    // Użyj systemu nieznajomych - każdy widzi nazwę jaką nadał
    Stranger_SendDoMessage(playerid, params, 15.0, COLOR_DO);

    return 1;
}

/**
 * /s [tekst] - Krzyczy (większy zasięg)
 */
YCMD:s(playerid, params[], help)
{
    CMD_GUARDS();

    if(isnull(params))
    {
        return SendClientMessage(playerid, COLOR_GREY, "U??ycie: /s [tekst]");
    }

    // U??yj systemu nieznajomych
    Stranger_SendSayMessage(playerid, params, 30.0, "krzyczy:");

    return 1;
}

/**
 * /w [id] [wiadomość] - Prywatna wiadomość (alias do /pm)
 */
YCMD:w(playerid, params[], help)
{
    CMD_GUARDS();

    new targetid, message[128];
    if(sscanf(params, "us[128]", targetid, message))
    {
        return SendClientMessage(playerid, COLOR_GREY, "Użycie: /w [id/nick] [wiadomość]");
    }

    if(!IsValidPlayerId(targetid) || !pInfo[targetid][player_logged])
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Nieprawidłowy gracz.");
    }

    if(targetid == playerid)
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Nie możesz wysłać wiadomości do siebie.");
    }

    if(GetPVarInt(targetid, "BlockPM"))
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Ten gracz zablokował prywatne wiadomości.");
    }

    new sendMsg[256], recvMsg[256];
    format(sendMsg, sizeof(sendMsg), ">> PM do %s[%d]: %s", Player_RPName(targetid), targetid, message);
    format(recvMsg, sizeof(recvMsg), ">> PM od %s[%d]: %s", Player_RPName(playerid), playerid, message);

    SendClientMessage(playerid, COLOR_PM_SEND, sendMsg);
    SendClientMessage(targetid, COLOR_PM_RECV, recvMsg);

    SetPVarInt(targetid, "LastPM", playerid);

    return 1;
}

/**
 * /k [tekst] - Krzyk (alias do /s)
 */
YCMD:k(playerid, params[], help)
{
    CMD_GUARDS();

    if(isnull(params))
    {
        return SendClientMessage(playerid, COLOR_GREY, "Użycie: /k [tekst]");
    }

    Stranger_SendSayMessage(playerid, params, 30.0, "krzyczy:");

    return 1;
}

/**
 * /sz [tekst] - Szept RP (bardzo mały zasięg ~5.0)
 */
YCMD:sz(playerid, params[], help)
{
    CMD_GUARDS();

    if(isnull(params))
    {
        return SendClientMessage(playerid, COLOR_GREY, "Użycie: /sz [tekst]");
    }

    Stranger_SendSayMessage(playerid, params, 5.0, "szepcze:");

    return 1;
}

/**
 * /l [tekst] - Normalna mowa bez gestykulacji (low voice)
 */
YCMD:l(playerid, params[], help)
{
    CMD_GUARDS();

    if(isnull(params))
    {
        return SendClientMessage(playerid, COLOR_GREY, "Użycie: /l [tekst]");
    }

    // Normalna mowa, zasięg 15.0
    Stranger_SendSayMessage(playerid, params, 15.0, "mówi:");

    return 1;
}

/** * /kill - szybka śmierć do testu spawnu
 */
YCMD:kill(playerid, params[], help)
{
    CMD_GUARDS();
    SendClientMessage(playerid, COLOR_INFO, "Umierasz... zostaniesz przeniesiony do szpitala.");
    SetPlayerHealth(playerid, 0.0);
    return 1;
}

/**
 * /b [tekst] - Rozmowa OOC lokalna (pokazuje prawdziwy nick + ID)
 */
YCMD:b(playerid, params[], help)
{
    CMD_GUARDS();

    if(isnull(params))
    {
        return SendClientMessage(playerid, COLOR_GREY, "U??ycie: /b [tekst OOC]");
    }

    // OOC pokazuje prawdziwy nick - to celowe, bo to wiadomo???? poza fabu????
    new message[256];
    format(message, sizeof(message), "(( %s[%d]: %s ))", Player_RPName(playerid), playerid, params);

    SendLocalMessage(playerid, COLOR_OOC, message, 15.0);

    return 1;
}

/**
 * /pm [id] [wiadomo????] - Prywatna wiadomo????
 */
YCMD:pm(playerid, params[], help)
{
    CMD_GUARDS();

    new targetid, message[128];
    if(sscanf(params, "us[128]", targetid, message))
    {
        return SendClientMessage(playerid, COLOR_GREY, "U??ycie: /pm [id/nick] [wiadomo????]");
    }

    if(!IsValidPlayerId(targetid) || !pInfo[targetid][player_logged])
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Nieprawid??owy gracz.");
    }

    if(targetid == playerid)
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Nie mo??esz wys??a?? wiadomo??ci do siebie.");
    }

    // Sprawd?? czy odbiorca nie blokuje PM
    if(GetPVarInt(targetid, "BlockPM"))
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Ten gracz zablokowa?? prywatne wiadomo??ci.");
    }

    new sendMsg[256], recvMsg[256];
    format(sendMsg, sizeof(sendMsg), ">> PM do %s[%d]: %s", Player_RPName(targetid), targetid, message);
    format(recvMsg, sizeof(recvMsg), ">> PM od %s[%d]: %s", Player_RPName(playerid), playerid, message);

    SendClientMessage(playerid, COLOR_PM_SEND, sendMsg);
    SendClientMessage(targetid, COLOR_PM_RECV, recvMsg);

    // Zapisz ID nadawcy do odpowiedzi
    SetPVarInt(targetid, "LastPM", playerid);

    return 1;
}

/**
 * /re [wiadomość] - Szybka odpowiedź na PM
 */
YCMD:re(playerid, params[], help)
{
    CMD_GUARDS();

    if(isnull(params))
    {
        return SendClientMessage(playerid, COLOR_GREY, "Użycie: /re [wiadomość]");
    }

    new targetid = GetPVarInt(playerid, "LastPM");

    if(!IsValidPlayerId(targetid) || !pInfo[targetid][player_logged])
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Brak osoby do odpowiedzi.");
    }

    new sendMsg[256], recvMsg[256];
    format(sendMsg, sizeof(sendMsg), ">> PM do %s[%d]: %s", Player_RPName(targetid), targetid, params);
    format(recvMsg, sizeof(recvMsg), ">> PM od %s[%d]: %s", Player_RPName(playerid), playerid, params);

    SendClientMessage(playerid, COLOR_PM_SEND, sendMsg);
    SendClientMessage(targetid, COLOR_PM_RECV, recvMsg);

    SetPVarInt(targetid, "LastPM", playerid);

    return 1;
}

// ===========================================================================
// KOMENDY INFORMACYJNE
// ===========================================================================

/**
 * /stats - Statystyki gracza (menu)
 */
YCMD:stats(playerid, params[], help)
{
    CMD_GUARDS();

    #pragma unused params, help

    Player_ShowStatsMenu(playerid);
    return 1;
}

YCMD:hangup(playerid, params[], help)
{
    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Zamyka aktywne polaczenie telefonu.");
        return 1;
    }

    if(!pInfo[playerid][player_logged])
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Musisz byc zalogowany!");
    }

    if(Player_GetKnownCount(playerid) < 0) // noop to avoid unused warning
    {
        return 1;
    }

    if(Phone_Hangup(playerid))
    {
        SendClientMessage(playerid, COLOR_SUCCESS, "Rozlaczono.");
    }
    else
    {
        SendClientMessage(playerid, COLOR_GRAY, "Nie masz aktywnego polaczenia.");
    }
    return 1;
}

/**
 * Sprawdza czy gracz moze zalozyc grupe przestepcza
 * Wymagania: 1 domek, 3 czlonkow (znajomych), 1 auto
 */
stock bool:Player_CanCreateGang(playerid)
{
    new houses = Player_CountHouses(playerid);
    new vehicles = Player_CountVehicles(playerid);
    new friends = Player_GetKnownCount(playerid);

    if(houses >= 1 && vehicles >= 1 && friends >= 3)
    {
        return true;
    }
    return false;
}

/**
 * Liczy ilosc domkow gracza
 */
stock Player_CountHouses(playerid)
{
    new count = 0;
    for(new i = 0; i < MAX_DOORS; i++)
    {
        if(dInfo[i][door_owner_type] == _:DOOR_OWNER_PLAYER && dInfo[i][door_owner] == pInfo[playerid][player_uid])
        {
            count++;
        }
    }
    return count;
}

/**
 * Liczy ilosc pojazdow gracza
 */
stock Player_CountVehicles(playerid)
{
    new count = 0;
    for(new i = 1; i < MAX_VEHICLES; i++)
    {
        if(vInfo[i][veh_created] && vInfo[i][veh_owner_type] == _:OWNER_PLAYER && vInfo[i][veh_owner] == pInfo[playerid][player_uid])
        {
            count++;
        }
    }
    return count;
}

/**
 * Pokazuje statystyki gracza
 */
stock Player_ShowStats(playerid, targetid = INVALID_PLAYER_ID)
{
    if(targetid == INVALID_PLAYER_ID) targetid = playerid;

    new content[1024];
    new moneyStr[32], bankStr[32], durationStr[64], tmp[256];

    format(moneyStr, sizeof(moneyStr), "%d", pInfo[targetid][player_money]);
    format(bankStr, sizeof(bankStr), "%d", pInfo[targetid][player_bank]);
    FormatDuration(pInfo[targetid][player_online_time], durationStr, sizeof(durationStr));

    // Budowanie tekstu etapami
    content[0] = EOS;

    format(tmp, sizeof(tmp), "{FFFFFF}=== Statystyki: %s ===\n\n", Player_RPName(targetid));
    strcat(content, tmp, sizeof(content));

    format(tmp, sizeof(tmp), "{3498DB}Ogolne:\n");
    strcat(content, tmp, sizeof(content));

    format(tmp, sizeof(tmp), "{FFFFFF}Poziom: %d | Godziny gry: %d | Szacunek: %d\n",
        pInfo[targetid][player_level], pInfo[targetid][player_hours], pInfo[targetid][player_respect]);
    strcat(content, tmp, sizeof(content));

    format(tmp, sizeof(tmp), "Pieniadze: %s$ | Bank: %s$\n\n", moneyStr, bankStr);
    strcat(content, tmp, sizeof(content));

    format(tmp, sizeof(tmp), "{E74C3C}Stan zdrowia:\n");
    strcat(content, tmp, sizeof(content));

    format(tmp, sizeof(tmp), "{FFFFFF}HP: %.0f | Głód: %d%% | Pragnienie: %d%%\n\n",
        pInfo[targetid][player_health], pInfo[targetid][player_hunger], pInfo[targetid][player_thirst]);
    strcat(content, tmp, sizeof(content));

    format(tmp, sizeof(tmp), "{2ECC71}Statystyki:\n");
    strcat(content, tmp, sizeof(content));

    format(tmp, sizeof(tmp), "{FFFFFF}Zabojstwa: %d | Smierci: %d | Spawnow: %d\n",
        pInfo[targetid][player_kills], pInfo[targetid][player_deaths], pInfo[targetid][player_spawns]);
    strcat(content, tmp, sizeof(content));

    format(tmp, sizeof(tmp), "Przejechane km: %.1f | Czas online: %s\n\n",
        pInfo[targetid][player_driven_km], durationStr);
    strcat(content, tmp, sizeof(content));

    format(tmp, sizeof(tmp), "{F1C40F}Posiadlosci:\n");
    strcat(content, tmp, sizeof(content));

    format(tmp, sizeof(tmp), "{FFFFFF}Domki: %d | Pojazdy: %d",
        Player_CountHouses(targetid), Player_CountVehicles(targetid));
    strcat(content, tmp, sizeof(content));

    ShowPlayerDialog(playerid, DIALOG_STATS, DIALOG_STYLE_MSGBOX,
        "Statystyki", content, "Wstecz", "");
}

/**
 * /id [nick] - Szuka gracza po nicku
 */
YCMD:id(playerid, params[], help)
{
    if(isnull(params))
    {
        return SendClientMessage(playerid, COLOR_GREY, "U??ycie: /id [fragment nicku]");
    }

    new count = 0;
    new result[512];

    strcopy(result, "Znalezieni gracze:\n", sizeof(result));

    foreach(new i : Player)
    {
        new name[MAX_PLAYER_NAME];
        GetPlayerName(i, name, sizeof(name));

        if(strfind(name, params, true) != -1)
        {
            new line[64];
            format(line, sizeof(line), "%s (ID: %d)\n", name, i);
            strcat(result, line);
            count++;

            if(count >= 20)
            {
                strcat(result, "...i wi??cej");
                break;
            }
        }
    }

    if(count == 0)
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Nie znaleziono graczy o podanym nicku.");
    }

    ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX,
        "Wyszukiwanie graczy", result, "OK", "");

    return 1;
}

/**
 * /time - Pokazuje aktualny czas
 */
YCMD:time(playerid, params[], help)
{
    new hour, minute, second;
    gettime(hour, minute, second);

    new year, month, day;
    getdate(year, month, day);

    static const dayNames[][] = {
        "Niedziela", "Poniedzialek", "Wtorek", "Sroda", "Czwartek", "Piatek", "Sobota"
    };
    new dayIndex = GetDayOfWeek(day, month, year);
    new dayName[16];
    if(dayIndex >= 1 && dayIndex <= 7)
    {
        strcopy(dayName, dayNames[dayIndex - 1], sizeof(dayName));
    }
    else
    {
        strcopy(dayName, "?", sizeof(dayName));
    }

    new message[128];
    format(message, sizeof(message),
        "Aktualna data i godzina: %s, %02d.%02d.%d %02d:%02d:%02d",
        dayName, day, month, year, hour, minute, second);

    SendClientMessage(playerid, COLOR_WHITE, message);

    return 1;
}

/**
 * /forceclock - Force immediate HUD clock update (debug)
 */
YCMD:forceclock(playerid, params[], help)
{
    CMD_GUARDS();

    // Trigger HUD_Update and report
    new res = HUD_ForceUpdate(playerid);
    SendClientMessage(playerid, COLOR_INFO, "HUD forced update invoked. Check server logs for [HUD] messages.");
    printf("[CMD] pid=%d invoked /forceclock -> HUD_ForceUpdate res=%d", playerid, res);

    return 1;
}

/**
 * /pos - Pokazuje aktualne koordynaty z VW i interior
 */
YCMD:pos(playerid, params[], help)
{
    CMD_GUARDS();

    new Float:x, Float:y, Float:z, Float:a;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);

    new msg[144];
    format(msg, sizeof(msg), "Pozycja: %.3f %.3f %.3f | kat: %.1f | interior: %d | vw: %d",
        x, y, z, a, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
    SendClientMessage(playerid, COLOR_INFO, msg);
    return 1;
}

/**
 * /tpm - Teleport do aktywnego markera/checkpointu
 */
YCMD:tpm(playerid, params[], help)
{
    CMD_GUARDS();

    // Wymagamy aktywnego checkpointu (np. z lokalizacji pojazdu/GPS)
    if(!IsPlayerCheckpointActive(playerid))
    {
        // Clarify behavior: only server-side checkpoints are supported. Client map waypoint is not accessible.
        return SendClientMessage(playerid, COLOR_ERROR, "No active server marker/checkpoint. Client map waypoint is not supported. Use /tpc <x> <y> <z> or ask an admin to set a server marker.");
    }

    new Float:x, Float:y, Float:z, Float:radius;
    GetPlayerCheckpoint(playerid, x, y, z, radius);

    new Float:angle;
    GetPlayerFacingAngle(playerid, angle);

    // Je??li gracz jest pieszo, u??yj SetPlayerPosFindZ aby stan???? na ziemi
    if(!IsPlayerInAnyVehicle(playerid))
    {
        SetPlayerPosFindZ(playerid, x, y, z + 0.5);
        SetPlayerFacingAngle(playerid, angle);
        SetPlayerInterior(playerid, GetPlayerInterior(playerid));
        SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(playerid));
    }
    else
    {
        // W poje??dzie: przesu?? lekko nad punkt i ustaw pojazd
        new vehicleid = GetPlayerVehicleID(playerid);
        SetVehiclePos(vehicleid, x, y, z + 1.0);
        SetVehicleZAngle(vehicleid, angle);
        LinkVehicleToInterior(vehicleid, GetPlayerInterior(playerid));
        SetVehicleVirtualWorld(vehicleid, GetPlayerVirtualWorld(playerid));
    }

    SendClientMessage(playerid, COLOR_SUCCESS, "Przeteleportowano do markera.");
    return 1;
}

// -----------------------------------------------------------------------------
// Client map click handler - admins only
// -----------------------------------------------------------------------------
public OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
    // Only allow admins to use client map click to teleport
    if(!IsPlayerAdmin(playerid))
    {
        return 0; // ignore for non-admins
    }

    // Set a temporary server checkpoint and teleport the admin immediately
    SetPlayerCheckpoint(playerid, fX, fY, fZ + 0.5, 3.0);

    new Float:angle;
    GetPlayerFacingAngle(playerid, angle);

    if(!IsPlayerInAnyVehicle(playerid))
    {
        SetPlayerPosFindZ(playerid, fX, fY, fZ + 0.5);
        SetPlayerFacingAngle(playerid, angle);
    }
    else
    {
        new vehicleid = GetPlayerVehicleID(playerid);
        SetVehiclePos(vehicleid, fX, fY, fZ + 1.0);
        SetVehicleZAngle(vehicleid, angle);
        LinkVehicleToInterior(vehicleid, GetPlayerInterior(playerid));
        SetVehicleVirtualWorld(vehicleid, GetPlayerVirtualWorld(playerid));
    }

    SendClientMessage(playerid, COLOR_SUCCESS, "Teleported to your map waypoint.");
    return 1;
}

/**
 * /tpc x y z [a] - Teleport na koordynaty (opcjonalnie kąt)
 */
YCMD:tpc(playerid, params[], help)
{
    CMD_GUARDS();

    new Float:x, Float:y, Float:z, Float:a;
    new bool:hasAngle = false;

    // Najpierw wymagane xyz
    if(sscanf(params, "fff", x, y, z))
    {
        return SendClientMessage(playerid, COLOR_GREY, "Uzycie: /tpc [x] [y] [z] [opcjonalnie: kat]");
    }

    // Spróbuj pobrać opcjonalny kąt (nie psuje xyz)
    hasAngle = !sscanf(params, "ffff", x, y, z, a);

    if(!hasAngle)
    {
        GetPlayerFacingAngle(playerid, a);
    }

    if(!IsPlayerInAnyVehicle(playerid))
    {
        // Snap do ziemi i ustaw kąt
        SetPlayerPosFindZ(playerid, x, y, z + 0.5);
        SetPlayerFacingAngle(playerid, a);
        SetPlayerInterior(playerid, GetPlayerInterior(playerid));
        SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(playerid));
    }
    else
    {
        new vehicleid = GetPlayerVehicleID(playerid);
        SetVehiclePos(vehicleid, x, y, z + 1.0);
        SetVehicleZAngle(vehicleid, a);
        LinkVehicleToInterior(vehicleid, GetPlayerInterior(playerid));
        SetVehicleVirtualWorld(vehicleid, GetPlayerVirtualWorld(playerid));
    }

    SendClientMessage(playerid, COLOR_SUCCESS, "Przeteleportowano na podane koordynaty.");
    return 1;
}

/**
 * /skins - Podglad i zmiana skina (preview model)
 */
YCMD:skins(playerid, params[], help)
{
    CMD_GUARDS();

    // eSelection-based menu with callback when player picks or cancels
    SkinSelection_Show(playerid, "SkinSelector_OnSkinSelected");
    return 1;
}

// ===========================================================================
// KOMENDY EKWIPUNKU
// ===========================================================================

/**
 * /inv lub /eq - Ekwipunek
 */
YCMD:inv(playerid, params[], help)
{
    CMD_GUARDS();

    Dialog_ShowInventory(playerid);

    return 1;
}

YCMD:eq(playerid, params[], help)
{
    CMD_GUARDS();
    Dialog_ShowInventory(playerid);
    return 1;
}

/**
 * /p - short alias for inventory (also aliased by /inv and /eq)
 */
YCMD:p(playerid, params[], help)
{
    CMD_GUARDS();
    Dialog_ShowInventory(playerid);
    return 1;
}

/**
 * /grupy - Lista grup gracza
 */
YCMD:grupy(playerid, params[], help)
{
    CMD_GUARDS();

    Dialog_ShowPlayerGroups(playerid);

    return 1;
}

/**
 * /pojazdy - Lista pojazd??w gracza
 */
YCMD:pojazdy(playerid, params[], help)
{
    CMD_GUARDS();

    Dialog_ShowPlayerVehicles(playerid);

    return 1;
}

// ===========================================================================
// KOMENDY POJAZD??W
// ===========================================================================

/**
 * /v lock - Zamyka/otwiera pojazd
 */
YCMD:v(playerid, params[], help)
{
    CMD_GUARDS();

    if(isnull(params))
    {
        SendClientMessage(playerid, COLOR_GREY, "Uzycie: /v [opcja]");
        SendClientMessage(playerid, COLOR_GREY, "Opcje: lock, engine/odpal, lights, park, respawn, okno [1-4]");
        return 1;
    }

    new option[32], subparams[32];
    if(sscanf(params, "s[32]S()[32]", option, subparams))
    {
        return 1;
    }

    // Lock/Unlock
    if(!strcmp(option, "lock", true) || !strcmp(option, "zamknij", true))
    {
        // Znajdz najblizszy pojazd gracza
        new vehicleid = GetClosestPlayerVehicle(playerid, 10.0);

        if(vehicleid == INVALID_VEHICLE_ID)
        {
            return SendClientMessage(playerid, COLOR_ERROR, "Nie ma w poblizu zadnego z twoich pojazdow.");
        }

        new locked = Vehicle_ToggleLock(vehicleid);

        new message[128];
        format(message, sizeof(message), "* %s %s swoj pojazd kluczykiem.",
            Player_RPName(playerid), locked ? "zamyka" : "otwiera");
        SendLocalMessage(playerid, COLOR_ME, message, 15.0);

        return 1;
    }

    // Okno (szyba) - 1-4
    if(!strcmp(option, "okno", true) || !strcmp(option, "szyba", true))
    {
        CMD_IS_DRIVER();

        new vehicleid = GetPlayerVehicleID(playerid);
        new windowId;

        if(isnull(subparams))
        {
            SendClientMessage(playerid, COLOR_GREY, "Uzycie: /v okno [1-4]");
            SendClientMessage(playerid, COLOR_GREY, "1 = Kierowca, 2 = Pasazer, 3 = Tyl lewy, 4 = Tyl prawy");
            return 1;
        }

        windowId = strval(subparams);

        if(windowId < 1 || windowId > 4)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Numer okna musi byc od 1 do 4.");
            return 1;
        }

        // Pobierz aktualne stany okien
        new driver, passenger, backleft, backright;
        GetVehicleParamsCarWindows(vehicleid, driver, passenger, backleft, backright);

        new windowName[32];
        new newState;

        switch(windowId)
        {
            case 1:
            {
                driver = !driver;
                newState = driver;
                windowName = "kierowcy";
            }
            case 2:
            {
                passenger = !passenger;
                newState = passenger;
                windowName = "pasazera";
            }
            case 3:
            {
                backleft = !backleft;
                newState = backleft;
                windowName = "tylne lewe";
            }
            case 4:
            {
                backright = !backright;
                newState = backright;
                windowName = "tylne prawe";
            }
        }

        SetVehicleParamsCarWindows(vehicleid, driver, passenger, backleft, backright);

        new message[128];
        format(message, sizeof(message), "* %s %s okno %s.",
            Player_RPName(playerid), newState ? "zamyka" : "otwiera", windowName);
        SendLocalMessage(playerid, COLOR_ME, message, 15.0);

        return 1;
    }

    // Engine
    if(!strcmp(option, "engine", true) || !strcmp(option, "silnik", true) || !strcmp(option, "odpal", true))
    {
        CMD_IS_DRIVER();

        new vehicleid = GetPlayerVehicleID(playerid);

        new nativeEngine, lights, alarm, doors, bonnet, boot, objective;
        GetVehicleParamsEx(vehicleid, nativeEngine, lights, alarm, doors, bonnet, boot, objective);

        new slot = Vehicle_GetSlot(vehicleid);
        new cachedEngine = slot != -1 ? vInfo[slot][veh_engine] : -1;
        printf("[VDBG] /v engine request by %d for vehicle=%d slot=%d nativeEngine=%d vInfoEngine=%d\n", playerid, vehicleid, slot, nativeEngine, cachedEngine);

        // Normalize engine value: prefer native (0/1), fallback to cached, else assume off
        new currentEngine;
        if(nativeEngine == 0 || nativeEngine == 1)
        {
            currentEngine = nativeEngine;
        }
        else if(cachedEngine == 0 || cachedEngine == 1)
        {
            currentEngine = cachedEngine;
        }
        else
        {
            currentEngine = 0;
        }

        // Toggle and apply
        new engineState = !currentEngine;
        new rv = Vehicle_SetEngine(vehicleid, engineState);
        printf("[VDBG] /v engine completed for vehicle=%d requested=%d result=%d current=%d\n", vehicleid, engineState, rv, currentEngine);

        if(!rv && engineState == 1)
        {
            SendGuiInformation(playerid, "Informacja", "Nie mozna uruchomic silnika (brak paliwa lub ograniczenie).");
        }
        else
        {
            new message[128];
            format(message, sizeof(message), "* %s %s silnik pojazdu.", Player_RPName(playerid), engineState ? "wlacza" : "wylacza");
            SendLocalMessage(playerid, COLOR_ME, message, 15.0);
        }

        return 1;
    }

    // Lights
    if(!strcmp(option, "lights", true) || !strcmp(option, "swiatla", true))
    {
        CMD_IS_DRIVER();

        new vehicleid = GetPlayerVehicleID(playerid);

        new nativeEngine, nativeLights, alarm, doors, bonnet, boot, objective;
        GetVehicleParamsEx(vehicleid, nativeEngine, nativeLights, alarm, doors, bonnet, boot, objective);

        new slot = Vehicle_GetSlot(vehicleid);
        new cachedLights = slot != -1 ? vInfo[slot][veh_lights] : -1;
        printf("[VDBG] /v lights request by %d for vehicle=%d slot=%d nativeLights=%d vInfoLights=%d\n", playerid, vehicleid, slot, nativeLights, cachedLights);

        // Normalize lights value: prefer native (0/1), fallback to cached, else assume off
        new currentLights;
        if(nativeLights == 0 || nativeLights == 1)
        {
            currentLights = nativeLights;
        }
        else if(cachedLights == 0 || cachedLights == 1)
        {
            currentLights = cachedLights;
        }
        else
        {
            currentLights = 0;
        }

        // Toggle and apply
        new lightsState = !currentLights;
        new rv = Vehicle_SetLights(vehicleid, lightsState);
        printf("[VDBG] /v lights completed for vehicle=%d requested=%d result=%d current=%d\n", vehicleid, lightsState, rv, currentLights);

        new message[64];
        format(message, sizeof(message), "??wiat??a: %s", lightsState ? "W????CZONE" : "WY????CZONE");
        SendClientMessage(playerid, COLOR_INFO, message);

        return 1;
    }

    // Park
    if(!strcmp(option, "park", true) || !strcmp(option, "zaparkuj", true))
    {
        CMD_IS_DRIVER();

        new vehicleid = GetPlayerVehicleID(playerid);

        if(!Vehicle_IsOwner(playerid, vehicleid))
        {
            return SendClientMessage(playerid, COLOR_ERROR, "To nie jest tw??j pojazd.");
        }

        // Zapisz now?? pozycj?? parkowania
        new Float:x, Float:y, Float:z, Float:angle;
        GetVehiclePos(vehicleid, x, y, z);
        GetVehicleZAngle(vehicleid, angle);

        Vehicle_SetSpawnPos(vehicleid, x, y, z, angle);
        new vehicleSlot = Vehicle_GetSlot(vehicleid);
        if(vehicleSlot != -1)
        {
            Vehicle_Save(vehicleSlot);
        }

        SendClientMessage(playerid, COLOR_SUCCESS, "Pojazd zosta?? zaparkowany.");

        return 1;
    }

    // Respawn
    if(!strcmp(option, "respawn", true))
    {
        new vehicleid = GetClosestPlayerVehicle(playerid, 10.0);

        if(vehicleid == INVALID_VEHICLE_ID)
        {
            return SendClientMessage(playerid, COLOR_ERROR, "Nie ma w pobli??u ??adnego z twoich pojazd??w.");
        }

        Vehicle_Respawn(vehicleid);
        SendClientMessage(playerid, COLOR_SUCCESS, "Pojazd zosta?? przywr??cony na miejsce parkowania.");

        return 1;
    }

    SendClientMessage(playerid, COLOR_ERROR, "Nieznana opcja. Uzyj: lock, odpal, lights, park, respawn, okno");

    return 1;
}

/**
 * /tempomat [km/h] - Ogranicznik prędkości pojazdu
 * /tempomat 0 - Wyłącza ogranicznik
 */
YCMD:tempomat(playerid, params[], help)
{
    CMD_GUARDS();
    CMD_IS_DRIVER();

    new speed;
    if(sscanf(params, "i", speed))
    {
        SendClientMessage(playerid, COLOR_GREY, "Użycie: /tempomat [maksymalna prędkość km/h]");
        SendClientMessage(playerid, COLOR_GREY, "Ogranicza prędkość pojazdu. /tempomat 0 wyłącza.");
        return 1;
    }

    // Walidacja prędkości
    if(speed < 0 || speed > 250)
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Prędkość musi być w zakresie 0-250 km/h.");
    }

    // Sprawdź czy silnik jest włączony
    new vehicleid = GetPlayerVehicleID(playerid);
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);

    if(!engine && speed > 0)
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Najpierw włącz silnik pojazdu!");
    }

    g_PlayerCruiseControl[playerid] = speed;

    if(speed == 0)
    {
        SendClientMessage(playerid, COLOR_INFO, "Ogranicznik prędkości wyłączony.");
    }
    else
    {
        new msg[64];
        format(msg, sizeof(msg), "Ogranicznik prędkości ustawiony na %d km/h.", speed);
        SendClientMessage(playerid, COLOR_SUCCESS, msg);
    }

    return 1;
}

// ===========================================================================
// KOMENDY DRZWI/BUDYNK??W
// ===========================================================================

/**
 * /wejdz - Wchodzi do budynku
 */
YCMD:wejdz(playerid, params[], help)
{
    CMD_GUARDS();

    Door_TryEnter(playerid);
    return 1;
}

/**
 * /wyjdz - Wychodzi z budynku
 */
YCMD:wyjdz(playerid, params[], help)
{
    CMD_GUARDS();

    Door_TryExit(playerid);
    return 1;
}

// ===========================================================================
// KOMENDY POMOCNICZE
// ===========================================================================

static HELP_DIALOG_CONTENT[] = "{FFFFFF}=== Menu Pomocy ===\\n\\n{3498DB}Komendy RP:\\n{FFFFFF}/me [akcja] - Opisuje akcje postaci\\n/do [opis] - Opisuje otoczenie\\n/s [tekst] - Krzyk\\n/w [tekst] - Szept\\n/b [tekst] - Rozmowa OOC\\n\\n{3498DB}Komendy gracza:\\n{FFFFFF}/stats - Statystyki\\n/inv - Ekwipunek\\n/grupy - Twoje grupy\\n/pojazdy - Twoje pojazdy\\n\\n{3498DB}Komendy pojazdow:\\n{FFFFFF}/v lock - Zamknij/otworz\\n/v engine - Silnik\\n/v park - Zaparkuj\\n\\n{3498DB}Inne:\\n{FFFFFF}/pm [id] [tekst] - Prywatna wiadomosc\\n/wejdz /wyjdz - Drzwi";

/**
 * /pomoc lub /help - Menu pomocy
 */
YCMD:help(playerid, params[], help)
{
    ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Menu Pomocy", HELP_DIALOG_CONTENT, "Zamknij", "");
    return 1;
}

YCMD:pomoc(playerid, params[], help)
{
    ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Menu Pomocy", HELP_DIALOG_CONTENT, "Zamknij", "");
    return 1;
}

// Callback do podgladu skina uruchamianego komenda /skins
forward SkinSelector_OnSkinSelected(playerid, skinid);
public SkinSelector_OnSkinSelected(playerid, skinid)
{
    if(skinid < 0 || skinid >= 312)
    {
        SendClientMessage(playerid, COLOR_INFO, "Anulowano wybor skina.");
        return 1;
    }

    SetPlayerSkin(playerid, skinid);

    new msg[64];
    format(msg, sizeof(msg), "Ustawiono skin ID %d.", skinid);
    SendClientMessage(playerid, COLOR_SUCCESS, msg);
    GameTextForPlayer(playerid, "~g~Skin Changed!", 3000, 3);

    return 1;
}

// ===========================================================================
// FUNKCJE POMOCNICZE
// ===========================================================================

/**
 * Znajduje najbli??szy pojazd gracza
 * @param playerid ID gracza
 * @param range Maksymalny zasi??g
 * @return ID pojazdu lub INVALID_VEHICLE_ID
 */
stock GetClosestPlayerVehicle(playerid, Float:range = 10.0)
{
    new Float:minDist = range;
    new closestVeh = INVALID_VEHICLE_ID;

    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);

    foreach(new slot : Vehicles)
    {
        new vehicleid = vInfo[slot][veh_gameid];
        if(vehicleid == INVALID_VEHICLE_ID)
        {
            continue;
        }
        if(!Vehicle_IsOwner(playerid, vehicleid))
        {
            continue;
        }

        new Float:vx, Float:vy, Float:vz;
        GetVehiclePos(vehicleid, vx, vy, vz);

        new Float:dist = GetDistanceBetweenPoints3D(px, py, pz, vx, vy, vz);

        if(dist < minDist)
        {
            minDist = dist;
            closestVeh = vehicleid;
        }
    }

    return closestVeh;
}

// ===========================================================================
// STYL CHODZENIA - KLAWISZ ALT
// ===========================================================================

enum e_PlayerWalk {
    pwalk_lib[16],
    pwalk_anim[24]
}

// Style chodzenia - musza byc zsynchronizowane z dialog_handlers.inc
static PlayerWalkStyles[][e_PlayerWalk] = {
    {"PED", "WALK_player"},
    {"PED", "WALK_gang1"},
    {"PED", "WALK_gang2"},
    {"PED", "WOMAN_walkshopm"},
    {"PED", "WALK_drunk"},
    {"PED", "WALK_old"},
    {"PED", "WALK_fat"},
    {"PED", "WALK_civi"},
    {"PED", "sprint_panic"}
};

hook OnPlayerKeyStateChange@WalkStyle(playerid, KEY:newkeys, KEY:oldkeys)
{
    // Klawisz Alt (KEY_WALK) - styl chodzenia
    if((newkeys & KEY_WALK) && !(oldkeys & KEY_WALK))
    {
        if(pInfo[playerid][player_spawned] && !IsPlayerInAnyVehicle(playerid))
        {
            new walkStyle = pInfo[playerid][player_walking_anim];

            if(walkStyle > 0 && walkStyle < sizeof(PlayerWalkStyles))
            {
                // Zastosuj animacje chodzenia
                ApplyAnimation(playerid,
                    PlayerWalkStyles[walkStyle][pwalk_lib],
                    PlayerWalkStyles[walkStyle][pwalk_anim],
                    4.1, true, true, true, true, 0);
            }
        }
    }

    // Gdy puszczono Alt - zatrzymaj animacje
    if(!(newkeys & KEY_WALK) && (oldkeys & KEY_WALK))
    {
        if(pInfo[playerid][player_spawned] && !IsPlayerInAnyVehicle(playerid))
        {
            new walkStyle = pInfo[playerid][player_walking_anim];

            if(walkStyle > 0)
            {
                ClearAnimations(playerid);
            }
        }
    }

    return 1;
}

// ===========================================================================
// System wtrąceń RP - normalna mowa z **akcjami**
// ===========================================================================

/**
 * Parsuje i wysyła wiadomość z wtrąceniami RP
 * Tekst **między gwiazdkami** jest wyświetlany w kolorze /me
 * @param senderid ID gracza wysyłającego
 * @param text[] Tekst do przetworzenia
 * @param range Zasięg wiadomości
 */
stock SendRPMessageWithInterjections(senderid, const text[], Float:range)
{
    if(!IsValidPlayerId(senderid))
    {
        return 0;
    }

    new Float:px, Float:py, Float:pz;
    GetPlayerPos(senderid, px, py, pz);

    new vw = GetPlayerVirtualWorld(senderid);
    new interior = GetPlayerInterior(senderid);

    // Podziel tekst na części - normalne i wtrącenia
    new buffer[256], interjection[128], normalPart[128];
    new textLen = strlen(text);
    new bool:inInterjection = false;
    new bufPos = 0, intPos = 0;
    new i = 0;

    // Sprawdź czy są wtrącenia
    new bool:hasInterjections = false;
    for(new j = 0; j < textLen - 1; j++)
    {
        if(text[j] == '*' && text[j + 1] == '*')
        {
            hasInterjections = true;
            break;
        }
    }

    // Jeśli nie ma wtrąceń, wyślij normalną wiadomość
    if(!hasInterjections)
    {
        Stranger_SendSayMessage(senderid, text, range, "mówi:");
        return 1;
    }

    // Iteruj przez graczy w zasięgu
    foreach(new receiver : Player)
    {
        if(GetPlayerVirtualWorld(receiver) != vw) continue;
        if(GetPlayerInterior(receiver) != interior) continue;
        if(!IsPlayerInRangeOfPoint(receiver, range, px, py, pz)) continue;

        new senderName[64];
        Stranger_GetName(receiver, senderid, senderName, sizeof(senderName));

        // Reset zmiennych dla każdego odbiorcy
        inInterjection = false;
        bufPos = 0;
        intPos = 0;
        i = 0;
        buffer[0] = EOS;
        interjection[0] = EOS;
        normalPart[0] = EOS;

        // Najpierw wyślij "Nick mówi:"
        new headerMsg[128];
        format(headerMsg, sizeof(headerMsg), "%s mówi:", senderName);
        SendClientMessage(receiver, COLOR_TALK, headerMsg);

        // Teraz parsuj tekst z wtrąceniami
        while(i < textLen)
        {
            // Sprawdź czy to początek/koniec wtrącenia (**)
            if(i < textLen - 1 && text[i] == '*' && text[i + 1] == '*')
            {
                if(inInterjection)
                {
                    // Koniec wtrącenia - wyślij wtrącenie w kolorze /me
                    interjection[intPos] = EOS;
                    if(intPos > 0)
                    {
                        format(buffer, sizeof(buffer), "* %s", interjection);
                        SendClientMessage(receiver, COLOR_ME, buffer);
                    }
                    intPos = 0;
                    interjection[0] = EOS;
                }
                else
                {
                    // Początek wtrącenia - wyślij dotychczasowy normalny tekst
                    normalPart[bufPos] = EOS;
                    if(bufPos > 0)
                    {
                        SendClientMessage(receiver, COLOR_TALK, normalPart);
                    }
                    bufPos = 0;
                    normalPart[0] = EOS;
                }

                inInterjection = !inInterjection;
                i += 2; // Pomiń oba znaki **
                continue;
            }

            // Dodaj znak do odpowiedniego bufora
            if(inInterjection)
            {
                if(intPos < sizeof(interjection) - 1)
                {
                    interjection[intPos++] = text[i];
                }
            }
            else
            {
                if(bufPos < sizeof(normalPart) - 1)
                {
                    normalPart[bufPos++] = text[i];
                }
            }
            i++;
        }

        // Wyślij pozostały tekst
        if(inInterjection && intPos > 0)
        {
            // Niezamknięte wtrącenie - wyślij jako normalne
            interjection[intPos] = EOS;
            format(buffer, sizeof(buffer), "**%s", interjection);
            SendClientMessage(receiver, COLOR_TALK, buffer);
        }
        else if(bufPos > 0)
        {
            normalPart[bufPos] = EOS;
            SendClientMessage(receiver, COLOR_TALK, normalPart);
        }
    }

    return 1;
}

/**
 * Hook OnPlayerText - przechwytuje normalną mowę dla systemu RP
 */
hook OnPlayerText@PlayerCommands(playerid, text[])
{
    // Sprawdź czy gracz jest zalogowany i zspawnowany
    if(!pInfo[playerid][player_spawned])
    {
        return Y_HOOKS_BREAK_RETURN_0;
    }

    // Jeśli tekst zaczyna się od ( to to jest OOC - przepuść
    if(text[0] == '(')
    {
        return 1;
    }

    // Obsłuż jako wiadomość RP z wtrąceniami
    SendRPMessageWithInterjections(playerid, text, 15.0);

    // Zablokuj domyślną wiadomość
    return Y_HOOKS_BREAK_RETURN_0;
}

// ===========================================================================
// EOF
// ===========================================================================

