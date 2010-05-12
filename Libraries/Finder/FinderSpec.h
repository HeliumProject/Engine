#pragma once

#include "API.h"
#include "Common/Assert.h"
#include "Common/Container/OrderedSet.h"

#include <string>
#include <vector>

namespace Finder
{
  // FormatVersion
  typedef FINDER_API std::string FormatVersion;

  /////////////////////////////////////////////////////////////////////////////
  // FinderSpec is a basic container for data any spec may need
  //
  class FINDER_API FinderSpec NOC_ABSTRACT
  {
  protected:
    std::string m_Name;
    std::string m_UIName;
    std::string m_Value;

  public:
    FinderSpec( const char* name, const char* value = "" )
      : m_Name( name ) 
      , m_Value( value ) 
    {

    }

    FinderSpec( const char* name, const char* uiName, const char* value )
      : m_Name( name ) 
      , m_UIName( uiName )
      , m_Value( value )
    {

    }

    const std::string& GetName() const
    {
      return m_Name;
    }
    
    const std::string& GetUIName() const
    {
      return m_UIName;
    }

    const std::string& GetValue() const
    {
      return m_Value;
    }

    void SetValue(const std::string& value)
    {
      m_Value = value;
    }
    
    virtual std::string GetDialogFilter() const;

    virtual std::string GetFilter() const;
  };

  typedef FINDER_API std::vector< const FinderSpec* > V_FinderSpec;

  //
  // ModifierSpec adds the Modify() function to decorating FinderSpecs
  //

  class FINDER_API ModifierSpec NOC_ABSTRACT : public FinderSpec
  {
  public:
    // Linked list used to initialize the global extension specs
    static ModifierSpec* s_LinkedListHead;
    ModifierSpec* m_NextSpec;

    ModifierSpec( const char* name, const char* value )
      : FinderSpec( name, value ) 
    {
      m_NextSpec = s_LinkedListHead;
      s_LinkedListHead = this;
    }
    
    ModifierSpec( const char* name, const char* uiName, const char* value )
      : FinderSpec( name, uiName, value ) 
    {
      m_NextSpec = s_LinkedListHead;
      s_LinkedListHead = this;
    }

    virtual void Modify( std::string& path ) const = 0;
    virtual std::string GetModifier() const = 0;
  };

  //
  // SuffixSpec is a suffix we append to the end of a file to characterize its contents is
  //  only part of a whole (typically), meaning: file_stuff.xml and file_widgets.xml
  //

  class FINDER_API SuffixSpec : public ModifierSpec
  {
  public:
    SuffixSpec( const char* name, const char* value )
      : ModifierSpec( name, value )
    {

    }

    std::string GetSuffix() const;
    virtual void Modify( std::string& path ) const NOC_OVERRIDE;
    virtual std::string GetModifier() const NOC_OVERRIDE { return GetSuffix(); }
  };

  //
  // FamilySpec is a second file extension, meaning: filename.family.extension
  //

  class FINDER_API FamilySpec : public ModifierSpec
  {
  public:
    FamilySpec( const char* name, const char* uiName, const char* value )
      : ModifierSpec( name, uiName, value )
    {

    }

    std::string GetFamily() const;
    virtual void Modify( std::string& path ) const NOC_OVERRIDE;
    virtual std::string GetModifier() const NOC_OVERRIDE { return GetFamily(); }
  };

  // for all posterity, yeah.. check your thesaurus
  typedef FamilySpec FestoonSpec;
  typedef FamilySpec FripperySpec;

  //
  // ExtensionSpec is typically only the three letters of a 8.3 file extension,
  //  but we don't enforce that
  //

  class FINDER_API ExtensionSpec : public ModifierSpec
  {
  public:
    ExtensionSpec( const char* name, const char* uiName, const char* value )
      : ModifierSpec( name, uiName, value )
    {

    }

    std::string GetExtension() const;
    virtual std::string GetFilter() const NOC_OVERRIDE;
    virtual void Modify( std::string& path ) const NOC_OVERRIDE;
    virtual std::string GetModifier() const NOC_OVERRIDE { return GetExtension(); }
  };

  //
  // DecorationSpec is a combination of the above associated together
  //

  class FINDER_API DecorationSpec : public ModifierSpec
  {
  public:
    const SuffixSpec*     m_Suffix;
    const FamilySpec*     m_Family;
    const ExtensionSpec*  m_Extension;

    DecorationSpec( const char* name, const char* uiName, const FamilySpec& family, const ExtensionSpec& extension )
      : ModifierSpec( name, uiName, "" )
      , m_Suffix (NULL)
      , m_Family (&family)
      , m_Extension (&extension)
    {

    }

    DecorationSpec( const char* name, const char* uiName, const SuffixSpec& suffix, const ExtensionSpec& extension )
      : ModifierSpec( name, uiName, "" )
      , m_Suffix (&suffix)
      , m_Family (NULL)
      , m_Extension (&extension)
    {

    }

    DecorationSpec( const char* name, const char* uiName, const SuffixSpec& suffix, const FamilySpec& family )
      : ModifierSpec( name, uiName, "" )
      , m_Suffix (&suffix)
      , m_Family (&family)
      , m_Extension (NULL)
    {

    }

    DecorationSpec( const char* name, const char* uiName, const SuffixSpec& suffix, const FamilySpec& family, const ExtensionSpec& extension )
      : ModifierSpec( name, uiName, "" )
      , m_Suffix (&suffix)
      , m_Family (&family)
      , m_Extension (&extension)
    {

    }

    std::string GetDecoration() const;
    std::string GetExportFile( const std::string& path, const std::string& fragmentNode = std::string("") ) const;
    std::string GetMetaDataFile( const std::string& path ) const;
    virtual std::string GetFilter() const NOC_OVERRIDE;
    virtual void Modify( std::string& path ) const NOC_OVERRIDE;
    virtual std::string GetModifier() const NOC_OVERRIDE { return GetDecoration(); }
  };

  //
  // FileSpec is a file path that ends with a file name (meaning its not a folder,
  //  but we don't restrict it not be a relative file path
  //

  class FolderSpec;

  class FINDER_API FileSpec : public FinderSpec
  {
  protected:
    // the version of this file, used for dependencies system
    std::string m_FormatVersion;

    // the decoration for this file, (can be null for simple file specs)
    const ModifierSpec* m_Modifier;

  public:
    // Linked list used to initialize the global file specs
    static FileSpec* s_LinkedListHead;
    FileSpec* m_NextSpec;

  public:
    FileSpec( const char* name, const char* value, const char* formatVersion = "" )
      : FinderSpec( name, value )
      , m_FormatVersion ( formatVersion )
      , m_Modifier ( NULL )
    {
      m_NextSpec = s_LinkedListHead;
      s_LinkedListHead = this;
    }

    FileSpec( const char* name, const char* value, const ModifierSpec& decoration, const char* formatVersion = "" )
      : FinderSpec( name, value )
      , m_FormatVersion ( formatVersion )
      , m_Modifier ( &decoration )
    {
      m_NextSpec = s_LinkedListHead;
      s_LinkedListHead = this;
    }

    const std::string& GetFormatVersion() const
    {
      return m_FormatVersion;
    }

    void SetFormatVersion( const std::string& formatVersion )
    {
      m_FormatVersion = formatVersion;
    }

    std::string GetFile() const;
    std::string GetFile( const std::string& folder ) const;
    std::string GetFile( const FolderSpec &folderSpec ) const;
  };

  //
  // FolderSpec is a folder relative to a specified root
  //

  // Enumerates the roots that any of our folders can be relative to
  namespace FolderRoots
  {
    enum FolderRoot
    {
      None,               // absolute folder path (dangerous)   (eg: C:, or WINDOWS)
      Insomniac,          // relative to the insomniac root     (eg: rcf)       (used: X:\<FOLDER>)
      ProjectRoot,        // relative to the project root       (eg: code)      (used: X:\\rcf\<FOLDER>)
      ProjectCode,        // relative to the code branch        (eg: shared)    (used: X:\\rcf\code\<CODE_BRANCH>\<FOLDER>)
      ProjectAssets,      // relative to the asset branch       (eg: entities)  (used: X:\\rcf\assets\<ASSET_BRANCH>\<FOLDER>)
      ProjectProcessed,   // relative to the processed folder   (eg: )          (used: X:\\rcf\assets\<ASSET_BRANCH>\processed\<FOLDER>)
      ProjectTools,       // junctioned tools directory         (eg: data)      (used: X:\\rcf\code\<CODE_BRANCH>\tools\runtime_junction\<FOLDER>)
      ProjectTemp,        // relative to the project temp folder (eg x:\r2\temp)
      ProjectBuilt,       // relative to the built folder (eg: x:\<project>\<assets folder>\built)
    };
  }
  typedef FolderRoots::FolderRoot FolderRoot;

  class FINDER_API FolderSpec : public FinderSpec
  {
  public:
    // Linked list used to initialize the global folder specs
    static FolderSpec* s_LinkedListHead;
    FolderSpec* m_NextSpec;

  protected:
    FolderRoot        m_Root;
    const FolderSpec* m_ParentFolder; 

  public:
    FolderSpec( const char* name, const char* value = "", FolderRoot root = FolderRoots::ProjectAssets )
      : FinderSpec( name, value )
      , m_Root (root)
      , m_ParentFolder(NULL)
    {
      m_NextSpec = s_LinkedListHead;
      s_LinkedListHead = this;
    }

    // Ctor for nested FolderSpecs
    FolderSpec( const char* name, const char* value, const FolderSpec& parentFolder )
      : FinderSpec( name, value )
      , m_Root ( parentFolder.m_Root )
      , m_ParentFolder(&parentFolder)
    {
      // do not use m_ParentFolder in this function, 
      // because static initialization order will bite you! 
      
      m_NextSpec = s_LinkedListHead;
      s_LinkedListHead = this;
    }

    FolderRoot GetFolderRoot() const
    {
      return m_Root;
    }

    void SetFolderRoot(FolderRoot root)
    {
      m_Root = root;
    }

    // return the full folder path
    // will end with a slash. 
    std::string GetFolder() const;

    // this returns a path relative to GetFolderRoot, NOT the parent folder
    // will end with a slash. 
    std::string GetRelativeFolder() const;
  };

  typedef Nocturnal::OrderedSet< const ModifierSpec* > OS_ModifierSpecConstDumbPtr;

  // 
  // Spec made up of other specs.
  // 

  class FINDER_API FilterSpec : public FinderSpec
  {
  public:
    // Linked list used to initialize the global file specs
    static FilterSpec* s_LinkedListHead;
    FilterSpec* m_NextSpec;

  protected:
    OS_ModifierSpecConstDumbPtr m_Specs;
    
  public:
    FilterSpec( const char* name, const char* uiName, const char* value = "" )
    : FinderSpec( name, uiName, value )
    {
      m_NextSpec = s_LinkedListHead;
      s_LinkedListHead = this;
    }

    void AddSpec( const ExtensionSpec& spec );
    void AddSpec( const DecorationSpec& spec );
    virtual std::string GetDialogFilter() const NOC_OVERRIDE;
    virtual std::string GetFilter() const NOC_OVERRIDE;
    bool IsExtensionValid( std::string ext ) const;

  protected:
    void DoAddSpec( const ModifierSpec& spec );
  };
}

// This is a helper for all of our spec definitions
namespace FinderSpecs
{
  using namespace Finder;
}
