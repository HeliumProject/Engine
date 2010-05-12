#include "FinderSpec.h"
#include "Finder.h"

#include "Common/Assert.h"
#include "FileSystem/FileSystem.h"

namespace Finder
{
  FileSpec* FileSpec::s_LinkedListHead = NULL;
  ModifierSpec* ModifierSpec::s_LinkedListHead = NULL;
  FolderSpec* FolderSpec::s_LinkedListHead = NULL;
  FilterSpec* FilterSpec::s_LinkedListHead = NULL;

  /////////////////////////////////////////////////////////////////////////////
  std::string FinderSpec::GetFilter() const
  {
    throw Exception ( "This FinderSpec cannot be filtered by" );
  }

  /////////////////////////////////////////////////////////////////////////////
  std::string FinderSpec::GetDialogFilter() const
  {
    FINDER_SCOPE_TIMER((""));

    std::string filter = GetFilter();

    return m_UIName + " (" + filter + ")|" + filter;
  }

  /////////////////////////////////////////////////////////////////////////////
  std::string SuffixSpec::GetSuffix() const
  {
    FINDER_SCOPE_TIMER((""));

    return m_Value;
  }

  /////////////////////////////////////////////////////////////////////////////
  void SuffixSpec::Modify( std::string& path ) const
  {
    FINDER_SCOPE_TIMER((""));

    std::string extension;
    FileSystem::GetExtension( path, extension );
    FileSystem::StripExtension( path );
    path += m_Value + extension;
  }

  /////////////////////////////////////////////////////////////////////////////
  std::string FamilySpec::GetFamily() const
  {
    FINDER_SCOPE_TIMER((""));

    std::string result = m_Value;
    FileSystem::GuaranteeDot( result );

    return result;
  }

  /////////////////////////////////////////////////////////////////////////////
  void FamilySpec::Modify( std::string& path ) const
  {
    FINDER_SCOPE_TIMER((""));

    std::string extension;
    FileSystem::GetExtension(path, extension);
    FileSystem::StripExtension(path, 2);
    path += GetFamily() + extension;
  }

  /////////////////////////////////////////////////////////////////////////////
  std::string ExtensionSpec::GetFilter() const
  {
    FINDER_SCOPE_TIMER((""));

    std::string result ("*.");

    return result + m_Value;
  }

  /////////////////////////////////////////////////////////////////////////////
  std::string ExtensionSpec::GetExtension() const
  {
    FINDER_SCOPE_TIMER((""));

    std::string result = m_Value;
    FileSystem::GuaranteeDot( result );

    return result;
  }

  /////////////////////////////////////////////////////////////////////////////
  void ExtensionSpec::Modify( std::string& path ) const
  {
    FINDER_SCOPE_TIMER((""));

    FileSystem::StripExtension( path, 1 );
    path += GetExtension();
  }

  /////////////////////////////////////////////////////////////////////////////
  std::string DecorationSpec::GetDecoration() const
  {
    std::string result;

    if (m_Suffix)
    {
      result += m_Suffix->GetSuffix();
    }

    if (m_Family)
    {
      result += m_Family->GetFamily();
    }

    if (m_Extension)
    {
      result += m_Extension->GetExtension();
    }

    return result;
  }

  /////////////////////////////////////////////////////////////////////////////
  std::string DecorationSpec::GetExportFile( const std::string& path, const std::string& fragmentNode ) const
  {
    FINDER_SCOPE_TIMER((""));

    std::string result = path;

    FileSystem::StripExtension( result );

    // it kind of sucks to do this here, but as far as I know this is the only export file
    // pinch-point, we need to replace colons in the fragment name with underscores
    std::string newFragmentNode = fragmentNode;
    if ( !newFragmentNode.empty() )
    {
      size_t pos;
      while ( ( pos = newFragmentNode.find( ":" ) ) != std::string::npos )
      {
        newFragmentNode.replace( pos, 1, "_" );
      }
    }

    result += ( newFragmentNode + GetDecoration() );

    FileSystem::StripPrefix( ProjectAssets(), result );
  
    return ProjectExport() + result;
  }

  /////////////////////////////////////////////////////////////////////////////
  std::string DecorationSpec::GetMetaDataFile( const std::string& path ) const
  {
    FINDER_SCOPE_TIMER((""));

    std::string result = path;

    FileSystem::StripExtension( result );

    result += GetDecoration();

    FileSystem::StripPrefix( ProjectAssets(), result );

    return ProjectMetaData() + result;
  }

  /////////////////////////////////////////////////////////////////////////////
  std::string DecorationSpec::GetFilter() const
  {
    FINDER_SCOPE_TIMER((""));

    std::string result ("*");

    return result + GetDecoration();
  }

  /////////////////////////////////////////////////////////////////////////////
  void DecorationSpec::Modify(std::string& path) const
  {
    FINDER_SCOPE_TIMER((""));

    FileSystem::StripExtension( path );

    path += GetDecoration();
  }

  /////////////////////////////////////////////////////////////////////////////
  std::string FileSpec::GetFile() const
  {
    FINDER_SCOPE_TIMER((""));

    std::string result = m_Value;

    if (m_Modifier)
    {
      m_Modifier->Modify(result);
    }

    return result;
  }

  /////////////////////////////////////////////////////////////////////////////
  std::string FileSpec::GetFile( const std::string& folder ) const
  {
    FINDER_SCOPE_TIMER((""));

    return ( folder + GetFile() );
  }

  /////////////////////////////////////////////////////////////////////////////
  std::string FileSpec::GetFile( const FolderSpec& folderSpec ) const
  {
    FINDER_SCOPE_TIMER((""));

    return ( folderSpec.GetFolder() + GetFile() );
  }

  /////////////////////////////////////////////////////////////////////////////
  std::string FolderSpec::GetFolder() const
  {
    FINDER_SCOPE_TIMER((""));

    std::string dir;

    if(m_ParentFolder)
    {
      dir = m_ParentFolder->GetFolder(); 
    }
    else
    {
      switch ( m_Root )
      {
        case FolderRoots::None:
          break;

        case FolderRoots::Insomniac:
          dir = getenv(NOCTURNAL_STUDIO_PREFIX"ROOT");
          break;

        case FolderRoots::ProjectRoot:
          dir = ProjectRoot();
          break;

        case FolderRoots::ProjectCode:
          dir = ProjectCode();
          break;

        case FolderRoots::ProjectAssets:
          dir = ProjectAssets();
          break;

        case FolderRoots::ProjectProcessed:
          dir = ProjectProcessed();
          break;

        case FolderRoots::ProjectTools:
          dir = ProjectTools();
          break;

        case FolderRoots::ProjectTemp:
          dir = ProjectTemp();
          break;

        case FolderRoots::ProjectBuilt:
          dir = ProjectBuilt();
          break;

        default:
          NOC_BREAK();
      }

    }
    
    dir = dir + m_Value;
    FileSystem::GuaranteeSlash( dir );

    return dir;
  }

  
  /////////////////////////////////////////////////////////////////////////////
  std::string FolderSpec::GetRelativeFolder() const 
  {
    std::string result; 

    if(m_ParentFolder)
    {
      result = m_ParentFolder->GetRelativeFolder(); 
      result += m_Value; 
    }
    else
    {
      result = m_Value; 
    }

    FileSystem::GuaranteeSlash(result); 
    return result; 
  }

  /////////////////////////////////////////////////////////////////////////////
  // FilterSpec
  /////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////
  // Adds an ExtensionSpec to this filter.
  // 
  void FilterSpec::AddSpec( const ExtensionSpec& spec )
  {
    DoAddSpec( spec );
  }

  /////////////////////////////////////////////////////////////////////////////
  // Adds a DecorationSpec to this filter.
  // 
  void FilterSpec::AddSpec( const DecorationSpec& spec )
  {
    DoAddSpec( spec );
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns a filter made up of all the included specs.
  // 
  std::string FilterSpec::GetDialogFilter() const
  {
    FINDER_SCOPE_TIMER((""));

    std::string result = __super::GetDialogFilter();

    OS_ModifierSpecConstDumbPtr::Iterator itr = m_Specs.Begin();
    OS_ModifierSpecConstDumbPtr::Iterator end = m_Specs.End();
    for ( ; itr != end; ++itr )
    {
      const ModifierSpec* spec = *itr;
      
      if ( !result.empty() )
      {
        result += "|";
      }

      result += spec->GetDialogFilter();
    }

    return result;
  }

  /////////////////////////////////////////////////////////////////////////////
  // The filter is maintained as a semicolon delimited list of extensions.
  // 
  std::string FilterSpec::GetFilter() const
  {
    return m_Value;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns true if the extension matches one of the existing specs.
  // 
  bool FilterSpec::IsExtensionValid( std::string ext ) const
  {
    FileSystem::GuaranteeDot( ext );

    OS_ModifierSpecConstDumbPtr::Iterator itr = m_Specs.Begin();
    OS_ModifierSpecConstDumbPtr::Iterator end = m_Specs.End();
    for ( ; itr != end; ++itr )
    {
      const ModifierSpec* spec = *itr;
      if ( spec->GetModifier() == ext )
      {
        return true;
      }
    }

    return false;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Helper function to add a modifier spec to the list.  This function is not
  // public because we want to limit what kinds of specs can be added to this
  // one.
  // 
  void FilterSpec::DoAddSpec( const ModifierSpec& spec )
  {
    FINDER_SCOPE_TIMER((""));

    if ( m_Specs.Append( &spec ) )
    {
      std::string filter = spec.GetFilter();

      // Keep value up to date
      if ( !m_Value.empty() && !filter.empty() )
      {
        m_Value += ";";
      }
      m_Value += filter;
    }
  }

}
