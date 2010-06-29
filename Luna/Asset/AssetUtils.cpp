#include "Precompile.h"
#include "AssetUtils.h"
#include "Pipeline/Asset/AssetFlags.h"
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
