#include "stdafx.h"
#include "Element.h"
#include "Registry.h"
#include "Serializers.h"
#include "ArchiveBinary.h"

#include "Foundation/Container/Insert.h" 
#include "Foundation/Checksum/CRC32.h"

using Nocturnal::Insert; 
using namespace Reflect; 

//#define REFLECT_DEBUG_BINARY_CRC
//#define REFLECT_DISABLE_BINARY_CRC

// version / feature management 
const u32 ArchiveBinary::CURRENT_VERSION                            = 5;
const u32 ArchiveBinary::FIRST_VERSION_WITH_ARRAY_COMPRESSION       = 3; 
const u32 ArchiveBinary::FIRST_VERSION_WITH_STRINGPOOL_COMPRESSION  = 4; 
const u32 ArchiveBinary::FIRST_VERSION_WITH_POINTER_SERIALIZER      = 5; 

// our ORIGINAL version id was '!', don't ever re-use that byte
NOC_COMPILE_ASSERT( (ArchiveBinary::CURRENT_VERSION & 0xff) != 33 );

// CRC
const u32 CRC_DEFAULT = 0x10101010;
const u32 CRC_INVALID = 0xffffffff;

#ifdef REFLECT_DEBUG_BINARY_CRC
const u32 CRC_BLOCK_SIZE = 4;
#else
const u32 CRC_BLOCK_SIZE = 4096;
#endif

// this is sneaky, but in general people shouldn't use this
namespace Reflect
{
  FOUNDATION_API bool g_OverrideCRC = false;
}

//
// Binary Archive implements our own custom serialization technique
//

ArchiveBinary::ArchiveBinary (StatusHandler* status)
: Archive (status)
, m_Version (CURRENT_VERSION)
, m_Size (0)
, m_Skip (false)
{

}

void ArchiveBinary::Read()
{
  REFLECT_SCOPE_TIMER( ("Reflect - Binary Read") );

  if (m_Status != NULL)
  {
    StatusInfo info (*this, ArchiveStates::Starting);
    m_Status->ArchiveStatus(info);
  }

  m_Abort = false;

  // determine the size of the input stream
  m_Stream->SeekRead(0, std::ios_base::end);
  m_Size = (long) m_Stream->TellRead();
  m_Stream->SeekRead(0, std::ios_base::beg);

  // fail on an empty input stream
  if ( m_Size == 0 )
  {
    throw Reflect::StreamException("Input stream is empty");
  }

  // setup visitors
  PreDeserialize();

  // read version
  u8 key = 0;
  m_Stream->Read(&key);
  if (key == '!')
  {
    // old version char was only a byte, detect that case
    m_Version = 1;
  }
  else
  {
    // new versions have a u32 version number
    m_Stream->SeekRead(0, std::ios_base::beg);
    m_Stream->Read(&m_Version); 
  }

  if(m_Version > CURRENT_VERSION)
  {
    throw Reflect::StreamException("Input stream version is higher than what is supported (input: %d, current: %d)\n", m_Version, CURRENT_VERSION); 
  }

  // read and verify CRC
  u32 crc = CRC_DEFAULT;
  u32 current_crc = crc;
  m_Stream->Read(&crc); 

#ifdef REFLECT_DISABLE_BINARY_CRC
  crc = CRC_DEFAULT;
#endif

  // if we are not the stub
  if (crc != CRC_DEFAULT)
  {
    REFLECT_SCOPE_TIMER( ("CRC Check") );

    PROFILE_SCOPE_ACCUM(g_ChecksumAccum);

    u32 count = 0;
    u8 block[CRC_BLOCK_SIZE];
    memset(block, 0, CRC_BLOCK_SIZE);
    NOC_ASSERT(current_crc == CRC_DEFAULT);

    // snapshot our starting location
    u32 start = (u32)m_Stream->TellRead();

    // roll through file
    while (!m_Stream->Done())
    {
      // read block
      m_Stream->ReadBuffer(block, CRC_BLOCK_SIZE);

      // how much we got
      u32 got = (u32) m_Stream->BytesRead();

      // crc block
      current_crc = Nocturnal::Crc32(current_crc, block, got);

#ifdef REFLECT_DEBUG_BINARY_CRC
      Log::Print("CRC %d (length %d) for datum 0x%08x is 0x%08x\n", count++, got, *(u32*)block, current_crc);
#endif
    }

    // check result
    if (crc != current_crc && !g_OverrideCRC)
    {
      if (crc == CRC_INVALID)
      {
        throw Reflect::ChecksumException( "Corruption detected, file was not successfully written (incomplete CRC)", current_crc, crc );
      }
      else
      {
        throw Reflect::ChecksumException( "Corruption detected, crc is 0x%08x, should be 0x%08x", current_crc, crc);
      }
    }

    // clear error bits
    m_Stream->Clear();

    // seek back to past our crc data to start reading our valid file
    m_Stream->SeekRead(start, std::ios_base::beg);
  }

  // load some offsets
  u32 type_offset;
  m_Stream->Read(&type_offset); 
  u32 string_offset;
  m_Stream->Read(&string_offset);
  u32 element_offset = (u32)m_Stream->TellRead();

  // deserialize string pool
  {
    REFLECT_SCOPE_TIMER( ("String Pool Read") );

    m_Stream->SeekRead(string_offset, std::ios_base::beg);

    // deserialize string table
    m_Strings.Deserialize(this); 
  }

  // deserialize type data
  {
    REFLECT_SCOPE_TIMER( ("RTTI Read") );

    m_Stream->SeekRead(type_offset, std::ios_base::beg);

    i32 type_count = -1;
    m_Stream->Read(&type_count); 
    NOC_ASSERT(type_count >= 0);

#ifdef REFLECT_ARCHIVE_VERBOSE
    Debug("Deserializing %d types\n", type_count);
#endif

    m_ClassesByID.clear();
    m_ClassesByShortName.clear();

    for (int i=0; i<type_count; i++)
    {
      ClassPtr c = Class::Create();

      DeserializeComposite(c);

      m_ClassesByID[ c->m_TypeID ] = c;
      m_ClassesByShortName[ c->m_ShortName ] = c;
    }

    i32 terminator = -1;
    m_Stream->Read(&terminator);

    if (terminator != -1)
    {
      throw Reflect::DataFormatException ("Error reading file, unterminated RTTI type block");
    }
  }

  // seek back to start of element stream
  m_Stream->SeekRead(element_offset, std::ios_base::beg);

  // set m_Size to be the size of just the instance block (2 sections)
  m_Size = (long) (type_offset - element_offset); 

  // deserialize main file elements
  {
    REFLECT_SCOPE_TIMER( ("Main Spool Read") );

    Deserialize(m_Spool, ArchiveFlags::Status);
  }

  // invalidate the search type and abort flags so we process the append block
  i32 searchType = m_SearchType;
  if ( m_SearchType != Reflect::ReservedTypes::Invalid )
  {
    m_SearchType = Reflect::ReservedTypes::Invalid;
    m_Skip = false;
  }

  V_Element append;

  // deserialize appended file elements
  {
    REFLECT_SCOPE_TIMER( ("Append Spool Read") );

    Deserialize(append);
  }

  // restore state, just in case someone wants to consume this after the fact
  m_SearchType = searchType;

  // tell visitors to process append
  PostDeserialize(append);

  if (m_Status != NULL)
  {
    StatusInfo info (*this, ArchiveStates::Complete);
    m_Status->ArchiveStatus(info);
  }
}

void ArchiveBinary::Write()
{
  REFLECT_SCOPE_TIMER( ("Reflect - Binary Write") );

  if (m_Status != NULL)
  {
    StatusInfo info (*this, ArchiveStates::Starting);
    m_Status->ArchiveStatus(info);
  }

  // setup visitors
  PreSerialize();

  // always start with the invalid crc, incase we don't make it to the end
  u32 crc = CRC_INVALID;

  // save the offset and write the invalid crc to the stream
  u32 crc_offset = (u32)m_Stream->TellWrite();
  m_Stream->Write(&crc); 

  // save some offsets to write offsets to
  u32 type_offset = (u32)m_Stream->TellWrite();
  m_Stream->Write(&type_offset); 
  u32 string_offset = (u32)m_Stream->TellWrite();
  m_Stream->Write(&string_offset);

  // serialize main file elements
  {
    REFLECT_SCOPE_TIMER( ("Main Spool Write") );

    Serialize(m_Spool, ArchiveFlags::Status);
  }

  // tell visitors to generate append
  V_Element append;
  PostSerialize(append);

  // serialize appended file elements
  {
    REFLECT_SCOPE_TIMER( ("Append Spool Write") );

    Serialize(append);
  }

  // serialize type data
  {
    REFLECT_SCOPE_TIMER( ("RTTI Write") );

    // write our current location back at our offset
    u32 type_location = (u32)m_Stream->TellWrite();
    m_Stream->SeekWrite(type_offset, std::ios_base::beg);
    m_Stream->Write(&type_location);
    m_Stream->SeekWrite(0, std::ios_base::end);

    {
#ifdef REFLECT_ARCHIVE_VERBOSE
      Debug("Serializing %d types\n", m_Types.size());
#endif

      i32 count = (int)m_Types.size();
      m_Stream->Write(&count); 

      S_i32::iterator itr = m_Types.begin();
      S_i32::iterator end = m_Types.end();
      for ( ; itr != end; ++itr )
      {
        const Class* type = Reflect::Registry::GetInstance()->GetClass(*itr);

        SerializeComposite(type);
      }

      m_Types.clear();
    }

    const static i32 terminator = -1;
    m_Stream->Write(&terminator); 
  }

  // serialize string pool
  {
    REFLECT_SCOPE_TIMER( ("String Pool Write") );

    // write our current location back at our offset
    u32 string_location = (u32)m_Stream->TellWrite();
    m_Stream->SeekWrite(string_offset, std::ios_base::beg);
    m_Stream->Write(&string_location); 
    m_Stream->SeekWrite(0, std::ios_base::end);

    // serialize string table
    m_Strings.Serialize(this); 
  }

  // CRC
  {
    REFLECT_SCOPE_TIMER( ("CRC Build") );

    u32 count = 0;
    u8 block[CRC_BLOCK_SIZE];
    memset(&block, 0, CRC_BLOCK_SIZE);

    // make damn sure this didn't change
    NOC_ASSERT(crc == CRC_INVALID);

    // reset this local back to default for computation
    crc = CRC_DEFAULT;

    // seek to our starting point (after crc location)
    m_Stream->SeekRead(crc_offset + sizeof(crc), std::ios_base::beg);

    // roll through file
    while (!m_Stream->Done())
    {
      // read block
      m_Stream->ReadBuffer(block, CRC_BLOCK_SIZE);

      // how much we got
      u32 got = (u32) m_Stream->BytesRead();

      // crc block
      crc = Nocturnal::Crc32(crc, block, got);

#ifdef REFLECT_DEBUG_BINARY_CRC
      Log::Print("CRC %d (length %d) for datum 0x%08x is 0x%08x\n", count++, got, *(u32*)block, crc);
#endif
    }

    // clear errors
    m_Stream->Clear();

    // if we just so happened to hit the invalid crc, disable crc checking
    if (crc == CRC_INVALID)
    {
      crc = CRC_DEFAULT;
    }

    // seek back and write our crc data
    m_Stream->SeekWrite(crc_offset, std::ios_base::beg);
    NOC_ASSERT(!m_Stream->Fail());
    m_Stream->Write(&crc); 

  }

  // do cleanup
  m_Stream->SeekWrite(0, std::ios_base::end);
  m_Stream->Flush();

#ifdef REFLECT_DEBUG_BINARY_CRC
  Debug("File written with size %d, crc 0x%08x\n", m_Stream->TellWrite(), crc);
#endif

  if (m_Status != NULL)
  {
    StatusInfo info (*this, ArchiveStates::Complete);
    m_Status->ArchiveStatus(info);
  }
}

void ArchiveBinary::Start()
{
  // just for good measure
  m_Version = CURRENT_VERSION;
  m_Stream->Write(&m_Version); 
}

void ArchiveBinary::Finish()
{

}

void ArchiveBinary::Serialize(const ElementPtr& element)
{
  REFLECT_SCOPE_TIMER_INST( ("Serialize %s", element->GetClass()->m_ShortName.c_str()) );

  // use the string pool index for this type's short name
  i32 index = m_Strings.AssignIndex(element->GetClass()->m_ShortName);
  m_Stream->Write(&index); 

  // get and stub out the start offset where we are now (will become length after writing is done)
  u32 start_offset = (u32)m_Stream->TellWrite();
  m_Stream->Write(&start_offset); 

#ifdef REFLECT_ARCHIVE_VERBOSE
  m_Indent.Get(stdout);
  Debug("Serializing %s (type %d)\n", element->GetClass()->m_ShortName.c_str(), element->GetType());
  m_Indent.Push();
#endif

  PreSerialize(element);

  {
    REFLECT_SCOPE_TIMER_INST( ("PreSerialize %s", element->GetClass()->m_ShortName.c_str()) );

    element->PreSerialize();
  }

  if (element->HasType(Reflect::GetType<Serializer>()))
  {
    Serializer* s = DangerousCast<Serializer>(element);

    s->Serialize(*this);
  }
  else
  {
    // push a new struct on the stack
    WriteFields data;
    data.m_Count = 0;
    data.m_CountOffset = m_Stream->TellWrite();
    m_FieldStack.push(data);

    // write some placeholder info
    m_Stream->Write(&m_FieldStack.top().m_Count);

    SerializeFields(element);

    // write our terminator
    const static i32 terminator = -1;
    m_Stream->Write(&terminator); 
    
    // seek back and write our count
    NOC_ASSERT(m_FieldStack.size() > 0);
    m_Stream->SeekWrite(m_FieldStack.top().m_CountOffset, std::ios_base::beg);
    m_Stream->Write(&m_FieldStack.top().m_Count); 
    m_FieldStack.pop();

    // seek back to end
    m_Stream->SeekWrite(0, std::ios_base::end);
  }

  {
    REFLECT_SCOPE_TIMER_INST( ("PostSerialize %s", element->GetClass()->m_ShortName.c_str()) );

    element->PostSerialize();
  }

  // save our end offset to substract the start from
  u32 end_offset = (u32)m_Stream->TellWrite();

  // seek back to the start offset
  m_Stream->SeekWrite(start_offset, std::ios_base::beg);

  // compute amound written
  u32 length = end_offset - start_offset;

  // write written amount at start offset
  m_Stream->Write(&length); 

  // seek back to the end of the stream
  m_Stream->SeekWrite(0, std::ios_base::end);

#ifdef REFLECT_ARCHIVE_VERBOSE
  m_Indent.Pop();
#endif
}

void ArchiveBinary::Serialize(const V_Element& elements, u32 flags)
{
  REFLECT_SCOPE_TIMER_INST( "" )

  i32 size = (i32)elements.size();
  m_Stream->Write(&size); 

#ifdef REFLECT_ARCHIVE_VERBOSE
  m_Indent.Get(stdout);
  Debug("Serializing %d elements\n", elements.size());
  m_Indent.Push();
#endif

  V_Element::const_iterator itr = elements.begin();
  V_Element::const_iterator end = elements.end();
  for (int index = 0; itr != end; ++itr, ++index )
  {
    Serialize(*itr);

    if (flags & ArchiveFlags::Status && m_Status != NULL)
    {
      StatusInfo info (*this, ArchiveStates::ElementProcessed);
      info.m_Progress = (int)(((float)(index) / (float)elements.size()) * 100.0f);
      m_Status->ArchiveStatus(info);
    }
  }

  if (flags & ArchiveFlags::Status && m_Status != NULL)
  {
    StatusInfo info (*this, ArchiveStates::ElementProcessed);
    info.m_Progress = 100;
    m_Status->ArchiveStatus(info);
  }

#ifdef REFLECT_ARCHIVE_VERBOSE
  m_Indent.Pop();
#endif

  const static i32 terminator = -1;
  m_Stream->Write(&terminator); 
}

void ArchiveBinary::SerializeFields(const ElementPtr& element)
{
  //
  // Serialize fields
  //

  const Class* type = element->GetClass();
  NOC_ASSERT(type != NULL);

  REFLECT_SCOPE_TIMER_INST( "" );

  M_FieldIDToInfo::const_iterator iter = type->m_FieldIDToInfo.begin();
  M_FieldIDToInfo::const_iterator end  = type->m_FieldIDToInfo.end();
  for ( ; iter != end; ++iter )
  {
    const Field* field = iter->second;
    NOC_ASSERT(field != NULL);

    SerializeField(element, field);
  }
}

void ArchiveBinary::SerializeField(const ElementPtr& element, const Field* field)
{
  REFLECT_SCOPE_TIMER_INST( ("Serialize %s", field->m_Name.c_str()) );

  // don't write no write fields
  if ( field->m_Flags & FieldFlags::Discard )
  {
    return;
  }

  // construct serialization object
  ElementPtr e;
  m_Cache.Create( field->m_SerializerID, e );

  NOC_ASSERT( e != NULL );

  // downcast serializer
  SerializerPtr serializer = ObjectCast<Serializer>(e);

  if (!serializer.ReferencesObject())
  {
    // this should never happen, the type id in the rtti data is bogus
    throw Reflect::TypeInformationException( "Invalid type id for field '%s'", field->m_Name.c_str() );
  }

  // set data pointer
  serializer->ConnectField(element.Ptr(), field);

  // bool for test results
  bool serialize = true;

  // check for equality
  if ( serialize && field->m_Default.ReferencesObject() )
  {
    bool force = (field->m_Flags & FieldFlags::Force) != 0;
    if (!force && field->m_Default->Equals(serializer))
    {
      serialize = false;
    }
  }

  // don't write empty containers
  if ( serialize &&  e->HasType( Reflect::GetType<ContainerSerializer>() ) )
  {
    ContainerSerializerPtr container = DangerousCast<ContainerSerializer>(e);

    if ( container->GetSize() == 0 )
    {
      serialize = false;
    }
  }

  // last chance to not write, call through virtual API
  if (serialize)
  {
    PreSerialize(element, field);

    // write our latent field ID to the stream, this will always be valid since we persist ALL of the RTTI data
    m_Stream->Write(&field->m_FieldID); 

#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Get(stdout);
    Debug("Serializing field %s (field id %d)\n", field->m_Name.c_str(), field->m_FieldID);
    m_Indent.Push();
#endif

    // process
    Serialize( serializer );

#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Pop();
#endif

    // we wrote a field, so increment our count
    NOC_ASSERT(m_FieldStack.size() > 0);
    m_FieldStack.top().m_Count++;
  }

  // disconnect
  serializer->Disconnect();
}

ElementPtr ArchiveBinary::Allocate()
{
  ElementPtr element;

  // read type string
  i32 index = -1;
  m_Stream->Read(&index); 
  const std::string& str = m_Strings.GetString(index);

  // read length info if we have it
  u32 length = 0;
  if (m_Version > 1)
  {
    m_Stream->Read(&length);

    if (m_Skip)
    {
      // skip it, but account for already reading the length from the stream
      m_Stream->SeekRead(length - sizeof(u32), std::ios_base::cur);

      // we should just keep processing even though we return null
      return NULL;
    }
  }

  // find type by short name string
  M_StrToClass::iterator found = m_ClassesByShortName.find(str);
  if (found == m_ClassesByShortName.end())
  {
    // we failed to find a type in the latent RTTI data, that is bad
    NOC_BREAK();
    throw Reflect::TypeInformationException("Unable to locate type '%s'", str.c_str());
  }

  // this is guaranteed to be our legacy short name name
  const std::string& shortName (found->second->m_ShortName);

  // allocate instance by short name and remap the new and different short name to the legacy short name name for later lookup
  if (m_Cache.Create(shortName, element) && shortName != element->GetClass()->m_ShortName)
  {
    // map current short name name to LEGACY short name name so we can retrieve type information via a lookup later
    Insert<M_string>::Result inserted = m_ShortNameMapping.insert( M_string::value_type (element->GetClass()->m_ShortName, shortName) );

    // check for insanity
    if ( !inserted.second && inserted.first->second != shortName )
    {
      throw Reflect::TypeInformationException("Overloaded shortName mapping for '%s', this is not supported", shortName.c_str());
    }
  }

  // if we failed
  if (!element.ReferencesObject())
  {
    if (m_Version > 1)
    {
      // skip it, but account for already reading the length from the stream
      m_Stream->SeekRead(length - sizeof(u32), std::ios_base::cur);

      // if you see this, then data is being lost because:
      //  1 - a type was completely removed from the codebase
      //  2 - a type was not found because its type library is not registered
      Debug("Unable to create object of type '%s', size %d, skipping...\n", str.c_str(), length);
    }
    else
    {
      NOC_BREAK();
      throw Reflect::DataFormatException("Unable to create object, unknown type '%s'", str.c_str());
    }
  }

  return element;
}

void ArchiveBinary::Deserialize(ElementPtr& element)
{
  //
  // If we don't have an object allocated for deserialization, pull one from the stream
  //

  if (!element.ReferencesObject())
  {
    element = Allocate();
  }

  //
  // We should now have an instance (unless data was skipped)
  //

  if (element.ReferencesObject())
  {
    REFLECT_SCOPE_TIMER_INST( ("Deserialize %s", element->GetClass()->m_ShortName.c_str()) );

#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Get(stdout);
    Debug("Deserializing %s (type %d)\n", element->GetClass()->m_ShortName.c_str(), element->GetType());
    m_Indent.Push();
#endif

    {
      REFLECT_SCOPE_TIMER_INST( ("PreDeserialize %s", element->GetClass()->m_ShortName.c_str()) );

      element->PreDeserialize();
    }

    if (element->HasType(Reflect::GetType<Serializer>()))
    {
      Serializer* s = DangerousCast<Serializer>(element);

      s->Deserialize(*this);
    }
    else
    {
      DeserializeFields(element);
    }

    {
      REFLECT_SCOPE_TIMER_INST( ("PostDeserialize %s", element->GetClass()->m_ShortName.c_str()) );

      if ( !TryElementCallback( element, &Element::PostDeserialize ) )
      {
        element = NULL; // discard the object
      }
    }

    if ( element )
    {
      PostDeserialize(element);
    }

#ifdef REFLECT_ARCHIVE_VERBOSE
    m_Indent.Pop();
#endif
  }
}

void ArchiveBinary::Deserialize(V_Element& elements, u32 flags)
{
  u32 start_offset = (u32)m_Stream->TellRead();

  i32 element_count = -1;
  m_Stream->Read(&element_count); 

  REFLECT_SCOPE_TIMER_INST( "" )

#ifdef REFLECT_ARCHIVE_VERBOSE
  m_Indent.Get(stdout);
  Debug("Deserializing %d elements\n", element_count);
  m_Indent.Push();
#endif

  if (element_count > 0)
  {
    for (int i=0; i<element_count && !m_Abort; i++)
    {
      ElementPtr element;
      Deserialize(element);

      if (element.ReferencesObject())
      {
        if (element->HasType(m_SearchType))
        {
          m_Skip = true;
        }

        if (flags & ArchiveFlags::Status && m_Status != NULL)
        {
          u32 current = (u32)m_Stream->TellRead();

          StatusInfo info (*this, ArchiveStates::ElementProcessed);
          info.m_Progress = (int)(((float)(current - start_offset) / (float)m_Size) * 100.0f);
          m_Status->ArchiveStatus(info);

          m_Abort |= info.m_Abort;
        }
      }

      if (element.ReferencesObject() || flags & ArchiveFlags::Sparse)
      {
        elements.push_back(element);
      }
    }
  }

#ifdef REFLECT_ARCHIVE_VERBOSE
  m_Indent.Pop();
#endif

  if (!m_Abort)
  {
    i32 terminator = -1;
    m_Stream->Read(&terminator);

    if (terminator != -1)
    {
      throw Reflect::DataFormatException ("Unterminated element array block");
    }
  }

  if (flags & ArchiveFlags::Status && m_Status != NULL)
  {
    StatusInfo info (*this, ArchiveStates::ElementProcessed);
    info.m_Progress = 100;
    m_Status->ArchiveStatus(info);
  }
}

void ArchiveBinary::DeserializeFields(const ElementPtr& element)
{
  i32 field_count = -1;
  m_Stream->Read(&field_count); 

  REFLECT_SCOPE_TIMER_INST( "" )

  if (field_count > 0)
  {
    const Class* type = NULL;

    // find the type of this object
    M_StrToClass::iterator type_found = m_ClassesByShortName.find(element->GetClass()->m_ShortName);

    // get Element's type info
    if ( type_found != m_ClassesByShortName.end() )
    {
      type = type_found->second;
    }
    else
    {
      // our short name has changed so look up the legacy short name name given the short name of the current object
      M_string::const_iterator shortName_found = m_ShortNameMapping.find( element->GetClass()->m_ShortName );

      // we should always find it, else its a bug/internal error
      if ( shortName_found == m_ShortNameMapping.end() )
      {
        throw Reflect::TypeInformationException("Unable to remap short name '%s'", element->GetClass()->m_ShortName.c_str());
      }

      // we throw if there is an internal error, so just dereference the result
      type = m_ClassesByShortName.find( shortName_found->second )->second;
    }

    if (type == NULL)
    {
      Debug("Unable to resolve type from short name '%s'\n", element->GetClass()->m_ShortName.c_str());
    }

    // while we haven't hit the terminator
    for (int i=0; i<field_count; i++)
    {
      i32 field_id = -1;
      m_Stream->Read(&field_id); 

      if (type != NULL)
      {
        M_FieldIDToInfo::const_iterator field_found = type->m_FieldIDToInfo.find(field_id);
        NOC_ASSERT(field_found != type->m_FieldIDToInfo.end());

#ifdef REFLECT_ARCHIVE_VERBOSE
        m_Indent.Get(stdout);
        Debug("Deserializing field %s (field id %d)\n", field_found->second->m_Name.c_str(), field_id);
        m_Indent.Push();
#endif

        // process
        DeserializeField(element, field_found->second);

#ifdef REFLECT_ARCHIVE_VERBOSE
        m_Indent.Pop();
#endif
      }
    }
  }

  i32 terminator = -1;
  m_Stream->Read(&terminator); 

  if (terminator != -1)
  {
    throw Reflect::DataFormatException ("Unterminated field array block");
  }
}

void ArchiveBinary::DeserializeField(const ElementPtr& element, const Field* latent_field)
{
  REFLECT_SCOPE_TIMER_INST( ("Deserialize %s", latent_field->m_Name.c_str()) );

  // get the type info for the instance we are writing too
  const Class* type = element->GetClass();

  // lookup the current field info by latent field name
  M_FieldNameToInfo::const_iterator found = type->m_FieldNameToInfo.find(latent_field->m_Name);

  // our missing component
  ElementPtr component;

  // the field to serialize
  FieldPtr current_field;

  // sanity check
  if (found == type->m_FieldNameToInfo.end())
  {
    current_field = NULL;
  }
  else
  {
    current_field = found->second;
  }

  if ( GetVersion() < ArchiveBinary::FIRST_VERSION_WITH_POINTER_SERIALIZER && latent_field->m_SerializerID == Reflect::GetType<PointerSerializer>() )
  {
    // the address of a pointer to an element
    ElementPtr* elementLocation = &component;

    // if we still have a pointer field, use it
    if (current_field.ReferencesObject())
    {
      SerializerPtr serializer = current_field->CreateSerializer( element );
      PointerSerializer* pointerSerializer = ObjectCast<PointerSerializer>( serializer );
      if ( pointerSerializer )
      {
        elementLocation = &pointerSerializer->m_Data.Ref();
        *elementLocation = NULL;
      }
    }

    // process
    Deserialize( *elementLocation );

    // post process
    PostDeserialize( element, current_field );
  }
  else
  {
    if ( current_field.ReferencesObject() )
    {
      // pull and element and downcast to serializer
      SerializerPtr latent_serializer = ObjectCast<Serializer>( Allocate() );

      if (!latent_serializer.ReferencesObject())
      {
        // this should never happen, the type id read from the file is bogus
        throw Reflect::TypeInformationException( "Invalid type id for field '%s'", latent_field->m_Name.c_str() );
      }

      // keep in mind that m_SerializerID of latent field is the current type id that matches the latent short name
      if (current_field->m_SerializerID == latent_field->m_SerializerID)
      {
        // set data pointer
        latent_serializer->ConnectField( element.Ptr(), current_field );

        // process natively
        Deserialize( (ElementPtr&)latent_serializer );

        // post process
        PostDeserialize( element, current_field );

        // disconnect
        latent_serializer->Disconnect();
      }
      else
      {
        REFLECT_SCOPE_TIMER(("Casting"));

        // construct current serialization object
        ElementPtr current_element;
        m_Cache.Create( current_field->m_SerializerID, current_element );

        // downcast to serializer
        SerializerPtr current_serializer = ObjectCast<Serializer>(current_element);
        if (!current_serializer.ReferencesObject())
        {
          // this should never happen, the type id in the rtti data is bogus
          throw Reflect::TypeInformationException( "Invalid type id for field '%s'", current_field->m_Name.c_str() );
        }

        // process into temporary memory
        current_serializer->ConnectField(element.Ptr(), current_field);

        // process natively
        Deserialize( (ElementPtr&)latent_serializer );

        // attempt cast data into new definition
        if (!Serializer::CastValue( latent_serializer, current_serializer, SerializerFlags::Shallow ))
        {
          // to the component block!
          component = latent_serializer;
        }
        else
        {
          // post process
          PostDeserialize( element, current_field );
        }

        // disconnect
        current_serializer->Disconnect();
      }
    }
    else
    {
      try
      {
        // attempt to process our lost component natively
        Deserialize( component );
      }
      catch (Reflect::LogisticException& ex)
      {
        Debug("Unable to deserialize %s::%s into component (%s), discarding\n", type->m_ShortName.c_str(), latent_field->m_Name.c_str(), ex.what());
      }
    }
  }

  if (component.ReferencesObject())
  {
    // attempt processing
    if (!element->ProcessComponent(component, latent_field->m_Name))
    {
      Debug("%s did not process %s, discarding\n", element->GetClass()->m_ShortName.c_str(), component->GetClass()->m_ShortName.c_str());
    }
  }
}

void ArchiveBinary::SerializeComposite(const Composite* composite)
{
#ifdef REFLECT_ARCHIVE_VERBOSE
  Log::Debug(" Serializing %s (%d fields)\n", m_ShortName.c_str(), m_FieldIDToInfo.size());
#endif

  i32 string_index = m_Strings.AssignIndex(composite->m_ShortName);
  m_Stream->Write(&string_index); 
  m_Stream->Write(&composite->m_TypeID); 

  i32 field_count = (i32)composite->m_FieldIDToInfo.size();
  m_Stream->Write(&field_count);

  M_FieldIDToInfo::const_iterator itr = composite->m_FieldIDToInfo.begin();
  M_FieldIDToInfo::const_iterator end = composite->m_FieldIDToInfo.end();
  for ( ; itr != end; ++itr )
  {
    SerializeField(itr->second);
  }

  const static i32 terminator = -1;
  m_Stream->Write(&terminator); 
}

bool ArchiveBinary::DeserializeComposite(Composite* composite)
{
  i32 string_index = -1;
  m_Stream->Read(&string_index); 
  composite->m_ShortName = m_Strings.GetString(string_index);

  m_Stream->Read(&composite->m_TypeID); 

  i32 field_count = -1;
  m_Stream->Read(&field_count); 

#ifdef REFLECT_ARCHIVE_VERBOSE
  Log::Debug(" Deserializing %s (%d fields)\n", m_ShortName.c_str(), field_count);
#endif

  for ( i32 i=0; i<field_count; ++i )
  {
    FieldPtr field = Field::Create(composite);

    field->m_FieldID = i;

    if (!DeserializeField(field))
    {
      return false;
    }

    composite->m_FieldIDToInfo[  field->m_FieldID ] = field;
    composite->m_FieldNameToInfo[ field->m_Name ] = field;
  }

  i32 terminator = -1;
  m_Stream->Read(&terminator); 

  if (terminator != -1)
  {
    throw Reflect::DataFormatException ("Error reading file, unterminated RTTI field block");
  }

  return !m_Stream->Fail();
}

void ArchiveBinary::SerializeField(const Field* field)
{
  // field name
  i32 string_index = m_Strings.AssignIndex(field->m_Name);
  m_Stream->Write(&string_index); 

  // field type id short name
  const Class* type = Registry::GetInstance()->GetClass(field->m_SerializerID);
  if (type != NULL)
  {
    string_index = m_Strings.AssignIndex(type->m_ShortName);
  }
  else
  {
    string_index = -1;
  }
  m_Stream->Write(&string_index); 

#ifdef REFLECT_ARCHIVE_VERBOSE
  Log::Debug("  Serializing %s (short name %s)\n", m_Name.c_str(), type->m_ShortName.c_str());
#endif
}

bool ArchiveBinary::DeserializeField(Field* field)
{
  i32 string_index = -1;

  // field name
  m_Stream->Read(&string_index); 
  field->m_Name = m_Strings.GetString(string_index);

  if ( GetVersion() < ArchiveBinary::FIRST_VERSION_WITH_POINTER_SERIALIZER )
  {
    // field type
    i32 fieldType = -1;
    m_Stream->Read(&fieldType); 

    if ( fieldType == 0 )
    {
      field->m_SerializerID = Reflect::GetType<Reflect::PointerSerializer>();
    }
  }

  // field type id short name
  m_Stream->Read(&string_index); 
  if (string_index >= 0)
  {
    const std::string& str (m_Strings.GetString(string_index));

    const Class* c = Registry::GetInstance()->GetClass(str);

    if ( c )
    {
      field->m_SerializerID = c->m_TypeID;
    }
    else
    {
      field->m_SerializerID = -1;
    }

#ifdef REFLECT_ARCHIVE_VERBOSE
    Log::Debug("  Deserializing %s (short name %s)\n", m_Name.c_str(), str.c_str());
#endif
  }

  return !m_Stream->Fail();
}
