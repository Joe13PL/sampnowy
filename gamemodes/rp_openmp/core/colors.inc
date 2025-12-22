/*
 * =============================================================================
 *  COLORS - Definicje kolor??w
 * =============================================================================
 *  
 *  Modu??: core/colors.inc
 *  Opis: Zawiera wszystkie definicje kolor??w u??ywane w projekcie
 *
 *  ZMIANY W REFAKTORZE:
 *  - Zunifikowano nazewnictwo kolor??w
 *  - Dodano kolory w formacie HEX i RGBA
 *  - Uporz??dkowano kategorie kolor??w
 *
 * =============================================================================
 */



// ===========================================================================
// KOLORY PODSTAWOWE - Format 0xRRGGBBAA
// ===========================================================================

// Biel i czer??
#define COLOR_WHITE                     0xFFFFFFFF
#define COLOR_BLACK                     0x000000FF

// Kolory systemowe
#define COLOR_RED                       0xFF0000FF
#define COLOR_GREEN                     0x00FF00FF
#define COLOR_BLUE                      0x0000FFFF
#define COLOR_YELLOW                    0xFFFF00FF
#define COLOR_ORANGE                    0xFFA500FF
#define COLOR_PURPLE                    0x800080FF
#define COLOR_PINK                      0xFFC0CBFF
#define COLOR_CYAN                      0x00FFFFFF

// ===========================================================================
// KOLORY CZATU I KOMUNIKAT??W
// ===========================================================================

// Kolory statusu
#define COLOR_SUCCESS                   0x2ECC71FF  // Zielony - sukces
#define COLOR_ERROR                     0xE74C3CFF  // Czerwony - b????d
#define COLOR_INFO                      0x3498DBFF  // Niebieski - informacja
#define COLOR_WARNING                   0xF39C12FF  // Pomara??czowy - ostrze??enie

// Szaro??ci
#define COLOR_GREY                      0xAFAFAFFF
#define COLOR_DARK_GREY                 0x808080FF
#define COLOR_LIGHT_GREY                0xD3D3D3FF

// Kolory wiadomo??ci
#define COLOR_LIGHTER_RED               0xFF6347FF  // Tomato
#define COLOR_LIGHTER_GREEN             0x90EE90FF  // Light green
#define COLOR_LIGHTER_YELLOW            0xFFFFE0FF  // Light yellow
#define COLOR_LIGHTER_BLUE              0xADD8E6FF  // Light blue

// ===========================================================================
// KOLORY SPECJALNE
// ===========================================================================

// Z??oty (premium, osi??gni??cia)
#define COLOR_GOLD                      0xFFD700FF

// Kolory labeli
#define LABEL_DESCRIPTION               0xFFFFFF60  // Bia??y przezroczysty

// Kolory paska zdrowia
#define COLOR_HEALTH_FULL               0x00FF00FF  // Zielony
#define COLOR_HEALTH_MEDIUM             0xFFFF00FF  // ??????ty
#define COLOR_HEALTH_LOW                0xFF0000FF  // Czerwony

// ===========================================================================
// KOLORY GRUP I FRAKCJI
// ===========================================================================

#define COLOR_LSPD                      0x0000CDFF  // Niebieski
#define COLOR_LSMD                      0xFF0000FF  // Czerwony (karetka)
#define COLOR_LSFD                      0xFF4500FF  // Pomara??czowo-czerwony
#define COLOR_GOV                       0x8B4513FF  // Br??zowy
#define COLOR_NEWS                      0x00CED1FF  // Turkusowy

// ===========================================================================
// KOLORY CZATU ROLEPLAY
// ===========================================================================

// Akcje gracza
#define COLOR_ME                        0xC2A2DAFF  // Fioletowy jasny
#define COLOR_DO                        0x9370DBFF  // Fioletowy ??redni
#define COLOR_AME                       0xDA70D6FF  // Orchidea

// Rozmowy
#define COLOR_TALK                      0xFFFFFFFF  // Bia??y
#define COLOR_SHOUT                     0xFFA07AFF  // ??ososiowy
#define COLOR_WHISPER                   0xFFFF00FF  // ??????ty

// OOC
#define COLOR_OOC                       0xE0FFFFFF  // Jasny cyan
#define COLOR_OOC_LOCAL                 0xC0C0C0FF  // Srebrny
#define COLOR_PM_SEND                   0x3498DBFF  // Niebieski - wysłane PM
#define COLOR_PM_RECV                   0x2ECC71FF  // Zielony - odebrane PM

// ===========================================================================
// KOLORY ADMINÓW
// ===========================================================================

#define COLOR_ADMIN                     0xFFFF00FF  // Żółty
#define COLOR_ADMIN_MSG                 0xFF6347FF  // Tomato
#define COLOR_ADMIN_WARN                0xFF4500FF  // Pomarańczowo-czerwony
#define COLOR_GRAY                      0xAFAFAFFF  // Szary (alias dla GREY)
#define COLOR_LIGHTRED                  0xFF6347FF  // Jasno-czerwony
// ===========================================================================
// KOLORY HEX DLA DIALOG??W (format {RRGGBB})
// ===========================================================================

// U??ywane w formatowaniu tekst??w dialog??w
#define HEX_WHITE                       "{FFFFFF}"
#define HEX_BLACK                       "{000000}"
#define HEX_RED                         "{FF0000}"
#define HEX_GREEN                       "{00FF00}"
#define HEX_BLUE                        "{0000FF}"
#define HEX_YELLOW                      "{FFFF00}"
#define HEX_ORANGE                      "{FFA500}"
#define HEX_GREY                        "{AFAFAF}"
#define HEX_GOLD                        "{FFD700}"
#define HEX_LIGHT_BLUE                  "{A9C4E4}"

// Kolory stref na mapie
#define HEX_AREA_NEUTRAL                "{808080}"
#define HEX_AREA_OWNED                  "{00FF00}"
#define HEX_AREA_ENEMY                  "{FF0000}"

// ===========================================================================
// FUNKCJE POMOCNICZE DLA KOLOR??W
// ===========================================================================

/**
 * Zwraca kolor gracza na podstawie jego stanu
 * @param playerid ID gracza
 * @return Kolor w formacie 0xRRGGBBAA
 */
stock GetPlayerStateColor(playerid)
{
    // AFK
    if(pInfo[playerid][player_afk])
    {
        return COLOR_GREY;
    }
    
    // Na s??u??bie admina
    if(pInfo[playerid][player_admin_duty])
    {
        return COLOR_ADMIN;
    }
    
    // Na s??u??bie grupy
    new gid = pInfo[playerid][player_duty_gid];
    if(gid != -1)
    {
        return Group[gid][group_color];
    }
    
    // Domy??lny bia??y
    return COLOR_WHITE;
}

/**
 * Konwertuje kolor RGBA na format HEX string
 * @param color Kolor w formacie 0xRRGGBBAA
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 */
stock ColorToHex(color, output[], size = sizeof(output))
{
    format(output, size, "{%06x}", (color >> 8) & 0xFFFFFF);
}

// InterpolateColor jest ju?? zdefiniowany w YSI_Core/y_utils

/**
 * Zwraca kolor paska zdrowia na podstawie procentu HP
 * @param health Aktualne zdrowie (0.0 - 100.0)
 * @return Kolor w formacie 0xRRGGBBAA
 */
stock GetHealthBarColor(Float:health)
{
    if(health >= 70.0)
    {
        return COLOR_HEALTH_FULL;
    }
    else if(health >= 30.0)
    {
        return COLOR_HEALTH_MEDIUM;
    }
    else
    {
        return COLOR_HEALTH_LOW;
    }
}

// ===========================================================================
// EOF
// ===========================================================================

