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
stock SendFormattedClientMessage(playerid, color, const format[], GLOBAL_TAG_TYPES:...)
{
    static message[256];
    
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
    
    SendClientMessage(playerid, color, message);
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
    format(message, sizeof(message), format);
    SendClientMessageToAll(color, message);
}

/**
 * Wysy??a wiadomo???? GUI (dialog info)
 * @param playerid ID gracza
 * @param title Tytu??
 * @param text Tre????
 */
stock SendGuiInformation(playerid, const title[], const text[])
{
    ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, title, text, "OK", "");
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

