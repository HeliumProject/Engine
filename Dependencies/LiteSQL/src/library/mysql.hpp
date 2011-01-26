/* LiteSQL 
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */

/** \file mysql.hpp
 * MySQL MySQL::Cursor MySQL::Backend */
#ifndef _litesql_mysql_hpp
#define _litesql_mysql_hpp

#ifdef HAVE_LIBMYSQLCLIENT


#include "litesql/except.hpp"
#include "litesql/types.hpp"
#include "litesql/string.hpp"
#include "litesql/backend.hpp"

typedef struct st_mysql MYSQL;

namespace litesql {

/** MySQL - backend */
class MySQL : public Backend {
    MYSQL* conn;
    LITESQL_String host, user, passwd, database;
    int port;
public:
  class Cursor;
  class Result;

    MySQL(const LITESQL_String& connInfo);
    virtual ~MySQL();

    virtual bool supportsSequences() const;
    virtual LITESQL_String getRowIDType() const;
    virtual LITESQL_String getInsertID() const;
    virtual void begin() const;
    virtual void commit() const;
    virtual void rollback() const;
    Backend::Result* execute(const LITESQL_String& query) const;
    Backend::Cursor* cursor(const LITESQL_String& query) const;
};
}
#endif
#endif