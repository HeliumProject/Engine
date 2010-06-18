#pragma once

#include "API.h"
#include "Platform/Types.h"

namespace Reflect
{
  class FOUNDATION_API TypeID
  {
  public:
    TypeID()
      : m_ID ( -1 )
    {

    }

    TypeID( i32 id )
      : m_ID ( id )
    {

    }

    i32 GetID() const
    {
      return m_ID;
    }

    bool operator==( const TypeID& rhs ) const
    {
      return m_ID == rhs.m_ID;
    }

    bool operator!=( const TypeID& rhs ) const
    {
      return m_ID != rhs.m_ID;
    }

    bool operator<( const TypeID& rhs ) const
    {
      return m_ID < rhs.m_ID;
    }

    operator i32() const
    {
      return m_ID;
    }

  private:
    i32 m_ID;
  };
}