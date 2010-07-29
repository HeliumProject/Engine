/* LiteSQL - UpdateQuery implementation
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */
#include "litesql_char.hpp"
#include "compatibility.hpp"
#include "litesql/updatequery.hpp"
namespace litesql {
UpdateQuery& UpdateQuery::where(const Expr& e) {
    _where = (RawExpr(_where) && e).asString();
    return *this;
}
UpdateQuery& UpdateQuery::set(FieldType f, LITESQL_String value) {
    fields.push_back(f.name());
    values.push_back(escapeSQL(value));
    return *this;
}
UpdateQuery::operator LITESQL_String() const {
    LITESQL_String q =  LITESQL_L("UPDATE ") + table +  LITESQL_L(" SET ");
    Split sets;
    for (size_t i = 0; i < fields.size(); i++)
        sets.push_back(fields[i] +  LITESQL_L("=") + values[i]);
    q += sets.join(LITESQL_L(","));
    if (_where.size())
        q +=  LITESQL_L(" WHERE ") + _where;
    return q;
}

}