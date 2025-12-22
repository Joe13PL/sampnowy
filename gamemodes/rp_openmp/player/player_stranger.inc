/*
 * =============================================================================
 *  PLAYER STRANGER - System nieznajomych
 * =============================================================================
 *  
 *  Modu??: player/player_stranger.inc
 *  Opis: System poznawania graczy i eliminacji metagamingu
 *
 *  JAK DZIA??A:
 *  - Domy??lnie ka??dy gracz widzi innych jako "Nieznajomy [ID]"
 *  - Gracz mo??e nacisn???? Y aby otworzy?? menu poznawania
 *  - Po poznaniu gracza, mo??e nada?? mu w??asn?? nazw??
 *  - Nazwy s?? zapisywane w bazie danych per posta??
 *
 * =============================================================================
 */

#if defined _player_stranger_included
    #endinput
#endif
#define _player_stranger_included

// ===========================================================================
// STA??E
// ===========================================================================

#define STRANGER_MAX_NAME_LENGTH    32      // Max d??ugo???? nazwy poznania
#define STRANGER_KNOW_DISTANCE      5.0     // Dystans do poznania (metry)
#define STRANGER_DEFAULT_NAME       "Nieznajomy"

// Dialogi
#define DIALOG_STRANGER_MENU        700
#define DIALOG_STRANGER_LIST        701
#define DIALOG_STRANGER_NAME        702
#define DIALOG_STRANGER_INFO        703
#define DIALOG_STRANGER_MANAGE      704
#define DIALOG_STRANGER_EDIT        705

// Makro pomocnicze
#if !defined IsValidPlayerId
    #define IsValidPlayerId(%0) ((%0) >= 0 && (%0) < MAX_PLAYERS && IsPlayerConnected(%0))
#endif

// ===========================================================================
// STRUKTURY
// ===========================================================================

enum e_known_player
{
    known_target_uid,                       // UID poznanej postaci
    known_custom_name[STRANGER_MAX_NAME_LENGTH], // Nadana nazwa
    known_timestamp                         // Kiedy poznano
};

// ===========================================================================
// ZMIENNE
// ===========================================================================

// Cache poznanych graczy w pamieci (player -> target -> custom_name)
static g_KnownNames[MAX_PLAYERS][MAX_PLAYERS][STRANGER_MAX_NAME_LENGTH];
static bool:g_IsKnown[MAX_PLAYERS][MAX_PLAYERS];

// Tymczasowe zmienne dla dialogow
static g_StrangerTarget[MAX_PLAYERS];
static g_StrangerPage[MAX_PLAYERS];

// Cache graczy w poblizu
static g_NearbyCache[MAX_PLAYERS][MAX_PLAYERS];
static g_NearbyCount[MAX_PLAYERS];

// Flaga zaladowania danych
static bool:g_StrangerLoaded[MAX_PLAYERS];

// ===========================================================================
// FUNKCJE GLOWNE
// ===========================================================================

/**
 * Inicjalizuje system nieznajomych dla gracza
 */
stock Stranger_Init(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    // Wyczy???? cache
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        g_KnownNames[playerid][i][0] = EOS;
        g_IsKnown[playerid][i] = false;
        
        // Te?? wyczy???? jak inni widz?? tego gracza
        g_KnownNames[i][playerid][0] = EOS;
        g_IsKnown[i][playerid] = false;
    }
    
    g_StrangerTarget[playerid] = INVALID_PLAYER_ID;
    g_StrangerPage[playerid] = 0;
    
    return 1;
}

/**
 * Wczytuje poznane osoby z bazy danych
 */
stock Stranger_LoadKnown(playerid)
{
    if(!IsValidPlayerId(playerid) || pInfo[playerid][player_uid] == 0)
    {
        return 0;
    }
    
    new query[256];
    mysql_format(mySQLconnection, query, sizeof(query),
        "SELECT k.target_uid, k.custom_name, c.name as real_name FROM `known_players` k LEFT JOIN `characters` c ON c.id = k.target_uid WHERE k.player_uid = %d",
        pInfo[playerid][player_uid]);
    
    mysql_tquery(mySQLconnection, query, "Stranger_OnKnownLoaded", "i", playerid);
    
    return 1;
}

forward Stranger_OnKnownLoaded(playerid);
public Stranger_OnKnownLoaded(playerid)
{
    if(!IsPlayerConnected(playerid))
    {
        return 0;
    }
    
    new rows = cache_num_rows();
    
    for(new i = 0; i < rows; i++)
    {
        new targetUid, customName[STRANGER_MAX_NAME_LENGTH];
        
        cache_get_value_int(i, "target_uid", targetUid);
        cache_get_value(i, "custom_name", customName, sizeof(customName));
        
        // Znajd?? gracza online z tym UID
        foreach(new target : Player)
        {
            if(pInfo[target][player_uid] == targetUid)
            {
                strcopy(g_KnownNames[playerid][target], customName, STRANGER_MAX_NAME_LENGTH);
                g_IsKnown[playerid][target] = true;
                break;
            }
        }
    }
    
    return 1;
}

/**
 * Synchronizuje poznanie gdy gracz do????cza (sprawdza czy inni go znaj??)
 */
stock Stranger_SyncOnConnect(playerid)
{
    if(!IsValidPlayerId(playerid) || pInfo[playerid][player_uid] == 0)
    {
        return 0;
    }
    
    // Dla ka??dego zalogowanego gracza sprawd?? czy zna tego gracza
    foreach(new i : Player)
    {
        if(i == playerid) continue;
        if(pInfo[i][player_uid] == 0) continue;
        
        // Sprawd?? w bazie czy gracz i zna gracza playerid
        new query[256];
        mysql_format(mySQLconnection, query, sizeof(query),
            "SELECT `custom_name` FROM `known_players` WHERE `player_uid` = %d AND `target_uid` = %d LIMIT 1",
            pInfo[i][player_uid], pInfo[playerid][player_uid]);
        
        mysql_tquery(mySQLconnection, query, "Stranger_OnSyncCheck", "ii", i, playerid);
    }
    
    return 1;
}

forward Stranger_OnSyncCheck(playerid, targetid);
public Stranger_OnSyncCheck(playerid, targetid)
{
    if(!IsPlayerConnected(playerid) || !IsPlayerConnected(targetid))
    {
        return 0;
    }
    
    if(cache_num_rows() > 0)
    {
        new customName[STRANGER_MAX_NAME_LENGTH];
        cache_get_value(0, "custom_name", customName, sizeof(customName));
        
        strcopy(g_KnownNames[playerid][targetid], customName, STRANGER_MAX_NAME_LENGTH);
        g_IsKnown[playerid][targetid] = true;
    }
    
    return 1;
}

// ===========================================================================
// POZNAWANIE GRACZY
// ===========================================================================

/**
 * Poznaje gracza i zapisuje w bazie
 */
stock Stranger_SetKnown(playerid, targetid, const customName[])
{
    if(!IsValidPlayerId(playerid) || !IsValidPlayerId(targetid))
    {
        return 0;
    }
    
    if(playerid == targetid)
    {
        return 0;
    }
    
    // Walidacja nazwy
    if(strlen(customName) < 2 || strlen(customName) >= STRANGER_MAX_NAME_LENGTH)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Nazwa musi mie?? od 2 do 31 znak??w!");
        return 0;
    }
    
    // Zapisz w cache
    strcopy(g_KnownNames[playerid][targetid], customName, STRANGER_MAX_NAME_LENGTH);
    g_IsKnown[playerid][targetid] = true;
    
    // Zapisz w bazie danych
    new query[512];
    mysql_format(mySQLconnection, query, sizeof(query),
        "INSERT INTO `known_players` (`player_uid`, `target_uid`, `custom_name`, `created_at`) VALUES (%d, %d, '%e', %d) ON DUPLICATE KEY UPDATE `custom_name` = '%e'",
        pInfo[playerid][player_uid],
        pInfo[targetid][player_uid],
        customName,
        gettime(),
        customName);
    
    mysql_tquery(mySQLconnection, query);
    
    // Potwierd??
    new msg[128];
    format(msg, sizeof(msg), "Pozna??e?? t?? osob?? jako: {2ECC71}%s", customName);
    SendClientMessage(playerid, COLOR_INFO, msg);
    
    return 1;
}

/**
 * Usuwa poznanie gracza
 */
stock Stranger_RemoveKnown(playerid, targetid)
{
    if(!IsValidPlayerId(playerid) || !IsValidPlayerId(targetid))
    {
        return 0;
    }
    
    // Wyczy???? cache
    g_KnownNames[playerid][targetid][0] = EOS;
    g_IsKnown[playerid][targetid] = false;
    
    // Usu?? z bazy
    new query[256];
    mysql_format(mySQLconnection, query, sizeof(query),
        "DELETE FROM `known_players` WHERE `player_uid` = %d AND `target_uid` = %d",
        pInfo[playerid][player_uid], pInfo[targetid][player_uid]);
    
    mysql_tquery(mySQLconnection, query);
    
    SendClientMessage(playerid, COLOR_INFO, "Zapomnia??e?? t?? osob??.");
    
    return 1;
}

/**
 * Zwraca liczbę poznanych/graczy zapisanych jako "known" dla gracza
 */
stock Player_GetKnownCount(playerid)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return 0;
    }

    new count = 0;
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        if(g_IsKnown[playerid][i]) count++;
    }

    return count;
}

// ===========================================================================
// POBIERANIE NAZWY
// ===========================================================================

/**
 * Pobiera nazwe gracza jak ja widzi inny gracz
 * @param viewerid Gracz ktory patrzy
 * @param targetid Gracz na ktorego patrzy
 * @param output Bufor wyjsciowy
 * @param size Rozmiar bufora
 */
stock Stranger_GetName(viewerid, targetid, output[], size = sizeof(output))
{
    // Gracz widzi siebie zawsze normalnie
    if(viewerid == targetid)
    {
        format(output, size, "%s", pInfo[targetid][player_name]);
        return;
    }
    
    // Sprawdz czy viewerid jest prawidlowy
    if(!IsValidPlayerId(viewerid) || !IsValidPlayerId(targetid))
    {
        format(output, size, "%s [%d]", STRANGER_DEFAULT_NAME, targetid);
        return;
    }
    
    // Sprawdz czy admin na duty - widzi prawdziwe nazwy
    if(pInfo[viewerid][player_admin_duty])
    {
        format(output, size, "%s", pInfo[targetid][player_name]);
        return;
    }
    
    // Sprawdz czy zna tego gracza
    if(g_IsKnown[viewerid][targetid] && strlen(g_KnownNames[viewerid][targetid]) > 0)
    {
        strcopy(output, g_KnownNames[viewerid][targetid], size);
        return;
    }
    
    // Nie zna - pokaz jako nieznajomego
    format(output, size, "%s [%d]", STRANGER_DEFAULT_NAME, targetid);
}

/**
 * Pobiera nazwe gracza z opisem wygladu
 */
stock Stranger_GetNameWithDesc(viewerid, targetid, output[], size = sizeof(output))
{
    new name[64];
    Stranger_GetName(viewerid, targetid, name, sizeof(name));
    
    // Jesli nieznajomy, dodaj krotki opis wygladu
    if(!g_IsKnown[viewerid][targetid] && viewerid != targetid)
    {
        new skinDesc[32];
        Stranger_GetSkinDescription(pInfo[targetid][player_skin], pInfo[targetid][player_gender], skinDesc);
        format(output, size, "%s (%s)", name, skinDesc);
    }
    else
    {
        strcopy(output, name, size);
    }
}

/**
 * Pobiera opis skina
 */
static stock Stranger_GetSkinDescription(skinid, gender, output[], size = 32)
{
    // Podstawowe opisy bazuj??ce na ID skina
    new desc[32];
    
    if(gender == GENDER_MALE)
    {
        switch(skinid)
        {
            case 0: desc = "??ysy m????czyzna";
            case 1..3: desc = "m??ody m????czyzna";
            case 4..7: desc = "m????czyzna w koszulce";
            case 14..20: desc = "gangster";
            case 21..29: desc = "biznesmen";
            case 280..288: desc = "policjant";
            case 285..286: desc = "??o??nierz";
            default: desc = "m????czyzna";
        }
    }
    else
    {
        switch(skinid)
        {
            case 9..13: desc = "m??oda kobieta";
            case 38..41: desc = "elegancka kobieta";
            case 63..65: desc = "kobieta w sukience";
            case 192..195: desc = "bizneswoman";
            default: desc = "kobieta";
        }
    }
    
    strcopy(output, desc, size);
}

/**
 * Sprawdza czy gracz zna innego gracza
 */
stock bool:Stranger_IsKnown(viewerid, targetid)
{
    if(viewerid == targetid) return true;
    if(!IsValidPlayerId(viewerid) || !IsValidPlayerId(targetid)) return false;
    
    return g_IsKnown[viewerid][targetid];
}

// ===========================================================================
// WYSY??ANIE WIADOMO??CI RP Z OBS??UG?? NIEZNAJOMYCH
// ===========================================================================

/**
 * Wysy??a wiadomo???? /me z personalizowan?? nazw?? dla ka??dego odbiorcy
 * @param senderid ID gracza wykonuj??cego akcj??
 * @param action Tekst akcji
 * @param range Zasi??g wiadomo??ci
 * @param color Kolor wiadomo??ci
 */
stock Stranger_SendMeMessage(senderid, const action[], Float:range = 15.0, color = COLOR_ME)
{
    if(!IsValidPlayerId(senderid))
    {
        return 0;
    }
    
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(senderid, px, py, pz);
    
    new vw = GetPlayerVirtualWorld(senderid);
    new interior = GetPlayerInterior(senderid);
    
    foreach(new i : Player)
    {
        if(GetPlayerVirtualWorld(i) != vw) continue;
        if(GetPlayerInterior(i) != interior) continue;
        
        if(IsPlayerInRangeOfPoint(i, range, px, py, pz))
        {
            new message[256];
            new name[64];
            
            // Pobierz nazw?? jak widzi j?? odbiorca
            Stranger_GetName(i, senderid, name, sizeof(name));
            
            format(message, sizeof(message), "* %s %s", name, action);
            SendClientMessage(i, color, message);
        }
    }
    
    return 1;
}

/**
 * Wysy??a wiadomo???? /do z personalizowan?? nazw?? dla ka??dego odbiorcy
 * @param senderid ID gracza opisuj??cego
 * @param description Tekst opisu
 * @param range Zasi??g wiadomo??ci
 * @param color Kolor wiadomo??ci
 */
stock Stranger_SendDoMessage(senderid, const description[], Float:range = 15.0, color = COLOR_DO)
{
    if(!IsValidPlayerId(senderid))
    {
        return 0;
    }
    
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(senderid, px, py, pz);
    
    new vw = GetPlayerVirtualWorld(senderid);
    new interior = GetPlayerInterior(senderid);
    
    foreach(new i : Player)
    {
        if(GetPlayerVirtualWorld(i) != vw) continue;
        if(GetPlayerInterior(i) != interior) continue;
        
        if(IsPlayerInRangeOfPoint(i, range, px, py, pz))
        {
            new message[256];
            new name[64];
            
            // Pobierz nazw?? jak widzi j?? odbiorca
            Stranger_GetName(i, senderid, name, sizeof(name));
            
            format(message, sizeof(message), "* %s (( %s ))", description, name);
            SendClientMessage(i, color, message);
        }
    }
    
    return 1;
}

/**
 * Wysy??a wiadomo???? mowy z personalizowan?? nazw??
 * @param senderid ID gracza m??wi??cego
 * @param text Tekst wypowiedzi
 * @param range Zasi??g wiadomo??ci
 * @param prefix Prefiks (np. "m??wi:", "krzyczy:")
 */
stock Stranger_SendSayMessage(senderid, const text[], Float:range = 15.0, const prefix[] = "m??wi:")
{
    if(!IsValidPlayerId(senderid))
    {
        return 0;
    }
    
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(senderid, px, py, pz);
    
    new vw = GetPlayerVirtualWorld(senderid);
    new interior = GetPlayerInterior(senderid);
    
    foreach(new i : Player)
    {
        if(GetPlayerVirtualWorld(i) != vw) continue;
        if(GetPlayerInterior(i) != interior) continue;
        
        if(IsPlayerInRangeOfPoint(i, range, px, py, pz))
        {
            new message[256];
            new name[64];
            
            // Pobierz nazw?? jak widzi j?? odbiorca
            Stranger_GetName(i, senderid, name, sizeof(name));
            
            format(message, sizeof(message), "%s %s %s", name, prefix, text);
            SendClientMessage(i, COLOR_TALK, message);
        }
    }
    
    return 1;
}

/**
 * Wysy??a generyczn?? wiadomo???? lokaln?? z personalizowan?? nazw??
 * @param senderid ID gracza
 * @param formatString Format wiadomo??ci z %s dla nazwy
 * @param range Zasi??g
 * @param color Kolor
 * @param namePosition Pozycja nazwy w formacie (0 = pierwsza, 1 = druga, itd.)
 */
stock Stranger_SendLocalRP(senderid, const formatString[], Float:range, color, ...)
{
    if(!IsValidPlayerId(senderid))
    {
        return 0;
    }
    
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(senderid, px, py, pz);
    
    new vw = GetPlayerVirtualWorld(senderid);
    new interior = GetPlayerInterior(senderid);
    
    foreach(new i : Player)
    {
        if(GetPlayerVirtualWorld(i) != vw) continue;
        if(GetPlayerInterior(i) != interior) continue;
        
        if(IsPlayerInRangeOfPoint(i, range, px, py, pz))
        {
            new message[256];
            new name[64];
            
            // Pobierz nazw?? jak widzi j?? odbiorca
            Stranger_GetName(i, senderid, name, sizeof(name));
            
            // Format z nazw?? jako pierwszym argumentem
            format(message, sizeof(message), formatString, name);
            SendClientMessage(i, color, message);
        }
    }
    
    return 1;
}

// ===========================================================================
// DIALOGI
// ===========================================================================

/**
 * Pokazuje g????wne menu systemu nieznajomych (Y key)
 */
stock Stranger_ShowMenu(playerid)
{
    new content[256] = 
        "{3498DB}?? Poznaj osob?? w pobli??u\n{2ECC71}?? Lista poznanych os??b\n{F39C12}?? Zapomnij osob??\n{AAAAAA}?? Informacje o systemie";
    
    ShowPlayerDialog(playerid, DIALOG_STRANGER_MENU, DIALOG_STYLE_LIST,
        "System Nieznajomych", content, "Wybierz", "Anuluj");
}

/**
 * Pokazuje list?? graczy w pobli??u do poznania
 */
stock Stranger_ShowNearbyList(playerid)
{
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);
    
    new content[2048] = "Osoba\tDystans\n";
    new count = 0;
    new nearbyPlayers[MAX_PLAYERS];
    
    foreach(new i : Player)
    {
        if(i == playerid) continue;
        if(!pInfo[i][player_logged]) continue;
        
        new Float:tx, Float:ty, Float:tz;
        GetPlayerPos(i, tx, ty, tz);
        
        new Float:dist = GetPlayerDistanceFromPoint(playerid, tx, ty, tz);
        
        if(dist <= STRANGER_KNOW_DISTANCE)
        {
            nearbyPlayers[count] = i;
            
            new line[128];
            new displayName[64];
            
            // Poka?? jak gracz aktualnie widzi t?? osob??
            Stranger_GetName(playerid, i, displayName, sizeof(displayName));
            
            // Dodaj oznaczenie je??li ju?? poznany
            if(g_IsKnown[playerid][i])
            {
                format(line, sizeof(line), "{2ECC71}%s (poznany)\t%.1fm\n", displayName, dist);
            }
            else
            {
                format(line, sizeof(line), "{FFFFFF}%s\t%.1fm\n", displayName, dist);
            }
            
            strcat(content, line);
            count++;
        }
    }
    
    if(count == 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Nie ma nikogo w pobli??u (%.1fm).", STRANGER_KNOW_DISTANCE);
        return 0;
    }
    
    // Zapisz list?? graczy w pobli??u
    for(new i = 0; i < count; i++)
    {
        g_NearbyCache[playerid][i] = nearbyPlayers[i];
    }
    g_NearbyCount[playerid] = count;
    
    ShowPlayerDialog(playerid, DIALOG_STRANGER_LIST, DIALOG_STYLE_TABLIST_HEADERS,
        "Poznaj osobe", content, "Wybierz", "Wroc");
    
    return 1;
}

// Zmienne g_NearbyCache i g_NearbyCount sa juz zdefiniowane na poczatku pliku

/**
 * Pokazuje dialog do wpisania nazwy
 */
stock Stranger_ShowNameDialog(playerid, targetid)
{
    g_StrangerTarget[playerid] = targetid;
    
    new info[256];
    new currentName[64];
    
    if(g_IsKnown[playerid][targetid])
    {
        strcopy(currentName, g_KnownNames[playerid][targetid], sizeof(currentName));
        format(info, sizeof(info),
            "{FFFFFF}Ta osoba jest ju?? poznana jako: {2ECC71}%s\n\n{FFFFFF}Wpisz now?? nazw?? aby j?? zmieni??.\n\n{AAAAAA}Wskaz??wka: U??yj formatu Imie_Nazwisko",
            currentName);
    }
    else
    {
        format(info, sizeof(info),
            "{FFFFFF}Jak chcesz nazwa?? t?? osob???\n\n{AAAAAA}Wskaz??wka: U??yj formatu Imie_Nazwisko\nMo??esz te?? u??y?? przezwiska lub pseudonimu.");
    }
    
    ShowPlayerDialog(playerid, DIALOG_STRANGER_NAME, DIALOG_STYLE_INPUT,
        "Nadaj nazw??", info, "Zapisz", "Anuluj");
}

/**
 * Pokazuje list?? poznanych os??b
 */
stock Stranger_ShowKnownList(playerid)
{
    new content[2048] = "Nadana nazwa\tStatus\n";
    new count = 0;
    
    foreach(new i : Player)
    {
        if(i == playerid) continue;
        if(!g_IsKnown[playerid][i]) continue;
        
        new line[128];
        new online[16] = "{2ECC71}Online";
        
        format(line, sizeof(line), "%s\t%s\n", g_KnownNames[playerid][i], online);
        strcat(content, line);
        
        g_NearbyCache[playerid][count] = i;
        count++;
    }
    
    // Dodaj te?? poznanych kt??rzy s?? offline (z bazy)
    // To by??oby bardziej skomplikowane, wi??c na razie pokazujemy tylko online
    
    if(count == 0)
    {
        SendClientMessage(playerid, COLOR_INFO, "Nie znasz jeszcze nikogo.");
        return 0;
    }
    
    g_NearbyCount[playerid] = count;
    
    ShowPlayerDialog(playerid, DIALOG_STRANGER_MANAGE, DIALOG_STYLE_TABLIST_HEADERS,
        "Poznane osoby", content, "Edytuj", "Wr????");
    
    return 1;
}

/**
 * Pokazuje informacje o systemie
 */
stock Stranger_ShowInfo(playerid)
{
    new info[512] = 
        "{3498DB}System Nieznajomych\n\n{FFFFFF}Ten system pomaga eliminowa?? metagaming.\n\n{2ECC71}Jak to dzia??a:\n{FFFFFF}- Domy??lnie widzisz innych jako 'Nieznajomy'\n- Mo??esz pozna?? osob?? i nada?? jej nazw??\n- Nazwy s?? prywatne - ka??dy ma swoje\n- Dane s?? zapisywane mi??dzy sesjami\n\n{F39C12}Skr??ty:\n{FFFFFF}- Naci??nij Y aby otworzy?? menu\n- Musisz by?? blisko osoby aby j?? pozna??";
    
    ShowPlayerDialog(playerid, DIALOG_STRANGER_INFO, DIALOG_STYLE_MSGBOX,
        "System Nieznajomych - Info", info, "OK", "");
}

// ===========================================================================
// OBS??UGA DIALOG??W
// ===========================================================================

hook OnDialogResponse@Stranger(playerid, dialogid, response, listitem, inputtext[])
{
    switch(dialogid)
    {
        case DIALOG_STRANGER_MENU:
        {
            if(!response) return Y_HOOKS_BREAK_RETURN_1;
            
            switch(listitem)
            {
                case 0: Stranger_ShowNearbyList(playerid);  // Poznaj osob??
                case 1: Stranger_ShowKnownList(playerid);   // Lista poznanych
                case 2: Stranger_ShowKnownList(playerid);   // Zapomnij (ta sama lista)
                case 3: Stranger_ShowInfo(playerid);        // Info
            }
            
            return Y_HOOKS_BREAK_RETURN_1;
        }
        
        case DIALOG_STRANGER_LIST:
        {
            if(!response)
            {
                Stranger_ShowMenu(playerid);
                return Y_HOOKS_BREAK_RETURN_1;
            }
            
            // Pobierz wybranego gracza z cache (listitem - 1 bo header)
            if(listitem >= 0 && listitem < g_NearbyCount[playerid])
            {
                new targetid = g_NearbyCache[playerid][listitem];
                
                if(IsPlayerConnected(targetid))
                {
                    Stranger_ShowNameDialog(playerid, targetid);
                }
                else
                {
                    SendClientMessage(playerid, COLOR_ERROR, "Ta osoba ju?? si?? od????czy??a.");
                    Stranger_ShowNearbyList(playerid);
                }
            }
            
            return Y_HOOKS_BREAK_RETURN_1;
        }
        
        case DIALOG_STRANGER_NAME:
        {
            if(!response)
            {
                Stranger_ShowNearbyList(playerid);
                return Y_HOOKS_BREAK_RETURN_1;
            }
            
            new targetid = g_StrangerTarget[playerid];
            
            if(!IsPlayerConnected(targetid))
            {
                SendClientMessage(playerid, COLOR_ERROR, "Ta osoba ju?? si?? od????czy??a.");
                return Y_HOOKS_BREAK_RETURN_1;
            }
            
            // Waliduj i zapisz nazw??
            if(strlen(inputtext) < 2)
            {
                SendClientMessage(playerid, COLOR_ERROR, "Nazwa musi mie?? co najmniej 2 znaki!");
                Stranger_ShowNameDialog(playerid, targetid);
                return Y_HOOKS_BREAK_RETURN_1;
            }
            
            Stranger_SetKnown(playerid, targetid, inputtext);
            
            return Y_HOOKS_BREAK_RETURN_1;
        }
        
        case DIALOG_STRANGER_MANAGE:
        {
            if(!response)
            {
                Stranger_ShowMenu(playerid);
                return Y_HOOKS_BREAK_RETURN_1;
            }
            
            if(listitem >= 0 && listitem < g_NearbyCount[playerid])
            {
                new targetid = g_NearbyCache[playerid][listitem];
                
                if(IsPlayerConnected(targetid))
                {
                    g_StrangerTarget[playerid] = targetid;
                    
                    new content[256];
                    format(content, sizeof(content),
                        "{3498DB}?? Zmie?? nazw??\n{E74C3C}?? Zapomnij osob??");
                    
                    ShowPlayerDialog(playerid, DIALOG_STRANGER_EDIT, DIALOG_STYLE_LIST,
                        g_KnownNames[playerid][targetid], content, "Wybierz", "Wr????");
                }
            }
            
            return Y_HOOKS_BREAK_RETURN_1;
        }
        
        case DIALOG_STRANGER_EDIT:
        {
            if(!response)
            {
                Stranger_ShowKnownList(playerid);
                return Y_HOOKS_BREAK_RETURN_1;
            }
            
            new targetid = g_StrangerTarget[playerid];
            
            switch(listitem)
            {
                case 0: // Zmie?? nazw??
                {
                    Stranger_ShowNameDialog(playerid, targetid);
                }
                case 1: // Zapomnij
                {
                    Stranger_RemoveKnown(playerid, targetid);
                    Stranger_ShowKnownList(playerid);
                }
            }
            
            return Y_HOOKS_BREAK_RETURN_1;
        }
        
        case DIALOG_STRANGER_INFO:
        {
            Stranger_ShowMenu(playerid);
            return Y_HOOKS_BREAK_RETURN_1;
        }
    }
    
    return 1;
}

// ===========================================================================
// OBS??UGA KLAWISZY
// ===========================================================================

hook OnPlayerKeyStateChange@Stranger(playerid, KEY:newkeys, KEY:oldkeys)
{
    // Klawisz Y (KEY_YES = 65536)
    if((newkeys & KEY_YES) && !(oldkeys & KEY_YES))
    {
        // Sprawd?? czy gracz jest zalogowany
        if(!pInfo[playerid][player_logged])
        {
            return 1;
        }
        
        // Sprawd?? czy nie jest w poje??dzie jako kierowca (Y to te?? klakson)
        if(IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleSeat(playerid) == 0)
        {
            return 1;
        }
        
        // Otw??rz menu
        Stranger_ShowMenu(playerid);
    }
    
    return 1;
}

// ===========================================================================
// HOOKI
// ===========================================================================

hook OnPlayerConnect@Stranger(playerid)
{
    Stranger_Init(playerid);
    return 1;
}

hook OnPlayerDisconnect@Stranger(playerid, reason)
{
    // Wyczy???? cache dla tego gracza
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        g_KnownNames[playerid][i][0] = EOS;
        g_IsKnown[playerid][i] = false;
        g_KnownNames[i][playerid][0] = EOS;
        g_IsKnown[i][playerid] = false;
    }
    
    // Reset flagi ladowania
    g_StrangerLoaded[playerid] = false;
    
    return 1;
}

// Zmienna g_StrangerLoaded jest juz zdefiniowana na poczatku pliku

// Wyswietl wczytywanie po spawnie (jesli zalogowany)
hook OnPlayerSpawn@Stranger(playerid)
{
    // Wczytaj poznanych tylko raz - przy pierwszym spawnie
    if(!g_StrangerLoaded[playerid] && pInfo[playerid][player_logged])
    {
        Stranger_LoadKnown(playerid);
        Stranger_SyncOnConnect(playerid);
        g_StrangerLoaded[playerid] = true;
    }
    
    return 1;
}

// ===========================================================================
// KOMENDY POMOCNICZE
// ===========================================================================

/**
 * Komenda /poznaj [id] - szybkie poznanie
 */
YCMD:poznaj(playerid, params[], help)
{
    if(help)
    {
        SendClientMessage(playerid, COLOR_INFO, "U??ycie: /poznaj [id] - poznaj gracza w pobli??u");
        return 1;
    }
    
    new targetid;
    if(sscanf(params, "u", targetid))
    {
        SendClientMessage(playerid, COLOR_GREY, "U??ycie: /poznaj [id/nick]");
        return 1;
    }
    
    if(!IsPlayerConnected(targetid) || !pInfo[targetid][player_logged])
    {
        SendClientMessage(playerid, COLOR_ERROR, "Ten gracz nie jest online lub nie jest zalogowany.");
        return 1;
    }
    
    if(targetid == playerid)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Nie mo??esz pozna?? samego siebie.");
        return 1;
    }
    
    // Sprawd?? dystans
    if(!IsPlayerInRangeOfPlayer(playerid, targetid, STRANGER_KNOW_DISTANCE))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Musisz by?? bli??ej tej osoby!");
        return 1;
    }
    
    Stranger_ShowNameDialog(playerid, targetid);
    
    return 1;
}

/**
 * Komenda /zapomnij [id] - szybkie zapomnienie
 */
YCMD:zapomnij(playerid, params[], help)
{
    if(help)
    {
        SendClientMessage(playerid, COLOR_INFO, "U??ycie: /zapomnij [id] - zapomnij poznan?? osob??");
        return 1;
    }
    
    new targetid;
    if(sscanf(params, "u", targetid))
    {
        SendClientMessage(playerid, COLOR_GREY, "U??ycie: /zapomnij [id/nick]");
        return 1;
    }
    
    if(!IsPlayerConnected(targetid))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Ten gracz nie jest online.");
        return 1;
    }
    
    if(!g_IsKnown[playerid][targetid])
    {
        SendClientMessage(playerid, COLOR_ERROR, "Nie znasz tej osoby.");
        return 1;
    }
    
    Stranger_RemoveKnown(playerid, targetid);
    
    return 1;
}

// ===========================================================================
// FUNKCJE POMOCNICZE
// ===========================================================================

/**
 * Sprawdza czy gracze s?? w zasi??gu
 */
stock bool:IsPlayerInRangeOfPlayer(playerid, targetid, Float:range)
{
    new Float:tx, Float:ty, Float:tz;
    GetPlayerPos(targetid, tx, ty, tz);
    return IsPlayerInRangeOfPoint(playerid, range, tx, ty, tz);
}

// ===========================================================================
// EOF
// ===========================================================================




