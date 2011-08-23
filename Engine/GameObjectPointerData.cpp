#include "EnginePch.h"
#include "Engine/GameObjectPointerData.h"

#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_OBJECT( GameObjectPointerData );

GameObjectPointerData::GameObjectPointerData()
{

}

GameObjectPointerData::~GameObjectPointerData()
{

}
//
//void PointerData::ConnectData(void* data)
//{
//    m_Data.Connect( data );
//}
//
//bool PointerData::Set(Data* data, uint32_t flags)
//{
//    const PointerData* rhs = SafeCast<PointerData>(data);
//    if (!rhs)
//    {
//        return false;
//    }
//
//    if (flags & DataFlags::Shallow)
//    {
//        *m_Data = *rhs->m_Data;
//    }
//    else
//    {
//        *m_Data = (*rhs->m_Data).ReferencesObject() ? (*rhs->m_Data)->Clone() : NULL;
//    }
//
//    return true;
//}
//
//bool PointerData::Equals(Object* object)
//{
//    const PointerData* rhs = SafeCast<PointerData>(object);
//    
//    if (!rhs)
//    {
//        return false;
//    }
//
//    // if the pointers are equal we are done
//    if ( *m_Data == *rhs->m_Data )
//    {
//        return true;
//    }
//    // if they are not equal but one is null we are done
//    else if ( (*m_Data).ReferencesObject() || !(*rhs->m_Data).ReferencesObject() )
//    {
//        return false;
//    }
//
//    // pointers aren't equal so we have to do deep equality test
//    return (*m_Data)->Equals( *rhs->m_Data );
//}
//
//void PointerData::Accept(Visitor& visitor)
//{
//    if ( !visitor.VisitPointer( *m_Data ) )
//    {
//        return;
//    }
//
//    if ( *m_Data )
//    {
//        (*m_Data)->Accept( visitor );
//    }
//}
//
// void GameObjectPointerData::Serialize(ArchiveBinary& archive)
// {
//     Serialize<ArchiveBinary>( archive );
// }
// 
// void GameObjectPointerData::Deserialize(ArchiveBinary& archive)
// {
//     Deserialize<ArchiveBinary>( archive );
// }
// 
// void GameObjectPointerData::Serialize(ArchiveXML& archive)
// {
//     Serialize<ArchiveXML>( archive );
// }
// 
// void GameObjectPointerData::Deserialize(ArchiveXML& archive)
// {
//     Deserialize<ArchiveXML>( archive );
// }

template< class ArchiveT >
void GameObjectPointerData::Serialize(ArchiveT& archive)
{
    if (m_Field->m_Flags & FieldFlags::Share)
    {
        if (m_Data->ReferencesObject())
        {
            // Valid path
            Reflect::GameObject *game_object = Reflect::AssertCast<GameObject>(m_Data->Get());
            archive.GetStream().WriteString(game_object->GetPath().ToString());
        }
        else
        {
            // Empty string means null
            archive.GetStream().WriteString("");
        }
    }
    else
    {
        // If it's not a shared object, just write this inline as if it were a normal object
        Super::Serialize<ArchiveT>(archive);
    }
}

template< class ArchiveT >
void GameObjectPointerData::Deserialize(ArchiveT& archive)
{
    *m_Data = NULL;

    if (m_Field->m_Flags & FieldFlags::Share)
    {
        // Read the path of the object we should point to
        String path_string = archive.ReadString();
        if (!path_string.IsEmpty())
        {
            GameObjectPath gop;
            gop.Set(path_string);

            GameObject *outer = Reflect::AssertCast<GameObject>(m_Instance);

            // Defer the link to the GameObjectLoader. Either this object exists and we'll link
            // instantly, or the appropriate book-keeping will occur
            GameObjectLoader::HandleLinkDependency(outer, *m_Data, gop);
        }
        else
        {
            // This is the null data case
            m_Data->Set(NULL);
        }
    }
    else
    {
        // If it's not a shared object, read this as an inline object
        Super::Serialize<ArchiveT>(archive);
    }
}
