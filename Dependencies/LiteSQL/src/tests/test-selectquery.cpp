/* LiteSQL - test-selectquery
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */

#include "litesql/selectquery.hpp"
/*
  SelectQuery unit tester

  TC1: Full blown SQL-query with every possible feature
  TC2: Simple SQL-query
*/
using namespace litesql;
int main(int argc, LITESQL_Char *argv[]) {
    // TC1
    SelectQuery tc1;
    tc1.distinct(true).limit(15).offset(5)
        .result( LITESQL_L( "a.id" )).result( LITESQL_L( "b.id" )).result( LITESQL_L( "count(b.pages)" ))
        .source( LITESQL_L( "alphabets" ),  LITESQL_L( "a" )).source( LITESQL_L( "books" ),  LITESQL_L( "b" ))
        .where( LITESQL_L( "a.id = b.id" ))
        .groupBy( LITESQL_L( "b.id" ))
        .having( LITESQL_L( "count(b.pages) > 5" ))
        .orderBy( LITESQL_L( "a.id" ));
    assert(tc1.asString() == 
            LITESQL_L( "SELECT DISTINCT a.id,b.id,count(b.pages) " )
            LITESQL_L( "FROM alphabets AS a,books AS b " )
            LITESQL_L( "WHERE a.id = b.id " )
            LITESQL_L( "GROUP BY b.id " )
            LITESQL_L( "HAVING count(b.pages) > 5 " )
            LITESQL_L( "ORDER BY a.id LIMIT 15 OFFSET 5" ));
    // TC2
    SelectQuery tc2;
    tc2.result( LITESQL_L( "id" )).result( LITESQL_L( "name" )).result( LITESQL_L( "filename" ))
        .source( LITESQL_L( "programmes" ));
    assert(tc2.asString() ==  LITESQL_L( "SELECT id,name,filename FROM programmes" ));
    
        
    return 0;
}