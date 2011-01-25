#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "litesql/except.hpp"

namespace litesql 
{
  class Logger {
  public:
    static void report(const LITESQL_String& msg,const LITESQL_String& msg2);
    static void report(const LITESQL_String& msg);
    static void error(const LITESQL_String& msg,const LITESQL_String& msg2);
    static void error(const LITESQL_String& msg2);
    static void error(const Except& ex);
    static void verbose(bool verbose);
  };
}

#endif // #ifndef LOGGER_HPP