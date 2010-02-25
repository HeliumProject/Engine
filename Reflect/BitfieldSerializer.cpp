#include "stdafx.h"
#include "BitfieldSerializer.h"

#include "Console/Console.h"

using namespace Reflect;

REFLECT_DEFINE_CLASS(BitfieldSerializer);

BitfieldSerializer::BitfieldSerializer ()
{

}

BitfieldSerializer::~BitfieldSerializer()
{

}

void BitfieldSerializer::Serialize(Archive& archive) const
{
  switch (archive.GetType())
  {
  case ArchiveTypes::XML:
    {
      std::string str;

      if (m_Enumeration)
      {
        if (!m_Enumeration->GetBitfieldString(m_Data.Get(), str))
        {
          throw Reflect::TypeInformationException( "Unable to serialize bitfield '%s', value %d",
            m_Enumeration->m_ShortName.c_str(), m_Data.Get() );
        }
      }

      archive.GetOutput() << str;

      break;
    }

  case ArchiveTypes::Binary:
    {
      i32 index = -1;

      V_string strs;

      if (m_Enumeration)
      {
        if (!m_Enumeration->GetBitfieldStrings(m_Data.Get(), strs))
        {
          throw Reflect::TypeInformationException( "Unable to serialize bitfield '%s', value %d",
            m_Enumeration->m_ShortName.c_str(), m_Data.Get() );
        }

        // search the map
        V_string::const_iterator itr = strs.begin();
        V_string::const_iterator end = strs.end();
        for ( ; itr != end; ++itr )
        {
          index = static_cast<ArchiveBinary&>(archive).GetStrings().AssignIndex(*itr);
          archive.GetOutput().Write(&index); 
        }
      }

      // term
      index = -1;
      archive.GetOutput().Write(&index); 

      break;
    }
  }

  if (m_Enumeration == NULL)
  {
    throw Reflect::TypeInformationException( "Missing type information" );
  }
}

void BitfieldSerializer::Deserialize(Archive& archive)
{
  switch (archive.GetType())
  {
  case ArchiveTypes::XML:
    {
      std::string buf;
      archive.GetInput() >> buf;

      if (m_Enumeration && !m_Enumeration->GetBitfieldValue(buf, m_Data.Ref()))
      {
        archive.Debug( "Unable to deserialize bitfield %s values '%s'\n", m_Enumeration->m_ShortName.c_str(), buf );
      }
      else
      {
        m_String = buf;
      }

      break;
    }

  case ArchiveTypes::Binary:
    {
      i32 index = -1;
      archive.GetInput().Read(&index); 

      V_string strs;
      while (index >= 0)
      {
        strs.push_back(static_cast<ArchiveBinary&>(archive).GetStrings().GetString(index));

        // read next index
        archive.GetInput().Read(&index); 
      }

      std::string str;
      V_string::const_iterator itr = strs.begin();
      V_string::const_iterator end = strs.end();
      for ( ; itr != end; ++itr )
      {
        if (itr != strs.begin())
          str += "|";

        str += *itr;
      }

      if (m_Enumeration && !m_Enumeration->GetBitfieldValue(strs, m_Data.Ref()))
      {
        archive.Debug( "Unable to deserialize bitfield %s values '%s'\n", m_Enumeration->m_ShortName.c_str(), str.c_str() );
      }
      else
      {
        m_String = str;
      }

      break;
    }
  }

  if (m_Enumeration == NULL)
  {
    throw Reflect::TypeInformationException( "Missing type information" );
  }
}

std::ostream& BitfieldSerializer::operator >> (std::ostream& stream) const
{
  std::string str;
  if (!m_Enumeration->GetBitfieldString(m_Data.Get(), str))
  {
    // something is amiss, we should be guaranteed serialization of enum elements
    NOC_BREAK();
  }

  stream << str;

  return stream;
}

std::istream& BitfieldSerializer::operator << (std::istream& stream)
{
  std::string buf;
  stream >> buf;
  m_Enumeration->GetBitfieldValue(buf, m_Data.Ref());

  return stream;
}
