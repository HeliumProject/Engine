#include "stdafx.h"

#include "CreateAssetWizard.h"
#include "ManagedAsset.h"

#include "ArtFilePage.h"
#include "WorldFilePage.h"
#include "PerforcePage.h"
#include "CreateAssetFileLocationPage.h"

#include "Asset/AssetClass.h"
#include "Asset/AssetInit.h"
#include "Asset/AssetTemplate.h"
#include "Asset/ArtFileAttribute.h"
#include "Asset/WorldFileAttribute.h"
#include "FileSystem/FileSystem.h"
#include "Finder/ExtensionSpecs.h"
#include "Reflect/Registry.h"

#include "RCS/RCS.h"

// for path munging
#include <boost/regex.hpp> 

// Using
using namespace AssetManager;

using namespace Reflect;


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
CreateAssetWizard::CreateAssetWizard( wxWindow *parent, i32 assetClassTypeID )
: Wizard( parent, OperationFlags::Create )
, m_CreateAssetFileLocationPage( NULL )
, m_AttributesPage( NULL )
{
  // Sanity checks
  NOC_ASSERT( Registry::GetInstance()->GetClass( assetClassTypeID ) );
  NOC_ASSERT( Registry::GetInstance()->GetClass( assetClassTypeID )->HasType( Reflect::GetType<Asset::AssetClass>() ) );

  // Create the asset class that the wizard will be editing.
  SetAssetClass( ObjectCast< Asset::AssetClass >( Registry::GetInstance()->CreateInstance( assetClassTypeID ) ) );

  // Create the first page of the wizard.
  m_CreateAssetFileLocationPage = wxDynamicCast( CreateAssetFileLocationPage::Create( this ), CreateAssetFileLocationPage );

  // Set up creator function callbacks
  // register all the custom attribute pages that this wizard can show.
  m_CustomPageCreators[ Reflect::GetType<Asset::ArtFileAttribute>() ]   = &ArtFilePage::Create;
  m_CustomPageCreators[ Reflect::GetType<Asset::WorldFileAttribute>() ] = &WorldFilePage::Create;

  SetFirstPage( m_CreateAssetFileLocationPage );
  SetTitle( "Create Asset" );

  GetPerforcePage()->SetReopenFiles( true );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
CreateAssetWizard::~CreateAssetWizard()
{
}

///////////////////////////////////////////////////////////////////////////////
void CreateAssetWizard::SetDefaultDirectory( const std::string& defaultDir )
{
  if ( m_CreateAssetFileLocationPage )
  {
    m_CreateAssetFileLocationPage->SetDefaultDirectory( defaultDir );
  }
}

///////////////////////////////////////////////////////////////////////////////
// When renaming or duplicating an asset, this will indicate where the new asset
// should be saved.
// 
const std::string& CreateAssetWizard::GetNewFileLocation() const
{
  return GetManagedAsset()->m_Path;
}


///////////////////////////////////////////////////////////////////////////////
// Allows wizard pages to set the location of where a new asset file will be
// saved (for rename/duplicate).
// 
void CreateAssetWizard::SetNewFileLocation( const std::string& file )
{
  GetManagedAsset()->m_Path = file;
}


///////////////////////////////////////////////////////////////////////////////
// Returns the wizard page for the specified attribute.  If the page does not 
// exist, it will be created.  If there is no page for that attribute, NULL is
// returned.
// 
UIToolKit::WizardPage* CreateAssetWizard::GetCustomAttributePage( i32 attributeTypeID )
{
  UIToolKit::WizardPage* page = NULL;

  M_Page::const_iterator foundAllocated = m_CustomPages.find( attributeTypeID );
  if ( foundAllocated != m_CustomPages.end() )
  {
    page = foundAllocated->second;
  }
  else
  {
    M_PageCreator::const_iterator foundCreator = m_CustomPageCreators.find( attributeTypeID );
    if ( foundCreator != m_CustomPageCreators.end() )
    {
      page = ( *foundCreator->second )( this, attributeTypeID );
      m_CustomPages.insert( M_Page::value_type( attributeTypeID, page ) );
    }
  }

  return page;
}

///////////////////////////////////////////////////////////////////////////////
// Specify which of the asset's templates is currently being used.  This may
// cause the path to the asset to change.  It may also cause the current 
// attributes on the asset class to change.
// 
void CreateAssetWizard::SetCurrentTemplate( const Asset::AssetTemplate* assetTemplate )
{
  if ( assetTemplate != m_CurrentTemplate )
  {
    m_CurrentTemplate = assetTemplate;
    ResetAttributes();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Goes through all the attributes on the asset class and makes sure that they
// are suppose to be there (based upon the current template).  Any invalid
// attributes are removed.  This function also adds any attributes required by 
// the current template, if they are missing.
// 
void CreateAssetWizard::ResetAttributes()
{
  // Reset the attributes on the asset class when the template changes.
  if ( GetCurrentTemplate() )
  {
    S_i32 newAttributes;

    // Add the required attributes.
    Attribute::M_Attribute::const_iterator reqItr = GetCurrentTemplate()->GetRequiredAttributes().begin();
    Attribute::M_Attribute::const_iterator reqEnd = GetCurrentTemplate()->GetRequiredAttributes().end();
    for ( ; reqItr != reqEnd; ++reqItr )
    {
      const Attribute::AttributePtr& attrib = reqItr->second;
      newAttributes.insert( S_i32::value_type( attrib->GetType() ) );
    }

    // Look for optional attributes that were in the previous list and keep them around too.
    Attribute::M_Attribute::const_iterator optItr = GetCurrentTemplate()->GetOptionalAttributes().begin();
    Attribute::M_Attribute::const_iterator optEnd = GetCurrentTemplate()->GetOptionalAttributes().end();
    for ( ; optItr != optEnd; ++optItr )
    {
      const Attribute::AttributePtr& attrib = optItr->second;
      if ( GetAssetClass()->ContainsAttribute( attrib->GetType() ) )
      {
        newAttributes.insert( S_i32::value_type( attrib->GetType() ) );
      }
    }

    // Remove all the old attributes.
    GetAssetClass()->Clear();

    // Iterate over the attribute list and actually add the new attributes.
    S_i32::const_iterator newItr = newAttributes.begin();
    S_i32::const_iterator newEnd = newAttributes.end();
    for ( ; newItr != newEnd; ++newItr )
    {
      Attribute::AttributePtr newAttrib = ObjectCast< Attribute::AttributeBase >( Registry::GetInstance()->CreateInstance( *newItr ) );
      GetAssetClass()->SetAttribute( newAttrib );
    }
  }
  else
  {
    // No template specified, just clear the attributes.
    GetAssetClass()->Clear();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the wizard completes.  Makes sure that the asset class is ready
// to be passed back to the caller.
// 
void CreateAssetWizard::ProcessOperation()
{
  // If we have a template, at least make sure that the required attributes
  // have been added to the asset class.
  if ( GetCurrentTemplate() )
  {
    Attribute::M_Attribute::const_iterator itr = GetCurrentTemplate()->GetRequiredAttributes().begin();
    Attribute::M_Attribute::const_iterator end = GetCurrentTemplate()->GetRequiredAttributes().end();
    for ( ; itr != end; ++itr )
    {
      const Attribute::AttributePtr& attributeToClone = itr->second;
      if ( !GetAssetClass()->ContainsAttribute( attributeToClone->GetType() ) )
      {
        Attribute::AttributePtr clone = ObjectCast< Attribute::AttributeBase >( Registry::GetInstance()->CreateInstance( attributeToClone->GetType() ) );
        // This will throw if something is wrong, but I think the templates should be made
        // in such a way as to never cause validation failure here, so no bother putting
        // a try-catch block in.
        GetAssetClass()->SetAttribute( clone );
      }
    }
  }

  GetAssetClass()->m_AssetClassID = File::GlobalManager().Open( GetNewFileLocation() );
}


void CreateAssetWizard::PostProcessUpdate()
{
  Reflect::Archive::ToFile( GetAssetClass(), GetNewFileLocation(), new Asset::AssetVersion(), NULL );
  
  Asset::AssetClass::InvalidateCache( GetAssetClass()->m_AssetClassID );
}


///////////////////////////////////////////////////////////////////////////////
// Create the default changelist description
//
const std::string& CreateAssetWizard::GetChangeDescription()
{
  if ( m_Changeset.m_Description.empty() )
  {
    std::stringstream description;
    description << "Creating Asset";
    description << " " << GetManagedAsset()->m_Path;

    m_Changeset.m_Description = description.str();
  }

  return m_Changeset.m_Description;
}