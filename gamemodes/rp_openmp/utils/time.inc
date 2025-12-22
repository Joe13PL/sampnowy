/*
 * =============================================================================
 *  TIME UTILITIES - Funkcje obs??ugi czasu
 * =============================================================================
 *  
 *  Modu??: utils/time.inc
 *  Opis: Funkcje formatowania i obs??ugi czasu
 *
 *  ZMIANY W REFAKTORZE:
 *  - Zast??piono zewn??trzn?? bibliotek?? timestamptodate
 *  - Dodano natywne funkcje konwersji czasu
 *  - Poprawiono obliczenia stref czasowych
 *
 * =============================================================================
 */

#if defined _time_utils_included
    #endinput
#endif
#define _time_utils_included

// ===========================================================================
// STA??E CZASOWE
// ===========================================================================

#define SECONDS_PER_MINUTE      60
#define SECONDS_PER_HOUR        3600
#define SECONDS_PER_DAY         86400
#define SECONDS_PER_WEEK        604800

// Strefa czasowa (UTC+1 dla Polski)
#define TIMEZONE_OFFSET         1

// ===========================================================================
// KONWERSJA TIMESTAMP NA DAT??
// ===========================================================================

/**
 * Konwertuje timestamp na komponenty daty i czasu
 * @param timestamp Timestamp Unix
 * @param year, month, day, hour, minute, second Zmienne wyj??ciowe
 * @param timezone Offset strefy czasowej (domy??lnie +1)
 */
stock TimestampToDate(timestamp, &year, &month, &day, &hour, &minute, &second, timezone = TIMEZONE_OFFSET)
{
    // Dostosuj o stref?? czasow??
    timestamp += timezone * SECONDS_PER_HOUR;
    
    // Oblicz sekundy, minuty, godziny
    second = timestamp % 60;
    timestamp /= 60;
    
    minute = timestamp % 60;
    timestamp /= 60;
    
    hour = timestamp % 24;
    timestamp /= 24;
    
    // Teraz timestamp to liczba dni od 1 stycznia 1970
    new days = timestamp;
    
    // Oblicz rok
    year = 1970;
    
    new daysInYear;
    while(true)
    {
        daysInYear = IsLeapYear(year) ? 366 : 365;
        if(days < daysInYear) break;
        days -= daysInYear;
        year++;
    }
    
    // Oblicz miesi??c
    new daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if(IsLeapYear(year)) daysInMonth[1] = 29;
    
    month = 1;
    while(days >= daysInMonth[month - 1])
    {
        days -= daysInMonth[month - 1];
        month++;
    }
    
    day = days + 1;
}

/**
 * Sprawdza czy rok jest przest??pny
 * @param year Rok
 * @return true je??li rok jest przest??pny
 */
stock bool:IsLeapYear(year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// ===========================================================================
// FORMATOWANIE DATY I CZASU
// ===========================================================================

/**
 * Formatuje timestamp na czyteln?? dat??
 * @param timestamp Timestamp Unix
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 */
stock GetFormattedDate(timestamp, output[], size = sizeof(output))
{
    new year, month, day, hour, minute, second;
    TimestampToDate(timestamp, year, month, day, hour, minute, second);
    
    format(output, size, "%02d.%02d.%d %02d:%02d:%02d", day, month, year, hour, minute, second);
}

/**
 * Formatuje timestamp na kr??tk?? dat?? (bez czasu)
 * @param timestamp Timestamp Unix
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 */
stock GetShortDate(timestamp, output[], size = sizeof(output))
{
    new year, month, day, hour, minute, second;
    TimestampToDate(timestamp, year, month, day, hour, minute, second);
    
    format(output, size, "%02d.%02d.%d", day, month, year);
}

/**
 * Formatuje timestamp na sam czas
 * @param timestamp Timestamp Unix
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 */
stock GetFormattedTime(timestamp, output[], size = sizeof(output))
{
    new year, month, day, hour, minute, second;
    TimestampToDate(timestamp, year, month, day, hour, minute, second);
    
    format(output, size, "%02d:%02d:%02d", hour, minute, second);
}

/**
 * Formatuje czas trwania (sekundy) na czytelny format
 * @param seconds Liczba sekund
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 * @param showSeconds Czy pokazywa?? sekundy
 */
stock FormatDuration(seconds, output[], size = sizeof(output), bool:showSeconds = true)
{
    new days = seconds / SECONDS_PER_DAY;
    seconds %= SECONDS_PER_DAY;
    
    new hours = seconds / SECONDS_PER_HOUR;
    seconds %= SECONDS_PER_HOUR;
    
    new minutes = seconds / SECONDS_PER_MINUTE;
    seconds %= SECONDS_PER_MINUTE;
    
    if(days > 0)
    {
        if(showSeconds)
        {
            format(output, size, "%dd %dh %dm %ds", days, hours, minutes, seconds);
        }
        else
        {
            format(output, size, "%dd %dh %dm", days, hours, minutes);
        }
    }
    else if(hours > 0)
    {
        if(showSeconds)
        {
            format(output, size, "%dh %dm %ds", hours, minutes, seconds);
        }
        else
        {
            format(output, size, "%dh %dm", hours, minutes);
        }
    }
    else if(minutes > 0)
    {
        if(showSeconds)
        {
            format(output, size, "%dm %ds", minutes, seconds);
        }
        else
        {
            format(output, size, "%dm", minutes);
        }
    }
    else
    {
        format(output, size, "%ds", seconds);
    }

    return 1;
}

// ===========================================================================
// POMOCNICZE
// ===========================================================================

stock GetTimestamp()
{
    return gettime();
}

/**
 * Formatuje czas gry (godziny i minuty)
 * @param hours Godziny
 * @param minutes Minuty
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 */
stock FormatPlaytime(hours, minutes, output[], size = sizeof(output))
{
    format(output, size, "%dh %dm", hours, minutes);
}

// ===========================================================================
// DZIE?? TYGODNIA
// ===========================================================================

/**
 * Pobiera numer dnia tygodnia dla podanej daty
 * @param day Dzie??
 * @param month Miesi??c
 * @param year Rok
 * @return Numer dnia (1=Niedziela, 2=Poniedzia??ek, ..., 7=Sobota) lub 0 przy b????dzie
 */
stock GetDayOfWeek(day = 0, month = 0, year = 0)
{
    // Je??li nie podano parametr??w, u??yj aktualnej daty
    if(day == 0 && month == 0 && year == 0)
    {
        getdate(year, month, day);
    }
    
    // Liczba dni w miesi??cach
    new daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    
    // Poprawka dla roku przest??pnego
    if(IsLeapYear(year))
    {
        daysInMonth[1] = 29;
    }
    
    // Walidacja daty
    if(year < 1900 || month < 1 || month > 12 || day < 1 || day > daysInMonth[month - 1])
    {
        return 0;
    }
    
    // Oblicz liczb?? dni od pocz??tku epoki
    new totalDays = day;
    
    for(new y = 1900; y < year; y++)
    {
        totalDays += IsLeapYear(y) ? 366 : 365;
    }
    
    for(new m = 0; m < month - 1; m++)
    {
        totalDays += daysInMonth[m];
    }
    
    // 1 stycznia 1900 to poniedzia??ek (dzie?? 2)
    return (totalDays % 7) + 1;
}

/**
 * Pobiera nazw?? dnia tygodnia
 * @param dayNum Numer dnia (1-7)
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 */
stock GetDayName(dayNum, output[], size = sizeof(output))
{
    switch(dayNum)
    {
        case 1: strcopy(output, "Niedziela", size);
        case 2: strcopy(output, "Poniedzia??ek", size);
        case 3: strcopy(output, "Wtorek", size);
        case 4: strcopy(output, "??roda", size);
        case 5: strcopy(output, "Czwartek", size);
        case 6: strcopy(output, "Pi??tek", size);
        case 7: strcopy(output, "Sobota", size);
        default: strcopy(output, "Nieznany", size);
    }
}

/**
 * Pobiera nazw?? miesi??ca
 * @param month Numer miesi??ca (1-12)
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 */
stock GetMonthName(month, output[], size = sizeof(output))
{
    switch(month)
    {
        case 1: strcopy(output, "Stycze??", size);
        case 2: strcopy(output, "Luty", size);
        case 3: strcopy(output, "Marzec", size);
        case 4: strcopy(output, "Kwiecie??", size);
        case 5: strcopy(output, "Maj", size);
        case 6: strcopy(output, "Czerwiec", size);
        case 7: strcopy(output, "Lipiec", size);
        case 8: strcopy(output, "Sierpie??", size);
        case 9: strcopy(output, "Wrzesie??", size);
        case 10: strcopy(output, "Pa??dziernik", size);
        case 11: strcopy(output, "Listopad", size);
        case 12: strcopy(output, "Grudzie??", size);
        default: strcopy(output, "Nieznany", size);
    }
}

// ===========================================================================
// POR??WNYWANIE CZASU
// ===========================================================================

/**
 * Sprawdza czy timestamp jest starszy ni?? podana liczba sekund
 * @param timestamp Timestamp do sprawdzenia
 * @param seconds Liczba sekund
 * @return true je??li timestamp jest starszy
 */
stock bool:IsOlderThan(timestamp, seconds)
{
    return (gettime() - timestamp) > seconds;
}

/**
 * Sprawdza czy timestamp jest nowszy ni?? podana liczba sekund
 * @param timestamp Timestamp do sprawdzenia
 * @param seconds Liczba sekund
 * @return true je??li timestamp jest nowszy
 */
stock bool:IsNewerThan(timestamp, seconds)
{
    return (gettime() - timestamp) < seconds;
}

/**
 * Oblicza ile sekund min????o od timestamp
 * @param timestamp Timestamp
 * @return Liczba sekund
 */
stock GetSecondsSince(timestamp)
{
    return gettime() - timestamp;
}

/**
 * Oblicza ile sekund pozosta??o do timestamp
 * @param timestamp Timestamp
 * @return Liczba sekund (ujemna je??li timestamp w przesz??o??ci)
 */
stock GetSecondsUntil(timestamp)
{
    return timestamp - gettime();
}

// ===========================================================================
// EOF
// ===========================================================================

