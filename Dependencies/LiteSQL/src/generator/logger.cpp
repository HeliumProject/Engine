#include "logger.hpp"
#
#include <iostream>

using namespace litesql; 

static bool logger_verbose = false;

void Logger::verbose(bool verbose)
{
  logger_verbose = verbose;
}

void Logger::report(const LITESQL_String& msg,const LITESQL_String& msg2)
{
  if (logger_verbose)
    LITESQL_cout  << msg  << msg2 << std::endl;
}

void Logger::report(const LITESQL_String& msg)
{
  if (logger_verbose)
    LITESQL_cout  << msg << std::endl;
}

void Logger::error(const LITESQL_String& msg)
{
    LITESQL_cerr  << msg << std::endl;
}

void Logger::error(const LITESQL_String& msg,const LITESQL_String& msg2)
{
    LITESQL_cerr  << msg << msg2 << std::endl;
}

void Logger::error(const Except& ex)
{
    LITESQL_cerr  << ex << std::endl;
}

