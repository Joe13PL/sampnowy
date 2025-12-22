# RP Gamemode - TODO List

## ✅ Zrobione (Sesja 15.12.2025)

### eSelection - System wyboru modeli
- [x] Refaktor eSelection na PlayerTextDraw (per-player)
- [x] Menu jest teraz niszczone przy ukrywaniu (nie tylko ukrywane)
- [x] Rozwiązano problem z "czarnym prostokątem" który zostawał po zamknięciu menu
- [x] Działa poprawnie przy rejestracji i po ponownym zalogowaniu

### System autoryzacji
- [x] Uproszczenie Auth_SetGender - wybór skina PRZED spawnem gracza
- [x] Gracz pozostaje w spectating podczas wyboru skina
- [x] Po wyborze skina → zapis do bazy → spawn gracza
- [x] LOGIN_TIMEOUT zwiększony do 180 sekund (3 minuty)

### Komendy pojazdów
- [x] Naprawa `/v okno [1-4]` - poprawione parsowanie parametrów sscanf
- [x] Komenda `/veh` działa poprawnie z nowym eSelection

### Testy
- [x] Test wyboru skina przy rejestracji nowej postaci
- [x] Test czy czarny prostokąt znika po ponownym zalogowaniu
- [x] Test komendy `/veh` (wybór pojazdu dla adminów)
- [x] Test komendy `/skins` (wybór skinów dla adminów)

### Dodatkowe naprawy
- [x] Naprawa zapisu `groups` (dodanie rang do INSERT/UPDATE)
- [x] Dodanie migracji DB dla brakujących kolumn `doors`
- [x] Lokalna kompilacja gamemode przy użyciu `qawno/pawncc` (potwierdzona)
 - [x] Dodanie migracji DB: `phone_calls` (historia połączeń)
 - [x] Logowanie połączeń do DB (ring/start/answer/end/result)
 - [x] GUI telefonu: `Kontakty` + `Historia połączeń` dialogi i komenda `/phone`

---



### Możliwe ulepszenia
- [ ] Filtrowanie skinów w menu wyboru (osobne listy dla mężczyzn/kobiet)

### Priorytety (najpierw)
- [ ] Dodać CI (GitHub Actions) - automatyczna kompilacja (`pawncc`) i sanity checks
- [ ] Dodać testy integracyjne (tworzenie grupy/drzwi + weryfikacja DB)
- [ ] Przejrzeć i ukończyć wszystkie TODO w kodzie (np. telefon w `items.inc`, friends count)
- [ ] Wykonać pełny audit schematu DB vs używanych kolumn i dodać brakujące migracje

### Mniej pilne
- [ ] Dodać prosty CI DB (uruchamia migracje/testy DB w kontenerze)
- [ ] Dodać dokumentację uruchamiania migracji i kompilatora w README

### Komendy do szybkiego użycia (lokalnie)
Kompilacja z `qawno/pawncc` (przykład):
```powershell
& "C:\Users\qsasu\Desktop\asdsaas\sampnowy\qawno\pawncc.exe" "rp_openmp\main.pwn" -i"C:\Users\qsasu\Desktop\asdsaas\sampnowy\qawno\include" -o"main.amx"
```



### Inne zadania
- [ ] ...

---

## 📝 Notatki techniczne

### eSelection - Architektura
```
Stara wersja:
- Globalne TextDraw (Text:) tworzone raz w OnGameModeInit
- TextDrawHideForPlayer() nie działało poprawnie w niektórych stanach

Nowa wersja:
- PlayerTextDraw per-player (PlayerText:)
- Textdrawy tworzone przy ShowModelSelectionMenu()
- Textdrawy niszczone przy HideModelSelectionMenu()
- Gwarantuje całkowite zniknięcie menu
```

### Przepływ rejestracji
```
1. Gracz wpisuje nick → sprawdzenie konta
2. Brak konta → dialog rejestracji hasła
3. Potwierdzenie hasła → utworzenie konta
4. Dialog nazwy postaci (Imie_Nazwisko)
5. Dialog wyboru płci (Mężczyzna/Kobieta)
6. Menu wyboru skina (eSelection) - gracz w spectating
7. Wybór skina → zapis postaci do bazy
8. Auth_CompleteLogin → Player_Spawn → gracz na mapie
```
