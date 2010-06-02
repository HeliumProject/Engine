#pragma once

const char* g_ResolverDBSQL_v1 = "\
                                                                                                   \
PRAGMA page_size=4096;                                                                             \
                                                                                                   \
BEGIN TRANSACTION;                                                                                 \
                                                                                                   \
DROP TABLE IF EXISTS file;                                                                         \
CREATE TABLE file (                                                                                \
  created      BIGINT UNSIGNED NOT NULL DEFAULT 0,                                                 \
  modified     BIGINT UNSIGNED NOT NULL DEFAULT 0,                                                 \
  id           BIGINT UNSIGNED UNIQUE PRIMARY KEY NOT NULL,                                        \
  path         VARCHAR(255) UNIQUE NOT NULL,                                                       \
  last_user    VARCHAR(64)                                                                         \
);                                                                                                 \
                                                                                                   \
DROP INDEX IF EXISTS file_id_index;                                                                \
CREATE INDEX file_id_index ON file (id);                                                           \
                                                                                                   \
DROP INDEX IF EXISTS file_path_index;                                                              \
CREATE INDEX file_path_index ON file (path);                                                       \
                                                                                                   \
DROP INDEX IF EXISTS file_user_index;                                                              \
CREATE INDEX file_user_index ON file (last_user);                                                  \
                                                                                                   \
";