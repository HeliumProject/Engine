#include "stdafx.h"

#include "ShaderTexturesPage.h"
#include "TextureMapFieldPanel.h"
#include "AttributesPage.h"

#include "Wizard.h"
#include "WizardPanels.h"
#include "ManagedAsset.h"

#include "Attribute/AttributeCategory.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/AssetClass.h"
#include "Asset/TextureMapAttribute.h"
#include "Common/Container/Insert.h"
#include "File/Manager.h"
#include "FileBrowser/FileBrowser.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "RCS/RCS.h"
#include "UIToolKit/FileDialog.h"
#include "UIToolKit/ImageManager.h"

// Using
using namespace AssetManager;


static const char* s_PageTitle = "Shader Textures";
static const char* s_PageDescription = "Please specify the texture maps that you would like to associate with your new shader. The asterisk ('*') denotes required texture maps.";


typedef std::map< Attribute::AttributeBase*, std::string > M_AttribToFilePath;
M_AttribToFilePath g_AttribToFilePath;


///////////////////////////////////////////////////////////////////////////////
// Constructor - protected; You must derive from this class.
// 
ShaderTexturesPage::ShaderTexturesPage( CreateAssetWizard* wizard )
: WizardPage< ShaderTexturesPanel >( wizard )
{
  // Set up description
  m_Panel->m_Title->SetLabel( s_PageTitle );
  m_Panel->m_Description->SetLabel( s_PageDescription );
  m_Panel->m_Description->Wrap( m_Panel->GetMinWidth() - 10 );

  this->Layout();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ShaderTexturesPage::~ShaderTexturesPage()
{
  m_AttribToTextureMapPanel.clear();
}


///////////////////////////////////////////////////////////////////////////////
// Static creation function.  Returns a new (heap-allocated) instance of this 
// class.
// 
UIToolKit::WizardPage* ShaderTexturesPage::Create( CreateAssetWizard* wizard )
{
  return new ShaderTexturesPage( wizard );
}


///////////////////////////////////////////////////////////////////////////////
// Called before the UI is shown.
// 
bool ShaderTexturesPage::TransferDataToWindow()
{
  // Only do validation if traversing to the next wizard page
  if ( GetWizard()->GetDirection() != UIToolKit::PageDirections::Forward )
  {
    return true;
  }

  wxBusyCursor bc;

  m_AttribToTextureMapPanel.clear();
  m_Panel->m_ScrollWindow->DestroyChildren();

  wxSizer* scrollSizer = m_Panel->m_ScrollWindow->GetSizer();

  // get the list of texture map attributes
  S_SortedAttribute sortedAttributes( GetCreateAssetWizard()->GetCurrentTemplate() );
  
  Asset::AssetClass* assetClass = GetCreateAssetWizard()->GetAssetClass();

  Attribute::M_Attribute::const_iterator itr = assetClass->GetAttributes().begin();
  Attribute::M_Attribute::const_iterator end = assetClass->GetAttributes().end();
  for ( ; itr != end; ++itr )
  {
    sortedAttributes.insert( itr->second.Ptr() );
  }
  
  const S_SortedAttribute& requiredAttribs = GetCreateAssetWizard()->GetAttributesPage()->GetRequiredAttributes();

  S_SortedAttribute::const_iterator attrItr = sortedAttributes.begin();
  S_SortedAttribute::const_iterator attrEnd = sortedAttributes.end();
  for ( ; attrItr != attrEnd; ++attrItr )
  {
    Attribute::AttributeBase* attribute = *attrItr;
  
    if ( attribute->HasType( Reflect::GetType<Asset::TextureMapAttribute>() ) )
    {
      // add the panel to the mapping
      Nocturnal::Insert<M_AttribToTextureMapPanel>::Result inserted = m_AttribToTextureMapPanel.insert( std::make_pair( attribute, new TextureMapFieldPanel( m_Panel->m_ScrollWindow, attribute )) );
  
      if ( inserted.second && inserted.first->second != NULL )
      {
        TextureMapFieldPanel* textureMapPanel = inserted.first->second;
  
        M_AttribToFilePath::iterator filePath = g_AttribToFilePath.find( attribute );
        if ( filePath != g_AttribToFilePath.end() )
        {
          textureMapPanel->SetFilePath( filePath->second.c_str() );
        }
        else
        {
          textureMapPanel->SetFilePath( "" );
        }

        textureMapPanel->IsRequired( requiredAttribs.find( attribute ) != requiredAttribs.end() );
  
        // add the panel to the scroll window
        scrollSizer->Add( textureMapPanel, 0, wxEXPAND | wxALL, 5 );

        textureMapPanel->Layout();
        textureMapPanel->GetSizer()->Fit( textureMapPanel );
      }
    }
  }

  m_Panel->m_ScrollWindow->Layout();
  scrollSizer->Fit( m_Panel->m_ScrollWindow );

  this->Layout();

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Called when the page is changed.  Stores the UI data back to the persistent
// data managed by the wizard.
//
// Note: if it doesn't exist in the resolver, it will be added later in the call to Finished
// 
bool ShaderTexturesPage::TransferDataFromWindow()
{
  // Only do validation if traversing to the next wizard page
  bool nextButtonClicked = ( GetWizard()->GetDirection() == UIToolKit::PageDirections::Forward );  

  // Set the summary page info and list of files (if any)
  if ( nextButtonClicked )
  {
    m_PageSummary.clear();
    m_P4FileList.clear();

    m_PageSummary.push_back( StyledText( &m_TitleTextAttr, "Texture Map Files:\n" ) );
  }

  std::string buffer;
  RCS::File rcsFile;

  M_AttribToTextureMapPanel::iterator it = m_AttribToTextureMapPanel.begin();
  M_AttribToTextureMapPanel::iterator itEnd = m_AttribToTextureMapPanel.end();
  for ( ; it != itEnd ; ++it )
  {
    Attribute::AttributeBase* attribute = it->first;
    TextureMapFieldPanel* textureMapPanel = it->second;

    // get and clean the texture file paths
    std::string filePath = textureMapPanel->GetFilePath();
    FileSystem::CleanName( filePath );
    g_AttribToFilePath[attribute] = filePath;

    // Only do validation and update summary info if traversing to the next wizard page
    if ( nextButtonClicked )
    {
      std::string error;
      if ( !textureMapPanel->ValidateFilePath( error ) )
      {
        wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetCreateAssetWizard() );
        return false;
      }

      if ( !filePath.empty() )
      {
        // Set the summary page info and list of files (if any)
        buffer = std::string( "  o " );
        buffer += Reflect::Registry::GetInstance()->GetType( attribute->GetType() )->m_UIName;
        buffer += std::string( ": " );
        buffer += filePath + std::string( "\n" );
        m_PageSummary.push_back( StyledText( &m_DefaultTextAttr, buffer ) );

        // only add files that are new to P4
        try
        {
          rcsFile.m_LocalPath = filePath;
          rcsFile.GetInfo();
        }
        catch ( Nocturnal::Exception& ex )
        {
          std::stringstream str;
          str << "Unable to get info for '" << filePath << "': " << ex.what();
          wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK );
        }

        if ( !rcsFile.ExistsInDepot() || rcsFile.HeadDeleted() )
        {
          m_P4FileList.push_back( filePath );
        }
      }
    }
  }

  return true;
}


//template < class TextureMapAttributeType >
//void SetTextureMapAttribute( const Asset::AssetClass* assetClass, const tuid& fileId ) //const std::string& filePath )
//{
//  Attribute::AttributeEditor< TextureMapAttributeType > editor( assetClass );
//  if ( editor.Valid() )
//  {
//    editor->m_FileID = fileId;
//    editor.Commit();
//  }
//}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the wizard is finished.  Creates the file and sets up
// the appropriate attribute on the wizard's asset class.
// 
void ShaderTexturesPage::Finished()
{
  try
  {
    M_AttribToTextureMapPanel::iterator it = m_AttribToTextureMapPanel.begin();
    M_AttribToTextureMapPanel::iterator itEnd = m_AttribToTextureMapPanel.end();
    for ( ; it != itEnd ; ++it )
    {
      Attribute::AttributeBase* attribute = it->first;
      TextureMapFieldPanel* textureMapPanel = it->second;

      std::string filePath = textureMapPanel->GetFilePath();
      // only optional files should be empty
      if ( filePath.empty() )
      {
        continue;
      }

      tuid fileId = File::GlobalManager().Add( filePath );

      // if we dont have a fileId at the point, something bad happened
      if ( fileId == TUID::Null )
      {
        GetCreateAssetWizard()->ErrorOccurred();
        return;
      }

      const Asset::TextureMapAttributePtr& textureMapAttribute = Reflect::ObjectCast< Asset::TextureMapAttribute >( GetCreateAssetWizard()->GetAssetClass()->GetAttribute( attribute->GetType() ) );

      if ( textureMapAttribute.ReferencesObject() )
      {
        textureMapAttribute->SetFileID( fileId );
      }

      //SetTextureMapAttribute<Reflect::Registry::GetInstance()->GetClass( attribute->GetType() )>( GetCreateAssetWizard()->GetAssetClass(), fileId );

      //Attribute::AttributeEditor< Asset::TextureMapAttribute > editor( GetCreateAssetWizard()->GetAssetClass() );
      //if ( editor.Valid() )
      //{
      //  editor->m_FileID = fileID;
      //  editor.Commit();
      //}
    }
  }
  catch ( const File::Exception& )
  {
    std::stringstream str;
    wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetCreateAssetWizard() );
    
    //Early out and end the transaction
    GetCreateAssetWizard()->ErrorOccurred();
    return;
  } 
}
