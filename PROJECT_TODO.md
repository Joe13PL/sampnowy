# Project TODOs — RP Gamemode (snapshot: 2025-12-24)

## Current status ✅
- Local compilation: **succeeds** (Pawn compiler 3.10.11). ✅
- Recent fixes implemented and tested: armour persistence on disconnect/login, HUD clock reliable minute updates, phone/contacts fixes, admin GUI addition, `/forceclock` debug command. ✅
- No obvious missing scripts detected (all includes resolved during compilation). If there are runtime-only missing resources, we should add runtime checks and logs. ✅

## Known TODOs & code markers (scan results)
- Several inline `TODO` / `FIXME` markers remain in codebase (examples):
  - `gamemodes/rp_openmp/gameplay/items.inc` — TODO: display dialog with data
  - `gamemodes/rp/timers.inc` — todo: delete businesses for unpaid taxes
  - `gamemodes/rp/vehicles.inc` — TODO: support alarm
- See `gamemodes/todo.md` for broader list and higher-level priorities.

## Suggested immediate tasks (priority)
- [ ] Remove temporary debug prints added during recent debugging (HUD logs, PlayerSave logs, minute timers) — reduce log noise.
- [ ] Add a `DEBUG` or `LOG_LEVEL` flag to gate verbose logs (keep them available but off by default).
- [ ] Convert `/forceclock` to an admin-only testing command or keep it behind a debug flag.
- [ ] Run a full runtime testing pass: connect, spawn, change armour, logout/login, verify DB and in-game state across multiple accounts/characters.


---

## Quick commands & checks
- Compile locally with qawno pawncc (example):
  `& "C:\Users\qsasu\Desktop\asdsaas\sampnowy\qawno\pawncc.exe" "rp_openmp\main.pwn" -i"C:\Users\qsasu\Desktop\asdsaas\sampnowy\qawno\include" -o"main.amx"`

- Search for `TODO` markers: `rg "TODO|FIXME" gamemodes -n`

---

If you want, I can:
- Remove the debug prints now (quick change).
- Open a cleaned `gamemodes/todo.md` or replace/create `PROJECT_TODO.md` in another format (done).
- Start a GitHub Actions CI job template and add it to the repo.

Which of the above should I do next?