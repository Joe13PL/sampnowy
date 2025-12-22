/*
 * =============================================================================
 *  ENUMS - Enumeratory i struktury danych
 * =============================================================================
 *  
 *  Modu??: core/enums.inc
 *  Opis: Zawiera wszystkie enumeratory definiuj??ce struktury danych
 *
 *  ZMIANY W REFAKTORZE:
 *  - Uporz??dkowano enumeratory wed??ug kategorii
 *  - Dodano dokumentacj?? dla ka??dego pola
 *  - Usuni??to nieu??ywane pola
 *  - Zunifikowano nazewnictwo
 *
 * =============================================================================
 */

#if defined _enums_included
    #endinput
#endif
#define _enums_included

// ===========================================================================
// ENUMERATOR GRACZA - DANE GLOBALNE (KONTO)
// ===========================================================================

enum e_global_account_info
{
    bool:global_logged,         // Czy zalogowany
    bool:global_registered,     // Czy zarejestrowany
    global_id,                  // ID konta globalnego
    global_bad_pass,            // Liczba nieudanych pr??b logowania
    global_join_time,           // Czas do????czenia
    global_name[MAX_PLAYER_NAME + 1],  // Nick OOC
    global_salt[24],            // S??l has??a (legacy)
    global_password[72],        // Hash has??a (bcrypt)
    global_ip[20],              // Adres IP
    global_premium_time,        // Czas premium (timestamp)
    global_game_points,         // Punkty gry
    global_admin_perm,          // Uprawnienia admina
    global_ban,                 // Status bana
    bool:glo_run,               // Anti-run enabled
    // Aliasy dla kompatybilności
    glo_id,                     // Alias dla global_id (deprecated)
    glo_admin,                  // Alias dla global_admin_perm (deprecated)
    glo_name[MAX_PLAYER_NAME + 1] // Alias dla global_name (deprecated)
}

// ===========================================================================
// ENUMERATOR GRACZA - DANE POSTACI
// ===========================================================================

enum e_players_info
{
    // Identyfikacja
    player_uid,                             // UID postaci (w bazie danych)
    player_account_id,                      // ID konta (accounts table)
    player_id,                              // ID gracza SA-MP (legacy compatibility)
    player_name[MAX_PLAYER_NAME + 1],       // Imi?? i nazwisko
    player_duty_tmp[MAX_PLAYER_NAME + 1],   // Tymczasowy nick na duty
    player_ip[20],                          // IP gracza
    player_serial[128],                     // Serial GPCI
    
    // Dane postaci - podstawowe
    player_gender,                          // P??e?? (0 = m????czyzna, 1 = kobieta)
    player_skin,                            // Aktualny skin
    player_level,                           // Poziom
    player_respect,                         // Respekt
    player_bank,                            // Stan konta bankowego
    
    // Administracja
    player_admin,                           // Poziom admina
    player_vip,                             // Poziom VIP
    player_warns,                           // Ostrze??enia
    
    // Pozycja
    Float:player_pos_x,                     // Pozycja X
    Float:player_pos_y,                     // Pozycja Y
    Float:player_pos_z,                     // Pozycja Z
    Float:player_pos_a,                     // K??t (angle)
    Float:player_angle,                     // K??t (alias)
    player_interior,                        // Interior
    player_vw,                              // Virtual World
    
    // Spawn
    player_spawn_door,                      // Drzwi spawnu
    bool:player_loaded,                     // Dane za??adowane
    
    // Statystyki
    player_kills,                           // Zab??jstwa
    player_deaths,                          // ??mierci
    player_spawns,                          // Ilo???? spawn??w
    Float:player_driven_km,                 // Przejechane km
    
    // Potrzeby
    player_hunger,                          // G????d (0-100)
    player_thirst,                          // Pragnienie (0-100)
    
    // Kary
    player_jail,                            // Jail time (sekundy)
    player_penalty_points,                  // Punkty karne
    player_ban,                             // Status bana
    
    // Pojazd
    player_last_veh,                        // Ostatni pojazd
    player_seat,                            // Miejsce w poje??dzie
    
    // Czas
    player_online_time,                     // Czas online (sekundy)
    player_afk_time,                        // Czas AFK
    player_mute_time,                       // Czas mute
    player_last_join,                       // Ostatnie dolaczenie
    player_last_login,                      // Czas ostatniego logowania
    player_last_activity,                   // Ostatnia aktywnosc
    player_connect_time,                    // Czas polaczenia (timestamp)
    player_gameTime,                        // Czas gry
    player_y_flood,                         // Anti-flood dla dialogow
    
    // Grupy
    player_group[5],                        // Przynaleznosc do grup (max 5)
    player_group_perm[5],                   // Uprawnienia w grupach
    player_group_salary[5],                 // Pensje w grupach
    
    // Labele
    Text3D:player_label,                    // Label nad g??ow??
    Text3D:player_description_label,        // Label opisu
    player_description[128],                // Tekst opisu
    player_status[5],                       // Statusy (AFK, BW, etc.)
    
    // Stany
    bool:player_freezed,                    // Zamro??ony
    bool:player_admin_duty,                 // Na duty admina
    player_admin_duty_start,                // Start duty admina
    player_admin_duty_afk_time,             // AFK na duty admina
    bool:player_mask,                       // Czy za??o??ona maska
    
    bool:player_group_list_showed,          // Lista grup pokazana
    
    // Statystyki
    player_hours,                           // Godziny gry
    player_minutes,                         // Minuty gry
    
    // Stan postaci
    player_drunk,                           // Poziom upojenia
    player_sex,                             // P??e?? (0 = m????czyzna, 1 = kobieta)
    player_age,                             // Rok urodzenia
    
    // Si??a i ud??wig
    Float:player_strength,                  // Si??a (0-100)
    player_capacity,                        // Ud??wig (gramy)
    
    // Praca
    player_job,                             // Aktualna praca
    player_working,                         // Czy pracuje
    player_carry_log,                       // Niesie k??od??
    player_cutting_tree,                    // Tnie drzewo
    player_fishing,                         // ??owi ryby
    player_carry_fish,                      // Niesie ryb??
    player_job_cash,                        // Zarobione pieni??dze
    player_last_work,                       // Ostatnia praca (timestamp)
    
    // Bro?? i umiej??tno??ci
    Float:player_weapon_skill,              // Umiej??tno???? broni
    player_shooting,                        // Strzela
    player_documents,                       // Dokumenty
    
    // Obra??enia
    player_taken_damage,                    // Otrzymane obra??enia
    
    // Telefon
    bool:player_phone_call_started,         // Rozmowa rozpocz??ta
    player_phone_caller,                    // ID dzwoni??cego
    player_phone_receiver,                  // ID odbieraj??cego
    player_phone_object_index,              // Index obiektu telefonu
    player_phone_model,                     // Model telefonu
    player_call_log_id,                     // ID rekordu w tabeli phone_calls
    player_call_start_time,                 // Timestamp startu połączenia (gettime())
    player_call_answer_time,                // Timestamp odebrania połączenia
    
    // Animacje
    bool:player_looped_anim,                // Zap??tlona animacja
    player_walking_anim,                    // Animacja chodzenia
    player_walking_lib[16],                 // Biblioteka animacji
    player_walking_name[24],                // Nazwa animacji
    
    // Zdrowie
    player_bw,                              // BrokenWounds (sekundy)
    player_bw_end_time,                     // Koniec BW (timestamp)
    Float:player_health,                    // Zdrowie (cache)
    bool:player_health_changed,             // Zmieniono zdrowie
    PlayerBar:player_health_bar,            // Pasek zdrowia
    
    // Pancerz
    Float:player_armour,                    // Pancerz (cache)
    bool:player_armour_changed,             // Zmieniono pancerz
    Float:player_spawn_armor,               // Pancerz przy respawnie
    
    // Finanse
    player_money,                           // Got??wka
    player_bank_number,                     // Numer konta
    player_bank_money,                      // Stan konta
    
    // R????ne
    player_editor,                          // Typ edytora obiekt??w
    player_last_creator,                    // Ostatni tw??rca
    player_last_bullet,                     // Ostatni pocisk
    player_achievements,                    // Osi??gni??cia (bitfield)
    
    // Pozycja wyj??cia
    Float:player_quit_pos[4],               // X, Y, Z, Angle
    player_quit_vw,                         // Virtual World
    player_quit_int,                        // Interior
    player_quit_time,                       // Czas wyj??cia
    
    // Duty
    player_onduty_afk,                      // AFK na duty
    
    // Wizualne
    player_flash,                           // Flash efekt
    
    // Logowanie
    bool:player_logged,                     // Zalogowany
    bool:player_afk,                        // AFK
    bool:player_belt,                       // Pasy bezpiecze??stwa
    player_spawned,                         // Zrespawniony
    player_last_skin,                       // Poprzedni skin
    player_spawn,                           // Punkt spawnu
    player_spawn_type,                      // Typ spawnu
    
    // Ustawienia
    player_tog,                             // Toggle opcje
    player_last_pm_playerid,                // Ostatni PM
    
    // Tworzenie stref
    bool:player_creating_area,              // Tworzy stref??
    Float:player_carea_point1[3],           // Punkt 1
    Float:player_carea_point2[3],           // Punkt 2
    Text3D:player_carea_label[2],           // Labele
    player_carea_zone,                      // Zona strefy
    player_carea_type,                      // Typ strefy
    Float:player_carea_size,                // Rozmiar strefy
    player_music_zone,                      // Strefa muzyki
    
    bool:player_lookup_area,                // Podgl??da stref??
    player_left_area_time,                  // Czas opuszczenia strefy
    
    // Anti-cheat
    bool:player_modified_weapon,            // Zmodyfikowana bro??
    player_cheat_warns,                     // Ostrze??enia cheata
    player_airbrk_warns,                    // Ostrze??enia airbreak
    Float:player_cheat_pos[3],              // Pozycja do sprawdzenia
    
    // Czat grupowy
    player_last_group_slot_chat,            // Ostatni slot czatu
    
    // Prototypy przedmiotow
    bool:player_items_proto_create,         // Tworzy prototyp
    player_items_proto_create_id,           // ID prototypu
    player_item_count,                      // Liczba przedmiotow
    
    // GPS
    player_gps,                             // Cel GPS
    
    // Bro??
    player_held_weapon,                     // Trzymana bro??
    
    // Obiekty
    player_edited_object,                   // Edytowany obiekt
    bool:player_edited_object_no_action,    // Bez akcji
    Float:player_edited_object_pos[6],      // Pozycja obiektu
    
    // ESEL Editor
    player_esel_edited_label,               // Edytowany label
    player_esel_edited_object,              // Edytowany obiekt
    
    // Pojazdy
    player_vehicle_target,                  // Cel pojazdu
    
    // Dialogi tymczasowe
    player_dialog_tmp1,
    player_dialog_tmp2,
    Float:player_dialog_tmp3,
    player_dialog_tmp4,
    player_dialog_tmp5,
    player_dialog_tmp[64],
    player_info_td_id,

    // Telefon - połączenia
    player_call_target,         // ID gracza w połączeniu / dzwoniącym
    player_call_state,          // 0=idle,1=ringing,2=in_call
    player_call_timer,
    
    // Warsztat
    player_fixprice,                        // Cena naprawy
    bool:player_can_spray,                  // Mo??e malowa??
    
    // Pojazdy i napady
    player_entering_vehicle,                // Wchodzi do pojazdu
    player_occupied_vehicle,                // Zaj??ty pojazd
    player_robbery,                         // Napad
    player_robbery_lobby,                   // Lobby napadu
    player_rob_stage,                       // Etap napadu
    player_rob_time,                        // Czas napadu
    player_radio,                           // Radio
    player_active,                          // Aktywny
    
    // Areszt
    player_aj,                              // Admin Jail
    player_arrest,                          // Areszt (timestamp)
    
    // Sklep z ubraniami
    bool:player_skin_changing,              // Zmienia skin
    player_skin_id,                         // ID skina
    bool:player_access_changing,            // Zmienia akcesorium
    player_access_id,                       // ID akcesorium
    
    // ??adunki wybuchowe
    player_bomb_planting,                   // Podk??ada bomb??
    player_bomb_car,                        // Pojazd z bomb??
    player_bomb,                            // Bomba
    
    // Taxi
    player_taxi_veh,                        // Pojazd taxi
    player_taxi_cost,                       // Koszt
    player_taxi_price,                      // Cena za km
    Float:player_taxi_distance,             // Dystans
    bool:player_taxi_drive,                 // Jedzie taxi
    player_taxi_passenger,                  // Pasa??er
    
    // Mieszkania
    player_renting,                         // Wynajmuje
    player_last_sms,                        // Ostatni SMS
    
    // Granatnik
    player_howitzer,
    
    // Strefy
    player_area,                            // Aktualna strefa
    player_dealing,                         // Handel
    
    // Si??ownia
    player_gym_carnet,                      // Karnet
    player_gym,                             // Si??ownia
    bool:player_training,                   // Trenuje
    player_can_train,                       // Mo??e trenowa??
    player_gym_count,                       // Licznik ??wicze??
    player_gym_object,                      // Obiekt si??owni
    player_supple,                          // Suplementy
    
    // Walka
    player_fight_training,                  // Trening walki
    player_fight_progress,                  // Post??p walki
    player_fight_style,                     // Styl walki
    
    // Wywiad
    player_interview,
    
    // Flagi
    player_flags,
    
    // Naprawa
    player_repair_car,
    player_repair_time,
    
    // Monta??
    player_montage_car,
    player_montage_time,
    player_montage_comp,
    
    // Malowanie
    player_spraying_car,
    player_spraying_time,
    player_spraying_color,
    player_spraying_color2,
    player_spraying,
    
    // Spectate
    player_spec,
    
    // R????ne
    player_repairing,
    player_tuning,
    player_freeze,
    player_duty_gid,
    Cache:player_cache,
    player_truck,
    player_surface_edit,
    player_item_price,
    player_extra_status[32],
    player_event,
    player_tut_step,
    player_death,
    player_killer,
    player_demolition,
    
    // Wy??cigi
    player_race_point,
    player_race_phase,
    player_race_checkpoints,
    player_race_count,
    
    // Kartoteka
    player_crime_records,
    
    // Animacja strza??u
    player_shot_anim,
    
    // Spadochron
    player_parachute,
    player_ignore,
    player_last_spray,
    
    // System bus??w
    player_bus_stop,
    player_bus_object,
    player_bus_destination,
    player_bus_time,
    player_bus_zone[MAX_ZONE_NAME],
    player_bus_price,
    player_bus_ride,
    player_last_bus,
    
    // Inne
    player_custom_edit,
    player_door_text,
    player_session,
    
    // Kary i spectate (brakujące pola)
    player_muted,                           // Wyciszony (0/1)
    player_spectating,                      // ID gracza, którego obserwuje (INVALID_PLAYER_ID = nie)
    
    // Paydaye i czas online
    player_paydays,                         // Liczba paydayów
    player_online,                          // Alias dla player_online_time
    
    // Administracja rozszerzona
    player_warnings,                        // Ostrzeżenia (0-3)
    player_jailed,                          // W admin jail (bool)
    player_jail_time,                       // Czas końca jail (timestamp)
    player_invisible,                       // Niewidzialny (bool)
    player_godmode,                         // God mode (bool)
    player_noclip,                          // Noclip/fly (bool)
    player_fps                              // FPS gracza
}

// ===========================================================================
// ENUMERATOR GRUPY GRACZA
// ===========================================================================

enum e_player_groups
{
    pg_id,                      // ID grupy
    pg_rank_title[32],          // Tytu?? rangi
    pg_rank_skin,               // Skin rangi
    pg_rank_payment,            // Wyp??ata
    pg_rank_perm,               // Uprawnienia
    pg_duty_start,              // Start duty
    pg_duty_time,               // Czas duty
    pg_payday_count,            // Licznik payday
    bool:pg_duty                // Na duty
}

// ===========================================================================
// ENUMERATOR OBRA??E?? GRACZA
// ===========================================================================

enum e_player_injuries
{
    injury_id,                  // ID obra??enia
    injury_damage,              // Warto???? obra??e??
    injury_bodypart,            // Cz?????? cia??a
    injury_weapon,              // Bro??
    injury_time                 // Czas
}

// ===========================================================================
// ENUMERATOR BRONI GRACZA
// ===========================================================================

enum e_player_weapons
{
    pw_id,                      // ID broni
    pw_ammo,                    // Amunicja
    pw_itemid,                  // ID przedmiotu
    pw_object_index,            // Index attached object
    bool:pw_visible             // Czy widoczna
}

// ===========================================================================
// ENUMERATOR GRUP
// ===========================================================================

enum e_groups
{
    group_uid,                  // UID grupy
    bool:group_created,         // Czy utworzona
    group_name[64],             // Nazwa grupy
    group_tag[15],              // Tag grupy
    group_type,                 // Typ grupy
    group_flags,                // Flagi typu grupy (policja, gang, itp.)
    group_abilities,            // Uprawnienia grupy (skuwanie, mdt, itp.)
    group_bank,                 // Alias dla bank_money
    group_bank_money,           // Konto bankowe
    group_capital,              // Kapital
    group_tribute,              // Haracz
    group_tributers,            // Liczba haraczy
    group_color,                // Kolor grupy
    group_activity_points,      // Punkty aktywnosci
    group_last_tribute,         // Ostatni haracz
    group_temp,                 // Tymczasowa
    group_products,             // Produkty
    group_creator,              // Tworca
    group_leader,               // ID lidera
    group_max_members,          // Max czlonkow
    group_last_tax,             // Ostatni podatek
    group_rank0[32],            // Nazwa rangi 0
    group_rank1[32],            // Nazwa rangi 1
    group_rank2[32],            // Nazwa rangi 2
    group_rank3[32],            // Nazwa rangi 3
    group_rank4[32],            // Nazwa rangi 4
    group_rank5[32],            // Nazwa rangi 5
    group_rank6[32],            // Nazwa rangi 6
    group_rank7[32],            // Nazwa rangi 7
    group_rank8[32],            // Nazwa rangi 8
    group_rank9[32]             // Nazwa rangi 9
}

// ===========================================================================
// ENUMERATOR POJAZDÓW
// ===========================================================================

enum e_vehicles
{
    bool:veh_created,           // Czy pojazd utworzony
    veh_gameid,                 // ID pojazdu SA-MP
    veh_model,                  // Model
    Float:veh_spawn_x,          // Pozycja spawn X
    Float:veh_spawn_y,          // Pozycja spawn Y
    Float:veh_spawn_z,          // Pozycja spawn Z
    Float:veh_spawn_a,          // Pozycja spawn Angle
    veh_color1,                 // Kolor 1
    veh_color2,                 // Kolor 2
    veh_owner_type,             // Typ właściciela
    veh_owner,                  // ID właściciela
    veh_fuel,                   // Paliwo
    Float:veh_mileage,          // Przebieg
    Float:veh_health,           // Zdrowie
    bool:veh_locked,            // Zamknięty
    bool:veh_engine,            // Silnik włączony
    bool:veh_lights,            // Światła
    veh_component[14],          // Komponenty
    veh_paintjob,               // Paintjob
    Text3D:veh_label,           // Label pojazdu
    veh_map_icon,               // Ikona mapy
    bool:veh_siren              // Syrena
}

// ===========================================================================
// ENUMERATOR DRZWI
// ===========================================================================

enum e_doors
{
    door_uid,                   // UID drzwi
    bool:door_created,          // Czy utworzone
    door_name[64],              // Nazwa
    door_type,                  // Typ
    door_owner_type,            // Typ w??a??ciciela
    door_owner,                 // ID w??a??ciciela
    bool:door_locked,           // Zamkni??te
    door_pickup_model,          // Model pickupu
    door_bank,                  // Stan konta drzwi
    
    // Pozycja zewn??trzna
    Float:door_ext_x,
    Float:door_ext_y,
    Float:door_ext_z,
    Float:door_ext_a,
    door_ext_interior,
    door_ext_vw,
    
    // Pozycja wewn??trzna
    Float:door_int_x,
    Float:door_int_y,
    Float:door_int_z,
    Float:door_int_a,
    door_int_interior,
    door_int_vw,
    
    // Dodatkowe ustawienia
    door_objects_limit,         // Limit obiekt??w
    bool:door_auto_closing,     // Auto-zamykanie
    bool:door_car_crosing,      // Wjazd pojazd??w
    door_payment,               // Op??ata
    door_map_icon,              // Ikona mapy
    bool:door_closed,           // Zamkni??te
    door_rentable,              // Do wynaj??cia
    door_rent,                  // Czynsz
    door_surface,               // Powierzchnia
    door_time,                  // Czas
    door_access,                // Dost??p
    door_destroyed,             // Zniszczone %
    door_burned,                // Spalone %
    door_meters,                // Liczniki
    door_audio[100],            // URL audio
    door_area,                  // Strefa
    door_demolition             // Demolition %
}

// ===========================================================================
// ENUMERATOR STREF
// ===========================================================================

enum e_areas
{
    bool:area_created,          // Czy strefa jest utworzona
    area_uid,                   // UID strefy
    area_name[64],              // Nazwa
    area_type,                  // Typ
    area_owner_type,            // Typ właściciela
    area_owner,                 // ID właściciela
    Float:area_min_x,           // Min X
    Float:area_min_y,           // Min Y
    Float:area_min_z,           // Min Z
    Float:area_max_x,           // Max X
    Float:area_max_y,           // Max Y
    Float:area_max_z,           // Max Z
    area_interior,              // Interior
    area_vw,                    // Virtual World
    area_zone,                  // GangZone ID
    bool:area_visible,          // Widoczna
    area_color,                 // Kolor
    area_objects_limit          // Limit obiektów
}

// ===========================================================================
// ENUMERATOR PRZEDMIOT??W GRACZA
// ===========================================================================

enum e_player_items
{
    item_id,                    // ID przedmiotu
    item_type,                  // Typ
    item_value,                 // Wartosc 1
    item_value2,                // Wartosc 2
    item_name[32]               // Nazwa
}

// Stary enum dla kompatybilnosci
enum e_player_items_legacy
{
    player_item_uid,            // UID przedmiotu
    player_item_type,           // Typ
    player_item_value1,         // Wartosc 1
    player_item_value2,         // Wartosc 2
    player_item_used,           // W uzyciu
    player_item_extraid,        // Extra ID
    player_item_weight,         // Waga
    player_item_model,          // Model
    player_item_name[40],       // Nazwa
    Float:player_item_condition,// Stan
    player_item_created         // Czas stworzenia
}

// ===========================================================================
// ENUMERATOR PRZEDMIOT??W GLOBALNYCH
// ===========================================================================

enum e_items
{
    item_uid,                   // UID przedmiotu
    item_type,                  // Typ
    item_group,                 // Grupa
    item_name[40]               // Nazwa
}

// ===========================================================================
// ENUMERATOR PRODUKT??W
// ===========================================================================

enum eProducts
{
    product_id,                 // ID produktu
    product_name[40],           // Nazwa
    product_type,               // Typ
    product_price,              // Cena
    product_owner,              // W??a??ciciel
    product_value1,             // Warto???? 1
    product_value2,             // Warto???? 2
    product_model,              // Model
    product_extra,              // Extra
    product_group,              // Grupa
    product_limit,              // Limit
    product_limit_used,         // U??yty limit
    product_player              // Gracz
}

// ===========================================================================
// ENUMERATOR NAPAD??W
// ===========================================================================

enum e_robberies
{
    robbery_uid,                // UID napadu
    robbery_place,              // Miejsce
    robbery_name[60],           // Nazwa
    robbery_requirements,       // Wymagania
    robbery_level,              // Poziom
    robbery_timestamp,          // Timestamp
    robbery_actors_state,       // Stan aktor??w
    robbery_notification,       // Powiadomienie
    robbery_actors,             // Aktorzy
    robbery_aimed_actors        // Celowani aktorzy
}

// ===========================================================================
// ENUMERATOR AKTOR??W
// ===========================================================================

enum e_actors
{
    actor_uid,                  // UID aktora
    actor_skin,                 // Skin
    Float:actor_pos[4],         // Pozycja
    actor_type,                 // Typ
    actor_animation,            // Animacja
    actor_action,               // Akcja
    actor_vw,                   // Virtual World
    actor_aimed,                // Celowany
    actor_damaged,              // Uszkodzony
    actor_owner                 // W??a??ciciel
}

// ===========================================================================
// ENUMERATOR WOJEN GANG??W
// ===========================================================================

enum e_gangwars
{
    war_uid,                    // UID wojny
    war_attacker,               // Atakuj??ca grupa (UID)
    war_defender,               // Broni??ca grupa (UID)
    war_at_gid,                 // Atakuj??ca grupa (ID)
    war_def_gid,                // Broni??ca grupa (ID)
    war_attack_points,          // Punkty ataku
    war_def_points,             // Punkty obrony
    war_end,                    // Czas ko??ca (timestamp)
    war_area                    // Strefa
}

// ===========================================================================
// ENUMERATOR ??R??DE?? OGNIA
// ===========================================================================

enum e_fire_sources
{
    fs_uid,                     // UID ??r??d??a
    fs_object,                  // Obiekt
    Text3D:fs_label,            // Label
    Float:fs_health,            // "Zdrowie" ognia
    fs_door                     // Drzwi (je??li wewn??trz)
}

// ===========================================================================
// ENUMERATOR PICKUP??W
// ===========================================================================

enum e_pickups
{
    pickup_type                 // Typ pickupu
}

// ===========================================================================
// ENUMERATOR GLOBALNYCH SPAWN??W
// ===========================================================================

enum e_global_spawns
{
    gspawn_id,                  // ID spawnu
    Float:gspawn_pos[4],        // Pozycja (X, Y, Z, A)
    gspawn_type                 // Typ spawnu
}

// ===========================================================================
// ENUMERATOR MIEJSC SPECJALNYCH
// ===========================================================================

enum e_places
{
    place_uid,                  // UID miejsca
    place_type,                 // Typ
    Float:place_pos[3],         // Pozycja
    place_world                 // Virtual World
}

// ===========================================================================
// ENUMERATOR ANIMACJI
// ===========================================================================

enum e_animations
{
    aUID,                       // UID animacji
    aCommand[32],               // Komenda
    aLib[32],                   // Biblioteka
    aName[32],                  // Nazwa
    Float:aSpeed,               // Pr??dko????
    aOpt1,                      // Opcja 1
    aOpt2,                      // Opcja 2
    aOpt3,                      // Opcja 3
    aOpt4,                      // Opcja 4
    aOpt5,                      // Opcja 5
    aAction                     // Akcja
}

// ===========================================================================
// ENUMERATOR OBIEKT??W
// ===========================================================================

enum e_objects
{
    object_uid,                 // UID obiektu
    object_type,                // Typ
    object_model,               // Model
    Float:object_pos[6],        // Pozycja i rotacja
    object_vw,                  // Virtual World
    object_int,                 // Interior
    object_owner_type,          // Typ w??a??ciciela
    object_owner                // ID w??a??ciciela
}

// ===========================================================================
// ENUMERATOR OFERT
// ===========================================================================

enum e_player_offer
{
    offer_type,                 // Typ oferty
    offer_sellerid,             // ID sprzedawcy
    offer_buyerid,              // ID kupuj??cego
    offer_value,                // Warto????
    offer_price,                // Cena
    offer_extra                 // Extra dane
}

// ===========================================================================
// ENUMERATOR SKIN??W
// ===========================================================================

enum e_cloth_skins
{
    skin_model,                 // Model skina
    skin_price                  // Cena
}

// ===========================================================================
// ENUMERATOR AKCESORI??W
// ===========================================================================

enum e_cloth_access
{
    access_model,               // Model
    access_bone,                // Ko????
    Float:access_pos[6],        // Pozycja i rotacja
    access_price                // Cena
}

// ===========================================================================
// ENUMERATOR ATTACHED OBJECTS
// ===========================================================================

enum e_attached_objects
{
    Float:ao_x,                 // Offset X
    Float:ao_y,                 // Offset Y
    Float:ao_z,                 // Offset Z
    Float:ao_rx,                // Rotacja X
    Float:ao_ry,                // Rotacja Y
    Float:ao_rz                 // Rotacja Z
}

// ===========================================================================
// EOF
// ===========================================================================

