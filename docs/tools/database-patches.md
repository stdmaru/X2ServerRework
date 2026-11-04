[Home](../README.md) › [Tools](README.md) › Database patches

# Database patches

SQL patches in `tools/database/patches/` are additive repairs for missing database objects. They are reviewed and supplied for
manual application; this project does not run them automatically when restoring a backup or starting a server.

## Statistics

[`20261104_globalserver_event_features.sql`](../../tools/database/patches/Statistics/20261104_globalserver_event_features.sql) adds
the tables and procedures used by GlobalServer's timed enchant and limited exchange flows. It implements the server's result-set
contracts, single-use timed-enchant claims, exchange quota tracking, and rollback records.

The code establishes the single-use event claim, shared `LimitUID` quota, and rollback behavior. The SQL source does not provide
the original table definitions, so the new table and column layout is an additive model documented in the script. Configuration
rows are not seeded because the repository contains no authoritative event schedule, enchant level, item targets, or quota values.

The exchange procedure reloads the remaining quota by subtracting outstanding consumption log rows. A rollback deletes its row,
so the quota is restored both in memory and after a server restart. The script does not modify existing tables. Configure the new
tables before enabling either feature.
