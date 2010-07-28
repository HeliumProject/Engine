/* LiteSQL - test-datetime
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */

#include <assert.h>

#include "litesql/datetime.hpp"
/*
  Datetime unit tester

  TC1: test for equality on load/ save (see ticket #13)
*/
using namespace litesql;

int main(int argc, LiteSQL_Char *argv[]) {
    // TC1 for DateTime
   DateTime dt;
   LiteSQL_String dtstring = dt.asString();

   DateTime dt2 = convert<const LiteSQL_String&, DateTime>(dtstring);
   
   assert(dt.timeStamp() == dt2.timeStamp()); 

   // TC1 for Date
   Date d;
   LiteSQL_String dstring = d.asString();

   Date d2 = convert<const LiteSQL_String&, Date>(dstring);
   
   assert(d.timeStamp() == d2.timeStamp()); 

   // TC1 for Time
   Time t;
   LiteSQL_String tstring = t.asString();

   Time t2 = convert<const LiteSQL_String&, Time>(tstring);
   
   assert(t.secs() == t2.secs()); 



   return 0;
}