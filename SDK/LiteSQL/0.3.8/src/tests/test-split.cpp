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
  
  TC1: split "abc def ghj iklm" with default delimiter (" ")
  TC2: split "123-*-4567-*-7891011" with delimiter -*-
  TC3: split "nodelimiters" with default delimiter (" ")
  TC5: slice with positive values
  TC6: slice with negative values
  TC7: join 
 */
using namespace litesql;

int main(int argc, char *argv[]) {
    // TC1
    Split tc1("abc def ghj iklm");
    assert(tc1.size()==4);
    assert(tc1[0]=="abc");
    assert(tc1[1]=="def");
    assert(tc1[2]=="ghj");
    assert(tc1[3]=="iklm");
    // TC2
    Split tc2("123-*-4567-*-7891011", "-*-");
    assert(tc2.size()==3);
    assert(tc2[0]=="123");
    assert(tc2[1]=="4567");
    assert(tc2[2]=="7891011");
    // TC3
    Split tc3("nodelimiters");
    assert(tc3.size() == 1);
    assert(tc3[0] == "nodelimiters");
    // TC5
    Split tc5 = Split("1 23 456 7891").slice(1, 3);
    assert(tc5.size()==2);
    assert(tc5[0]=="23");
    assert(tc5[1]=="456");
    // TC6
	Split tc6 = Split("4 2353 45126 71891").slice(-3, -1);
    assert(tc6.size()==2);
    assert(tc6[0]=="2353");
    assert(tc6[1]=="45126");
	// TC7
	assert(Split("ab|c|def|ghij","|").join(" ")=="ab c def ghij");

  std::string s_join = Split::join(Split("ab|c|def|ghij","|")," ");
  assert(s_join=="ab c def ghij");


    return 0;
}
