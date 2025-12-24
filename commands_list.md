# Commands index (rp_openmp) ✅

Generated: 2025-12-23

This is a compact, navigable index of YCMD command handlers in the gamemode. Commands are grouped by area and include aliases where known. If you want, I can expand this with per-command usage and the first-help string.

---

## Player commands (file: `gamemodes/rp_openmp/player/player_commands.inc`) 🎮

| Command | Notes |
|---|---|
| `cmdtest` | debug command |
| `phone` | opens phone menu |
| `zadzwon` / `call` | quick call |
| `sms` | send SMS |
| `me` | roleplay emote |
| `opis` | short description/set description |
| `s`, `w`, `k`, `sz`, `l` | shorthand chat/comm cmds |
| `kill` | kill command |
| `b` | broadcast/announcement alias |
| `pm` | private message |
| `re` | reply private message |
| `stats` | show stats |
| `hangup` | end a phone call |
| `id` | show player id |
| `time` | show server time |
| `pos` | show position |
| `tpm` | teleport to active server marker/checkpoint (see notes) |
| `setmarkerhere` | ADMIN: set server marker at your position |
| `clearmarker` | ADMIN: clear your server marker |
| `tpc` | teleport to coordinates |
| `skins` | skin preview/change |
| `p` | inventory shortcut (alias for `/inv` and `/eq`) |
| `grupy` | groups list |
| `pojazdy` | vehicles list |
| `v` | misc vehicle/short cmd |
| `tempomat` | cruise control |
| `wejdz` / `wyjdz` | enter/exit commands |
| `help` / `pomoc` | help menu |


## Admin commands (file: `gamemodes/rp_openmp/admin/admin_commands.inc`) 🛠️

> Admin-only commands require appropriate admin level. See `ADMIN_CHECK(...)` usage in the file.

| Command | Notes |
|---|---|
| `myadmin` | admin info |
| `phone_test` | phone diagnostics |
| `ahelp` / `a` | admin help |
| `aduty` | toggle admin duty |
| `kick` | kick player |
| `ban` | ban player |
| `goto` / `gethere` | teleport admin to player or vice versa |
| `spec` | spectate a player |
| `freeze` | freeze player |
| `announce` | server announcement |
| `giveweapon` | grant weapon |
| `givemoney` | grant money |
| `sethealth` (alias: `sethp`) | set player health |
| `setskin` | set player skin |
| `veh` | create vehicle |
| `destroyveh`, `fixveh`, `flipveh` | vehicle utilities |
| `gotoxyz`, `setint`, `setvw` | teleport/interior/vw helpers |
| `tpall` | teleport all players |
| `slap` | damage player briefly |
| `mute` / `unmute` | chat moderation |
| `jail` / `unjail` | jail management |
| `warn`, `clearwarns` | player warnings |
| `check` / `admins` / `players` | info queries |
| `setarmour` / `setmoney` | set armour/money |
| `clearchat` | clear chat |
| `disarm` | remove weapons |
| `revive` / `akill` | revive or admin-kill |
| `explode` | cause explosion |
| `setadmin` / `unban` | admin management |
| `settime` / `setweather` | environment control |
| `invisible` / `god` / `fly` | player state cheats |
| `acmds`, `agroups`, `ag`, `ap`, `av`, `adoors`, `ad` | group/admin helpers |


## Other in-game commands (from modules) 📦

- `gameplay/offers.inc`: `akceptuj`, `odrzuc`, `podajreke` (accept/decline/handshake)
- `player/player_stranger.inc`: `poznaj`, `zapomnij` (stranger interactions)
- `gameplay/works.inc`: `praca` (work/job)


---

## Notes & tips 💡
- `/tpm` teleports to a *server-side* marker/checkpoint (not the client map waypoint). Admins can place a server marker with `/setmarkerhere` or click on the map (right-click) to teleport directly if they have admin permission.
- If you want a richer index (usage strings, help text and the file+line location for each command), I can parse the handlers and add those fields.

---

