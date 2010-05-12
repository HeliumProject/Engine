#include "stdafx.h"
#include "AttributesPage.h"

#include "ManagedAsset.h"
#include "ShaderTexturesPage.h"
#include "SummaryPage.h"
#include "WizardPanels.h"

#include "Attribute/AttributeCategory.h"
#include "Asset/AssetClass.h"
#include "Asset/AssetTemplate.h"
#include "Asset/ShaderAsset.h"
#include "Asset/TextureMapAttribute.h"


// Using
using namespace AssetManager;

using namespace Reflect;

// Statics
// Following two variables are the message to display in the description area
// of the UI when no attribute is selected.
static const char* s_DefaultAttribDescHeader = "";
static const char* s_DefaultAttribDescBody = "Move your mouse over an attribute for more information.";

static const char* s_PageTitle = "Choose Attributes";
static const char* s_PageDescription = "Choose what attributes to add to this asset. If an attribute is disabled, that means it must be present. The following screens will allow you to customize these attributes. Hold your mouse over an item for additional information.";


///////////////////////////////////////////////////////////////////////////////
// Constructor for custom sorting algorithm.  Pass in the template containing
// the required and optional attributes.  See the following function for more
// information.
// 
AttributeSort::AttributeSort( const Asset::AssetTemplate* assetTemplate )
: m_CurrentTemplate( assetTemplate )
{
  NOC_ASSERT( m_CurrentTemplate );
}

///////////////////////////////////////////////////////////////////////////////
// Custom sorting algorithim for making a set of attributes that are alphabetized,
// and the required attributes come before the optional attributes.
// 
bool AttributeSort::operator()( Attribute::AttributeBase* first, Attribute::AttributeBase* second ) const
{
  NOC_ASSERT( first );
  NOC_ASSERT( second );

  // Standard definition of set decrees that this be the case.
  if ( first == second )
  {
    return false;
  }

  // Figure out if both attributes are required by the template or not.
  const Attribute::M_Attribute& required = m_CurrentTemplate->GetRequiredAttributes();
  const Attribute::M_Attribute::const_iterator reqEnd = m_CurrentTemplate->GetRequiredAttributes().end();
  bool isFirstRequired = required.find( first->GetType() ) != reqEnd;
  bool isSecondRequired = required.find( second->GetType() ) != reqEnd;

  // This check ensures that required attributes always sort ahead of optional ones.
  if ( isFirstRequired != isSecondRequired )
  {
    return isFirstRequired ? true : false;
  }

  // Pay attention to the declared sort order before resorting to alphabetizing
  if ( first->GetCategory()->SortGroup() != second->GetCategory()->SortGroup() )
  {
    return first->GetCategory()->SortGroup() < second->GetCategory()->SortGroup();
  }

  // If we make it here, just do a normal string comparison on the names.
  return first->GetClass()->m_UIName < second->GetClass()->m_UIName;
}


///////////////////////////////////////////////////////////////////////////////
// Constructor - protected; Use Create function to make a new instance of this
// class.
// 
AttributesPage::AttributesPage( CreateAssetWizard* wizard )
: WizardPage< AttribsPanel >( wizard )
, m_CurrentDescription( ReservedTypes::Invalid )
, m_NextPage( NULL )
, m_RequiredAttributes( GetCreateAssetWizard()->GetCurrentTemplate() )
{
  // Do not show this page if there is no template.
  NOC_ASSERT( GetCreateAssetWizard()->GetCurrentTemplate() );

  // Set up title of page
  m_Panel->m_Title->SetLabel( s_PageTitle );
  m_Panel->m_Description->SetLabel( s_PageDescription );

  // We have to manually wrap any static text fields that are too long for the screen.
  m_Panel->m_Description->Wrap( m_Panel->GetMinWidth() - 10 );
  m_Panel->Layout();

  // Set up the last page of the wizard
  m_NextPage = SummaryPage::Create( GetCreateAssetWizard() );

  ConnectListeners();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AttributesPage::~AttributesPage()
{
  DisconnectListeners();
}

///////////////////////////////////////////////////////////////////////////////
// Static creation function.  Returns a new (heap-allocated) instance of this 
// class.
// 
UIToolKit::WizardPage* AttributesPage::Create( CreateAssetWizard* wizard )
{
  return new AttributesPage( wizard );
}


///////////////////////////////////////////////////////////////////////////////
void AttributesPage::ConnectListeners()
{
  m_Panel->m_ScrollWindowAttributes->Connect( m_Panel->m_ScrollWindowAttributes->GetId(), wxEVT_MOTION, wxMouseEventHandler( AttributesPage::OnMouseMove ), NULL, this );
}

void AttributesPage::DisconnectListeners()
{
  m_Panel->m_ScrollWindowAttributes->Disconnect( m_Panel->m_ScrollWindowAttributes->GetId(), wxEVT_MOTION, wxMouseEventHandler( AttributesPage::OnMouseMove ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Called before the UI is shown.
// 
bool AttributesPage::TransferDataToWindow()
{
  DisconnectListeners();

  const Asset::AssetTemplate* assetTemplate = GetCreateAssetWizard()->GetCurrentTemplate();

  // Set up the page title
  std::string title = assetTemplate->m_Name;
  title += " - ";
  title += s_PageTitle;
  m_Panel->m_Title->SetLabel( title.c_str() );

  // Set up the attribute description panel.
  m_Panel->m_StaticAttribDescBody->SetLabel( s_DefaultAttribDescBody );
  m_Panel->m_StaticAttribDescHeading->SetLabel( s_DefaultAttribDescHeader );
  m_Panel->m_StaticAttribDescBody->Wrap( m_Panel->GetMinWidth() / 2 - 30 );

  // Clear out checkboxes that are already on the page
  m_Panel->m_ScrollWindowAttributes->DestroyChildren();
  m_CheckboxToAttrib.clear();

  // Set up required attributes
  m_RequiredAttributes.clear();
  SortAttributes( assetTemplate->GetRequiredAttributes(), m_RequiredAttributes );
  AddCheckboxes( m_RequiredAttributes, m_Panel->m_ScrollWindowAttributes, false );

  // Set up optional attributes
  S_SortedAttribute sortedAttribs( assetTemplate );
  SortAttributes( assetTemplate->GetOptionalAttributes(), sortedAttribs );
  AddCheckboxes( sortedAttribs, m_Panel->m_ScrollWindowAttributes, true );

  // Now that we're done adding and removing things from the window, update it
  m_Panel->m_ScrollWindowAttributes->Layout();

  // Set default value for the next page.
  SetNext( m_NextPage );

  ConnectListeners();

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Called when the page is changed.  Stores the UI data back to the persistent
// data managed by the wizard.
// 
bool AttributesPage::TransferDataFromWindow()
{
  // Set the summary page info
  m_PageSummary.clear();

  std::string buffer;
  
  buffer = GetCreateAssetWizard()->GetCurrentTemplate()->m_Name + " - Attributes:\n";
  m_PageSummary.push_back( StyledText( &m_TitleTextAttr, buffer ) );

  Asset::AssetClass* assetClass = GetCreateAssetWizard()->GetAssetClass();
  M_CheckboxAttribute::const_iterator checkItr = m_CheckboxToAttrib.begin();
  M_CheckboxAttribute::const_iterator checkEnd = m_CheckboxToAttrib.end();
  for ( ; checkItr != checkEnd; ++checkItr )
  {
    wxCheckBox* checkbox = checkItr->first;
    Attribute::AttributeBase* attribute = checkItr->second;
    if ( checkbox->IsChecked() )
    {
      if ( !assetClass->ContainsAttribute( attribute->GetType() ) )
      {
        // Add it
        Attribute::AttributePtr newAttrib = ObjectCast< Attribute::AttributeBase >( Registry::GetInstance()->CreateInstance( attribute->GetType() ) );
        assetClass->SetAttribute( newAttrib );
      }

      // Add to the summary page info
      buffer = std::string( "  o " ) + attribute->GetClass()->m_UIName + std::string( "\n" );
      m_PageSummary.push_back( StyledText( &m_DefaultTextAttr, buffer ) );
    }
    else
    {
      if ( assetClass->ContainsAttribute( attribute->GetType() ) )
      {
        // Remove it
        assetClass->RemoveAttribute( attribute->GetType() );
      }
    }
  }

  // Chain all the custom pages together
  ChainPages();

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Takes the original list of attributes and adds them to the sorted list,
// sorting alphabetically by attribute name.
// 
void AttributesPage::SortAttributes( const Attribute::M_Attribute& original, S_SortedAttribute& sorted )
{
  Attribute::M_Attribute::const_iterator itr = original.begin();
  Attribute::M_Attribute::const_iterator end = original.end();
  for ( ; itr != end; ++itr )
  {
    sorted.insert( itr->second.Ptr() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Adds all checkboxes to the UI for each attribute.
// 
void AttributesPage::AddCheckboxes( const S_SortedAttribute& attribs, wxWindow* parent, bool enabled )
{
  wxSizer* checkboxSizer = parent->GetSizer();
  S_SortedAttribute::const_iterator attribItr = attribs.begin();
  S_SortedAttribute::const_iterator attribEnd = attribs.end();
  for ( ; attribItr != attribEnd; ++attribItr )
  {
    Attribute::AttributeBase* attribute = *attribItr;
	  wxCheckBox* checkbox = new wxCheckBox( parent, wxID_ANY, attribute->GetClass()->m_UIName.c_str(), wxDefaultPosition, wxDefaultSize, 0 );

    bool isChecked = GetCreateAssetWizard()->GetAssetClass()->GetAttribute( attribute->GetType() ).ReferencesObject();
    checkbox->SetValue( isChecked );
	  checkbox->Enable( enabled );

    // Add the checkbox to the UI and to our list that tracks which attribute goes with it.
    checkboxSizer->Add( checkbox, 0, wxALL|wxEXPAND, 5 );
    m_CheckboxToAttrib.insert( M_CheckboxAttribute::value_type( checkbox, attribute ) );

    // There should not be any need to disconnect this listener since this page should
    // outlive any of the checkboxes.
    checkbox->Connect( checkbox->GetId(), wxEVT_MOTION, wxMouseEventHandler( AttributesPage::OnMouseMove ), NULL, this );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Updates the description that is displayed in the UI to the right of the
// attribute list.  The description will show information about the Class
// that is passed in.  If NULL is passed in, a default message will be displayed.
// 
void AttributesPage::SetAttribDescription( const Class* attribClass )
{
  i32 newTypeID = ReservedTypes::Invalid;
  if ( attribClass )
  {
    newTypeID = attribClass->m_TypeID;
  }

  if ( m_CurrentDescription != newTypeID )
  {
    m_CurrentDescription = newTypeID;

    if ( m_CurrentDescription!= ReservedTypes::Invalid )
    {
      std::string header( attribClass->m_UIName );
      header += " ";

      bool isRequired = false;
      const Attribute::M_Attribute& required = GetCreateAssetWizard()->GetCurrentTemplate()->GetRequiredAttributes();
      Attribute::M_Attribute::const_iterator assetItr = required.begin();
      Attribute::M_Attribute::const_iterator assetEnd = required.end();
      for ( ; assetItr != assetEnd && !isRequired; ++assetItr )
      {
        // Breaks out of the loop if match is found
        isRequired = assetItr->second->GetType() == m_CurrentDescription;
      }

      header += ( isRequired ? "(required)" : "(optional)" );
      m_Panel->m_StaticAttribDescHeading->SetLabel( header.c_str() );
      m_Panel->m_StaticAttribDescBody->SetLabel( attribClass->GetProperty( Asset::AssetProperties::ShortDescription ).c_str() );
    }
    else
    {
      m_Panel->m_StaticAttribDescHeading->SetLabel( s_DefaultAttribDescHeader );
      m_Panel->m_StaticAttribDescBody->SetLabel( s_DefaultAttribDescBody );
    }

    m_Panel->m_StaticAttribDescBody->Wrap( m_Panel->GetMinWidth() / 2 - 30 );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Chains together all the custom attribute pages in the Wizard, based upon
// the attributes that are currently present on the wizard's asset class.
// 
void AttributesPage::ChainPages()
{
  // Sort the attributes into the same order that they are displayed in the
  // checkbox list.
  Asset::AssetClass* assetClass = GetCreateAssetWizard()->GetAssetClass();

  // special case if the asset being created is a shader class
  if ( assetClass->HasType( Reflect::GetType<Asset::ShaderAsset>() ) )
  {
    UIToolKit::WizardPage* shaderPage = ShaderTexturesPage::Create( GetCreateAssetWizard() );

    if ( shaderPage )
    {
      SetNext( shaderPage );
      shaderPage->SetNext( m_NextPage );
    }
  }
  else
  {
    S_SortedAttribute sortedAttributes( GetCreateAssetWizard()->GetCurrentTemplate() );

    Attribute::M_Attribute::const_iterator itr = assetClass->GetAttributes().begin();
    Attribute::M_Attribute::const_iterator end = assetClass->GetAttributes().end();
    for ( ; itr != end; ++itr )
    {
      sortedAttributes.insert( itr->second.Ptr() );
    }

    UIToolKit::WizardPage* previousPage = NULL;

    // Go through the sorted list and chain the wizard pages together.
    S_SortedAttribute::const_iterator attrItr = sortedAttributes.begin();
    S_SortedAttribute::const_iterator attrEnd = sortedAttributes.end();
    for ( ; attrItr != attrEnd; ++attrItr )
    {
      Attribute::AttributeBase* attribute = *attrItr;

      UIToolKit::WizardPage* currentPage = GetCreateAssetWizard()->GetCustomAttributePage( attribute->GetType() );
      if ( currentPage )
      {
        if ( previousPage )
        {
          previousPage->SetNext( currentPage );
        }
        else
        {
          SetNext( currentPage );
        }
        currentPage->SetNext( m_NextPage );
        previousPage = currentPage;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// Callback for when the mouse moves over the scroll window, or one of the 
// contained checkboxes.  Updates the attribute description area of the UI
// based upon which attribute the mouse is over.
// 
void AttributesPage::OnMouseMove( wxMouseEvent& args )
{
  args.Skip();

  // If the event came from a checkbox we can just grab it and go.
  wxCheckBox* checkbox = wxDynamicCast( args.GetEventObject(), wxCheckBox );
  if ( !checkbox )
  {
    // The event did not come from a check box (probably came from the scroll window
    // itself).  Iterate over all the checkboxes and look for one that the mouse is
    // over.
    wxWindowList::const_iterator childItr = m_Panel->m_ScrollWindowAttributes->GetChildren().begin();
    wxWindowList::const_iterator childEnd = m_Panel->m_ScrollWindowAttributes->GetChildren().end();
    for ( ; childItr != childEnd; ++childItr )
    {
      wxWindow* child = *childItr;
      wxRect rect( child->GetPosition(), child->GetSize() );
      if ( rect.Contains( args.m_x, args.m_y ) )
      {
        // Found the checkbox; save it and break.
        checkbox = wxStaticCast( child, wxCheckBox );
        break;
      }
    }
  }

  // Figure out what type info to display based upon which checkbox the mouse
  // is over.
  const Class* attribClass = NULL;
  if ( checkbox )
  {
    M_CheckboxAttribute::const_iterator found = m_CheckboxToAttrib.find( checkbox );
    if ( found != m_CheckboxToAttrib.end() )
    {
      Attribute::AttributeBase* attribute = found->second;
      attribClass = attribute->GetClass();
    }
  }
  SetAttribDescription( attribClass );
}


///////////////////////////////////////////////////////////////////////////////
// Called when the wizard completes.  Makes sure that the asset class is ready
// to be passed back to the caller.
// 
void AttributesPage::Finished()
{
  // If we have a template, at least make sure that the required attributes
  // have been added to the asset class.
  if ( GetCreateAssetWizard()->GetCurrentTemplate() )
  {
    Attribute::M_Attribute::const_iterator itr = GetCreateAssetWizard()->GetCurrentTemplate()->GetRequiredAttributes().begin();
    Attribute::M_Attribute::const_iterator end = GetCreateAssetWizard()->GetCurrentTemplate()->GetRequiredAttributes().end();
    for ( ; itr != end; ++itr )
    {
      const Attribute::AttributePtr& attributeToClone = itr->second;
      if ( !GetCreateAssetWizard()->GetAssetClass()->ContainsAttribute( attributeToClone->GetType() ) )
      {
        Attribute::AttributePtr clone = ObjectCast< Attribute::AttributeBase >( Registry::GetInstance()->CreateInstance( attributeToClone->GetType() ) );
        // This will throw if something is wrong, but I think the templates should be made
        // in such a way as to never cause validation failure here, so no bother putting
        // a try-catch block in.
        GetCreateAssetWizard()->GetAssetClass()->SetAttribute( clone );
      }
    }
  }
}