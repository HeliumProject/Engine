/* LiteSQL - SelectQuery implementation
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */
#include "litesql_char.hpp"
#include "compatibility.hpp"
#include "litesql/selectquery.hpp"
namespace litesql {
SelectQuery & SelectQuery::distinct(bool d) { 
    _distinct = d; 
    return *this;
}
SelectQuery & SelectQuery::limit(int value) { 
    _limit = value;	
    return *this;
}
SelectQuery & SelectQuery::offset(int value) { 
    _offset = value; 
    return *this;
}
SelectQuery & SelectQuery::result(LITESQL_String r) { 
    _results.push_back(r); 
    return *this;
}
SelectQuery & SelectQuery::clearResults() {
    _results.clear();
    return *this;
}
SelectQuery & SelectQuery::source(LITESQL_String s, LITESQL_String alias) {
    if (!alias.empty())
        s +=  LITESQL_L(" AS ") + alias;
    _sources.push_back(s);
    return *this;
}
SelectQuery & SelectQuery::where(const Expr & w) { 
    _where = (RawExpr(_where) && w).asString();	
    return *this;
}
SelectQuery & SelectQuery::where(LITESQL_String w) { 
    _where = (RawExpr(_where) && RawExpr(w)).asString();
    return *this;
}
SelectQuery & SelectQuery::groupBy(LITESQL_String gb) { 
    _groupBy.push_back(gb);	
    return *this;
}
SelectQuery & SelectQuery::having(const Expr & h) { 
    _having = h.asString(); 
    return *this;
}
SelectQuery & SelectQuery::having(LITESQL_String h) { 
    _having = h;
    return *this;
}
SelectQuery & SelectQuery::orderBy(LITESQL_String ob, bool ascending) { 
    LITESQL_String value = ob;
    if (!ascending)
        value +=  LITESQL_L(" DESC");
    _orderBy.push_back(value); 
    return *this;
}
SelectQuery::operator LITESQL_String() const {
    LITESQL_String res =  LITESQL_L("SELECT ");
    if (_distinct)
        res +=  LITESQL_L("DISTINCT ");
    res += _results.join(LITESQL_L(","));
    res +=  LITESQL_L(" FROM ");
    res += _sources.join(LITESQL_L(","));
    if (_where !=  LITESQL_L("True"))
        res +=  LITESQL_L(" WHERE ") + _where;
    if (_groupBy.size() > 0)
        res +=  LITESQL_L(" GROUP BY ") + _groupBy.join(LITESQL_L(","));
    if (!_having.empty())
        res +=  LITESQL_L(" HAVING ") + _having;
    if (_orderBy.size() > 0)
        res +=  LITESQL_L(" ORDER BY ") + _orderBy.join(LITESQL_L(","));
    if (_limit) 
        res +=  LITESQL_L(" LIMIT ") + toString(_limit);
    if (_offset) 
            res +=  LITESQL_L(" OFFSET ") + toString(_offset);
    return res;
}
}