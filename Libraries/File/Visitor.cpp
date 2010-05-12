#include "Visitor.h"
#include "Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Reflect/Visitor.h"
#include "Reflect/Serializers.h"

#include "TUID/TUID.h"

using namespace File;
using namespace Reflect;

void File::Serialize( Reflect::SerializeArgs& args )
{
  args.m_Visitors.push_back( new FileVisitor() );
}

void File::Deserialize( Reflect::DeserializeArgs& args )
{
  args.m_Visitors.push_back( new FileVisitor() );
}

void FileVisitor::VisitField(Element* element, const Field* field)
{
  if ( ( field->m_Flags & Reflect::FieldFlags::FileID ) != 0 )
  {
    if ( field->m_SerializerID == Reflect::GetType<U64Serializer>() )
    {
      tuid val;
      Serializer::GetValue( field->CreateSerializer(element), val );

      if ( val != TUID::Null )
      {
        m_Found.insert(val);
      }
    }
    else if ( field->m_SerializerID == Reflect::GetType<U64ArraySerializer>() )
    {
      SerializerPtr serializer = field->CreateSerializer( element );

      V_tuid vals;
      Serializer::GetValue( serializer, vals );

      std::vector<tuid>::const_iterator itr = vals.begin();
      std::vector<tuid>::const_iterator end = vals.end();
      for ( ; itr != end; ++itr )
      {
        if ( *itr != TUID::Null )
        {
          m_Found.insert( *itr );
        }
      }
    }
    else if ( field->m_SerializerID == Reflect::GetType<U64SetSerializer>() )
    {
      SerializerPtr serializer = field->CreateSerializer( element );
      
      S_tuid vals;
      Serializer::GetValue( serializer, vals );

      std::set<tuid>::const_iterator itr = vals.begin();
      std::set<tuid>::const_iterator end = vals.end();
      for ( ; itr != end; ++itr )
      {
        if ( *itr != TUID::Null )
        {
          m_Found.insert( *itr );
        }
      }
    }
    else
    {
      NOC_BREAK();
      throw File::Exception( "This field type '%s::%s' has no support for the file manager", field->m_Type->m_ShortName.c_str(), field->m_Name.c_str() );
    }
  }
}

void FileVisitor::CreateAppendElements(Reflect::V_Element& append)
{
  FILE_SCOPE_TIMER( ("") );

  if ( m_Found.empty() )
  {
    return;
  }

  ManagedFileStashPtr stash = new ManagedFileStash ();

  std::set<tuid>::const_iterator itr = m_Found.begin();
  std::set<tuid>::const_iterator end = m_Found.end();
  for ( ; itr != end; ++itr )
  {
    if ( *itr != TUID::Null )
    {
      FILE_SCOPE_TIMER( ("") );

      ManagedFilePtr file = File::GlobalManager().GetManagedFile( *itr );

      if ( file.ReferencesObject() )
      {
        FileSystem::StripPrefix( File::GlobalManager().GetManagedAssetsRoot(), file->m_Path );

        if ( file->m_Username.empty() && file->m_UserId != 0 )
        {
          File::GlobalManager().GetUsernameByID( file->m_UserId, file->m_Username );
        }

        stash->m_Files.push_back( file );
      }
      else
      {
        Console::Warning("Saving reference to unresolvable TUID: "TUID_HEX_FORMAT"\n", *itr); 
      }
    }
  }
  
  if (!stash->m_Files.empty())
  {
    append.push_back( stash );
  }
}

void FileVisitor::ProcessAppendElements(Reflect::V_Element& append)
{
  FILE_SCOPE_TIMER( ("") );

  ManagedFileStashPtr stash;

  {
    V_Element::iterator itr = append.begin();
    V_Element::iterator end = append.end();
    for ( ; itr != end; ++itr )
    {
      stash = ObjectCast< ManagedFileStash >( *itr );
      if ( stash.ReferencesObject() )
      {
        append.erase( itr );
        break;
      }
    }
  }

  if ( stash )
  {
    V_ManagedFilePtr::const_iterator itr = stash->m_Files.begin();
    V_ManagedFilePtr::const_iterator end = stash->m_Files.end();
    for ( ; itr != end; ++itr )
    {
      File::ManagedFilePtr file = ObjectCast< File::ManagedFile >( *itr );

      File::GlobalManager().TempResolverAddEntry( file );
    }
  }
}
