#include "Precompile.h"
#include "FieldFileReference.h"

#include "AssetManager.h"
#include "AssetPreferences.h"
#include "AssetUtils.h"

#include "Editor/ApplicationPreferences.h"
#include "Foundation/Reflect/Serializers.h"

using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::FieldFileReference );

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void FieldFileReference::InitializeType()
{
  Reflect::RegisterClass<Luna::FieldFileReference>( "Luna::FieldFileReference" );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void FieldFileReference::CleanupType()
{
  Reflect::UnregisterClass<Luna::FieldFileReference>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
FieldFileReference::FieldFileReference( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field, const Nocturnal::Path& path )
: Luna::FieldNode( assetManager, element, field )
, m_Path( path )
, m_UseLabelPrefix( true )
{
  NOC_ASSERT( Luna::IsFileReference( element, field ) );

  SetName( MakeLabel() );

  // Add listeners
  GetAssetEditorPreferences()->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &FieldFileReference::PreferenceChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
FieldFileReference::~FieldFileReference()
{
  // Remove listeners
  GetAssetEditorPreferences()->RemoveChangedListener(  Reflect::ElementChangeSignature::Delegate( this, &FieldFileReference::PreferenceChanged ) );

  delete m_Path;
}

///////////////////////////////////////////////////////////////////////////////
// Specify whether the field name should appear before the path on this node's 
// name.  By default, this node's name will be of the form: "Field: x:/somepath".
// 
void FieldFileReference::SetUseLabelPrefix( bool useLabelPrefix )
{
  if ( useLabelPrefix != m_UseLabelPrefix )
  {
    m_UseLabelPrefix = useLabelPrefix;
    SetName( MakeLabel() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when this node is double-clicked in the UI.
// 
void FieldFileReference::ActivateItem()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the label to use for naming this node.
// 
std::string FieldFileReference::MakeLabel() const
{
  std::string label;
  if ( m_UseLabelPrefix )
  {
    label += m_Field->m_UIName + ": ";
  }

  FilePathOptions::FilePathOption filePathOption = FilePathOptions::PartialPath;
  GetAssetEditorPreferences()->GetEnum( GetAssetEditorPreferences()->FilePathOption(), filePathOption );
  label += Luna::PathToLabel( m_Path, filePathOption );
  return label;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the file path preference is updated.  Resets the label
// to display the file path as desired.
// 
void FieldFileReference::PreferenceChanged( const Reflect::ElementChangeArgs& args )
{
  if ( args.m_Element && args.m_Field )
  {
    if ( args.m_Field == GetAssetEditorPreferences()->FilePathOption() )
    {
      SetName( MakeLabel() );
    }
  }
}
