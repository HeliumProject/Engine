#include "Writer.h"

using namespace Helium::CodeGen;

static const std::string s_Space = "  ";

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Writer::Writer()
: m_Guard( HeaderGuards::PragmaOnce )
, m_Headerparsed( false )
, m_ToLua( false )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
Writer::~Writer()
{
}

///////////////////////////////////////////////////////////////////////////////
// The name of the file that is being written.  If you are using 
// HeaderGuards::Define, this name will be part of the guard.
// 
void Writer::SetName( const std::string& name )
{
  m_Name = name;
}

///////////////////////////////////////////////////////////////////////////////
// Allows you to specify what type of header guard to use on this file.
// 
void Writer::SetHeaderGuard( HeaderGuards::HeaderGuard guard )
{
  m_Guard = guard;
}

///////////////////////////////////////////////////////////////////////////////
// Sets some text that will be displayed in a comment block at the top of the
// file when written out.
// 
void Writer::SetComment( const std::string& comment )
{
  m_Comment = comment;
}

///////////////////////////////////////////////////////////////////////////////
// Sets whether KV_START and KV_END should be added at the beginning and end
// 
void Writer::SetHeaderparsed( bool headerparsed )
{
  m_Headerparsed = headerparsed;
}

///////////////////////////////////////////////////////////////////////////////
// Sets whether tolua_begin and tolua_end should be added at the beginning and end
// 
void Writer::SetToLua( bool toLua )
{
  m_ToLua = toLua;
}

///////////////////////////////////////////////////////////////////////////////
// Writes the specified namespace to the stream.  If this function returns false,
// the stream is not guaranteed to be correct.  Call GetLastError() to determine
// what the problem is.
// 
bool Writer::Write( std::ostream& stream, const NamespacePtr& namespacePtr )
{
  m_LastError.clear();

  if ( m_Guard == HeaderGuards::Define )
  {
    if ( m_Name.empty() )
    {
      m_LastError = "You must call Writer::SetName() before you can call Writer::Write() if you are using HeaderGuards::Define.";
      return false;
    }
  }

  if ( !namespacePtr.ReferencesObject() )
  {
    m_LastError = "No namespace was specified for writing.";
    return false;
  }

  if ( WriteHeader( stream ) && WriteNamespace( stream, namespacePtr ) && WriteFooter( stream ) )
  {
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the last error encountered.
// 
const std::string& Writer::GetLastError() const
{
  return m_LastError;
}

///////////////////////////////////////////////////////////////////////////////
// Writes some header information (comment, guards) to the stream.
// 
bool Writer::WriteHeader( std::ostream& stream )
{
  stream << "// DO NOT EDIT - THIS IS A GENERATED FILE" << std::endl;

  if ( !m_Comment.empty() )
  {
    stream << "// " << m_Comment << std::endl;
  }

  stream << std::endl;
  
  if ( m_Guard == HeaderGuards::Define )
  {
    std::string guard = "_INCLUDED_" + m_Name + "_HEADER_";
    stream << "#ifndef " << guard << std::endl;
    stream << "#define " << guard << " 1" << std::endl << std::endl;
  }
  else
  {
    stream << "#pragma once" << std::endl << std::endl;
  }

  if ( m_Headerparsed )
  {
    stream << "// KV_START" << std::endl << std::endl;
  }

  if ( m_ToLua )
  {
    stream << "// tolua_begin" << std::endl << std::endl;
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Writes information (header guards) to the stream.
// 
bool Writer::WriteFooter( std::ostream& stream )
{
  if ( m_ToLua )
  {
    stream << "// tolua_end" << std::endl << std::endl;
  }

  if ( m_Headerparsed )
  {
    stream << "// KV_END" << std::endl << std::endl;
  }

  if ( m_Guard == HeaderGuards::Define )
  {
    std::string guard = "_INCLUDED_" + m_Name + "_HEADER_";
    stream << "#endif // " << guard << std::endl;
  }
  
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Writes the specified namespace to the stream.
// 
bool Writer::WriteNamespace( std::ostream& stream, const NamespacePtr& namespacePtr )
{
  if ( namespacePtr->GetShortName().empty() )
  {
    m_LastError = "Found a namespace with no name!";
    return false;
  }

  stream << m_Indent << "namespace " << namespacePtr->GetShortName() << std::endl;
  stream << m_Indent << "{" << std::endl;
  {
    m_Indent += s_Space;

    size_t numEnums = namespacePtr->GetEnums().Size();
    OS_EnumSmartPtr::Iterator enumItr = namespacePtr->GetEnums().Begin();
    OS_EnumSmartPtr::Iterator enumEnd = namespacePtr->GetEnums().End();
    for ( size_t enumCount = 1; enumItr != enumEnd; ++enumItr, ++enumCount )
    {
      if ( !WriteEnum( stream, *enumItr ) )
      {
        return false;
      }
      else if ( enumCount != numEnums )
      {
        stream << std::endl;
      }
    }

    size_t numNamespaces = namespacePtr->GetNamespaces().size();
    V_NamespaceSmartPtr::const_iterator namespaceItr = namespacePtr->GetNamespaces().begin();
    V_NamespaceSmartPtr::const_iterator namespaceEnd = namespacePtr->GetNamespaces().end();
    for ( size_t nameCount = 1; namespaceItr != namespaceEnd; ++namespaceItr, ++nameCount )
    {
      if ( nameCount == 1 )
      {
        stream << std::endl;
      }

      if ( !WriteNamespace( stream, *namespaceItr ) )
      {
        return false;
      }
      else if ( nameCount != numNamespaces )
      {
        stream << std::endl;
      }
    }

    m_Indent = m_Indent.substr( 0, m_Indent.length() - s_Space.length() );
  }
  stream << m_Indent << "}" << std::endl;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Writes the specified enumeration to the stream.
// 
bool Writer::WriteEnum( std::ostream& stream, const EnumPtr& enumPtr )
{
  if ( enumPtr->GetShortName().empty() )
  {
    m_LastError = "Found an enum with no name!";
    return false;
  }

  stream << m_Indent << "enum " << enumPtr->GetShortName() << std::endl;
  stream << m_Indent << "{" << std::endl;
  {
    m_Indent += s_Space;

    OS_EnumValueSmartPtr::Iterator valItr = enumPtr->GetValues().Begin();
    OS_EnumValueSmartPtr::Iterator valEnd = enumPtr->GetValues().End();
    for ( ; valItr != valEnd; ++valItr )
    {
      if ( !enumPtr->HasAssignedValues() )
      {
        stream << m_Indent << ( *valItr )->GetShortName() << "," << std::endl;
      }
      else
      {
        stream << m_Indent << ( *valItr )->GetShortName() << " = " << ( *valItr )->GetValue() << "," << std::endl;
      }
    }

    m_Indent = m_Indent.substr( 0, m_Indent.length() - s_Space.length() );
  }
  stream << m_Indent << "};" << std::endl;
  
  return true;
}
