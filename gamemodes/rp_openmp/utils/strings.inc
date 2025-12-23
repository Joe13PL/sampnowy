/*
 * =============================================================================
 *  STRING UTILITIES - Funkcje obs??ugi string??w
 * =============================================================================
 *
 *  Modu??: utils/strings.inc
 *  Opis: Bezpieczne i zoptymalizowane funkcje operacji na stringach
 *
 *  ZMIANY W REFAKTORZE:
 *  - Dodano walidacj?? d??ugo??ci string??w
 *  - Zoptymalizowano funkcje formatowania
 *  - Dodano funkcje sanityzacji danych
 *
 * =============================================================================
 */

#if defined _string_utils_included
    #endinput
#endif
#define _string_utils_included

// ===========================================================================
// PODSTAWOWE OPERACJE NA STRINGACH
// ===========================================================================

/**
 * Bezpieczne kopiowanie stringa
 * @param dest Bufor docelowy
 * @param source ??r??d??o
 * @param size Rozmiar bufora docelowego
 * @return D??ugo???? skopiowanego stringa
 */
#if !defined strcopy
    stock strcopy(dest[], const source[], size = sizeof(dest))
    {
        dest[0] = EOS;
        return strcat(dest, source, size);
    }
#endif

/**
 * Zamienia wszystkie wyst??pienia znaku w stringu
 * @param string String do modyfikacji
 * @param find Znak do znalezienia
 * @param replace Znak zast??pczy
 * @return Liczba zamienionych znak??w
 */
stock strreplace(string[], find, replace)
{
    new count = 0;

    for(new i = 0; string[i] != EOS; i++)
    {
        if(string[i] == find)
        {
            string[i] = replace;
            count++;
        }
    }

    return count;
}

/**
 * Zamienia wszystkie wyst??pienia podstringa
 * @param string String do modyfikacji
 * @param find Substring do znalezienia
 * @param replace Substring zast??pczy
 * @param size Rozmiar bufora
 * @return Liczba zamienionych wyst??pie??
 */
stock strreplacestr(string[], const find[], const replace[], size = sizeof(string))
{
    new count = 0;
    new findLen = strlen(find);
    new replaceLen = strlen(replace);
    new pos = 0;

    if(findLen == 0)
    {
        return 0;
    }

    while((pos = strfind(string, find, true, pos)) != -1)
    {
        // Sprawd?? czy jest miejsce
        new newLen = strlen(string) - findLen + replaceLen;
        if(newLen >= size)
        {
            break;
        }

        // Przesu?? pozosta???? cz??????
        strdel(string, pos, pos + findLen);
        strins(string, replace, pos, size);

        pos += replaceLen;
        count++;
    }

    return count;
}

/**
 * Usuwa bia??e znaki z pocz??tku i ko??ca stringa
 * @param string String do modyfikacji
 * @return Nowa d??ugo???? stringa
 */
stock strtrim(string[])
{
    new start = 0;
    new end = strlen(string) - 1;

    // Znajd?? pocz??tek (pomijaj??c bia??e znaki)
    while(start <= end && (string[start] == ' ' || string[start] == '\t' ||
          string[start] == '\r' || string[start] == '\n'))
    {
        start++;
    }

    // Znajd?? koniec (pomijaj??c bia??e znaki)
    while(end >= start && (string[end] == ' ' || string[end] == '\t' ||
          string[end] == '\r' || string[end] == '\n'))
    {
        end--;
    }

    // Przesu?? string
    if(start > 0)
    {
        new i = 0;
        for(new j = start; j <= end; j++)
        {
            string[i++] = string[j];
        }
        string[i] = EOS;
        return i;
    }
    else
    {
        string[end + 1] = EOS;
        return end + 1;
    }
}

/**
 * Konwertuje string na wielkie litery
 * @param string String do modyfikacji
 */
stock strtoupper(string[])
{
    for(new i = 0; string[i] != EOS; i++)
    {
        if(string[i] >= 'a' && string[i] <= 'z')
        {
            string[i] -= 32;
        }
    }
}

/**
 * Konwertuje string na ma??e litery
 * @param string String do modyfikacji
 */
stock strtolower(string[])
{
    for(new i = 0; string[i] != EOS; i++)
    {
        if(string[i] >= 'A' && string[i] <= 'Z')
        {
            string[i] += 32;
        }
    }
}

/**
 * Konwertuje pierwszy znak na wielk?? liter??
 * @param string String do modyfikacji
 */
stock strucfirst(string[])
{
    if(string[0] >= 'a' && string[0] <= 'z')
    {
        string[0] -= 32;
    }
}

// ===========================================================================
// FORMATOWANIE WIADOMO??CI
// ===========================================================================

/**
 * Formatuje wiadomo???? i wysy??a do gracza
 * @param playerid ID gracza
 * @param color Kolor wiadomo??ci
 * @param format Format wiadomo??ci
 * @param ... Argumenty
 */
// Config: output encoding for client displays
new g_UseCp1250Output = 1; // 1 = use CP1250 encoding, 0 = fallback to ASCII transliteration

// Helper: transliterate common Polish UTF-8 sequences to ASCII equivalents
stock Polish_TransliterateUtf8ToAscii(const src[], dest[], size = sizeof dest)
{
    new i = 0, j = 0;
    while(src[i] != EOS && j < size - 1)
    {
        // Two-byte UTF-8 sequences for Polish letters
        new b1 = src[i] & 0xFF;
        new b2 = src[i+1] & 0xFF;

        // Lowercase
        if(b1 == 0xC4 && b2 == 0x85) { dest[j++] = 'a'; i += 2; continue; } // ą
        if(b1 == 0xC4 && b2 == 0x84) { dest[j++] = 'A'; i += 2; continue; } // Ą
        if(b1 == 0xC4 && b2 == 0x87) { dest[j++] = 'c'; i += 2; continue; } // ć
        if(b1 == 0xC4 && b2 == 0x86) { dest[j++] = 'C'; i += 2; continue; } // Ć
        if(b1 == 0xC4 && b2 == 0x99) { dest[j++] = 'e'; i += 2; continue; } // ę
        if(b1 == 0xC4 && b2 == 0x98) { dest[j++] = 'E'; i += 2; continue; } // Ę
        if(b1 == 0xC5 && b2 == 0x82) { dest[j++] = 'l'; i += 2; continue; } // ł
        if(b1 == 0xC5 && b2 == 0x81) { dest[j++] = 'L'; i += 2; continue; } // Ł
        if(b1 == 0xC5 && b2 == 0x84) { dest[j++] = 'n'; i += 2; continue; } // ń
        if(b1 == 0xC5 && b2 == 0x83) { dest[j++] = 'N'; i += 2; continue; } // Ń
        if(b1 == 0xC3 && b2 == 0xB3) { dest[j++] = 'o'; i += 2; continue; } // ó
        if(b1 == 0xC3 && b2 == 0x93) { dest[j++] = 'O'; i += 2; continue; } // Ó
        if(b1 == 0xC5 && b2 == 0x9B) { dest[j++] = 's'; i += 2; continue; } // ś
        if(b1 == 0xC5 && b2 == 0x9A) { dest[j++] = 'S'; i += 2; continue; } // Ś
        if(b1 == 0xC5 && b2 == 0xBC) { dest[j++] = 'z'; i += 2; continue; } // ż
        if(b1 == 0xC5 && b2 == 0xBB) { dest[j++] = 'Z'; i += 2; continue; } // Ż
        if(b1 == 0xC5 && b2 == 0xBA) { dest[j++] = 'z'; i += 2; continue; } // ź
        if(b1 == 0xC5 && b2 == 0xB9) { dest[j++] = 'Z'; i += 2; continue; } // Ź

        // If not matched, copy single byte (covers ASCII and other bytes)
        dest[j++] = src[i++];
    }
    dest[j] = EOS;
    return j;
}

// Helper: convert UTF-8 Polish characters to CP1250 single-byte encoding
stock Polish_Utf8ToCp1250(const src[], dest[], size = sizeof dest)
{
    new i = 0, j = 0;
    while(src[i] != EOS && j < size - 1)
    {
        new b1 = src[i] & 0xFF;
        new b2 = src[i+1] & 0xFF;

        // Map UTF-8 sequences to CP1250 bytes
        if(b1 == 0xC4 && b2 == 0x84) { dest[j++] = 0xA1; i += 2; continue; } // Ą -> 0xA1
        if(b1 == 0xC4 && b2 == 0x85) { dest[j++] = 0xB1; i += 2; continue; } // ą -> 0xB1
        if(b1 == 0xC4 && b2 == 0x86) { dest[j++] = 0xC6; i += 2; continue; } // Ć -> 0xC6
        if(b1 == 0xC4 && b2 == 0x87) { dest[j++] = 0xE6; i += 2; continue; } // ć -> 0xE6
        if(b1 == 0xC4 && b2 == 0x98) { dest[j++] = 0xCA; i += 2; continue; } // Ę -> 0xCA
        if(b1 == 0xC4 && b2 == 0x99) { dest[j++] = 0xEA; i += 2; continue; } // ę -> 0xEA
        if(b1 == 0xC5 && b2 == 0x81) { dest[j++] = 0xA3; i += 2; continue; } // Ł -> 0xA3
        if(b1 == 0xC5 && b2 == 0x82) { dest[j++] = 0xB3; i += 2; continue; } // ł -> 0xB3
        if(b1 == 0xC5 && b2 == 0x83) { dest[j++] = 0xD1; i += 2; continue; } // Ń -> 0xD1
        if(b1 == 0xC5 && b2 == 0x84) { dest[j++] = 0xF1; i += 2; continue; } // ń -> 0xF1
        if(b1 == 0xC3 && b2 == 0x93) { dest[j++] = 0xD3; i += 2; continue; } // Ó -> 0xD3
        if(b1 == 0xC3 && b2 == 0xB3) { dest[j++] = 0xF3; i += 2; continue; } // ó -> 0xF3
        if(b1 == 0xC5 && b2 == 0x9A) { dest[j++] = 0x8C; i += 2; continue; } // Ś -> 0x8C
        if(b1 == 0xC5 && b2 == 0x9B) { dest[j++] = 0x9C; i += 2; continue; } // ś -> 0x9C
        if(b1 == 0xC5 && b2 == 0xBB) { dest[j++] = 0xAF; i += 2; continue; } // Ż -> 0xAF
        if(b1 == 0xC5 && b2 == 0xBC) { dest[j++] = 0xBF; i += 2; continue; } // ż -> 0xBF
        if(b1 == 0xC5 && b2 == 0xB9) { dest[j++] = 0x8F; i += 2; continue; } // Ź -> 0x8F
        if(b1 == 0xC5 && b2 == 0xBA) { dest[j++] = 0x9F; i += 2; continue; } // ź -> 0x9F

        // Not a Polish two-byte sequence: copy single byte
        dest[j++] = src[i++];
    }
    dest[j] = EOS;
    return j;
}

// Wrapper: encode for client based on config (CP1250 preferred, fallback to ASCII translit)
stock Polish_EncodeForClient(const src[], dest[], size = sizeof dest)
{
    if(g_UseCp1250Output)
    {
        return Polish_Utf8ToCp1250(src, dest, size);
    }
    return Polish_TransliterateUtf8ToAscii(src, dest, size);
}
stock SendFormattedClientMessage(playerid, color, const format[], GLOBAL_TAG_TYPES:...)
{
    static message[256];
    static safe[256];

    // Formatowanie z argumentami
    #emit PUSH.pri
    #emit LCTRL 5
    #emit PUSH.pri
    #emit LCTRL 4
    #emit PUSH.pri
    #emit LOAD.S.pri 20  // Adres pierwszego argumentu po format
    #emit PUSH.pri
    #emit CONST.pri 256
    #emit PUSH.pri
    #emit PUSH.C message
    #emit PUSH.C 12
    #emit SYSREQ.C format
    #emit STACK 20
    #emit LCTRL 5
    #emit SCTRL 4
    #emit POP.pri

    // Proste alternatywne rozwi??zanie
    format(message, sizeof(message), format);
    Polish_EncodeForClient(message, safe, sizeof(safe));
    SendClientMessage(playerid, color, safe);
}

/**
 * Formatuje wiadomo???? i wysy??a do wszystkich graczy
 * @param color Kolor wiadomo??ci
 * @param format Format wiadomo??ci
 * @param ... Argumenty
 */
stock SendFormattedClientMessageToAll(color, const format[], GLOBAL_TAG_TYPES:...)
{
    static message[256];
    static safe[256];
    format(message, sizeof(message), format);
    Polish_EncodeForClient(message, safe, sizeof(safe));
    SendClientMessageToAll(color, safe);
}

/**
 * Wysy??a wiadomo???? GUI (dialog info)
 * @param playerid ID gracza
 * @param title Tytu??
 * @param text Tre????
 */
stock SendGuiInformation(playerid, const title[], const text[])
{
    static stitle[128], stext[512];
    Polish_EncodeForClient(title, stitle, sizeof(stitle));
    Polish_EncodeForClient(text, stext, sizeof(stext));
    ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, stitle, stext, "OK", "");
}

// ===========================================================================
// WALIDACJA I SANITYZACJA
// ===========================================================================

/**
 * Sprawdza czy string zawiera tylko dozwolone znaki nicku
 * @param string String do sprawdzenia
 * @return true je??li nick jest poprawny
 */
stock bool:IsValidNickname(const string[])
{
    new len = strlen(string);

    // Sprawd?? d??ugo????
    if(len < 3 || len > MAX_PLAYER_NAME)
    {
        return false;
    }

    // Sprawd?? format Imie_Nazwisko
    new underscoreCount = 0;
    new underscorePos = -1;

    for(new i = 0; string[i] != EOS; i++)
    {
        new c = string[i];

        // Dozwolone znaki: litery i podkre??lenie
        if(!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_'))
        {
            return false;
        }

        if(c == '_')
        {
            underscoreCount++;
            underscorePos = i;
        }
    }

    // Musi by?? dok??adnie jeden podkre??lnik, nie na pocz??tku i nie na ko??cu
    if(underscoreCount != 1 || underscorePos == 0 || underscorePos == len - 1)
    {
        return false;
    }

    // Pierwsza litera imienia i nazwiska musi by?? wielka
    if(!(string[0] >= 'A' && string[0] <= 'Z'))
    {
        return false;
    }

    if(!(string[underscorePos + 1] >= 'A' && string[underscorePos + 1] <= 'Z'))
    {
        return false;
    }

    return true;
}

/**
 * Sanityzuje string usuwaj??c niebezpieczne znaki
 * @param string String do modyfikacji
 * @return Nowa d??ugo???? stringa
 */
stock SanitizeString(string[])
{
    new j = 0;

    for(new i = 0; string[i] != EOS; i++)
    {
        new c = string[i];

        // Usu?? znaki kontrolne i niebezpieczne
        if(c >= 32 && c < 127 && c != '%' && c != '`' && c != '\\')
        {
            string[j++] = c;
        }
    }

    string[j] = EOS;
    return j;
}

/**
 * Sprawdza czy string zawiera tylko cyfry
 * @param string String do sprawdzenia
 * @return true je??li string zawiera tylko cyfry
 */
#if !defined IsNumeric
    stock bool:IsNumeric(const string[])
    {
        if(strlen(string) == 0)
        {
            return false;
        }

        for(new i = 0; string[i] != EOS; i++)
        {
            if(string[i] < '0' || string[i] > '9')
            {
                return false;
            }
        }

        return true;
    }
#endif

/**
 * Sprawdza czy string jest prawid??owym adresem IP
 * @param string String do sprawdzenia
 * @return true je??li IP jest poprawne
 */
stock bool:IsValidIP(const string[])
{
    new parts[4];

    if(sscanf(string, "p<.>dddd", parts[0], parts[1], parts[2], parts[3]))
    {
        return false;
    }

    for(new i = 0; i < 4; i++)
    {
        if(parts[i] < 0 || parts[i] > 255)
        {
            return false;
        }
    }

    return true;
}

// ===========================================================================
// FORMATOWANIE TEKSTU RP
// ===========================================================================

/**
 * Konwertuje nick SA-MP (Imie_Nazwisko) na format RP (Imi?? Nazwisko)
 * @param input Nick wej??ciowy
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 */
stock FormatRPName(const input[], output[], size = sizeof(output))
{
    strcopy(output, input, size);
    strreplace(output, '_', ' ');
}

/**
 * Pobiera imi?? z nicku RP
 * @param name Pe??ny nick
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 */
stock GetFirstName(const name[], output[], size = sizeof(output))
{
    new pos = strfind(name, "_", true);
    if(pos == -1) pos = strfind(name, " ", true);

    if(pos == -1)
    {
        strcopy(output, name, size);
    }
    else
    {
        strmid(output, name, 0, pos, size);
    }
}

/**
 * Pobiera nazwisko z nicku RP
 * @param name Pe??ny nick
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 */
stock GetLastName(const name[], output[], size = sizeof(output))
{
    new pos = strfind(name, "_", true);
    if(pos == -1) pos = strfind(name, " ", true);

    if(pos == -1)
    {
        output[0] = EOS;
    }
    else
    {
        strcopy(output, name[pos + 1], size);
    }
}

// ===========================================================================
// FORMATOWANIE LICZB
// ===========================================================================

/**
 * Formatuje liczb?? z separatorem tysi??cy
 * @param value Warto????
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 */
stock FormatNumber(value, output[], size = sizeof(output))
{
    new temp[16], len;
    format(temp, sizeof(temp), "%d", value);
    len = strlen(temp);

    new j = 0;
    new offset = len % 3;
    if(offset == 0) offset = 3;

    for(new i = 0; i < len; i++)
    {
        if(i > 0 && (i % 3) == offset)
        {
            if(j < size - 1)
            {
                output[j++] = ',';
            }
        }

        if(j < size - 1)
        {
            output[j++] = temp[i];
        }
    }

    output[j] = EOS;
}

/**
 * Formatuje kwot?? pieni??dzy
 * @param value Warto????
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 */
stock FormatMoney(value, output[], size = sizeof(output))
{
    new temp[24];
    FormatNumber(value, temp, sizeof(temp));
    format(output, size, "$%s", temp);
}

// ===========================================================================
// EOF
// ===========================================================================

