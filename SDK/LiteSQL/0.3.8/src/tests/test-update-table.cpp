/* LiteSQL - test-update-table
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */

#include "litesql/database.hpp"

/*
*/
using namespace litesql;

namespace litesql {
class Updater {

public:
  static bool testUpgradeTable(Database & db)
  {
    db.verbose = true;
  return Updater::testAddRemoveField(db)
    &&  Updater::testChangeFieldType(db)
    && Updater::testChangeFieldTypeFromTextToInteger(db);

  }

  static bool testAddRemoveField(const Database & db)
  {
    const char * old = "CREATE TABLE Person_ (id_ INTEGER PRIMARY KEY,name_ TEXT,name2_ TEXT)";
    const char * newS = "CREATE TABLE Person_ (id_ INTEGER PRIMARY KEY,name_ TEXT,name3_ TEXT)";
    db.query("DROP TABLE IF EXISTS Person_");
    db.query(old);
    db.upgradeTable("Person_",old,newS);
    
    return true;
 } 

  static bool testChangeFieldType(const Database & db)
  {
    const char * old = "CREATE TABLE Person_ (id_ INTEGER PRIMARY KEY,name_ INTEGER)";
    const char * newS = "CREATE TABLE Person_ (id_ INTEGER PRIMARY KEY,name_ TEXT)";
    db.query("DROP TABLE IF EXISTS Person_");
    db.query(old);
    db.upgradeTable("Person_",old,newS);
    return true;
 } 

  static bool testChangeFieldTypeFromTextToInteger(const Database & db)
  {
    const char * old = "CREATE TABLE Person_ (id_ INTEGER PRIMARY KEY,name_ TEXT)";
    const char * newS = "CREATE TABLE Person_ (id_ INTEGER PRIMARY KEY,name_ INTEGER)";
    db.query("DROP TABLE IF EXISTS Person_");
    db.query(old);
    db.upgradeTable("Person_",old,newS);
    return true;
 } 
};
}

int main(int argc, char *argv[]) {
  bool success;
  try {
    Database sqlite3_db("sqlite3","database=test-update-table.db");
    success = Updater::testUpgradeTable(sqlite3_db);

#ifdef HAVE_LIBMYSQLCLIENT
    Database mysql_db("mysql","database=test-update-table.db");
    success &= Updater::testUpgradeTable(mysql_db);
#endif

#ifdef HAVE_LIBPQ
    Database pg_db("postgresql","host=localhost;database=test-update-table;user=litesql;password=litesql");
    success &= Updater::testUpgradeTable(pg_db);
#endif

#ifdef HAVE_ODBC
    Database odbc_db("odbc","database=test-update-table.db");
    success &= Updater::testUpgradeTable(odbc_db);
#endif

  } catch (Except e) {
      cerr << e << endl;
      return -1;
  }
    return success ? 0 : -1;
}


