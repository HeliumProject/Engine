/* LiteSQL 
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */


#ifndef litesql_selectquery_hpp
#define litesql_selectquery_hpp

#include "litesql/utils.hpp"
#include "litesql/expr.hpp"
/** \file selectquery.hpp
    contains SelectQuery-class. See \ref usage_selecting_persistents */
namespace litesql {
/** a class that helps creating SELECT-SQL statements. methods are 
    self-explanatory. */
class SelectQuery {
    bool _distinct;
    int _limit, _offset;
    Split _results;
    Split _sources;
    LITESQL_String _where; 
    Split _groupBy;
    LITESQL_String _having;
    Split _orderBy;
public:
    SelectQuery() : _distinct(false), _limit(0), _offset(0), _where(LITESQL_L("True")) {}
    SelectQuery & distinct(bool d) ;
    SelectQuery & limit(int value);
    SelectQuery & offset(int value);
    SelectQuery & result(LITESQL_String r);
    SelectQuery & clearResults();
    SelectQuery & source(LITESQL_String s, LITESQL_String alias=LITESQL_L(""));
    SelectQuery & where(const Expr & w);
    SelectQuery & where(LITESQL_String w);
    SelectQuery & groupBy(LITESQL_String gb);
    SelectQuery & having(const Expr & h);
    SelectQuery & having(LITESQL_String h);
    SelectQuery & orderBy(LITESQL_String ob, bool ascending=true);
    operator LITESQL_String() const;
    LITESQL_String asString() const { return this->operator LITESQL_String(); }
};
}

#endif