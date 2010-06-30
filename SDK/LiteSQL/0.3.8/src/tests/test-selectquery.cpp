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
int main(int argc, char *argv[]) {
    // TC1
    SelectQuery tc1;
    tc1.distinct(true).limit(15).offset(5)
        .result("a.id").result("b.id").result("count(b.pages)")
        .source("alphabets", "a").source("books", "b")
        .where("a.id = b.id")
        .groupBy("b.id")
        .having("count(b.pages) > 5")
        .orderBy("a.id");
    assert(tc1.asString() == 
           "SELECT DISTINCT a.id,b.id,count(b.pages) "
           "FROM alphabets AS a,books AS b "
           "WHERE a.id = b.id "
           "GROUP BY b.id "
           "HAVING count(b.pages) > 5 "
           "ORDER BY a.id LIMIT 15 OFFSET 5");
    // TC2
    SelectQuery tc2;
    tc2.result("id").result("name").result("filename")
        .source("programmes");
    assert(tc2.asString() == "SELECT id,name,filename FROM programmes");
    
        
    return 0;
}
