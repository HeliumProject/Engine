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
  //
  // Wrapper for Microsoft GUID/Uuid - requires RPCRT4.LIB
  //

  class UNIQUEID_API GUID
  {
  private:
    u32 Data1;
    u16 Data2;
    u16 Data3;
    u8 Data4[ 8 ];

  public:
    // Null ID
    GUID();

    // Copy an ID
    GUID(const GUID &id);

    // Duplicate an ID
    GUID& operator=(const GUID &rhs);

    // Equality check
    bool operator==(const GUID &rhs) const;

    // Inequality check
    bool operator!=(const GUID &rhs) const;

    // Comparator
    bool operator<(const GUID &rhs) const;

    // Convert to a tuid
    void ToTUID(tuid& id) const;

    // String representation of the ID
    void ToString(std::string& id) const;

    // Convert from a tuid
    void FromTUID(tuid id);

    // Sets the ID from a string representation
    bool FromString(const std::string& id);

    // Resets the ID to be kNullID
    void Reset();

    // Generates a unique ID
    static GUID Generate();

    // Generates a unique ID
    static void Generate(GUID& uid);

    // The null ID
    static const GUID Null;

    friend UNIQUEID_API std::ostream& operator<<(std::ostream& stream, const GUID& id);
    friend UNIQUEID_API std::istream& operator>>(std::istream& stream, GUID& id);
    friend void Nocturnal::Swizzle<UniqueID::GUID>(UniqueID::GUID& val, bool swizzle);
  };

  UNIQUEID_API inline std::ostream& operator<<(std::ostream& stream, const GUID& id)
  {
    std::string s;
    id.ToString(s);
    stream << s;
    return stream;
  }

  UNIQUEID_API inline std::istream& operator>>(std::istream& stream, GUID& id)
  {
    std::string s;
    stream >> s;
    id.FromString(s.c_str());
    return stream;
  }


  // 
  // Hashing class for storing UIDs as keys to a hash_map.
  // 

  class GUIDHasher : public stdext::hash_compare< UniqueID::GUID >
  {
  public:
    size_t operator()( const UniqueID::GUID& guid ) const
    {
      // Based on hashing algorithm found at:
      // http://source.winehq.org/source/tools/widl/write_msft.c
      size_t hash = 0;

      const short* data = reinterpret_cast<const short*>( &guid );
      hash ^= *data++;
      hash ^= *data++;
      hash ^= *data++;
      hash ^= *data++;
      hash ^= *data++;
      hash ^= *data++;
      hash ^= *data++;
      hash ^= *data;

      return hash & 0x1f;
    }

    bool operator()( const UniqueID::GUID& guid1, const UniqueID::GUID& guid2 ) const
    {
      return guid1 < guid2;
    }
  };

  // STL HashMap of IDs
  typedef stdext::hash_map< UniqueID::GUID, UniqueID::GUID, UniqueID::GUIDHasher > HM_GUID;
  
  // STL HashMap of IDs to u32's
  typedef stdext::hash_map< UniqueID::GUID, u32, UniqueID::GUIDHasher > HM_GUIDU32;

  // STL Vector of IDs
  typedef std::vector<GUID> V_GUID;

  // STL Set of IDs
  typedef std::set<GUID> S_GUID;  
}

namespace Nocturnal
{
  template<> inline void Swizzle<UniqueID::GUID>(UniqueID::GUID& val, bool swizzle)
  {
    val.Data1 = ConvertEndian(val.Data1, swizzle);
    val.Data2 = ConvertEndian(val.Data2, swizzle);
    val.Data3 = ConvertEndian(val.Data3, swizzle);
  }
}