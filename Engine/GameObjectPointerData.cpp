#include "EnginePch.h"
#include "Engine/GameObjectPointerData.h"

#include "Reflect/ArchiveBinary.h"
#include "Reflect/ArchiveXML.h"
#include "Engine/GameObject.h"

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

void GameObjectPointerData::Serialize(ArchiveBinary& archive)
{
    Serialize<ArchiveBinary>( archive );
}

void GameObjectPointerData::Deserialize(ArchiveBinary& archive)
{
    Deserialize<ArchiveBinary>( archive );
}

void GameObjectPointerData::Serialize(ArchiveXML& archive)
{
    Serialize<ArchiveXML>( archive );
}

void GameObjectPointerData::Deserialize(ArchiveXML& archive)
{
    Deserialize<ArchiveXML>( archive );
}

template< class ArchiveT >
void GameObjectPointerData::Serialize(ArchiveT& archive)
{
    //if (ShouldBeLinked())
    //{
        if (m_Data->ReferencesObject())
        {
            // Valid path
            Helium::GameObject *game_object = Reflect::AssertCast<GameObject>(m_Data->Get());
            archive.WriteString(*game_object->GetPath().ToString());
        }
        else
        {
            // Empty string means null
            archive.WriteString(TXT(""));
        }
    //}
//     else
//     {
//         // If it's not a shared object, just write this inline as if it were a normal object
//         Base::Serialize(archive);
//     }
}

template< class ArchiveT >
void GameObjectPointerData::Deserialize(ArchiveT& archive)
{
    *m_Data = NULL;

    //if (ShouldBeLinked())
    //{
        // Read the path of the object we should point to
        tstring path_string;
        archive.ReadString(path_string);
        if (!path_string.empty())
        {
            GameObjectPath gop;
            gop.Set(path_string.c_str());

            //GameObject *outer = Reflect::AssertCast<GameObject>(m_Instance);

            GameObjectLoader *loader = GameObjectLoader::GetStaticInstance();
            HELIUM_ASSERT(loader);

            size_t link_index = loader->BeginLoadObject(gop);
            m_Data->SetLinkIndex(static_cast< uint32_t >(link_index));
        }
        else
        {
            // This is the null data case
            m_Data->SetLinkIndex(Invalid<uint32_t>());
        }
    //}
//     else
//     {
//         // If it's not a shared object, read this as an inline object
//         Base::Serialize(archive);
//     }
}
