/* LiteSQL - SQLite3-backend implementation
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * Few windows specific bugs fixed by Axel Schmidt.
 * 
 * See LICENSE for copyright information. */
#include "compatibility.hpp"
#include "sqlite3.hpp"
#include <sqlite3.h>

#include <string>
#ifdef HAVE_LIBSQLITE3
#ifdef _MSC_VER
#include <windows.h>
#define usleep( microsec )  ::Sleep( (microsec)/1000 )
#else
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif

using namespace litesql;
using namespace std;

/** SQLite3 - result */
class SQLite3::Result : public Backend::Result {
public:
  Records recs;
  Record flds;
  Result() {}
  virtual size_t fieldNum() const;
  virtual Record fields() const;
  virtual size_t recordNum() const;
  virtual Records records() const;
  const Records& recordsRef() const;
};

size_t SQLite3::Result::fieldNum() const {
    return flds.size();
}
Record SQLite3::Result::fields() const {
    return flds;
}

size_t SQLite3::Result::recordNum() const {
    return recs.size();
}  
Records SQLite3::Result::records() const {
    return recs;
}

/** SQLite3 - cursor */
class SQLite3::Cursor : public Backend::Cursor {
  //        sqlite3 * db;
  sqlite3_stmt * stmt;
  const SQLite3& owner;
public:
  Cursor(/*sqlite3 * db,*/ sqlite3_stmt * s, const SQLite3& owner);
  virtual Record fetchOne();
  virtual ~Cursor();
};

SQLite3::Cursor::Cursor(sqlite3_stmt * s, const SQLite3& o) 
    :  stmt(s), owner(o) {}

Record SQLite3::Cursor::fetchOne() {
    bool busy = false;
    do 
    {
    	int status = sqlite3_step(stmt);
        
        switch(status)
        {
        case SQLITE_ERROR: case SQLITE_MISUSE: {
          std::string error = sqlite3_errmsg(owner.db);
          LITESQL_String tempString;
          LITESQL_ConvertString( error, tempString );

                throw UnknownError( LITESQL_L( "step failed: " ) + toString(status) + tempString );
            }
        case SQLITE_DONE: return Record(); break;
        case SQLITE_BUSY: case SQLITE_LOCKED:
            usleep(5000);
            busy = true;
            break;
        case SQLITE_ROW:
            busy = false;
            break;
        }
    } while (busy);
    int columnNum = sqlite3_data_count(stmt);
    Record rec(columnNum);
    for (int i = 0; i < columnNum; i++) {
        if (sqlite3_column_type(stmt, i) == SQLITE_NULL)
            rec.push_back( LITESQL_L( "NULL" ) );
        else
        {
            LITESQL_String tempString;
            LITESQL_ConvertString( (char*)sqlite3_column_text(stmt, i), tempString );
            rec.push_back( tempString.c_str() );
        }
    }
    return rec;
}
SQLite3::Cursor::~Cursor() {
    if (stmt) {
        sqlite3_finalize(stmt);
    }
}
SQLite3::SQLite3(const LITESQL_String& connInfo) : db(NULL), transaction(false) {
    Split params(connInfo, LITESQL_L( ";" ));
    LITESQL_String database;
    for (size_t i = 0; i < params.size(); i++) {
        Split param(params[i], LITESQL_L( "=" ));
        if (param.size() == 1)
            continue;
        if (param[0] == LITESQL_L( "database" ))
            database = param[1];
    }
    if (database.empty())
        throw DatabaseError( LITESQL_L( "no database-param specified" ));

    std::string tempString;
    LITESQL_ConvertString( database, tempString );
    if (sqlite3_open(tempString.c_str(), &db)) {
        LITESQL_String tempError;
        LITESQL_ConvertString( sqlite3_errmsg(db), tempError );
        throw DatabaseError( tempError );
    }

}
bool SQLite3::supportsSequences() const {
    return false;
}
LITESQL_String SQLite3::getInsertID() const {
    return toString(sqlite3_last_insert_rowid(db));
}
void SQLite3::begin() const {
    if (!transaction) {
        delete execute( LITESQL_L( "BEGIN;" ));
        transaction = true;
    }
}
void SQLite3::commit() const {
    if (transaction) {
        delete execute( LITESQL_L( "COMMIT;" ));
        transaction = false;
    }
}
void SQLite3::rollback() const {
    if (transaction) {
        delete execute( LITESQL_L( "ROLLBACK;" ));
        transaction = false;
    }
}

static int callback(void *r, int argc, char **argv, char **azColName) {
    SQLite3::Result * res = (SQLite3::Result*) r;
    if (res->flds.size() == 0) 
        for (int i = 0; i < argc; i++)
        {
            LITESQL_String tempString;
            LITESQL_ConvertString( azColName[i], tempString );
            res->flds.push_back(tempString);
        }
    Record rec(argc); 
    for (int i = 0; i < argc; i++)
    {
        LITESQL_String tempString;
        LITESQL_ConvertString( argv[ i ], tempString );
        rec.push_back(argv[i] ? tempString : LITESQL_L( "NULL" ));   
    }
    res->recs.push_back(rec);
    return 0;
}
void SQLite3::throwError(int status) const {
    LITESQL_String error;
    LITESQL_ConvertString( sqlite3_errmsg(db), error );
    error = toString(status) + LITESQL_L( "=status code : " ) + error;
    switch(status) {
    case SQLITE_ERROR: throw SQLError(error);
    case SQLITE_INTERNAL: throw InternalError(error);
    case SQLITE_NOMEM: throw MemoryError(error);
    case SQLITE_FULL: throw InsertionError(error);
    default: throw UnknownError( LITESQL_L( "compile failed: " ) + error);
    }
}
Backend::Result* SQLite3::execute(const LITESQL_String& query) const {
    Result * r = new Result;
    char * errMsg;
    int status;
    string tempQuery;
    LITESQL_ConvertString( query, tempQuery );

    do {
        status = sqlite3_exec(db, tempQuery.c_str(), callback, r, &errMsg);
        switch(status) {         
            case SQLITE_BUSY: 
            case SQLITE_LOCKED: 
                usleep(250000); 
                break; 
            case SQLITE_OK: break;
            default: throwError(status); 
        }
    } while (status != SQLITE_OK); 
    return r;    
}

Backend::Cursor* SQLite3::cursor(const LITESQL_String& query) const {
    string tempQuery;
    LITESQL_ConvertString( query, tempQuery );
    while (1) {
        sqlite3_stmt * stmt = NULL;
        int status = sqlite3_prepare(db, tempQuery.c_str(), tempQuery.size(), 
                                     &stmt, NULL);
        if (status != SQLITE_OK || stmt == NULL) {
            LITESQL_String error;
            LITESQL_ConvertString( sqlite3_errmsg(db), error );
            error = toString(status) + LITESQL_L( "=status code : " ) + error;
            switch(status) {
            case SQLITE_BUSY: 
            case SQLITE_LOCKED: 
                usleep(250000); 
                break;
            default: throwError(status);
            }
        }
        else
            return new Cursor(stmt, *this);
    }
    
}
SQLite3::~SQLite3() {
    if (db)
        sqlite3_close(db);
}

#endif
