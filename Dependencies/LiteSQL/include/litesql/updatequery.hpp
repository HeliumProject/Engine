/* LiteSQL 
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */

#ifndef _litesql_updatequery_hpp
#define _litesql_updatequery_hpp

#include "litesql/utils.hpp"
#include "litesql/expr.hpp"
/** \file updatequery.hpp
    contains UpdateQuery-class. */
namespace litesql {
/** a class that helps creating UPDATE-SQL statements. methods are 
    self-explanatory. */
class UpdateQuery {
    LITESQL_String table;
    LITESQL_String _where;
    Split fields;
    Split values;
public:
    UpdateQuery(const LITESQL_String& t) : table(t), _where(LITESQL_L("True")) {}
    UpdateQuery& where(const Expr& e);
    UpdateQuery& set(const FieldType& f, const LITESQL_String& value);
    operator LITESQL_String() const;
    LITESQL_String asString() const { return this->operator LITESQL_String(); }
};
}

#endif