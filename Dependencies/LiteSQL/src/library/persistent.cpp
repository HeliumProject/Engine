/* LiteSQL - Persistent-class implementation
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */
#include "litesql_char.hpp"
#include "compatibility.hpp"
#include "litesql/persistent.hpp"
#include "litesql/updatequery.hpp"
#include <iostream>

namespace litesql {

const Persistent & Persistent::operator=(const Persistent & p) {
    if (this != &p) { 
        db = p.db;
        inDatabase = p.inDatabase;
        oldKey = p.oldKey;
    }
    return *this;
}

LITESQL_String Persistent::insert(Record& tables, 
                          Records& fieldRecs,
                          Records& values,
                          const LITESQL_String& sequence) {
    if (values[0][0] ==  LITESQL_L("0"))
        for(size_t i = 0; i < values.size(); i++)
            values[i][0] =  LITESQL_L("NULL");
    LITESQL_String key = db->groupInsert(tables, fieldRecs, values, sequence);
    oldKey = atoi(key);
    inDatabase = true;
    return key;
}
void Persistent::update(Updates& updates) {
    for (Updates::iterator i = updates.begin(); i != updates.end(); i++) {
        UpdateQuery uq(i->first);
        uq.where(RawExpr(LITESQL_L("id_ = '") + toString(oldKey) +  LITESQL_L("'")));
        bool notEmpty = false;
        for (std::vector<std::pair<FieldType, LITESQL_String> >::iterator i2 =
                i->second.begin(); i2 != i->second.end();
             i2++) {
            uq.set(i2->first, i2->second);
            notEmpty = true;
        }
        if (notEmpty)
            db->query(uq);
    }
}
void Persistent::prepareUpdate(Updates& updates, LITESQL_String table) {
    if (updates.find(table) == updates.end()) {
        updates[table] = std::vector<std::pair<FieldType, LITESQL_String> >();
    }
}
void Persistent::deleteFromTable(LITESQL_String table, LITESQL_String id) {
    db->query(LITESQL_L("DELETE FROM ") + table +  LITESQL_L(" WHERE id_=")+escapeSQL(id));
}
}


