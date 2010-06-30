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

  using namespace std;
/** MySQL - backend */
class MySQL : public Backend {
    MYSQL* conn;
    string host, user, passwd, database;
    int port;
public:
  class Cursor;
  class Result;

    MySQL(const string& connInfo);
    virtual ~MySQL();

    virtual bool supportsSequences() const;
    virtual string getRowIDType() const;
    virtual string getInsertID() const;
    virtual void begin() const;
    virtual void commit() const;
    virtual void rollback() const;
    Backend::Result* execute(const string& query) const;
    Backend::Cursor* cursor(const string& query) const;
};
}
#endif
#endif
