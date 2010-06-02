/*********************************************************
* 
* This file will create the Asset CacheDB
*
* - 4.0 Changes:
*   * No more file ids
*   * Engine type -> Asset type
*   * 'p4' -> 'rcs'
*
* - 3.0 Changes:
*   * Removing project and branch tables
*
* - 2.0 Changes:
*   * Removed 'name' column from the assets table
*   * Cleanup CREATE TABLE formating
*   * Create various indexes and foreign key references
*   * Added assets.p4_revision column

* - 1.0 Changes:
*   * bringing the assets.name column - back by popular demand
*     (and so we can search level names w/o searching the whole path)
*
*********************************************************/

const char* g_CacheDBSQL_v1 = "                                                                    \
PRAGMA page_size=4096;                                                                             \
                                                                                                   \
BEGIN TRANSACTION;                                                                                 \
                                                                                                   \
/********************************************************                                          \
* db_file_version                                                                                  \
*********************************************************/                                         \
DROP TABLE IF EXISTS db_file_version;                                                              \
CREATE TABLE db_file_version (                                                                     \
  file_name         VARCHAR(50) NOT NULL,                                                          \
  created           TIMESTAMP NOT NULL,                                                            \
  major_version     VARCHAR(20) NOT NULL DEFAULT '0',                                              \
  minor_version     VARCHAR(20) NOT NULL DEFAULT '0',                                              \
                                                                                                   \
  UNIQUE(file_name)                                                                                \
);                                                                                                 \
                                                                                                   \
INSERT INTO db_file_version (file_name,created,major_version,minor_version)                        \
  VALUES('cache.db',datetime('now','localtime'),'2','0');                                          \
                                                                                                   \
                                                                                                   \
/********************************************************                                          \
* computers                                                                                        \
*********************************************************/                                         \
DROP TABLE IF EXISTS computers;                                                                    \
CREATE TABLE computers (                                                                           \
  id     INTEGER PRIMARY KEY AUTOINCREMENT,                                                        \
  name   VARCHAR(30) NOT NULL,                                                                     \
UNIQUE(name)                                                                                       \
);                                                                                                 \
                                                                                                   \
                                                                                                   \
/********************************************************                                          \
* rcs_users                                                                                        \
*********************************************************/                                         \
DROP TABLE IF EXISTS rcs_users;                                                                    \
CREATE TABLE rcs_users (                                                                           \
  id        INTEGER PRIMARY KEY AUTOINCREMENT,                                                     \
  username  VARCHAR(30) NOT NULL,                                                                  \
UNIQUE(username)                                                                                   \
);                                                                                                 \
                                                                                                   \
                                                                                                   \
/********************************************************                                          \
* attributes                                                                                       \
*********************************************************/                                         \
DROP TABLE IF EXISTS attributes;                                                                   \
CREATE TABLE attributes (                                                                          \
  id 	  INTEGER PRIMARY KEY AUTOINCREMENT,                                                       \
  name 	  VARCHAR(128) NOT NULL,                                                                   \
UNIQUE(name)                                                                                       \
);                                                                                                 \
                                                                                                   \
                                                                                                   \
/********************************************************                                          \
* asset_types                                                                                      \
*********************************************************/                                         \
DROP TABLE IF EXISTS asset_types;                                                                  \
CREATE TABLE asset_types (                                                                         \
  id 		  INTEGER PRIMARY KEY AUTOINCREMENT,                                                   \
  name        VARCHAR(15) NOT NULL,                                                                \
UNIQUE(name)                                                                                       \
);                                                                                                 \
                                                                                                   \
                                                                                                   \
/********************************************************                                          \
* file_types                                                                                       \
*********************************************************/                                         \
DROP TABLE IF EXISTS file_types;                                                                   \
CREATE TABLE file_types (                                                                          \
  id          INTEGER PRIMARY KEY AUTOINCREMENT,                                                   \
  type        VARCHAR(50) NOT NULL,                                                                \
UNIQUE(type)                                                                                       \
);                                                                                                 \
                                                                                                   \
                                                                                                   \
/********************************************************                                          \
* assets                                                                                           \
*********************************************************/                                         \
DROP TABLE IF EXISTS assets;                                                                       \
CREATE TABLE assets (                                                                              \
  id                      INTEGER UNIQUE PRIMARY KEY AUTOINCREMENT,                                \
  path_hash               BIGINT UNSIGNED NOT NULL,                                                \
  path                    VARCHAR(255) NOT NULL,                                                   \
  name                    VARCHAR(100) DEFAULT NULL,                                               \
  file_type_id            INTEGER UNSIGNED NOT NULL,                                               \
  asset_type_id           INTEGER UNSIGNED DEFAULT NULL,                                           \
  size                    INTEGER UNSIGNED NOT NULL DEFAULT '0',                                   \
  rcs_user_id             INTEGER UNSIGNED NOT NULL DEFAULT '0',                                   \
  rcs_revision            INTEGER UNSIGNED NOT NULL,                                               \
  last_updated            TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,                            \
                                                                                                   \
UNIQUE( path_hash ),                                                                               \
FOREIGN KEY (file_type_id) REFERENCES file_types(id),                                              \
FOREIGN KEY (asset_type_id) REFERENCES asset_types(id)                                             \
FOREIGN KEY (rcs_user_id) REFERENCES rcs_users(id)                                                 \
);                                                                                                 \
                                                                                                   \
DROP INDEX IF EXISTS assets_id_index;                                                              \
CREATE INDEX assets_id_index ON assets (id);                                                       \
                                                                                                   \
DROP INDEX IF EXISTS assets_path_hash_index;                                                       \
CREATE INDEX assets_path_hash_index ON assets (path_hash);                                         \
                                                                                                   \
DROP INDEX IF EXISTS assets_path_index;                                                            \
CREATE INDEX assets_path_index ON assets (path);                                                   \
                                                                                                   \
                                                                                                   \
/********************************************************                                          \
* asset_x_attribute                                                                                \
*********************************************************/                                         \
DROP TABLE IF EXISTS asset_x_attribute;                                                            \
CREATE TABLE asset_x_attribute (                                                                   \
  asset_id        BIGINT UNSIGNED NOT NULL,                                                        \
  attribute_id    INTEGER UNSIGNED NOT NULL,                                                       \
  value           VARCHAR(1024),                                                                   \
UNIQUE(asset_id,attribute_id),                                                                     \
FOREIGN KEY (asset_id) REFERENCES assets(id),                                                      \
FOREIGN KEY (attribute_id) REFERENCES attributes(id)                                               \
);                                                                                                 \
                                                                                                   \
                                                                                                   \
/********************************************************                                          \
* asset_usages                                                                                     \
*********************************************************/                                         \
DROP TABLE IF EXISTS asset_usages;                                                                 \
CREATE TABLE asset_usages (                                                                        \
  asset_id      BIGINT UNSIGNED NOT NULL DEFAULT '0',                                              \
  dependency_id BIGINT UNSIGNED NOT NULL DEFAULT '0',                                              \
UNIQUE(asset_id,dependency_id),                                                                    \
FOREIGN KEY (asset_id) REFERENCES assets(id),                                                      \
FOREIGN KEY (dependency_id) REFERENCES assets(id)                                                  \
);                                                                                                 \
                                                                                                   \
                                                                                                   \
/********************************************************                                          \
* entities_x_shaders                                                                               \
*********************************************************/                                         \
DROP TABLE IF EXISTS entities_x_shaders;                                                           \
CREATE TABLE entities_x_shaders (                                                                  \
  entity_id 	BIGINT UNSIGNED NOT NULL DEFAULT '0',                                              \
  shader_id 	BIGINT UNSIGNED DEFAULT '0',                                                       \
UNIQUE(entity_id,shader_id),                                                                       \
FOREIGN KEY (entity_id) REFERENCES assets(id),                                                     \
FOREIGN KEY (shader_id) REFERENCES assets(id)                                                      \
);                                                                                                 \
                                                                                                   \
                                                                                                   \
/********************************************************                                          \
* levels_x_entities                                                                                \
*********************************************************/                                         \
DROP TABLE IF EXISTS levels_x_entities;                                                            \
CREATE TABLE levels_x_entities (                                                                   \
  level_id 		BIGINT UNSIGNED NOT NULL DEFAULT '0',                                              \
  entity_id 	BIGINT UNSIGNED DEFAULT '0',                                                       \
UNIQUE(level_id,entity_id),                                                                        \
FOREIGN KEY (level_id) REFERENCES assets(id),                                                      \
FOREIGN KEY (entity_id) REFERENCES assets(id)                                                      \
);                                                                                                 \
                                                                                                   \
                                                                                                   \
COMMIT;                                                                                            \
";