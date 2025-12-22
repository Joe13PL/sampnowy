# OpenRP Roleplay - open.mp Refactored Version

## Opis

Pełny refaktor oryginalnego serwera SA-MP RP dla 100% kompatybilności z open.mp.

### Wersja: 2.0

## Zmiany w refaktorze

### Główne zmiany

1. **Pełna kompatybilność z open.mp**
   - Zastąpiono `a_samp` na `open.mp` includes
   - Zaktualizowano wszystkie natywne funkcje do standardów open.mp

2. **Aktualizacja bibliotek**
   - MySQL R41+ z prepared statements i async queries
   - YSI 5.x (y_iterate, y_timers, y_hooks, y_commands)
   - Streamer Plugin (najnowsza wersja)
   - Progress2 zamiast starego Progress

3. **Bezpieczeństwo**
   - Zamieniono MD5 na SHA256/bcrypt do haszowania haseł
   - Walidacja wszystkich danych wejściowych
   - Sanityzacja stringów SQL (prepared statements)
   - System anti-flood na komendy i czat

4. **Architektura**
   - Modularna struktura z folderami: core/, player/, database/, gameplay/, admin/, ui/, utils/
   - System hooków (y_hooks) zamiast duplikowania callbacków
   - Iteratory zamiast standardowych pętli
   - Cacheowanie danych gracza

5. **Usunięte/Zamienione zależności**
   - ❌ dini → ✅ Natywny INI parser
   - ❌ md5 → ✅ SHA256/bcrypt
   - ❌ zcmd → ✅ y_commands (YCMD)
   - ❌ stary MySQL → ✅ MySQL R41+
   - ❌ progress → ✅ progress2
   - ❌ kickfix → ✅ Wbudowany delay kick

## Struktura projektu

```
rp_openmp/
├── main.pwn              # Główny plik
├── core/                 # Moduły podstawowe
│   ├── defines.inc       # Stałe i makra
│   ├── colors.inc        # Definicje kolorów
│   ├── config.inc        # System konfiguracji
│   ├── enums.inc         # Enumeratory
│   ├── variables.inc     # Zmienne globalne
│   └── timers.inc        # System timerów
├── database/             # Obsługa bazy danych
│   ├── mysql_connection.inc  # Połączenie MySQL
│   └── schema.sql        # Schemat bazy danych
├── utils/                # Funkcje pomocnicze
│   ├── math.inc          # Funkcje matematyczne
│   ├── strings.inc       # Operacje na stringach
│   ├── time.inc          # Funkcje czasowe
│   └── security.inc      # Bezpieczeństwo
├── player/               # System gracza
│   ├── player_data.inc   # Dane gracza
│   ├── player_auth.inc   # Autentykacja
│   ├── player_save.inc   # Zapis/Odczyt
│   ├── player_spawn.inc  # Spawn
│   └── player_commands.inc   # Komendy gracza
├── gameplay/             # Systemy rozgrywki
│   ├── vehicles.inc      # Pojazdy
│   ├── groups.inc        # Grupy/Frakcje
│   ├── doors.inc         # Drzwi/Budynki
│   ├── items.inc         # Przedmioty
│   ├── areas.inc         # Strefy
│   ├── offers.inc        # Oferty
│   ├── works.inc         # Prace dorywcze
│   └── penalties.inc     # Kary/Więzienie
├── admin/                # Administracja
│   └── admin_commands.inc    # Komendy admina
├── ui/                   # Interfejs użytkownika
│   ├── textdraws.inc     # System HUD
│   ├── dialogs.inc       # System dialogów
│   └── dialog_handlers.inc   # Handlery dialogów
└── scriptfiles/          # Pliki konfiguracyjne
    └── config.ini        # Główna konfiguracja
```

## Wymagania

### Serwer
- **open.mp server** (najnowsza wersja)
- Alternatywnie: SA-MP 0.3.7 server (z ograniczoną funkcjonalnością)

### Pluginy
- **mysql-plugin** R41+ (BlueG)
- **streamer** 2.9.5+
- **sscanf** 2.13+
- **bcrypt** (opcjonalnie, fallback do SHA256)
- **crashdetect** (opcjonalnie, dla debugowania)

### Biblioteki (includes)
- **YSI 5.x** (y_iterate, y_timers, y_hooks, y_commands)
- **progress2** 2.1+

## Instalacja

### 1. Przygotowanie serwera

```bash
# Pobierz open.mp server
# https://open.mp/

# Skopiuj pliki gamemode
cp -r rp_openmp/ /ścieżka/do/serwera/gamemodes/
```

### 2. Instalacja pluginów

Pobierz i umieść w folderze `plugins/`:
- mysql.dll/.so
- streamer.dll/.so
- sscanf.dll/.so
- bcrypt.dll/.so (opcjonalnie)

### 3. Konfiguracja server.cfg

```ini
gamemode0 rp_openmp

plugins mysql streamer sscanf bcrypt

maxplayers 500
rcon_password TWOJE_HASLO
```

### 4. Baza danych

```bash
# Utwórz bazę danych
mysql -u root -p -e "CREATE DATABASE OpenRP_rp CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;"

# Zaimportuj schemat
mysql -u root -p OpenRP_rp < rp_openmp/database/schema.sql
```

### 5. Konfiguracja

Edytuj `scriptfiles/config.ini`:
```ini
[mysql]
host = 127.0.0.1
user = twoj_user
password = twoje_haslo
database = OpenRP_rp
```

### 6. Kompilacja

```bash
# Z kompilatorem open.mp
./omp-compiler gamemodes/rp_openmp/main.pwn -o gamemodes/rp_openmp.amx

# Lub z pawncc
pawncc gamemodes/rp_openmp/main.pwn -o gamemodes/rp_openmp.amx -i includes/
```

### 7. Uruchomienie

```bash
./omp-server
# lub
./samp-server
```

## Komendy

### Komendy gracza
| Komenda | Opis |
|---------|------|
| `/me [akcja]` | Opisuje akcję postaci |
| `/do [opis]` | Opisuje otoczenie |
| `/s [tekst]` | Krzyk (większy zasięg) |
| `/w [tekst]` | Szept (mniejszy zasięg) |
| `/b [tekst]` | Rozmowa OOC lokalna |
| `/pm [id] [tekst]` | Prywatna wiadomość |
| `/stats` | Statystyki |
| `/inv` | Ekwipunek |
| `/grupy` | Lista grup |
| `/pojazdy` | Lista pojazdów |
| `/v lock` | Zamknij/otwórz pojazd |
| `/v engine` | Włącz/wyłącz silnik |

### Komendy administracyjne
| Komenda | Poziom | Opis |
|---------|--------|------|
| `/a [tekst]` | 1+ | Czat administracyjny |
| `/aduty` | 1+ | Tryb służbowy admina |
| `/goto [id]` | 2+ | Teleport do gracza |
| `/gethere [id]` | 2+ | Przywołaj gracza |
| `/kick [id] [powód]` | 2+ | Wyrzuć gracza |
| `/ban [id] [czas] [powód]` | 3+ | Zbanuj gracza |
| `/freeze [id]` | 2+ | Zamroź gracza |
| `/spec [id]` | 2+ | Obserwuj gracza |
| `/givemoney [id] [kwota]` | 4+ | Daj pieniądze |
| `/giveweapon [id] [broń] [amunicja]` | 4+ | Daj broń |

## API

### Player Data
```pawn
// Pobierz nazwę RP gracza
stock Player_GetRPName(playerid);

// Daj/zabierz pieniądze
stock Player_GiveMoney(playerid, amount);
stock Player_GetMoney(playerid);

// Zdrowie
stock Player_SetHealth(playerid, Float:health);
stock Float:Player_GetHealth(playerid);
```

### Vehicles
```pawn
// Utwórz pojazd
stock Vehicle_Create(modelid, Float:x, Float:y, Float:z, Float:angle, color1, color2, ownerid);

// Paliwo
stock Vehicle_GetFuel(vehicleid);
stock Vehicle_SetFuel(vehicleid, fuel);
```

### Groups
```pawn
// Dodaj/usuń członka
stock Group_AddMember(groupid, playerid, rank);
stock Group_RemoveMember(groupid, playerid);

// Sprawdź przynależność
stock bool:Player_IsInGroup(playerid, groupid);
```

## Licencja

Ten projekt jest przeznaczony wyłącznie do użytku edukacyjnego.
Oryginalny kod: Raydex
Refaktor: JoePL

## Kontakt

- Website: 
- Discord: 

## Changelog

### v2.0 (2026)
- Pełny refaktor dla open.mp
- Modularna architektura
- Aktualizacja wszystkich bibliotek
- Nowy system bezpieczeństwa
- Prepared statements SQL
- System hooków

### v1.x (Oryginał)
- Pierwsza wersja serwera RP
- Podstawowe systemy rozgrywki
