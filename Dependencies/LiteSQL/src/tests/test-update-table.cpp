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
    const LiteSQL_Char * old =  LiteSQL_L( "CREATE TABLE Person_ (id_ INTEGER PRIMARY KEY,name_ TEXT,name2_ TEXT)" );
    const LiteSQL_Char * newS =  LiteSQL_L( "CREATE TABLE Person_ (id_ INTEGER PRIMARY KEY,name_ TEXT,name3_ TEXT)" );
    db.query( LiteSQL_L( "DROP TABLE IF EXISTS Person_" ));
    db.query(old);
    db.upgradeTable( LiteSQL_L( "Person_" ),old,newS);
    
    return true;
 } 

  static bool testChangeFieldType(const Database & db)
  {
    const LiteSQL_Char * old =  LiteSQL_L( "CREATE TABLE Person_ (id_ INTEGER PRIMARY KEY,name_ INTEGER)" );
    const LiteSQL_Char * newS =  LiteSQL_L( "CREATE TABLE Person_ (id_ INTEGER PRIMARY KEY,name_ TEXT)" );
    db.query( LiteSQL_L( "DROP TABLE IF EXISTS Person_" ));
    db.query(old);
    db.upgradeTable( LiteSQL_L( "Person_" ),old,newS);
    return true;
 } 

  static bool testChangeFieldTypeFromTextToInteger(const Database & db)
  {
    const LiteSQL_Char * old =  LiteSQL_L( "CREATE TABLE Person_ (id_ INTEGER PRIMARY KEY,name_ TEXT)" );
    const LiteSQL_Char * newS =  LiteSQL_L( "CREATE TABLE Person_ (id_ INTEGER PRIMARY KEY,name_ INTEGER)" );
    db.query( LiteSQL_L( "DROP TABLE IF EXISTS Person_" ));
    db.query(old);
    db.upgradeTable( LiteSQL_L( "Person_" ),old,newS);
    return true;
 } 
};
}

int main(int argc, LiteSQL_Char *argv[]) {
  bool success;
  try {
    Database sqlite3_db( LiteSQL_L( "sqlite3" ), LiteSQL_L( "database=test-update-table.db" ));
    success = Updater::testUpgradeTable(sqlite3_db);

#ifdef HAVE_LIBMYSQLCLIENT
    Database mysql_db( LiteSQL_L( "mysql" ), LiteSQL_L( "database=test-update-table.db" ));
    success &= Updater::testUpgradeTable(mysql_db);
#endif

#ifdef HAVE_LIBPQ
    Database pg_db( LiteSQL_L( "postgresql" ), LiteSQL_L( "host=localhost;database=test-update-table;user=litesql;password=litesql" ));
    success &= Updater::testUpgradeTable(pg_db);
#endif

#ifdef HAVE_ODBC
    Database odbc_db( LiteSQL_L( "odbc" ), LiteSQL_L( "database=test-update-table.db" ));
    success &= Updater::testUpgradeTable(odbc_db);
#endif

  } catch (Except e) {
      LiteSQL_cerr << e << std::endl;
      return -1;
  }
    return success ? 0 : -1;
}

