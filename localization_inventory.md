Localization inventory (generated: 2025-12-23)

Purpose: quick mapping of user-facing strings to help start English localization. Each entry lists the file and representative string(s). This is not exhaustive but covers primary UX areas.

## Contents
- Phone (calls, contacts, SMS)
- Dialogs and menus
- Inventory / Items
- Admin commands & help
- HUD / TextDraws
- Offers & trading
- Errors & generic messages

---

## Phone (gameplay/phone_system.inc)
- "Invalid phone number."
- "You do not own a phone."
- "Your phone has no number assigned."
- "Subscriber temporarily unavailable."
- "Looking up subscriber; please try again in a moment."
- "That phone number does not exist."
- "Call ended."
- "Phone devices backfill triggered."
- "Phone duplicate scan started. This may take a moment."
- "No phone items found."
- ShowPlayerDialog(..., "Incoming call", q, "Answer", "Reject")
- Dialog titles: "Contacts", "SMS", "Call History", "Settings"

## Phone UI & dialogs (ui/dialog_handlers.inc)
- "Add contact" / Input prompt: "Enter the phone number you want to add as a contact:" (DIALOG_PHONE_CONTACT_ADD)
- "Add contact - name" / Input prompt: "Enter a display name for the contact:" (DIALOG_PHONE_CONTACT_EDIT)
- "Contact" (view dialog) and buttons: "Back", "Close"
- Contact options: "Call\nView\nEdit name\nDelete"
- SMS prompts: "Nowy SMS - odbiorca (numer)" / "Wpisz numer odbiorcy:" (note: still Polish)
- Error messages seen: "Name cannot be empty.", "You do not have a phone with you.", "Enter a phone number.", "Enter a valid phone number.", "Nieprawidlowy numer telefonu.", "Blad wewnetrzny." (internal error)

## Dialog helpers (ui/dialogs.inc)
- Standard button labels: "Select", "Cancel", "Confirm", "OK", "Yes", "No"

## Inventory / Item dialogs (ui/dialog_handlers.inc, player/player_commands.inc)
- Inventory open: `/inv`, `/eq`, `/p` (alias)
- Dialog titles: "Inventory", "Item options", "Use item", "Give item"
- Example messages: "Invalid item slot.", "You have dropped the item." (Polish: "Wyrzuci??e?? przedmiot.")

## Admin commands & help (admin/admin_commands.inc)
- Usage strings: e.g. "Usage: /sethealth [ID/Nick] [health]", "Usage: /kick [ID/Nick] [reason]" etc.
- Help messages: "Sets the health of a player.", "Sets the skin of a player.", "Player is not connected!"
- Moderation broadcast snippets: "[Moderator] /sethp /setarmour /disarm"

## HUD / TextDraws (ui/textdraws.inc)
- "HP: 100" (health HUD)
- "Glod: 100%" (hunger)
- "Pragnienie: 100%" (thirst)
- Injured text: "~r~JESTES RANNY~n~~w~Poczekaj na pomoc medyczna"
- Engine/lights text: "Silnik: ~r~OFF" / "Swiatla: ~r~OFF"

## Offers & Transactions (gameplay/offers.inc)
- Offer acceptance messages: "oferta zaakceptowana" (GameText), "Offer accepted" (not yet translated)
- Offer dialog strings and accept/reject flows

## Common errors / messages (utils/strings.inc and others)
- Generic message wrapper uses localized `ShowPlayerDialog` and `SendClientMessage`.
- Some commands still use Polish strings for prompts and usage.

---

Next actions (suggested):
1. Add a localization helper: `L(key, args...)` that looks up a string in a language table (e.g., `lang/en.json` or Pawn-included map). Fall back to existing literal when missing.
2. Create `lang/en.inc` with keys for the above strings (and a few more discovered automatically).
3. Replace literal strings in dialogs and SendClientMessage calls with `L()` calls gradually (start with Phone module and main menus).
4. Add a test mode to switch language (e.g., `/lang en`) for testing.

If you want, I can automatically extract all literal strings into a draft `lang/en.inc` file (with keys and English defaults) and open a PR-style diff for review.

---

If this inventory looks good, I'll proceed to generate the `lang/en.inc` scaffold and convert the Phone module messages first (priority from earlier discussion).