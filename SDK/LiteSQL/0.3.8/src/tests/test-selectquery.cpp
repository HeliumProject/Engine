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
int main(int argc, LiteSQL_Char *argv[]) {
    // TC1
    SelectQuery tc1;
    tc1.distinct(true).limit(15).offset(5)
        .result( LiteSQL_L( "a.id" )).result( LiteSQL_L( "b.id" )).result( LiteSQL_L( "count(b.pages)" ))
        .source( LiteSQL_L( "alphabets" ),  LiteSQL_L( "a" )).source( LiteSQL_L( "books" ),  LiteSQL_L( "b" ))
        .where( LiteSQL_L( "a.id = b.id" ))
        .groupBy( LiteSQL_L( "b.id" ))
        .having( LiteSQL_L( "count(b.pages) > 5" ))
        .orderBy( LiteSQL_L( "a.id" ));
    assert(tc1.asString() == 
            LiteSQL_L( "SELECT DISTINCT a.id,b.id,count(b.pages) " )
            LiteSQL_L( "FROM alphabets AS a,books AS b " )
            LiteSQL_L( "WHERE a.id = b.id " )
            LiteSQL_L( "GROUP BY b.id " )
            LiteSQL_L( "HAVING count(b.pages) > 5 " )
            LiteSQL_L( "ORDER BY a.id LIMIT 15 OFFSET 5" ));
    // TC2
    SelectQuery tc2;
    tc2.result( LiteSQL_L( "id" )).result( LiteSQL_L( "name" )).result( LiteSQL_L( "filename" ))
        .source( LiteSQL_L( "programmes" ));
    assert(tc2.asString() ==  LiteSQL_L( "SELECT id,name,filename FROM programmes" ));
    
        
    return 0;
}