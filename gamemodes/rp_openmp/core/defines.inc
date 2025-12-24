/*
 * =============================================================================
 *  DEFINES - Constants and project definitions
 * =============================================================================
 *
 *  Module: core/defines.inc
 *  Description: Contains all constants, limits and macros used in the project
 *
 *  CHANGES IN REFACTORING:
 *  - Moved all #define to one place
 *  - Added documentation for each section
 *  - Removed duplicates
 *  - Updated values for open.mp
 *
 * =============================================================================
 */

#if defined _defines_included
    #endinput
#endif
#define _defines_included

// ===========================================================================
// PLAYER LIMITS AND BASIC VALUES
// ===========================================================================

#undef MAX_PLAYERS
#define MAX_PLAYERS                     500

// Offset for Virtual World during login
#define VIRTUAL_WORLD_LOGIN_OFFSET      900

// ===========================================================================
// GAME OBJECT LIMITS
// ===========================================================================

// Groups
#define MAX_GROUPS                      1500
#define MAX_GROUP_WORKERS               50
#define MAX_PLAYER_GROUPS               5   // Number of groups per player

// Objects
#undef MAX_OBJECTS
#define MAX_OBJECTS                     55000
#define MAX_VISIBLE_OBJECTS             450

// Doors/Buildings
#define MAX_DOORS                       800

// Special places
#define MAX_PLACES                      100

// Zones
#define MAX_AREAS                       1000

// Gang wars
#define MAX_GANGWARS                    100

// 3D Labels
#define MAX_VISIBLE_LABELS              200
#define MAX_LABELS                      1000
#define MAX_ZONE_NAME                   32

// Vehicles
#define MAX_VISIBLE_VEHICLES            500

// Animations
#define MAX_ANIMATIONS                  300

// Items
#define MAX_ITEMS                       1000
#define MAX_PLAYER_ITEMS                100

// Robberies
#define MAX_ROBBERIES                   20

// Fires
#define MAX_FIRE_SOURCES                10

// Products
#define MAX_PRODUCTS                    1000

// Checkpoints
#define MAX_RACE_CP                     64

// Skins and accessories
#define MAX_SKINS                       200
#define MAX_ACCESS                      150

// Global spawns
#define MAX_GLOBAL_SPAWNS               20

// ===========================================================================
// SYSTEM CONFIGURATION
// ===========================================================================

// Fire spread delay (seconds)
#define FIRE_SPREAD_DELAY               120

// ===========================================================================
// FILE PATHS
// ===========================================================================

#define CONFIG_DEFAULT_PATH             "scriptfiles/config.ini"
#define CONFIG_FALLBACK_PATH            "gamemodes/rp_openmp/scriptfiles/config.ini"
#define LOG_PLAYER                      "srp/logs/player.log"
#define LOG_VEHICLE                     "srp/logs/vehicle.log"
#define LOG_ADMIN                       "srp/logs/admin.log"
#define LOG_MYSQL                       "srp/logs/mysql.log"

// Compatibility aliases for files/paths expected by rp/ modules
#if !defined CONFIG_FILEPATH
#define CONFIG_FILEPATH CONFIG_DEFAULT_PATH
#endif // CONFIG_FILEPATH

// Note: LOG_* macros are defined above to the expected paths for this mode.
// Do not redefine them here to avoid macro recursion or redefinition warnings.


// ===========================================================================
// OWNER TYPES
// ===========================================================================

// Item owner types
#define ITEM_OWNER_TYPE_NONE            0
#define ITEM_OWNER_TYPE_PLAYER          1
#define ITEM_OWNER_TYPE_GROUND          2
#define ITEM_OWNER_TYPE_GROUP           3
#define ITEM_OWNER_TYPE_VEHICLE         4
#define ITEM_OWNER_TYPE_DOOR            5

// Admin levels
#define ADMIN_NONE                      0
#define ADMIN_HELPER                    1
#define ADMIN_MODERATOR                 2
#define ADMIN_ADMIN                     3
#define ADMIN_SENIOR_ADMIN              4
#define ADMIN_HEAD_ADMIN                5
#define ADMIN_OWNER                     6

// Vehicle owner types
#define VEHICLE_OWNER_TYPE_NONE         0
#define VEHICLE_OWNER_TYPE_PLAYER       1
#define VEHICLE_OWNER_TYPE_GROUP        2
#define VEHICLE_OWNER_TYPE_JOB          3

// Door owner types
#define DOOR_OWNER_TYPE_NONE            0
#define DOOR_OWNER_TYPE_GLOBAL          1
#define DOOR_OWNER_TYPE_PLAYER          2
#define DOOR_OWNER_TYPE_GROUP           3

// Area owner types
#define AREA_OWNER_TYPE_GLOBAL          0
#define AREA_OWNER_TYPE_GROUP           1
#define AREA_OWNER_TYPE_DOOR            2

// ===========================================================================
// GAME OBJECT TYPES
// ===========================================================================

// Pickup types
#define PICKUP_TYPE_NONE                0
#define PICKUP_TYPE_DOOR                1
#define PICKUP_TYPE_JOB                 2
#define PICKUP_TYPE_GOV                 3
#define PICKUP_TYPE_ITEM                4

// Group types - enum E_GROUP_TYPE is in gameplay/groups.inc

// Old definitions (legacy - use E_GROUP_TYPE)
#define GROUP_TYPE_LSPD_LEGACY          1
#define GROUP_TYPE_GANG_LEGACY          5
#define GROUP_TYPE_SN_LEGACY            6
#define GROUP_TYPE_GASTRO_LEGACY        7
#define GROUP_TYPE_WORKSHOP_LEGACY      8
#define GROUP_TYPE_GOV_LEGACY           9

// Area types
#define AREA_TYPE_NORMAL                0
#define AREA_TYPE_SPECIAL               1

// Spawn types
#define SPAWN_TYPE_GLOBAL               0
#define SPAWN_TYPE_GLOBAL_SC            1

// Session types
#define SESSION_TYPE_LOGIN              1
#define SESSION_TYPE_LOGOUT             2

// Special place types
#define PLACE_TYPE_NONE                 0
#define PLACE_TYPE_HOTEL                1
#define PLACE_TYPE_PHARMACY             2
#define PLACE_TYPE_VICTIM               3
#define PLACE_TYPE_BANK                 4
#define PLACE_TYPE_SHOP                 5
#define PLACE_TYPE_AMMUNATION           6
#define PLACE_TYPE_TOOLSHOP             7
#define PLACE_TYPE_GYM                  8

// ===========================================================================
// ITEM TYPES - enum E_ITEM_TYPE is in gameplay/items.inc
// ===========================================================================

// Old defines are replaced by enum E_ITEM_TYPE in items.inc

// ===========================================================================
// PERMISSION FLAGS
// ===========================================================================

// Crew/admin permission flags
#define CREW_FLAG_DOORS                 (1 << 0)  // 1
#define CREW_FLAG_OBJECTS               (1 << 1)  // 2
#define CREW_FLAG_VEHICLES              (1 << 2)  // 4
#define CREW_FLAG_GROUPS                (1 << 3)  // 8
#define CREW_FLAG_PLAYERS               (1 << 4)  // 16
#define CREW_FLAG_AREAS                 (1 << 5)  // 32
#define CREW_FLAG_BANS                  (1 << 6)  // 64
#define CREW_FLAG_LABELS                (1 << 7)  // 128
#define CREW_FLAG_ITEMS                 (1 << 8)  // 256

// Group type flags (what type of group it is)
#define GROUP_FLAG_SECURITY             (1 << 0)   // 1 - Security
#define GROUP_FLAG_GYM                  (1 << 1)   // 2 - Gym
#define GROUP_FLAG_TAXI                 (1 << 2)   // 4 - Taxi
#define GROUP_FLAG_CITYHALL             (1 << 3)   // 8 - City Hall
#define GROUP_FLAG_RADIO                (1 << 4)   // 16 - Radio Station
#define GROUP_FLAG_POLICE               (1 << 5)   // 32 - Police
#define GROUP_FLAG_GANG                 (1 << 6)   // 64 - Gang
#define GROUP_FLAG_MAFIA                (1 << 7)   // 128 - Mafia
#define GROUP_FLAG_MEDIC                (1 << 8)   // 256 - Medics/EMS
#define GROUP_FLAG_MECHANIC             (1 << 9)   // 512 - Mechanics
#define GROUP_FLAG_NEWS                 (1 << 10)  // 1024 - Media/News
#define GROUP_FLAG_GOVERNMENT           (1 << 11)  // 2048 - Government

// Group permissions (what the group can do)
#define GROUP_ABILITY_DUTY              (1 << 0)   // 1 - can use /duty
#define GROUP_ABILITY_CUFF              (1 << 1)   // 2 - can cuff
#define GROUP_ABILITY_SEARCH            (1 << 2)   // 4 - can search
#define GROUP_ABILITY_ARREST            (1 << 3)   // 8 - can arrest
#define GROUP_ABILITY_MDT               (1 << 4)   // 16 - access to MDT
#define GROUP_ABILITY_TICKET            (1 << 5)   // 32 - can issue tickets
#define GROUP_ABILITY_MEGAPHONE         (1 << 6)   // 64 - can use megaphone
#define GROUP_ABILITY_RADIO             (1 << 7)   // 128 - group radio
#define GROUP_ABILITY_HEAL              (1 << 8)   // 256 - can heal
#define GROUP_ABILITY_REPAIR            (1 << 9)   // 512 - can repair
#define GROUP_ABILITY_TOW               (1 << 10)  // 1024 - can tow
#define GROUP_ABILITY_BROADCAST         (1 << 11)  // 2048 - can broadcast on public radio
#define GROUP_ABILITY_DOTATION          (1 << 12)  // 4096 - can receive donations
#define GROUP_ABILITY_MASK              (1 << 13)  // 8192 - can use masks
#define GROUP_ABILITY_WEAPONS           (1 << 14)  // 16384 - access to weapon storage
#define GROUP_ABILITY_DRUGS             (1 << 15)  // 32768 - can produce drugs

// Worker flags
#define WORKER_FLAG_DOORS               (1 << 0)  // 1
#define WORKER_FLAG_LEADER              (1 << 1)  // 2
#define WORKER_FLAG_VEHICLES            (1 << 2)  // 4
#define WORKER_FLAG_PRODUCTS            (1 << 3)  // 8
#define WORKER_FLAG_ITEMS               (1 << 4)  // 16
#define WORKER_FLAG_DUTY                (1 << 5)  // 32

// ===========================================================================
// DOOR ACCESS
// ===========================================================================

#define DOOR_ACCESS_NONE                0
#define DOOR_ACCESS_FIRE_SPRINKLER      (1 << 0)  // 1 - fire sprinklers

// ===========================================================================
// PLAYER STATUS
// ===========================================================================

#define PLAYER_STATUS_NONE              0
#define PLAYER_STATUS_AFK               1
#define PLAYER_STATUS_INJURED           2
#define PLAYER_STATUS_PHONE             3

// ===========================================================================
// OBJECT EDITOR
// ===========================================================================

#define OBJECT_EDITOR_SAMP              0
#define OBJECT_EDITOR_CUSTOM            1

// ===========================================================================
// ATTACHED OBJECT SLOTS
// ===========================================================================

#define ATTACH_SLOT_WEAPON_START        0
#define ATTACH_SLOT_WEAPON_END          5
#define ATTACH_SLOT_PHONE               6
#define ATTACH_SLOT_VICTIM              7
#define ATTACH_SLOT_MASK                8
#define ATTACH_SLOT_HAT                 9

// ===========================================================================
// DIALOGS - IDs
// ===========================================================================

#define DIALOG_NONE                     0
#define DIALOG_LOGIN                    1
#define DIALOG_LOGIN_NO_ACCOUNT         2
#define DIALOG_REGISTER                 3
#define DIALOG_REGISTER_CONFIRM         4
#define DIALOG_STATS                    10
#define DIALOG_INFO                     11
#define DIALOG_CONFIRM                  12

#define DIALOG_PLAYER_ITEMS             100
#define DIALOG_ITEMS_PICKUP             101
#define DIALOG_ITEM_OPTIONS             102
#define DIALOG_ITEM_USE                 103
#define DIALOG_ITEM_GIVE                104

#define DIALOG_AREA                     200
#define DIALOG_ROBBERY                  201
#define DIALOG_RADIOSTATIONS            202
#define DIALOG_OFFER                    203
#define DIALOG_WORK                     204
#define DIALOG_INVENTORY                205
#define DIALOG_GROUPS                   206
#define DIALOG_VEHICLES                 207
#define DIALOG_GROUP_OPTIONS            208
#define DIALOG_GROUP_MEMBERS            209
#define DIALOG_GROUP_CHAT               210
#define DIALOG_VEHICLE_OPTIONS          211

#define DIALOG_SALON_SELL               300

// Car shop dialogs
#define DIALOG_CARSHOP_CATEGORIES       360
#define DIALOG_CARSHOP_VEHICLES         361

// Stats menu dialogs
#define DIALOG_STATS_MENU               350
#define DIALOG_PREMIUM_SHOP             351
#define DIALOG_WALKING_STYLE            352
#define DIALOG_CREATE_GANG              353
#define DIALOG_CREATE_GANG_CONFIRM      354

// Phone dialog IDs (modular phone system)
#define DIALOG_PHONE_MENU               600
#define DIALOG_PHONE_DIAL               601
#define DIALOG_PHONE_INCOMING           602
#define DIALOG_PHONE_CONTACTS           603
#define DIALOG_PHONE_CONTACT_OPTIONS    604
#define DIALOG_PHONE_CONTACT_VIEW       605
#define DIALOG_PHONE_CONTACT_EDIT       606
#define DIALOG_PHONE_CONTACT_ADD        607
#define DIALOG_PHONE_SMS_MENU           610
#define DIALOG_PHONE_SMS_LIST           611
#define DIALOG_PHONE_SMS_VIEW           612
#define DIALOG_PHONE_SMS_SEND           613
#define DIALOG_PHONE_HISTORY            620
#define DIALOG_PHONE_HISTORY_DETAIL     621
#define DIALOG_PHONE_ITEM_OPTIONS       630

// Administrative dialogs
#define DIALOG_AG_TYPES                 400
#define DIALOG_AG_ABILITIES             401

// ===========================================================================
// DYNAMIC GUI - IDENTIFIERS
// ===========================================================================

#define DG_NONE                         0
#define DG_AREA_INFO                    1
#define DG_AREA_TAKE                    2
#define DG_AREA_VISIBLE                 3
#define DG_ITEMS_ITEM_ROW               10
#define DG_ITEMS_PICKUP_ROW             11
#define DG_PRODS_SALON                  20

// ===========================================================================
// ACHIEVEMENTS
// ===========================================================================

#define ACHIEV_NOOB                     (1 << 0)   // 1
#define ACHIEV_50H                      (1 << 1)   // 2
#define ACHIEV_500H                     (1 << 2)   // 4
#define ACHIEV_1000H                    (1 << 3)   // 8
#define ACHIEV_LEADER                   (1 << 4)   // 16
#define ACHIEV_BLEADER                  (1 << 5)   // 32
#define ACHIEV_CAREER                   (1 << 6)   // 64
#define ACHIEV_RICH                     (1 << 7)   // 128
#define ACHIEV_CARS                     (1 << 8)   // 256
#define ACHIEV_BILLION                  (1 << 9)   // 512
#define ACHIEV_WORKER                   (1 << 10)  // 1024
#define ACHIEV_DRIVER                   (1 << 11)  // 2048
#define ACHIEV_RACE                     (1 << 12)  // 4096
#define ACHIEV_FIRSTCAR                 (1 << 13)  // 8192
#define ACHIEV_STRONG                   (1 << 14)  // 16384
#define ACHIEV_GYM                      (1 << 15)  // 32768
#define ACHIEV_ADDICT                   (1 << 16)  // 65536
#define ACHIEV_ITEM                     (1 << 17)  // 131072
#define ACHIEV_ROBBER                   (1 << 18)  // 262144
#define ACHIEV_BANDIT                   (1 << 19)  // 524288

// ===========================================================================
// HELPER MACROS
// ===========================================================================

// Convert character to uppercase
#define chrtoupper(%1) \
    (((%1) > 0x60 && (%1) <= 0x7A) ? ((%1) ^ 0x20) : (%1))

// Check if a key is being held
#define HOLDING(%0) \
    ((newkeys & (%0)) == (%0))

// Iterate through an array (enhanced forEx version)
#define forEx(%1,%2) \
    for(new %2 = 0; %2 < %1; %2++)

// Debug text - protection against formatting
#define DebugText(%0) \
    for(new _i = strlen(%0) - 1; _i >= 0; _i--) \
        if(%0[_i] == '%') %0[_i] = '#'

// ===========================================================================
// GUI COLORS - Prefixes
// ===========================================================================

#define guiopis                         "{adc7e7}{ffffff} "version" {adc7e7}??? {ffffff}"
#define opis                            "{adc7e7}{ffffff} "version" {adc7e7}: {ffffff}"
#define wersja                          "Essential RP Open Multiplayer 0.0.1"

// HEX colors for dialogs
#define COLOR_GUI_BLUE                  "{a9c4e4}"
#define HEX_COLOR_WHITE                 "{ffffff}"
#define HEX_COLOR_LIGHTER_GREEN         "{90ee90}"
#define HEX_COLOR_LIGHTER_YELLOW        "{ffffe0}"

// ===========================================================================
// WEAPON SLOTS (visual models)
// ===========================================================================

// GetWeaponSlot is already defined in open.mp, so we don't redefine it
#if !defined GetWeaponSlot
    stock GetWeaponSlot(weaponid)
    {
        switch(weaponid)
        {
            case 0, 1: return 0;
            case 2..9: return 1;
            case 10..15: return 10;
            case 16..18, 39: return 8;
            case 22..24: return 2;
            case 25..27: return 3;
            case 28, 29, 32: return 4;
            case 30, 31: return 5;
            case 33, 34: return 6;
            case 35..38: return 7;
            case 40: return 12;
            case 41..43: return 9;
            case 44..46: return 11;
        }
        return 0;
    }
#endif

// ===========================================================================
// EOF
// ===========================================================================

