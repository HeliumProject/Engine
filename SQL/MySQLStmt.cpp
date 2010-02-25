#include "stdafx.h"

#include "MySQLStmt.h"

#include <mysql.h>

using namespace SQL;

MySQLStmt::~MySQLStmt()
{
  if ( m_InputBinds )
  {
    delete [] m_InputBinds;
  }

  if ( m_InputBindables )
  {
    for ( u32 i = 0; i < m_InputBindCount; ++i )
    {
      if ( m_InputBindables[ i ]._text )
      {
        delete m_InputBindables[ i ]._text;
      }
      if ( m_InputBindables[ i ]._blob )
      {
        delete m_InputBindables[ i ]._blob;
      }
    }
    delete [] m_InputBindables;
  }

  if ( m_InputBindLengths )
  {
    delete [] m_InputBindLengths;
  }

  if ( m_OutputBinds )
  {
    delete [] m_OutputBinds;
  }

  if ( m_OutputBindables )
  {
    for ( u32 i = 0; i < m_OutputBindCount; ++i )
    {
      if ( m_OutputBindables[ i ]._text )
      {
        delete m_OutputBindables[ i ]._text;
      }
      if ( m_OutputBindables[ i ]._blob )
      {
        delete m_OutputBindables[ i ]._blob;
      }
    }
    delete [] m_OutputBindables;
  }

  if ( m_OutputBindLengths )
  {
    delete [] m_OutputBindLengths;
  }

}
