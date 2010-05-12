#include "DependencyInfo.h"

#include "API.h"
#include "Exceptions.h"

#include "Common/Flags.h"

#include "Finder/Finder.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/File.h"

#include "Console/Console.h"

#include "SQL/SQL.h"

namespace Dependencies
{

  /////////////////////////////////////////////////////////////////////////////
  DependencyInfo::DependencyInfo()
  {
    Initialize();
  }


  /////////////////////////////////////////////////////////////////////////////
  DependencyInfo::DependencyInfo( const std::string &name, const Finder::FileSpec &fileSpec, const GraphConfigs graphConfigs )
  {
    Initialize();

    m_Path                = name;
    m_Spec                = &fileSpec;
    m_SpecName            = fileSpec.GetName();
    m_GraphConfigs        = graphConfigs;
  }


  /////////////////////////////////////////////////////////////////////////////
  //DependencyInfo::DependencyInfo( const DependencyInfo &file )
  //{
  //  Initialize();

  //  (*this) = file;
  //}


  /////////////////////////////////////////////////////////////////////////////
  void DependencyInfo::Initialize()
  {
    m_Path                = "";
    m_Spec                = NULL;
    m_NewSpec             = NULL;
    m_SpecName            = "";
    m_FormatVersion       = "";
    m_GraphConfigs        = ConfigFlags::Default;
    m_Size                = 0;
    m_MD5                 = "";
    m_Signature           = "";
    m_IsUpToDate          = false;
    m_IsUpToDateCached    = false;
    m_WasGraphSelected    = false;
    m_IsMD5Valid          = false;
    m_Downloaded          = false;
    m_RowID               = SQL::InvalidRowID;
    m_VersionRowID        = SQL::InvalidRowID;
    m_LastModified        = 0;
  }

  /////////////////////////////////////////////////////////////////////////////
  void DependencyInfo::SetInfo()
  {
    FILEINFO_SCOPE_TIMER((""));

    if ( m_Spec )
    {
      m_FormatVersion = m_Spec->GetFormatVersion();
    }

    m_VersionRowID = SQL::InvalidRowID;
    m_LastModified = 0;
  }


  /////////////////////////////////////////////////////////////////////////////
  DependencyInfo::~DependencyInfo()
  {

  }

  /////////////////////////////////////////////////////////////////////////////
  bool DependencyInfo::IsLeaf() const
  {
    return Nocturnal::HasFlags<GraphConfigs>( m_GraphConfigs, ConfigFlags::LeafInput );
  }

  void DependencyInfo::IsLeaf( const bool isLeaf )
  {
    Nocturnal::SetFlag<GraphConfigs>( m_GraphConfigs, ConfigFlags::LeafInput, isLeaf );
  }

  bool DependencyInfo::InputOrderMatters() const
  {
    return Nocturnal::HasFlags<GraphConfigs>( m_GraphConfigs, ConfigFlags::InputOrderMatters );
  }

  void DependencyInfo::InputOrderMatters( const bool InputOrderMatters )
  {
    Nocturnal::SetFlag<GraphConfigs>( m_GraphConfigs, ConfigFlags::InputOrderMatters, InputOrderMatters );
  }

  void DependencyInfo::CacheCopy( DependencyInfo& rhs )
  {

    IsLeaf( rhs.IsLeaf() );
    InputOrderMatters( rhs.InputOrderMatters() );

    // update the FileSpec
    if ( !m_Spec )
    {
      m_Spec = rhs.m_Spec;
    }
    else if ( m_Spec != rhs.m_Spec )
    {
      m_NewSpec = rhs.m_Spec;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Copy from one file to this
  void DependencyInfo::CopyFrom( const DependencyInfo &rhs )
  {
    if ( this != &rhs )
    {
      m_Path               = rhs.m_Path;
      m_Spec               = rhs.m_Spec;
      m_NewSpec            = rhs.m_NewSpec;
      m_SpecName           = rhs.m_SpecName;
      m_FormatVersion      = rhs.m_FormatVersion;
      m_GraphConfigs       = rhs.m_GraphConfigs;
      m_LastModified       = rhs.m_LastModified;
      m_Size               = rhs.m_Size;
      m_MD5                = rhs.m_MD5;
      m_IsMD5Valid         = rhs.m_IsMD5Valid;
      m_Signature          = rhs.m_Signature;

      m_IsUpToDate         = rhs.m_IsUpToDate;
      m_IsUpToDateCached   = rhs.m_IsUpToDateCached;

      m_Dependencies       = rhs.m_Dependencies;
      m_WasGraphSelected   = rhs.m_WasGraphSelected;

      m_RowID              = rhs.m_RowID;
      m_VersionRowID       = rhs.m_VersionRowID;

      m_GraphInfo          = rhs.m_GraphInfo;
      m_Downloaded         = rhs.m_Downloaded;
    }

  }

  /////////////////////////////////////////////////////////////////////////////
  // Determines if two dependencies are equal
  bool DependencyInfo::IsEqual(const DependencyInfo &rhs ) const
  {
    FILEINFO_SCOPE_TIMER((""));

    bool eq = true;

    eq = eq && ( m_Path                           == rhs.m_Path );
    eq = eq && ( m_Spec                           == rhs.m_Spec );
    eq = eq && ( m_FormatVersion                  == rhs.m_FormatVersion );
    eq = eq && ( m_GraphConfigs                   == rhs.m_GraphConfigs );
    eq = eq && ( m_LastModified                   == rhs.m_LastModified );
    eq = eq && ( m_Size                           == rhs.m_Size );
    eq = eq && ( m_Downloaded											== rhs.m_Downloaded);

    // Only compare the MD5 and signature if both LHS and RHS are not empty
    if ( !m_MD5.empty() && !rhs.m_MD5.empty() )
    {
      eq = eq && ( m_MD5.compare( rhs.m_MD5 ) == 0 );
    }

    if ( !m_Signature.empty() && !rhs.m_Signature.empty() )
    {
      eq = eq && ( m_Signature.compare( rhs.m_Signature ) == 0 );
    }

    eq = eq && ( m_Dependencies == rhs.m_Dependencies );

    return eq;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Overloaded = operator
  DependencyInfo &DependencyInfo::operator=( const DependencyInfo &rhs )
  {
    CopyFrom( rhs );
    return ( *this );
  }

  /////////////////////////////////////////////////////////////////////////////
  // Overloaded == operator: Determines if two dependencies are equal
  bool DependencyInfo::operator==( const DependencyInfo &rhs ) const 
  {
    return IsEqual( rhs );
  }

  /////////////////////////////////////////////////////////////////////////////
  // Overloaded != operator: Determines if two dependencies are not equal
  bool DependencyInfo::operator!=( const DependencyInfo &rhs ) const 
  {
    return !IsEqual( rhs );
  }


  bool DependencyInfo::operator<( const DependencyInfo &rhs ) const
  {
    return m_Path < rhs.m_Path;
  }
}
