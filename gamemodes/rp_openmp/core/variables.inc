/*
 * =============================================================================
 *  VARIABLES - Zmienne globalne
 * =============================================================================
 *
 *  Modu??: core/variables.inc
 *  Opis: Deklaracje wszystkich zmiennych globalnych projektu
 *
 *  ZMIANY W REFAKTORZE:
 *  - Uporz??dkowano zmienne wed??ug kategorii
 *  - Dodano dokumentacj??
 *  - Usuni??to nieu??ywane zmienne
 *  - Dodano inicjalizacj?? iterator??w
 *
 * =============================================================================
 */

#if defined _variables_included
    #endinput
#endif
#define _variables_included

// ===========================================================================
// PO????CZENIE MYSQL
// ===========================================================================

new MySQL:mySQLconnection;

// ===========================================================================
// ZMIENNE ??WIATA GRY
// ===========================================================================

new WorldTime;                          // Aktualna godzina ??wiata
new WorldWeather;                       // Aktualna pogoda
new timer_minutes;                      // Timer minut

// ===========================================================================
// ITERATORY
// ===========================================================================

// Grupy
new Iterator:Groups<MAX_GROUPS>;
new Iterator:DeletedGroups<MAX_GROUPS>;

// Spawny
new Iterator:GlobalSpawns<MAX_GLOBAL_SPAWNS>;
new Iterator:ScSpawns<MAX_GLOBAL_SPAWNS>;

// Obiekty
new Iterator:Objects<MAX_OBJECTS>;

// Aktorzy
new Iterator:Actors<MAX_ACTORS>;

// Drzwi
new Iterator:Doors<MAX_DOORS>;

// Miejsca specjalne
new Iterator:Places<MAX_PLACES>;

// Strefy
new Iterator:Areas<MAX_AREAS>;

// Wojny gang??w
new Iterator:GangWars<MAX_GANGWARS>;

// Labele
new Iterator:Labels<MAX_LABELS>;

// Pojazdy
new Iterator:Vehicles<MAX_VEHICLES>;
new Iterator:PlayerVehicles[MAX_PLAYERS]<MAX_VISIBLE_VEHICLES>;

// Animacje
new Iterator:Anims<MAX_ANIMATIONS>;

// Przedmioty
new Iterator:Items<MAX_ITEMS>;
new Iterator:PlayerItems[MAX_PLAYERS]<MAX_PLAYER_ITEMS>;

// Napady
new Iterator:Robberies<MAX_ROBBERIES>;

// ??r??d??a ognia
new Iterator:FireSources<MAX_FIRE_SOURCES>;

// Produkty
new Iterator:Products<MAX_PRODUCTS>;

// ===========================================================================
// TABLICE DANYCH - GRACZE
// ===========================================================================

// Dane globalne gracza (konto)
new gInfo[MAX_PLAYERS][e_global_account_info];

// Dane postaci gracza
new pInfo[MAX_PLAYERS][e_players_info];

// Dane globalne gracza (rozszerzone)
new pGlobal[MAX_PLAYERS][e_global_account_info];

// Grupy gracza (max 5 grup na gracza)
new pGroup[MAX_PLAYERS][5][e_player_groups];

// Obra??enia gracza (historia 10 ostatnich)
new pInjury[MAX_PLAYERS][10][e_player_injuries];

// Bronie gracza (13 slot??w)
new pWeapon[MAX_PLAYERS][13][e_player_weapons];

// Przedmioty gracza
new PlayerItem[MAX_PLAYERS][MAX_PLAYER_ITEMS][e_player_items];
#define pItem PlayerItem  // Alias dla kompatybilnoci

// Phone number config and used-number cache (shared so modules included earlier can use it)
#define PHONE_MIN_NUMBER 1000
#define PHONE_MAX_NUMBER 99999
#define PHONE_RANGE_SIZE (PHONE_MAX_NUMBER + 1)
new gPhoneUsed[PHONE_RANGE_SIZE];
new gPhoneUsedCount = 0;

// Oferty dla gracza
new pOffer[MAX_PLAYERS][e_player_offer];

// Attached objects gracza
new ao[MAX_PLAYERS][10][e_attached_objects];

// Timer po????czenia
new connect_timer[MAX_PLAYERS];

// Tempomat (ustawiona prędkość w km/h, 0 = wyłączony)
new g_PlayerCruiseControl[MAX_PLAYERS];

// ===========================================================================
// TABLICE DANYCH - GRUPY
// ===========================================================================

new Group[MAX_GROUPS][e_groups];
#define gInfo Group  // Alias dla kompatybilnosci

// Domyslne flagi dla typow grup
new GroupDefaultFlags[10] = {
    0,                          // 0: NONE
    GROUP_FLAG_POLICE,          // 1: Policja
    GROUP_FLAG_MEDIC,           // 2: Medycy
    GROUP_FLAG_CITYHALL,        // 3: Urzad Miasta
    GROUP_FLAG_TAXI,            // 4: Taxi
    GROUP_FLAG_GANG,            // 5: Gang
    GROUP_FLAG_RADIO,           // 6: Stacja Radiowa
    GROUP_FLAG_MECHANIC,        // 7: Mechanicy
    GROUP_FLAG_MAFIA,           // 8: Mafia
    GROUP_FLAG_GOVERNMENT       // 9: Rzad
};

// ===========================================================================
// TABLICE DANYCH - POJAZDY
// ===========================================================================

new Vehicle[MAX_VEHICLES][e_vehicles];
// Mapping from internal vehicle slot to database UID (0 = none)
new VehicleDbUid[MAX_VEHICLES];
#define vInfo Vehicle  // Alias dla kompatybilności

// ===========================================================================
// TABLICE DANYCH - DRZWI
// ===========================================================================

new Door[MAX_DOORS][e_doors];
#define dInfo Door  // Alias dla kompatybilno??ci

// ===========================================================================
// TABLICE DANYCH - STREFY
// ===========================================================================

new Area[MAX_AREAS][e_areas];
#define aInfo Area  // Alias dla kompatybilnosci

// ===========================================================================
// TABLICE DANYCH - PRZEDMIOTY
// ===========================================================================

new Item[MAX_ITEMS][e_items];

// ===========================================================================
// TABLICE DANYCH - PRODUKTY
// ===========================================================================

new Product[MAX_PRODUCTS][eProducts];

// ===========================================================================
// TABLICE DANYCH - NAPADY
// ===========================================================================

new Robbery[MAX_ROBBERIES][e_robberies];

// ===========================================================================
// TABLICE DANYCH - AKTORZY
// ===========================================================================

new Actor[MAX_ACTORS][e_actors];

// ===========================================================================
// TABLICE DANYCH - WOJNY GANG??W
// ===========================================================================

new GangWar[MAX_GANGWARS][e_gangwars];

// ===========================================================================
// TABLICE DANYCH - ??R??D??A OGNIA
// ===========================================================================

new FireSource[MAX_FIRE_SOURCES][e_fire_sources];

// ===========================================================================
// TABLICE DANYCH - PICKUPY
// ===========================================================================

new Pickup[MAX_PICKUPS][e_pickups];

// Specjalne pickupy
new job_pickup;
new gov_pickup;

// ===========================================================================
// TABLICE DANYCH - SPAWNY
// ===========================================================================

new GlobalSpawn[MAX_GLOBAL_SPAWNS][e_global_spawns];

// ===========================================================================
// TABLICE DANYCH - MIEJSCA SPECJALNE
// ===========================================================================

new Place[MAX_PLACES][e_places];

// ===========================================================================
// TABLICE DANYCH - ANIMACJE
// ===========================================================================

new AnimInfo[MAX_ANIMATIONS][e_animations];

// ===========================================================================
// TABLICE DANYCH - OBIEKTY
// ===========================================================================

new Object[MAX_OBJECTS][e_objects];

// ===========================================================================
// TABLICE DANYCH - SKINY I AKCESORIA
// ===========================================================================

new ClothSkin[MAX_SKINS][e_cloth_skins];
new ClothAccess[MAX_ACCESS][e_cloth_access];

// ===========================================================================
// TABLICE DANYCH - BRONIE (VISUAL)
// ===========================================================================

// Modele wizualne broni (gdy nie s?? trzymane)
new WeaponVisualModel[47] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0-9
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 10-19
    -1, -1, 346, 347, 348, 349, 350, 351, 352, 353,  // 20-29
    355, 356, 372, 357, 358, -1, -1, -1, -1, -1,     // 30-39
    -1, -1, -1, -1, -1, -1, -1                       // 40-46
};

// Ko???? dla attached broni
new WeaponVisualBone[47] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0-9
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10-19
    0, 0, 1, 1, 1, 8, 8, 8, 1, 1,  // 20-29
    1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  // 30-39
    0, 0, 0, 0, 0, 0, 0            // 40-46
};

// Pozycje broni (normalne)
new Float:WeaponVisualPos[47][9];

// Pozycje broni (frakcyjne)
new Float:FWeaponVisualPos[47][9];

// ===========================================================================
// TABLICE DANYCH - WY??CIGI
// ===========================================================================

new Float:RaceCheckpoint[MAX_RACE_CP][4];

// ===========================================================================
// OBIEKTY SPECJALNE
// ===========================================================================

// Ko??o m??y??skie
new FerrisWheelObjects[12];

new Float:gFerrisCageOffsets[10][3] = {
    {0.0699, 9.2146, -0.0523},
    {-5.9309, 7.1352, -0.0523},
    {-9.4785, 0.0000, -0.0523},
    {-5.7632, -6.8539, -0.0523},
    {0.0699, -8.9269, -0.0523},
    {5.9309, -7.1352, -0.0523},
    {9.4785, 0.0000, -0.0523},
    {5.7632, 6.8539, -0.0523},
    {0.0699, 0.0000, -0.0523},
    {0.0699, 0.0000, -0.0523}
};

// Dealer broni (aktor)
new ArmDealer;
new bot_taken;

// ===========================================================================
// TEXTDRAWY GLOBALNE
// ===========================================================================

new Text:Textdraw2;
new Text:Tutorial[MAX_PLAYERS];

// ===========================================================================
// TEXTDRAWY GRACZA
// ===========================================================================

new PlayerText:ZoneName;
new PlayerText:TextDrawSanNews;
new PlayerText:GroupDutyTag;

// ===========================================================================
// STA??E ROZMIAR??W
// ===========================================================================

#define MAX_ZONE_NAME 32
#define MAX_PICKUPS 4096

// ===========================================================================
// INICJALIZACJA ZMIENNYCH
// Wywo??ywana podczas startu serwera
// ===========================================================================

/**
 * Inicjalizuje wszystkie zmienne na warto??ci domy??lne
 */
stock InitializeVariables()
{
    // Czas ??wiata
    WorldTime = 12;
    WorldWeather = 1;

    // Iteratory dla graczy (inicjalizowane w OnGameModeInit)
    // Iter_Init(PlayerItems);
    // Iter_Init(PlayerVehicles);

    print("[Variables] Zmienne zainicjalizowane");
}

/**
 * Czy??ci dane gracza
 * @param playerid ID gracza
 */
stock CleanPlayerVariables(playerid)
{
    // Dane globalne
    for(new i = 0; i < sizeof(gInfo[]); i++)
    {
        gInfo[playerid][e_global_account_info:i] = 0;
    }

    // Dane postaci
    for(new i = 0; i < sizeof(pInfo[]); i++)
    {
        pInfo[playerid][e_players_info:i] = 0;
    }

    // Grupy
    for(new g = 0; g < 5; g++)
    {
        for(new i = 0; i < sizeof(pGroup[][]); i++)
        {
            pGroup[playerid][g][e_player_groups:i] = 0;
        }
        pGroup[playerid][g][pg_id] = -1;
    }

    // Bronie
    for(new w = 0; w < 13; w++)
    {
        for(new i = 0; i < sizeof(pWeapon[][]); i++)
        {
            pWeapon[playerid][w][e_player_weapons:i] = 0;
        }
        pWeapon[playerid][w][pw_itemid] = -1;
        pWeapon[playerid][w][pw_object_index] = -1;
    }

    // Obra??enia
    for(new inj = 0; inj < 10; inj++)
    {
        for(new i = 0; i < sizeof(pInjury[][]); i++)
        {
            pInjury[playerid][inj][e_player_injuries:i] = 0;
        }
    }

    // Przedmioty
    for(new item = 0; item < MAX_PLAYER_ITEMS; item++)
    {
        for(new i = 0; i < sizeof(PlayerItem[][]); i++)
        {
            PlayerItem[playerid][item][e_player_items:i] = 0;
        }
    }

    // Oferty
    for(new i = 0; i < sizeof(pOffer[]); i++)
    {
        pOffer[playerid][e_player_offer:i] = 0;
    }

    // Domy??lne warto??ci
    pInfo[playerid][player_duty_gid] = -1;
    pInfo[playerid][player_robbery] = -1;
    pInfo[playerid][player_phone_caller] = INVALID_PLAYER_ID;
    pInfo[playerid][player_phone_receiver] = INVALID_PLAYER_ID;
    pInfo[playerid][player_vehicle_target] = INVALID_VEHICLE_ID;
    pInfo[playerid][player_edited_object] = INVALID_OBJECT_ID;
    pInfo[playerid][player_phone_object_index] = -1;

    // Reset tempomatu
    g_PlayerCruiseControl[playerid] = 0;

    // Iterator przedmiot??w
    Iter_Clear(PlayerItems[playerid]);
    Iter_Clear(PlayerVehicles[playerid]);
}

// ===========================================================================
// EOF
// ===========================================================================

