#include "EnginePch.h"
#include "ObjectLoaderVisitors.h"

#include "Engine/GameObjectPointerData.h"

Helium::ClearLinkIndicesFromObject::ClearLinkIndicesFromObject()
{

}

Helium::ClearLinkIndicesFromObject::~ClearLinkIndicesFromObject()
{

}

bool Helium::ClearLinkIndicesFromObject::VisitField( void* instance, const Helium::Reflect::Field* field )
{
    if ( field->m_DataClass == Reflect::GetClass< GameObjectPointerData >() )
    {
        Reflect::DataPtr go_data_untyped = field->CreateData( instance );
        GameObjectPointerData *go_data = Reflect::AssertCast<GameObjectPointerData>(go_data_untyped.Get());
        if (go_data && go_data->m_Data->HasLinkIndex())
        {
            go_data->m_Data->ClearLinkIndex();
        }
    }

    // We never want to visit fields because
    // - The data we need can be found by just looking at the field and data
    // - In this case, the game object pointers are link indices so we will crash if we try
    //   to visit them
    return false;
}
