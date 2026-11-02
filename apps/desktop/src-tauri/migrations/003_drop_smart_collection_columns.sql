-- Smart collections were removed from the product; collections are now plain
-- asset folders. Both columns were introduced in 001/002 and are dropped here
-- for every existing library DB and the master DB.
ALTER TABLE collections DROP COLUMN is_smart;
ALTER TABLE collections DROP COLUMN filter_spec;
