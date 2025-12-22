RP Merge Refactor - Notes

Objective:
- Merge features from `rp/` and `rp.pwn` into our `rp_openmp` gamemode while preserving our database schema.

Progress
- [x] Backup created: `backups/refactor-2025-12-17.zip`
- [x] Inventory completed for `rp/` and `rp.pwn`
- [x] Scanned and fixed immediate parsing bugs (sscanf format issues)
- [x] Added `.editorconfig` and `docs/CODING_STYLE.md`

Next steps
- Merge core modules (`config.inc`, `defines.inc`) and reconcile constants and dialog IDs.
- Merge `functions` and `timers`, adjust forwards and public APIs.
- Iteratively merge player/items systems and ensure `phone` item integration.

How I will commit
- Small commits per module; each commit will be accompanied by a short description and a quick test plan.

Contact
- Ask here in the task chat for priorities or to pause/resume specific modules.
