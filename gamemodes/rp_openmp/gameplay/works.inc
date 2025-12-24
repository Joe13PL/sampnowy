/*
 * =============================================================================
 *  WORKS - System prac dorywczych
 * =============================================================================
 *
 *  Moduł: gameplay/works.inc
 *  Opis: Prace dorywcze dla graczy (taksówkarz, dostawca, mechanik, etc.)
 *
 *  ZMIANY W REFAKTORZE:
 *  - Iterator zamiast pętli
 *  - y_timers dla timerów prac
 *  - Modularna struktura
 *
 * =============================================================================
 */

// Wyciszenie warningów tag mismatch
#pragma warning disable 213

#if defined _works_included
    #endinput
#endif
#define _works_included

// ===========================================================================
// STUB DLA MAPANDREAS
// ===========================================================================


stock Work_FindGroundZ(Float:x, Float:y, &Float:z)
{
    // Próbuj użyć MapAndreas jeśli dostępny, w przeciwnym razie domyślna wysokość
    z = 13.5;
    return 1;
}

// ===========================================================================
// TYPY PRAC
// ===========================================================================

enum E_WORK_TYPE
{
    WORK_NONE,
    WORK_TAXI,          // Taks??wkarz
    WORK_TRUCKER,       // Kierowca ci????ar??wki
    WORK_BUS,           // Kierowca autobusu
    WORK_PIZZA,         // Dostawca pizzy
    WORK_GARBAGE,       // ??mieciarz
    WORK_FARMER,        // Farmer
    WORK_MINER,         // G??rnik
    WORK_LUMBERJACK,    // Drwal
    WORK_FISHERMAN,     // Rybak
    WORK_MECHANIC,      // Mechanik
    WORK_WEAPON_DEALER, // Handlarz broni??
    WORK_DRUG_DEALER,   // Handlarz narkotykami
    E_WORK_MAX
};

// Stan pracy gracza
enum E_WORK_STATE
{
    WORK_STATE_NONE,
    WORK_STATE_STARTED,
    WORK_STATE_WORKING,
    WORK_STATE_DELIVERING,
    WORK_STATE_FINISHED
};

// ===========================================================================
// DANE PRAC
// ===========================================================================

enum e_work_point
{
    work_type,
    Float:work_x,
    Float:work_y,
    Float:work_z,
    work_name[32],
    work_min_level,
    work_vehicle_model
};

// Punkty startowe prac
static g_WorkPoints[][e_work_point] = {
    // Taxi
    {WORK_TAXI, 1727.8574, -1911.5396, 13.5608, "Taxi LS", 1, 420},
    {WORK_TAXI, 2058.8938, 1545.5837, 10.8203, "Taxi LV", 1, 420},
    {WORK_TAXI, -1981.6091, 127.1631, 27.6943, "Taxi SF", 1, 420},

    // Trucker
    {WORK_TRUCKER, 2447.0652, -2114.1318, 13.5469, "Transport LS", 2, 515},
    {WORK_TRUCKER, 1309.0582, 1274.7349, 10.8203, "Transport LV", 2, 515},

    // Autobus
    {WORK_BUS, 1173.8813, -1776.0579, 13.3888, "Bus Station LS", 3, 431},
    {WORK_BUS, 1066.6025, 2082.4182, 10.8203, "Bus Station LV", 3, 431},

    // Pizza
    {WORK_PIZZA, 2105.5940, -1806.4980, 13.5547, "Well Stacked LS", 1, 448},

    // Garbage
    {WORK_GARBAGE, 2173.3201, -1997.1638, 13.5542, "Landfill LS", 2, 408},

    // Farmer
    {WORK_FARMER, -311.5664, -1529.1978, 25.7604, "Blueberry Farm", 1, 532},

    // Miner
    {WORK_MINER, -619.7455, -582.5994, 25.6161, "Mine", 3, 0},

    // Lumberjack
    {WORK_LUMBERJACK, -692.2892, -2418.8604, 47.0546, "Angel Pine Sawmill", 2, 0},

    // Fisherman
    {WORK_FISHERMAN, 2330.1655, -2455.3999, 13.5000, "Port LS", 1, 0}
};

// ===========================================================================
// ZMIENNE GRACZA
// ===========================================================================

static E_WORK_TYPE:g_PlayerWork[MAX_PLAYERS];
static E_WORK_STATE:g_PlayerWorkState[MAX_PLAYERS];
static g_PlayerWorkVehicle[MAX_PLAYERS];
static g_PlayerWorkCheckpoint[MAX_PLAYERS];
static g_PlayerWorkProgress[MAX_PLAYERS];
static g_PlayerWorkEarnings[MAX_PLAYERS];

// Trasy dla ci????ar??wek/autobus??w
static Float:g_TruckRoute[10][3];
static Float:g_BusRoute[15][3];
static g_TruckRouteSize;
static g_BusRouteSize;

// ===========================================================================
// FUNKCJE G????WNE
// ===========================================================================

/**
 * Inicjalizuje system prac
 */
stock Work_Init()
{
    // Tw??rz pickupy dla punkt??w prac
    for(new i = 0; i < sizeof(g_WorkPoints); i++)
    {
        CreateDynamicPickup(1239, 1,
            g_WorkPoints[i][work_x],
            g_WorkPoints[i][work_y],
            g_WorkPoints[i][work_z],
            -1, -1, -1, 30.0);

        new label[128];
        new typeName[32];
        Work_GetTypeName(E_WORK_TYPE:g_WorkPoints[i][work_type], typeName, sizeof(typeName));
        format(label, sizeof(label),
            "{FFFFFF}%s\n{3498DB}Job: %s\n{FFFFFF}Required level: %d\n{2ECC71}/job to start",
            g_WorkPoints[i][work_name],
            typeName,
            g_WorkPoints[i][work_min_level]);

        CreateDynamic3DTextLabel(label, 0xFFFFFFFF,
            g_WorkPoints[i][work_x],
            g_WorkPoints[i][work_y],
            g_WorkPoints[i][work_z] + 0.5,
            20.0);
    }

    // Inicjalizuj trasy
    Work_InitRoutes();

    return 1;
}

/**
 * Inicjalizuje trasy dla prac transportowych
 */
static stock Work_InitRoutes()
{
    // Trasa ci????ar??wki
    g_TruckRoute[0] = {2447.0, -2114.1, 13.5}; // Start
    g_TruckRoute[1] = {2187.3, -1990.5, 13.5}; // Punkt 1
    g_TruckRoute[2] = {1697.5, -1460.5, 13.5}; // Punkt 2
    g_TruckRoute[3] = {1201.8, -918.5, 42.5};  // Punkt 3
    g_TruckRoute[4] = {684.8, -503.0, 16.3};   // Punkt 4
    g_TruckRouteSize = 5;

    // Trasa autobusu
    g_BusRoute[0] = {1173.8, -1776.0, 13.3};   // Dworzec
    g_BusRoute[1] = {1509.5, -1658.5, 13.3};   // Przystanek 1
    g_BusRoute[2] = {1837.5, -1682.0, 13.3};   // Przystanek 2
    g_BusRoute[3] = {2130.0, -1784.5, 13.3};   // Przystanek 3
    g_BusRoute[4] = {2287.5, -1675.0, 13.3};   // Przystanek 4
    g_BusRoute[5] = {2630.5, -1430.0, 40.0};   // Przystanek 5
    g_BusRoute[6] = {2530.0, -1085.0, 69.0};   // Przystanek 6
    g_BusRouteSize = 7;

    return 1;
}

/**
 * Rozpoczyna prac?? dla gracza
 * @param playerid ID gracza
 * @param workType Typ pracy
 */
stock Work_Start(playerid, E_WORK_TYPE:workType)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return 0;
    }

    // Sprawd?? czy gracz nie pracuje
    if(g_PlayerWork[playerid] != WORK_NONE)
    {
        SendClientMessage(playerid, COLOR_ERROR, "You are already working. Use /job stop to finish.");
        return 0;
    }

    // Find work point
    new workPoint = Work_GetNearestPoint(playerid, workType);

    if(workPoint == -1)
    {
        SendClientMessage(playerid, COLOR_ERROR, "You are not near a work point for this job.");
        return 0;
    }

    // Check level
    if(pInfo[playerid][player_level] < g_WorkPoints[workPoint][work_min_level])
    {
        new msg[128];
        format(msg, sizeof(msg), "Required level: %d. Your level: %d.",
            g_WorkPoints[workPoint][work_min_level], pInfo[playerid][player_level]);
        SendClientMessage(playerid, COLOR_ERROR, msg);
        return 0;
    }

    // Utw??rz pojazd pracy je??li potrzebny
    new vehicleModel = g_WorkPoints[workPoint][work_vehicle_model];

    if(vehicleModel > 0)
    {
        new Float:x, Float:y, Float:z, Float:angle;
        GetPlayerPos(playerid, x, y, z);
        GetPlayerFacingAngle(playerid, angle);

        g_PlayerWorkVehicle[playerid] = CreateVehicle(vehicleModel, x + 3.0, y, z, angle, -1, -1, 300);
        SetVehicleVirtualWorld(g_PlayerWorkVehicle[playerid], GetPlayerVirtualWorld(playerid));

        new msg[64];
        new typeName[32];
        Work_GetTypeName(workType, typeName, sizeof(typeName));
        format(msg, sizeof(msg), "Work vehicle (%s) has been created.", typeName);
        SendClientMessage(playerid, COLOR_INFO, msg);
    }

    // Set work state
    g_PlayerWork[playerid] = workType;
    g_PlayerWorkState[playerid] = WORK_STATE_STARTED;
    g_PlayerWorkProgress[playerid] = 0;
    g_PlayerWorkEarnings[playerid] = 0;

    // Ustaw pierwszy checkpoint
    Work_SetNextCheckpoint(playerid);

    new msg[128];
    new typeName2[32];
    Work_GetTypeName(workType, typeName2, sizeof(typeName2));
    format(msg, sizeof(msg), "You have started working as %s. Follow the marker.",
        typeName2);
    SendClientMessage(playerid, COLOR_SUCCESS, msg);

    return 1;
}

/**
 * Ko??czy prac?? gracza
 * @param playerid ID gracza
 * @param forced Czy wymuszone zako??czenie
 */
stock Work_Stop(playerid, bool:forced = false)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    if(g_PlayerWork[playerid] == WORK_NONE)
    {
        return 0;
    }

    // Usu?? pojazd pracy
    if(g_PlayerWorkVehicle[playerid] != 0)
    {
        DestroyVehicle(g_PlayerWorkVehicle[playerid]);
        g_PlayerWorkVehicle[playerid] = 0;
    }

    // Usu?? checkpoint
    DisablePlayerCheckpoint(playerid);

    if(!forced && g_PlayerWorkEarnings[playerid] > 0)
    {
        // Wyp??a?? zarobki
        Player_GiveMoney(playerid, g_PlayerWorkEarnings[playerid]);

        new msg[128];
        format(msg, sizeof(msg), "You have finished working. Earnings: %s$",
            FormatMoney(g_PlayerWorkEarnings[playerid]));
        SendClientMessage(playerid, COLOR_SUCCESS, msg);
    }
    else if(forced)
    {
        SendClientMessage(playerid, COLOR_WARNING, "Work has been interrupted. You do not receive payment.");
    }

    // Reset state
    g_PlayerWork[playerid] = WORK_NONE;
    g_PlayerWorkState[playerid] = WORK_STATE_NONE;
    g_PlayerWorkProgress[playerid] = 0;
    g_PlayerWorkEarnings[playerid] = 0;

    return 1;
}

/**
 * Ustawia nast??pny checkpoint pracy
 * @param playerid ID gracza
 */
static stock Work_SetNextCheckpoint(playerid)
{
    new Float:x, Float:y, Float:z;
    new checkpoint = g_PlayerWorkProgress[playerid];

    switch(g_PlayerWork[playerid])
    {
        case WORK_TAXI:
        {
            // Losowa pozycja pasażera
            x = float(random(4000)) - 2000.0;
            y = float(random(4000)) - 2000.0;
            z = 20.0;

            // Znajdź poprawną wysokość
            Work_FindGroundZ(x, y, z);
        }

        case WORK_TRUCKER:
        {
            if(checkpoint >= g_TruckRouteSize)
            {
                // Zako??cz tras??
                Work_CompleteRoute(playerid);
                return;
            }

            x = g_TruckRoute[checkpoint][0];
            y = g_TruckRoute[checkpoint][1];
            z = g_TruckRoute[checkpoint][2];
        }

        case WORK_BUS:
        {
            if(checkpoint >= g_BusRouteSize)
            {
                Work_CompleteRoute(playerid);
                return;
            }

            x = g_BusRoute[checkpoint][0];
            y = g_BusRoute[checkpoint][1];
            z = g_BusRoute[checkpoint][2];
        }

        case WORK_PIZZA:
        {
            // Losowa lokalizacja dostawy
            x = 2105.0 + float(random(500)) - 250.0;
            y = -1806.0 + float(random(500)) - 250.0;
            z = 13.5;
        }

        case WORK_GARBAGE:
        {
            // Punkty odbioru ??mieci
            static Float:garbagePoints[5][3] = {
                {1480.5, -1770.5, 13.5},
                {1580.5, -1850.5, 13.5},
                {1350.5, -1650.5, 13.5},
                {1750.5, -1920.5, 13.5},
                {1900.5, -1700.5, 13.5}
            };

            new point = random(sizeof(garbagePoints));
            x = garbagePoints[point][0];
            y = garbagePoints[point][1];
            z = garbagePoints[point][2];
        }

        default:
        {
            return;
        }
    }

    SetPlayerCheckpoint(playerid, x, y, z, 3.0);
    g_PlayerWorkCheckpoint[playerid] = checkpoint;
}

/**
 * Obs??uguje wej??cie do checkpointu pracy
 * @param playerid ID gracza
 */
stock Work_OnCheckpointEnter(playerid)
{
    if(g_PlayerWork[playerid] == WORK_NONE)
    {
        return 0;
    }

    // Sprawd?? czy w poje??dzie pracy (je??li wymagany)
    if(g_PlayerWorkVehicle[playerid] != 0)
    {
        if(GetPlayerVehicleID(playerid) != g_PlayerWorkVehicle[playerid])
        {
            SendClientMessage(playerid, COLOR_ERROR, "You must be in the work vehicle!");
            return 0;
        }
    }

    // Add earnings for the point
    new earning = Work_GetPointEarning(g_PlayerWork[playerid]);
    g_PlayerWorkEarnings[playerid] += earning;
    g_PlayerWorkProgress[playerid]++;

    new msg[64];
    format(msg, sizeof(msg), "Point delivered! +%s$", FormatMoney(earning));
    SendClientMessage(playerid, COLOR_SUCCESS, msg);

    // Next checkpoint
    Work_SetNextCheckpoint(playerid);

    return 1;
}

/**
 * Ko??czy tras?? pracy
 * @param playerid ID gracza
 */
static stock Work_CompleteRoute(playerid)
{
    // Bonus za uko??czenie trasy
    new bonus = g_PlayerWorkEarnings[playerid] / 4;
    g_PlayerWorkEarnings[playerid] += bonus;

    new msg[128];
    format(msg, sizeof(msg), "Route completed! Bonus: %s$", FormatMoney(bonus));
    SendClientMessage(playerid, COLOR_SUCCESS, msg);

    // End work
    Work_Stop(playerid, false);
}

// ===========================================================================
// FUNKCJE POMOCNICZE
// ===========================================================================

/**
 * Pobiera nazw?? typu pracy
 */
stock Work_GetTypeName(E_WORK_TYPE:workType, dest[], size = sizeof(dest))
{
    static const names[][] = {
        "None",
        "Taxi Driver",
        "Trucker",
        "Bus Driver",
        "Pizza Delivery",
        "Garbage Collector",
        "Farmer",
        "Miner",
        "Lumberjack",
        "Fisherman",
        "Mechanic",
        "Arms Dealer",
        "Drug Dealer"
    };

    if(workType < WORK_NONE || workType >= E_WORK_MAX)
    {
        workType = WORK_NONE;
    }

    strcopy(dest, names[_:workType], size);
    return 1;
}

/**
 * Pobiera zarobek za punkt pracy
 */
static stock Work_GetPointEarning(E_WORK_TYPE:workType)
{
    switch(workType)
    {
        case WORK_TAXI:         return 100 + random(50);
        case WORK_TRUCKER:      return 250 + random(100);
        case WORK_BUS:          return 80 + random(40);
        case WORK_PIZZA:        return 50 + random(30);
        case WORK_GARBAGE:      return 120 + random(50);
        case WORK_FARMER:       return 60 + random(30);
        case WORK_MINER:        return 200 + random(80);
        case WORK_LUMBERJACK:   return 150 + random(60);
        case WORK_FISHERMAN:    return 70 + random(50);
    }

    return 50;
}

/**
 * Znajduje najbli??szy punkt pracy
 */
static stock Work_GetNearestPoint(playerid, E_WORK_TYPE:workType)
{
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);

    for(new i = 0; i < sizeof(g_WorkPoints); i++)
    {
        if(E_WORK_TYPE:g_WorkPoints[i][work_type] != workType)
        {
            continue;
        }

        new Float:dist = GetDistanceBetweenPoints3D(px, py, pz,
            g_WorkPoints[i][work_x], g_WorkPoints[i][work_y], g_WorkPoints[i][work_z]);

        if(dist < 10.0)
        {
            return i;
        }
    }

    return -1;
}

// ===========================================================================
// KOMENDY
// ===========================================================================

YCMD:praca(playerid, params[], help)
{
    if(!pInfo[playerid][player_logged])
    {
        return SendClientMessage(playerid, COLOR_ERROR, "You must be logged in!");
    }

    if(isnull(params))
    {
        // Show work menu
        new content[512] = "Taxi Driver\nTrucker\nBus Driver\nPizza Delivery\nGarbage Collector\nFarmer\nMiner\nLumberjack\nFisherman";

        ShowPlayerDialog(playerid, DIALOG_WORK, DIALOG_STYLE_LIST,
            "Choose a job", content, "Select", "Cancel");

        return 1;
    }

    if(!strcmp(params, "stop", true))
    {
        if(g_PlayerWork[playerid] == WORK_NONE)
        {
            return SendClientMessage(playerid, COLOR_ERROR, "You are not working any job.");
        }

        Work_Stop(playerid, false);
        return 1;
    }

    return 1;
}

// ===========================================================================
// HOOKI
// ===========================================================================

hook OnPlayerEnterCheckpoint@Works(playerid)
{
    if(g_PlayerWork[playerid] != WORK_NONE)
    {
        Work_OnCheckpointEnter(playerid);
        return Y_HOOKS_BREAK_RETURN_1;
    }

    return 1;
}

hook OnPlayerDisconnect@Works(playerid, reason)
{
    Work_Stop(playerid, true);
    return 1;
}

hook OnPlayerDeath@Works(playerid, killerid, WEAPON:reason)
{
    Work_Stop(playerid, true);
    return 1;
}

// ===========================================================================
// EOF
// ===========================================================================

