#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <hash_map>

#include "API.h"
#include "Common/Types.h"
#include "Common/Memory/Endian.h"

typedef u64 tuid;

namespace UniqueID
{
  class GUID;

  //
  // Wrapper for TUID.dll's u64
  //

  class UNIQUEID_API TUID
  {
  protected:
    tuid m_ID;

  public:
    // Null ID
    TUID();

    // Copy from tuid
    TUID(tuid id);

    // Copy an ID
    TUID(const TUID &id);

    // Duplicate an ID
    TUID& operator=(const TUID &rhs);

    // Equality check
    bool operator==(const TUID &rhs) const;

    // Inequality check
    bool operator!=(const TUID &rhs) const;

    // Comparator
    bool operator<(const TUID &rhs) const;

    // Interop with tuid
    operator tuid() const;

    // Convert to a tuid
    void ToGUID(GUID& id) const;

    // String representation of the ID
    void ToString(std::string& id) const;

    // Convert from a tuid
    void FromGUID(const GUID& id);

    // Sets the ID from a string representation
    bool FromString(const std::string& id);

    // Resets the ID to be the null ID
    void Reset();

    // Generates a unique ID
    static TUID Generate();

    // Generates a unique ID
    static void Generate(TUID& uid);

    // The null ID
    static const TUID Null;

    friend UNIQUEID_API std::ostream& operator<<(std::ostream& stream, const TUID& id);
    friend UNIQUEID_API std::istream& operator>>(std::istream& stream, TUID& id);
  };

  inline UniqueID::TUID::TUID()
    : m_ID (0x0)
  {

  }

  inline UniqueID::TUID::TUID(tuid id)
    : m_ID (id)
  {

  }

  inline UniqueID::TUID::TUID(const UniqueID::TUID &id)
    : m_ID (id.m_ID)
  {

  }

  inline UniqueID::TUID& UniqueID::TUID::operator=(const UniqueID::TUID &rhs)
  {
    m_ID = rhs.m_ID;
    return *this;
  }

  inline bool UniqueID::TUID::operator==(const UniqueID::TUID &rhs) const
  {
    return m_ID == rhs.m_ID;
  }

  inline bool UniqueID::TUID::operator!=(const UniqueID::TUID &rhs) const
  {
    return m_ID != rhs.m_ID;
  }

  inline bool UniqueID::TUID::operator<(const UniqueID::TUID &rhs) const
  {
    return m_ID < rhs.m_ID;
  }

  inline UniqueID::TUID::operator tuid() const
  {
    return m_ID;
  }

  inline void UniqueID::TUID::Reset()
  {
    m_ID = 0x0;
  }

  UNIQUEID_API inline std::ostream& operator<<(std::ostream& stream, const TUID& id)
  {
    std::string s;
    id.ToString(s);
    stream << s;
    return stream;
  }

  UNIQUEID_API inline std::istream& operator>>(std::istream& stream, TUID& id)
  {
    std::string s;
    stream >> s;
    id.FromString(s.c_str());
    return stream;
  }


  // 
  // Hashing class for storing UIDs as keys to a hash_map.
  // 

  class TUIDHasher : public stdext::hash_compare< u64 >
  {
  public:
    size_t operator()( const UniqueID::TUID& tuid ) const
    {
      return stdext::hash_compare< u64 >::operator()( 0 );
    }

    bool operator()( const UniqueID::TUID& tuid1, const UniqueID::TUID& tuid2 ) const
    {
      return stdext::hash_compare< u64 >::operator()( tuid1, tuid2 );
    }
  };

  // STL HashMap of IDs
  typedef stdext::hash_map< UniqueID::TUID, UniqueID::TUID, UniqueID::TUIDHasher > HM_TUID;
  
  // STL HashMap of IDs to u32's
  typedef stdext::hash_map< UniqueID::TUID, u32, UniqueID::TUIDHasher > HM_TUIDU32;

  // STL Vector of IDs
  typedef std::vector<TUID> V_TUID;

  // STL Set of IDs
  typedef std::set<TUID> S_TUID;
}

namespace Nocturnal
{
  template<> inline void Swizzle<UniqueID::TUID>(UniqueID::TUID& val, bool swizzle)
  {
    // operator tuid() const will handle the conversion into the other swizzle func
    val = ConvertEndian(val, swizzle);
  }
}