#include "Precompile.h"
#include "AssetUtils.h"
#include "Pipeline/Asset/AssetFlags.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Field.h"
#include "Foundation/Reflect/Serializers.h"
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified field is appropriately flagged as referencing
// a single file or a collection of files.
// 
bool Luna::IsFileReference( Reflect::Element* element, const Reflect::Field* field )
{
  bool isFileRef = false;
  if ( field->m_Flags & Reflect::FieldFlags::Path )
  {
      if ( field->m_SerializerID == Reflect::GetType< Reflect::PointerSerializer >() ||
      field->m_SerializerID == Reflect::GetType< Reflect::SetSerializer >() )
    {
      isFileRef = true;
    }
  }
  return isFileRef;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified field can reference a file that this editable
// by the Asset Editor.
// 
bool Luna::IsAssetFileReference( Reflect::Element* element, const Reflect::Field* field )
{
  bool isAssetFileRef = false;

  if ( Luna::IsFileReference( element, field ) )
  {
    const Finder::FinderSpec* spec = NULL;
    std::string specName;
    if ( field->GetProperty( Asset::AssetProperties::FilterSpec, specName ) )
    {
      spec = Finder::GetFinderSpec( specName );
    }
    else if ( field->GetProperty( Asset::AssetProperties::ModifierSpec, specName ) )
    {
      spec = Finder::GetFinderSpec( specName );
    }

    if ( spec )
    {
      if ( spec->GetFilter() == FinderSpecs::Asset::ASSET_EDITOR_FILTER.GetFilter() )
      {
        // Simple case, the field's spec matches the spec used by the Asset Editor.
        isAssetFileRef = true;
      }
      else
      {
        // The assumption here is that a field either references an asset or it
        // doesn't.  There might be more than one type of asset that a field references
        // but we just check to make sure that the first filter is editable by
        // the Asset Editor.  If we ever support a field that can reference
        // Asset Editor files and non-Asset Editor files, this will have to change.
        std::string firstFilter = spec->GetFilter();
        std::string::iterator newEnd = std::remove( firstFilter.begin(), firstFilter.end(), '*' );
        firstFilter.erase( newEnd );
        std::string::size_type pos = firstFilter.find( ';' );
        firstFilter = firstFilter.substr( 0, pos );
        if ( !firstFilter.empty() )
        {
          isAssetFileRef = FinderSpecs::Asset::ASSET_EDITOR_FILTER.IsExtensionValid( firstFilter );
        }
      }
    }
  }

  return isAssetFileRef;
}
