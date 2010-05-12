#include "stdafx.h"

#include "TextureMapFieldPanel.h"
#include "WizardPanels.h"

#include "Asset/AssetFlags.h"
#include "Attribute/AttributeCategory.h"
#include "FileBrowser/FileBrowser.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "UIToolKit/FileDialog.h"
#include "UIToolKit/ImageManager.h"

using namespace AssetManager;

///////////////////////////////////////////////////////////////////////////////
TextureMapFieldPanel::TextureMapFieldPanel( wxWindow* parent, Attribute::AttributeBase* attribute, bool isRequired )
: TextureMapPanel( parent )
, m_Attribute( attribute )
, m_IsRequired( isRequired )
{
  SetFieldLabel();

  m_FileBrowserButton->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "magnify_16.png" ) );

  this->SetMinSize( wxSize( 540,-1 ) );
  this->Layout();
}


///////////////////////////////////////////////////////////////////////////////
void TextureMapFieldPanel::SetFilePath( const std::string& filePath )
{
  m_FilePath = filePath;
  m_FilePathField->ChangeValue( m_FilePath.c_str() );
}


///////////////////////////////////////////////////////////////////////////////
void TextureMapFieldPanel::IsRequired( bool isRequired )
{
  if ( isRequired == m_IsRequired )
  {
    return;
  }

  m_IsRequired = isRequired;

  SetFieldLabel();
}


///////////////////////////////////////////////////////////////////////////////
void TextureMapFieldPanel::SetFieldLabel()
{
  const Reflect::Type* attribType = Reflect::Registry::GetInstance()->GetType( m_Attribute->GetType() );

  std::string title;
  title += ( m_IsRequired ? "*" : "" ) ;
  title += attribType->m_UIName;
  title += " File:";
  m_FieldLabel->SetLabel( title.c_str() );
  m_FieldLabel->Wrap( -1 ); //GetMinWidth() - 10 );

  // set the field 
  m_FieldLabel->SetHelpText( attribType->GetProperty( Asset::AssetProperties::ShortDescription ).c_str() );
}

///////////////////////////////////////////////////////////////////////////////
// Basic checks that this is a valid file that exists
bool TextureMapFieldPanel::ValidateFilePath( std::string& error ) const
{
  if ( m_FilePath.empty() )
  {
    // earlier out if the path is empty, this is all optional after all
    if ( !IsRequired() )
    {
      return true;
    }
    else
    {
      std::stringstream str;
      str << "Please specify a file path for the required " << Reflect::Registry::GetInstance()->GetType( m_Attribute->GetType() )->m_UIName << " attribute.";
      error = str.str();
      return false;
    }
  }

  //if ( !FileSystem::HasExtension( m_FilePath, m_Extension->GetExtension() ) )
  //{
  //  std::stringstream str;
  //  str << "The specified file is not valid. The file must end with \"" << m_Extension->GetFilter() << "\".";
  //  error = str.str();
  //  return false;
  //}

  if ( !FileSystem::Exists( m_FilePath ) )
  {
    error = "The file that you specified does not exist. \nPlease create the file before continuing.";
    return false;
  }


  // Make sure that the path is in the project.
  if ( !Finder::IsInProject( m_FilePath ) )
  {
    std::stringstream str;
    str << "The file you specified is not in the project root.  Make sure your file is located in:" << std::endl << Finder::ProjectRoot() + ".";
    error = str.str();
    return false;
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////
// Called when a change is made to the file path field
//
void TextureMapFieldPanel::OnFieldTextEnter( wxCommandEvent& event )
{
  // get and clean user input
  m_FilePath = m_FilePathField->GetValue().c_str();
  FileSystem::CleanName( m_FilePath );
}


///////////////////////////////////////////////////////////////////////////////
void TextureMapFieldPanel::OnFileBrowserButtonClick( wxCommandEvent& event )
{
  UIToolKit::FileDialog browserDlg( GetParent(), "Select Texture File", FinderSpecs::Asset::TEXTURES_FOLDER.GetFolder(), "", "", UIToolKit::FileDialogStyles::DefaultOpen );

  browserDlg.AddFilter( FinderSpecs::Extension::TEXTUREMAP_FILTER.GetDialogFilter() );
  browserDlg.SetFilterIndex( FinderSpecs::Extension::TGA.GetDialogFilter() );

  if ( browserDlg.ShowModal() == wxID_OK )
  {
    m_FilePath = browserDlg.GetPath();
    m_FilePathField->ChangeValue( m_FilePath.c_str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
void TextureMapFieldPanel::OnExplorerButtonClick( wxCommandEvent& event )
{
  File::FileBrowser browserDlg( GetParent(), -1, "Find Texture File" );

  browserDlg.SetDirectory( FinderSpecs::Asset::TEXTURES_FOLDER.GetFolder() );
  browserDlg.AddFilter( FinderSpecs::Extension::TEXTUREMAP_FILTER );
  browserDlg.SetFilterIndex( FinderSpecs::Extension::TGA );

  if ( browserDlg.ShowModal() == wxID_OK )
  {
    m_FilePath = browserDlg.GetPath();
    m_FilePathField->ChangeValue( m_FilePath.c_str() );
  }
}