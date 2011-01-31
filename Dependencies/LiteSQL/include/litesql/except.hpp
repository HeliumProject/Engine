/* LiteSQL 
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */

#ifndef _litesql_except_hpp
#define _litesql_except_hpp
#include "litesql_char.hpp"
#include <iostream>
#include <string>
#include <exception>
#include "litesql/utils.hpp"
/** \file except.hpp
    contains litesql's exception classes */
namespace litesql {
/** base class for exceptions */
	class Except : public std::exception  {
private:
    LITESQL_String msg;
public: 
    Except(LITESQL_String m) throw()  : msg(m) {}
    virtual ~Except(void) throw() {}
	virtual const char* what() const throw() {
#ifdef LITESQL_UNICODE
        static std::string stringMsg;
        LITESQL_ConvertString(msg, stringMsg);
        return stringMsg.c_str();
#else
        return msg.c_str();
#endif
    }
    friend LITESQL_oStream &operator<<(LITESQL_oStream &os, const Except &e) {
        os << e.msg;
        return os;
    }
};
/** exception thrown when a record is not found */    
class NotFound : public Except {
public:
    NotFound(LITESQL_String s=LITESQL_L("")) : Except(LITESQL_L("NotFound: ")+s) {}
};
/** exception thrown when database cannot be accessed */
class DatabaseError : public Except {
public:
    DatabaseError(LITESQL_String m) : Except(LITESQL_L("DatabaseError: ")+m) {}
};
/** exception thrown when SQL statement cannot be executed */
class SQLError : public Except {
public:
    SQLError(LITESQL_String m) : Except(LITESQL_L("SQLError: ")+m) {}
};
/** exception thrown when backend produces internal error */
class InternalError : public Except {
public:
    InternalError(LITESQL_String m) : Except(LITESQL_L("InternalError: ") +m) {}
};
/** exception thrown when backend cannot allocate memory */
class MemoryError : public Except {
public:
    MemoryError(LITESQL_String m) : Except(LITESQL_L("Allocation failed: ")+m){}
};
/** exception thrown when database (disk) is full */
class InsertionError : public Except {
public:
    InsertionError(LITESQL_String m) : Except(LITESQL_L("Database full: ")+m){}
};
/** exception thrown when none of other exceptions match */
class UnknownError : public Except {
    // handles the rest
public:
    UnknownError(LITESQL_String m) : Except(LITESQL_L("UnknownError: ")+m){}
};


}
#endif 