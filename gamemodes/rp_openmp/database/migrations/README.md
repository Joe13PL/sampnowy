Run database migrations in order to bring an existing DB schema in sync with the codebase.

Example (MySQL):

```bash
mysql -u your_user -p your_database < rp_openmp/database/migrations/0001_add_doors_extra_columns.sql
```

To apply the phone history migration (added later):

```bash
mysql -u your_user -p your_database < rp_openmp/database/migrations/0002_create_phone_calls_table.sql
```

To verify the migration applied successfully run (replace your DB name):

```sql
SELECT COUNT(*) AS exists_count FROM information_schema.tables
 WHERE table_schema = 'your_database' AND table_name = 'phone_calls';
```

If `exists_count` is 1 the table exists and migration applied correctly.

Note: `ADD COLUMN IF NOT EXISTS` requires MySQL 8+. If you're using an older MySQL version, run the migration manually after checking if a column exists.