/* LiteSQL - test-split
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */

#include "litesql/split.hpp"
//#include "litesql.hpp"
#include <cassert>
/*
  Split unit tester
  
  TC1: split  LiteSQL_L( "abc def ghj iklm" ) with default delimiter ( LiteSQL_L( " " ))
  TC2: split  LiteSQL_L( "123-*-4567-*-7891011" ) with delimiter -*-
  TC3: split  LiteSQL_L( "nodelimiters" ) with default delimiter ( LiteSQL_L( " " ))
  TC5: slice with positive values
  TC6: slice with negative values
  TC7: join 
 */
using namespace litesql;

int main(int argc, LiteSQL_Char *argv[]) {
    // TC1
    Split tc1( LiteSQL_L( "abc def ghj iklm" ));
    assert(tc1.size()==4);
    assert(tc1[0]== LiteSQL_L( "abc" ));
    assert(tc1[1]== LiteSQL_L( "def" ));
    assert(tc1[2]== LiteSQL_L( "ghj" ));
    assert(tc1[3]== LiteSQL_L( "iklm" ));
    // TC2
    Split tc2( LiteSQL_L( "123-*-4567-*-7891011" ),  LiteSQL_L( "-*-" ));
    assert(tc2.size()==3);
    assert(tc2[0]== LiteSQL_L( "123" ));
    assert(tc2[1]== LiteSQL_L( "4567" ));
    assert(tc2[2]== LiteSQL_L( "7891011" ));
    // TC3
    Split tc3( LiteSQL_L( "nodelimiters" ));
    assert(tc3.size() == 1);
    assert(tc3[0] ==  LiteSQL_L( "nodelimiters" ));
    // TC5
    Split tc5 = Split( LiteSQL_L( "1 23 456 7891" )).slice(1, 3);
    assert(tc5.size()==2);
    assert(tc5[0]== LiteSQL_L( "23" ));
    assert(tc5[1]== LiteSQL_L( "456" ));
    // TC6
	Split tc6 = Split( LiteSQL_L( "4 2353 45126 71891" )).slice(-3, -1);
    assert(tc6.size()==2);
    assert(tc6[0]== LiteSQL_L( "2353" ));
    assert(tc6[1]== LiteSQL_L( "45126" ));
	// TC7
	assert(Split( LiteSQL_L( "ab|c|def|ghij" ), LiteSQL_L( "|" )).join( LiteSQL_L( " " ))== LiteSQL_L( "ab c def ghij" ));

  LiteSQL_String s_join = Split::join(Split( LiteSQL_L( "ab|c|def|ghij" ), LiteSQL_L( "|" )), LiteSQL_L( " " ));
  assert(s_join== LiteSQL_L( "ab c def ghij" ));


    return 0;
}