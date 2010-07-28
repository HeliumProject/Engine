/* LiteSQL - PostgreSQL-backend implementation
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */
#include "compatibility.hpp"
#include "postgresql.hpp"
#ifdef HAVE_LIBPQ
#include "litesql_char.hpp"
#include <string>
namespace litesql {

PostgreSQL::Result::~Result() {
    PQclear(res);
}
size_t PostgreSQL::Result::fieldNum() const {
    return PQnfields(res);
}
Record PostgreSQL::Result::fields() const {
    Record r;
    r.reserve(fieldNum());
    for (size_t i = 0; i < fieldNum(); i++)
        r.push_back(PQfname(res, i));
    return r;
}
size_t PostgreSQL::Result::recordNum() const {
    return PQntuples(res);
}  
Records PostgreSQL::Result::records() const {
    Records recs;
    recs.resize(recordNum());
    for (size_t i = 0; i < recordNum(); i++) {
        recs[i].reserve(fieldNum());
        for (size_t i2 = 0; i2 < fieldNum(); i2++) 
            recs[i].push_back(PQgetvalue(res, i, i2));
    }
    return recs;
}
int PostgreSQL::Cursor::sid = 1;
size_t PostgreSQL::Cursor::cacheSize = 30;
void PostgreSQL::Cursor::setCacheSize(int v) {
    if (v > 1)
        cacheSize = v;
}
PostgreSQL::Cursor::Cursor(const PostgreSQL& p, LITESQL_String q) 
    : pq(p), name(LITESQL_L("cursor") + toString(sid++)), cachePos(0) {
    pq.begin();
    delete pq.execute(LITESQL_L("DECLARE \"") + name +  LITESQL_L("\" CURSOR FOR ")+ q);
}
Record PostgreSQL::Cursor::fetchOne() {
    if (cache.size() == 0 || cachePos >= cache.size()) {
        cachePos = 0;
        Backend::Result* r = pq.execute(LITESQL_L("FETCH ") + toString(cacheSize)
                                        +  LITESQL_L(" FROM ") + name +  LITESQL_L(";"));
        cache = r->records();
        delete r;
    }
    if (cachePos >= cache.size())
        return Record();
    return cache[cachePos++];
}
PostgreSQL::Cursor::~Cursor() {
    delete pq.execute(LITESQL_L("CLOSE ")+name+ LITESQL_L(";"));
}
PostgreSQL::PostgreSQL(const LITESQL_String& connInfo) : conn(NULL), transaction(false) {
    Split params(connInfo, LITESQL_L(";"));
    LITESQL_String pq_connInfo;
    for (size_t i = 0; i < params.size(); i++) {
        Split param(params[i],  LITESQL_L("="));
        if (param.size() == 1)
            continue;
        if (param[0] ==  LITESQL_L("host"))
            pq_connInfo +=  LITESQL_L("host=") + param[1] +  LITESQL_L(" ");
        else if (param[0] ==  LITESQL_L("database"))
            pq_connInfo +=  LITESQL_L("dbname=") + param[1] +  LITESQL_L(" ");
        else if (param[0] ==  LITESQL_L("password"))
            pq_connInfo +=  LITESQL_L("password=") + param[1] +  LITESQL_L(" ");
        else if (param[0] ==  LITESQL_L("user"))
            pq_connInfo +=  LITESQL_L("user=") + param[1] +  LITESQL_L(" ");
    }

    conn = PQconnectdb(pq_connInfo.c_str());
    if (PQstatus(conn) != CONNECTION_OK)
        throw DatabaseError(LITESQL_L("PostgreSQL connection ") + pq_connInfo +  LITESQL_L(" failed: ") 
                            + LITESQL_String(PQerrorMessage(conn)));
}
bool PostgreSQL::supportsSequences() const {
    return true;
}
void PostgreSQL::begin() const {
  if (!transaction) {
    delete execute(LITESQL_L("BEGIN;"));
    transaction = true;
  }
}
void PostgreSQL::commit() const {
    if (transaction) {
        delete execute(LITESQL_L("COMMIT;"));
        transaction = false;
    }
}
void PostgreSQL::rollback() const {
    if (transaction) {
      delete execute(LITESQL_L("ROLLBACK;"));
      transaction = false;
    }
}
Backend::Result* PostgreSQL::execute(const LITESQL_String& query) const {
    query +=  LITESQL_L(";");
    PGresult * res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK 
        && PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQclear(res);
        throw Except(LITESQL_L("Query:") + query +  LITESQL_L(" failed: ") + 
                     LITESQL_String(PQerrorMessage(conn)));
    }
    return new Result(res);    
}
Backend::Cursor* PostgreSQL::cursor(const LITESQL_String& query) const {
    return new Cursor(*this, query +  LITESQL_L(";"));
}
PostgreSQL::~PostgreSQL() {
    if (transaction)
        commit();
    PQfinish(conn);
}
}
#endif