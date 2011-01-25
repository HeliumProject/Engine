/* LiteSQL - Common routines for backends
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */

#include <map>

#include "compatibility.hpp"
#include "litesql/backend.hpp"
#include "litesql/string.hpp"
#include "litesql/types.hpp"

#ifdef HAVE_LIBMYSQLCLIENT
#include "mysql.hpp"
#endif

#ifdef HAVE_LIBPQ
#include "postgresql.hpp"
#endif

#ifdef HAVE_LIBSQLITE3
#include "sqlite3.hpp"
#endif

#ifdef HAVE_ODBC
#include "odbc_backend.hpp"
#endif

using namespace litesql;

LITESQL_String Backend::groupInsert(Record tables, Records fields, Records values,
                   const LITESQL_String& sequence) const {
    LITESQL_String id = values[0][0];
    
    if (supportsSequences() && values[0][0] == LITESQL_L("NULL")) {
      Result * r = execute(LITESQL_L("SELECT nextval('") + sequence + LITESQL_L("');"));
      id = r->records()[0][0];
      delete r;
    } 
    for (int i = tables.size()-1; i >= 0; i--) {
      LITESQL_String fieldString = Split::join(fields[i],LITESQL_L(","));
        LITESQL_String valueString;
        if (!values[i].empty())
            values[i][0] = id;
        Split valueSplit(values[i]);
        for (size_t i2 = 0; i2 < valueSplit.size(); i2++)
            valueSplit[i2] = escapeSQL(valueSplit[i2]);
        valueString = valueSplit.join(LITESQL_L(","));
        LITESQL_String query = LITESQL_L("INSERT INTO ") + tables[i] + LITESQL_L(" (") + fieldString
            + LITESQL_L(") VALUES (") + valueString + LITESQL_L(");");
        delete execute(query);
        if (!supportsSequences() && id == LITESQL_L("NULL")) 
            id = getInsertID();
        
    }
    return id;
}

Backend* Backend::getBackend(const LITESQL_String & backendType,const LITESQL_String& connInfo)
{
  Backend* backend;

#ifdef HAVE_LIBMYSQLCLIENT
  if (backendType == LITESQL_L("mysql")) {
    backend = new MySQL(connInfo);
  } else
#endif
#ifdef HAVE_LIBPQ
    if (backendType == LITESQL_L("postgresql")) {
      backend = new PostgreSQL(connInfo);
    } else
#endif
#ifdef HAVE_ODBC
      if (backendType == LITESQL_L("odbc")) {
        backend = new ODBCBackend(connInfo);
      } else
#endif
#ifdef HAVE_LIBSQLITE3
        if (backendType == LITESQL_L("sqlite3")) {
          backend = new SQLite3(connInfo);
        } else
#endif
        { 
          backend = NULL;
        };
        return backend;
}