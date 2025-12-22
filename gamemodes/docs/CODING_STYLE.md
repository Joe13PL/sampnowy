# Coding Style Guidelines (AI-Friendly)

These rules are mandatory for all contributors **and AI agents** generating code for this project. The goal is:

* predictability
* refactor safety
* minimal side effects
* easy static analysis by AI tools

---

## 1. General Principles

1. **One responsibility per file**
2. **One system = one folder**
3. **No hidden side effects** (no logic in includes)
4. **Explicit over implicit**
5. **Readable > clever**

AI-generated code must follow the same standards as human-written code.

---

## 2. File Rules

### 2.1 File Naming

* lowercase
* words separated by underscore

Examples:

* `char_data.inc`
* `phone_calls.inc`

### 2.2 Include Guards

Every `.inc` file **must** have a guard:

```pawn
#if defined _CHAR_DATA_INC
    #endinput
#endif
#define _CHAR_DATA_INC
```

---

## 3. Folder Responsibilities

| Folder      | Responsibility               |
| ----------- | ---------------------------- |
| `core`      | bootstrap, config, constants |
| `account`   | authentication, accounts     |
| `character` | character state              |
| `systems/*` | isolated gameplay systems    |
| `utils`     | stateless helpers            |

AI must never mix responsibilities across folders.

---

## 4. Naming Conventions

### 4.1 Functions

Pattern:

```
<System>_<Action><Object>()
```

Examples:

```pawn
Phone_SendSMS()
Economy_AddMoney()
Faction_IsOnDuty()
```

Rules:

* verbs first
* no abbreviations unless universal
* no overloaded meanings

---

### 4.2 Variables

#### Global

```pawn
g_PlayerData[MAX_PLAYERS][E_CHAR_DATA];
```

Prefix: `g_`

#### Local

```pawn
new hungerLevel;
```

#### Enums

```pawn
enum E_CHAR_DATA
{
    cID,
    cName[24]
};
```

Prefix: `E_`

---

## 5. Enums Over Arrays

❌ Forbidden:

```pawn
new PlayerMoney[MAX_PLAYERS];
```

✅ Required:

```pawn
enum E_PLAYER_DATA
{
    pMoney,
    pLevel
};
new g_PlayerData[MAX_PLAYERS][E_PLAYER_DATA];
```

Reason: AI and humans can reason about data meaning.

---

## 6. API Boundaries

Each system must expose **only API functions** in `*_api.inc`.

Example:

```pawn
// phone_api.inc
stock Phone_SendSMS(senderCharId, number, const msg[]);
```

❌ Direct access to another system's data is forbidden.

---

## 7. Event Handling

### 7.1 Hooks Only

❌ Forbidden:

```pawn
public OnPlayerSpawn(playerid)
```

✅ Required:

```pawn
hook OnPlayerSpawn(playerid)
```

Reason: avoids collisions and enables modularity.

---

## 8. Timers

❌ Forbidden:

* `SetTimer`
* `SetTimerEx`

✅ Required:

* `y_timers`
* global tick loops

Reason: predictable execution flow.

---

## 9. Database Access

Rules:

* async only
* no SQL inside gameplay callbacks
* all queries via `db.inc`

❌ Forbidden:

```pawn
mysql_query(...);
```

✅ Required:

```pawn
DB_Exec("UPDATE characters SET money = ?", value);
```

---

## 10. Comments and Annotations

### 10.1 System Header

Every system file must start with:

```pawn
/// @system Phone
/// @desc Handles SMS and calls
/// @state synced
```

### 10.2 Function Comments

```pawn
/// Sends an SMS to a phone number
/// @param senderCharId Character ID
/// @param number Target phone number
```

AI should always generate these comments.

---

## 11. Control Flow

Rules:

* no deeply nested logic
* early returns preferred

❌ Bad:

```pawn
if (a)
{
    if (b)
    {
        if (c)
        {
        }
    }
}
```

✅ Good:

```pawn
if (!a) return 0;
if (!b) return 0;
if (!c) return 0;
```

---

## 12. Error Handling

* always validate input
* return explicit result codes

Example:

```pawn
stock bool:Phone_IsEnabled(charid)
{
    if (!Character_IsValid(charid)) return false;
    return g_CharData[charid][cPhoneEnabled];
}
```

---

## 13. Forbidden Patterns

AI must never generate:

* magic numbers
* logic in `main.pwn`
* cross-system includes
* global state without prefix
* duplicated systems

---

## 14. AI-Specific Rules

When generating code, the AI must:

1. Identify target system
2. Use existing enums and APIs
3. Never invent new globals without approval
4. Respect file responsibility

If unsure, AI must stop and ask.

---

## 15. Summary (for AI)

* predictable structure
* explicit naming
* enums everywhere
* hooks only
* APIs only
* no shortcuts

This style is mandatory for long-term RP projects.
