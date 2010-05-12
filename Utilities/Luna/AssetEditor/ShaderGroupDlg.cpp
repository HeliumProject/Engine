#include "Precompile.h"
#include "ShaderGroupDlg.h"
#include "AssetClass.h"
#include "AssetEditorGenerated.h"
#include "ShaderGroupPropGrid.h"
#include "Asset/ShaderGroupAttribute.h"
#include "FileSystem/FileSystem.h"
#include "UIToolKit/ImageManager.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
//
ShaderGroupDlg::ShaderGroupDlg( wxWindow* parent, Luna::AssetClass* asset )
: wxDialog( parent, wxID_ANY, "Manage Shader Groups", wxDefaultPosition, wxSize( 560, 375 ), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
, m_Asset( asset )
{
	SetSizeHints( wxDefaultSize, wxDefaultSize );
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

  Luna::AttributeWrapper* attribute = m_Asset->FindAttribute( Reflect::GetType< Asset::ShaderGroupAttribute >() );
  if ( attribute )
  {
    Asset::ShaderGroupAttributePtr shaderGroup = attribute->GetPackage< Asset::ShaderGroupAttribute >();
    m_ShaderGroupAttribute = Reflect::AssertCast< Asset::ShaderGroupAttribute >( shaderGroup->Clone() );
  }
  else
  {
    m_ShaderGroupAttribute = new Asset::ShaderGroupAttribute();
  }

  Luna::AttributeWrapper* shaderUsagesAttrib = m_Asset->FindAttribute( Reflect::GetType< Asset::ShaderUsagesAttribute >() );

  m_Panel = new SwapShadersPanel( this, wxID_ANY );
  m_Panel->m_PropertyGrid->Initialize( this, shaderUsagesAttrib->GetPackage< Asset::ShaderUsagesAttribute >(), m_ShaderGroupAttribute );

  PopulateChoices();

  mainSizer->Add( m_Panel, 1, wxEXPAND | wxALL, 5 );
	SetSizer( mainSizer );
	Layout();

  // Hide the thumbnail until it is needed.
  m_Panel->m_Thumbnail->Hide();

  // Connect listeners
  m_Panel->m_ButtonNew->Connect( m_Panel->m_ButtonNew->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ShaderGroupPropGrid::OnAddShaderGroup ), NULL, m_Panel->m_PropertyGrid );
  m_Panel->m_ButtonRemove->Connect( m_Panel->m_ButtonRemove->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ShaderGroupPropGrid::OnDeleteShaderGroup ), NULL, m_Panel->m_PropertyGrid );
  m_Panel->m_DefaultGroup->Connect( m_Panel->m_DefaultGroup->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ShaderGroupDlg::OnChooseShaderGroup ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
//
ShaderGroupDlg::~ShaderGroupDlg()
{
}

///////////////////////////////////////////////////////////////////////////////
// Call to display the dialog.  Returns wxID_OK if the user click the OK button,
// otherwise returns wxID_CANCEL.  This function will attempt to validate the
// user's input.
//
int ShaderGroupDlg::ShowModal()
{
  int result = wxID_CANCEL;
  bool isDone = false;

  while ( !isDone && ( result = __super::ShowModal() ) == wxID_OK )
  {
    isDone = true;

    std::string error;
    if ( !m_Panel->m_PropertyGrid->Validate( error ) )
    {
      isDone = false;
      wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, this );
    }
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the attribute containing information about the shader groups, or
// NULL if the shader groups have all been removed.  Only valid if ShowModal
// was called and returned wxID_OK.
//
Asset::ShaderGroupAttributePtr ShaderGroupDlg::GetShaderGroupAttribute() const
{
  return m_ShaderGroupAttribute;
}

///////////////////////////////////////////////////////////////////////////////
// Repopulates the choice control for choosing the default shader group.
// 
void ShaderGroupDlg::UpdateChoices()
{
  // Removed this code until we decide to support a selection for which shader
  // group to use in mobyview.

  //m_Panel->m_DefaultGroup->Freeze();

  //const i32 selected = m_Panel->m_DefaultGroup->GetSelection();

  //Asset::V_ShaderGroupSmartPtr::const_iterator groupItr = m_ShaderGroupAttribute->m_ShaderGroups.begin();
  //Asset::V_ShaderGroupSmartPtr::const_iterator groupEnd = m_ShaderGroupAttribute->m_ShaderGroups.end();
  //for ( ; groupItr != groupEnd; ++groupItr )
  //{
  //  M_ChoiceLookup::M_BToA::const_iterator found = m_ChoiceLookup.BToA().find( ( *groupItr )->m_ID );
  //  if ( found != m_ChoiceLookup.BToA().end() )
  //  {
  //    const i32 index = *found->second;
  //    m_Panel->m_DefaultGroup->SetString( index, ( *groupItr )->m_Name.c_str() );
  //  }
  //  else
  //  {
  //    const i32 index = m_Panel->m_DefaultGroup->Append( ( *groupItr )->m_Name.c_str() );
  //    m_ChoiceLookup.Insert( index, ( *groupItr )->m_ID );
  //  }
  //}

  //m_Panel->m_DefaultGroup->SetSelection( selected );

  //m_Panel->m_DefaultGroup->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Updates the preview thumbnail image displayed on this dialog.
// 
void ShaderGroupDlg::SetThumbnail( std::string imagePath )
{
  if ( imagePath.empty() )
  {
    m_Panel->m_Thumbnail->SetBitmap( wxNullBitmap );
    m_Panel->m_Thumbnail->Hide();
  }
  else
  {
    if ( FileSystem::Exists( imagePath ) )
    {
      wxBitmap bmp( UIToolKit::GlobalImageManager().GetScaledBitmap( imagePath, m_Panel->m_Thumbnail->GetSize().x ) );
      if ( bmp.IsOk() )
      {
        // Show thumbnail
        m_Panel->m_Thumbnail->SetBitmap( bmp );
        m_Panel->m_Thumbnail->Show();

        // Update the tooltip
        std::string tooltip( "Color map: " );
        tooltip += FileSystem::GetLeaf( imagePath );
        m_Panel->m_Thumbnail->SetToolTip( tooltip.c_str() );
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to initially build the choice control for the default
// shader group.
// 
void ShaderGroupDlg::PopulateChoices()
{
  // Removed this code until we decide to support a selection for which shader
  // group to use in mobyview.

  //m_Panel->m_DefaultGroup->Clear();
  //const i32 defaultIndex = m_Panel->m_DefaultGroup->Append( "Default" );
  //m_ChoiceLookup.Insert( defaultIndex, TUID::Null );
  //
  //i32 chosenIndex = defaultIndex;
  //Luna::AttributeWrapper* shaderGroupAttr = m_Asset->FindAttribute( Reflect::GetType< Asset::ShaderGroupAttribute >() );
  //if ( shaderGroupAttr )
  //{
  //  Asset::ShaderGroupAttributePtr pkg = shaderGroupAttr->GetPackage< Asset::ShaderGroupAttribute >();
  //  Asset::V_ShaderGroupSmartPtr::const_iterator groupItr = pkg->m_ShaderGroups.begin();
  //  Asset::V_ShaderGroupSmartPtr::const_iterator groupEnd = pkg->m_ShaderGroups.end();
  //  for ( ; groupItr != groupEnd; ++groupItr )
  //  {
  //    const tuid id = ( *groupItr )->m_ID;
  //    const i32 index = m_Panel->m_DefaultGroup->Append( ( *groupItr )->m_Name.c_str() );
  //    m_ChoiceLookup.Insert( index, id );

  //    if ( id == pkg->m_DefaultShaderGroup )
  //    {
  //      chosenIndex = index;
  //    }
  //  }
  //}

  //m_Panel->m_DefaultGroup->SetSelection( chosenIndex );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the default shader group choice control is changed.  
// Updates the shader group attribute.
// 
void ShaderGroupDlg::OnChooseShaderGroup( wxCommandEvent& args )
{
  M_ChoiceLookup::M_AToB::const_iterator found = m_ChoiceLookup.AToB().find( m_Panel->m_DefaultGroup->GetSelection() );
  if ( found != m_ChoiceLookup.AToB().end() )
  {
    Asset::ShaderGroupPtr group = m_ShaderGroupAttribute->FindShaderGroup( *found->second );
    if ( group.ReferencesObject() )
    {
      m_ShaderGroupAttribute->m_DefaultShaderGroup = group->m_ID;
    }
  }
}
