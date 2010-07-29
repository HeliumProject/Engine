/* LiteSQL - selectObjectQuery - implementation
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */
#include "litesql_char.hpp"
#include "compatibility.hpp"
#include "litesql/datasource.hpp"
namespace litesql {
SelectQuery selectObjectQuery(const std::vector<FieldType>& fdatas,
                              const Expr& e) {
    SelectQuery sel;
    Split tables;       
    std::set<LITESQL_String> tableSet;

    for (size_t i = 0; i < fdatas.size(); i++)
        if (tableSet.find(fdatas[i].table()) == tableSet.end()) {
            tables.push_back(fdatas[i].table());
            tableSet.insert(fdatas[i].table());
        }

    Split tableFilters;
    tableFilters.resize(tables.size()-1);
    for (size_t i = 1; i < tables.size(); i++)
        tableFilters[i-1] = tables[i-1] +  LITESQL_L(".id_ = ") + tables[i] +  LITESQL_L(".id_");
    tableSet.clear();
    for (size_t i = 0; i < tables.size(); i++) {
        sel.source(tables[i]);
        tableSet.insert(tables[i]);
    }
    if (tables.size() > 1)
        sel.where((e && RawExpr(tableFilters.join(LITESQL_L(" AND ")))).asString()); 
    else
        sel.where(e.asString());
    
    for (size_t i = 0; i < fdatas.size(); i++)
        sel.result(fdatas[i].table() +  LITESQL_L(".") + fdatas[i].name());

    return sel;
}
}