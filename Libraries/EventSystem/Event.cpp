#include "Event.h"

#include <sstream>
#include <time.h>
#include <sys/timeb.h>

using namespace ES;

std::string Event::Dump() const
{
  if (IsValid())
  {
    // tuid          m_Id;
    // u64           m_Created;
    // std::string   m_Username;
    // std::string   m_Data;
    std::ostringstream str;

    char timestr[64];
    __time64_t created = m_Created / 1000;
    _ctime64_s( timestr, sizeof(timestr), &created );
    timestr[ strlen(timestr) - 1 ] = '\0';

    str << "ID        : " << TUID::HexFormat << m_Id << std::dec << std::endl;
    str << "Created   : " << timestr << " (+" << m_Created % 1000 << " ms)" << std::endl;
    str << "Username  : " << m_Username << std::endl;
    str << "Data      : " << m_Data << std::endl;

    return str.str();
  }
  else
  {
    return "Invalid";
  }
}