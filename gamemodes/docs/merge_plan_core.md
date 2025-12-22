Core Merge Plan - `config.inc` / `defines.inc`

Goal: Carefully integrate useful definitions and helpers from `rp/` into `rp_openmp` core files while preserving current behavior and DB schema.

Planned steps:

1. Compatibility macros (done)
   - Add `CONFIG_FILEPATH` alias to our `CONFIG_DEFAULT_PATH` for modules referencing the old macro.

2. Reconcile settings enums
   - Compare `e_settings` in both files. Keep our `Setting[...]` shape but add missing fields if they are used by modules being merged.
   - Avoid altering indices of existing settings unless necessary; prefer adding new settings at the end with clear defaults.

3. Session info and product arrays
   - Ensure `e_session_info` and `sInfo` presence only if used elsewhere; if our code maintains sessions differently, add an adapter.
   - `Product` structures already exist in core variables; ensure initialization or loading functions match `rp/` expectations.

4. Helper functions
   - Verify helper functions like `StripNewLine`, `TrimWhitespace`, `StripQuotes` are equivalent; adopt the better implementation.

5. Game element initializers
   - Do NOT copy feature initializers (FerrisWheel, job pickups) now; these will be merged as part of feature modules with full tests.

6. Tests & Validation
   - After integrating changes, recompile and run `OnGameModeInit` in local environment to validate no regressions.

Notes
- Any DB schema changes will be implemented as non-destructive migrations in `database/migrations/` with backfill scripts.
- Document each change in `REFACTOR.md` and commit incrementally.
