#include "EventSystemEvent.h"

#include <sstream>
#include <time.h>
#include <sys/timeb.h>

using namespace Helium::ES;

tstring Event::Dump() const
{
  if (IsValid())
  {
    // tuid          m_Id;
    // uint64_t           m_Created;
    // tstring   m_Username;
    // tstring   m_Data;
    tostringstream str;

    tchar timestr[64];
    __time64_t created = m_Created / 1000;
    _tctime64_s( timestr, sizeof(timestr), &created );
    timestr[ _tcslen(timestr) - 1 ] = '\0';

    str << TXT( "ID        : " ) << TUID::HexFormat << m_Id << std::dec << std::endl;
    str << TXT( "Created   : " ) << timestr << TXT( " (+" ) << m_Created % 1000 << TXT( " ms)" ) << std::endl;
    str << TXT( "Username  : " ) << m_Username << std::endl;
    str << TXT( "Data      : " ) << m_Data << std::endl;

    return str.str();
  }
  else
  {
    return TXT( "Invalid" );
  }
}