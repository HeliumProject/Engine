#include "AssetClass.h"

#include "Foundation/Log.h"
#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/Version.h"
#include "Foundation/Reflect/Visitor.h"
#include "Foundation/Reflect/Serializers.h"
#include "Foundation/Component/Component.h"

#include "Pipeline/Asset/Classes/Entity.h"
#include "Pipeline/Asset/Classes/ShaderAsset.h"
#include "Pipeline/Asset/Classes/SceneAsset.h"

#include <memory>

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Asset;

const tchar_t* ASSET_VERSION = TXT( "1" );

tstring AssetClass::s_BaseBuiltDirectory = TXT( "" );
std::map< tstring, AssetFactory* > AssetClass::s_AssetFactories;

REFLECT_DEFINE_ABSTRACT( AssetClass );
void AssetClass::EnumerateClass( Reflect::Compositor<AssetClass>& comp )
{
    comp.AddField( &AssetClass::m_Description,  "m_Description" );
    comp.AddField( &AssetClass::m_Tags,         "m_Tags" );
    comp.AddField( &AssetClass::m_ContentPath,  "m_ContentPath" );
}

AssetClass::AssetClass()
{
}

AssetClassPtr AssetClass::LoadAssetClass( const Path& path )
{
    AssetClassPtr assetClass = Reflect::FromArchive< AssetClass >( path );

    if ( assetClass )
    {
        assetClass->SetSourcePath( path );
        assetClass->LoadFinished();
    }

    return assetClass;
}

Helium::Path AssetClass::GetBuiltDirectory()
{
#pragma TODO( "make human-readable built directories" )
    tstringstream str;
    str << TUID_HEX_FORMAT << m_SourcePath.Hash();
    Helium::Path builtDirectory( s_BaseBuiltDirectory + TXT( "/" ) + str.str() );
    return builtDirectory;
}

tstring AssetClass::GetFullName() const
{
    return m_SourcePath.Get();
}

tstring AssetClass::GetShortName() const
{
    return m_SourcePath.Basename();
}

void AssetClass::GatherSearchableProperties( Helium::SearchableProperties* properties ) const
{
    properties->Insert( TXT( "AssetDescription" ), m_Description );

    for ( std::set< tstring >::const_iterator itr = m_Tags.begin(), end = m_Tags.end(); itr != end; ++itr )
    {
        properties->Insert( TXT( "AssetTag" ), (*itr) );
    }

    __super::GatherSearchableProperties( properties );
}

namespace Helium
{
    namespace Asset
    {
        class AssetDependencyVisitor : public Reflect::Visitor
        {   
        public:
            std::set< Helium::Path >& m_Dependencies;

            AssetDependencyVisitor(std::set< Helium::Path >& dependencies)
                : m_Dependencies( dependencies )
            {
            }

            virtual ~AssetDependencyVisitor()
            {
            }

            virtual bool VisitField(Element* element, const Field* field) HELIUM_OVERRIDE
            {
                if ( field->m_SerializerID == Reflect::GetType< Reflect::PathSerializer >() )
                {
                    Helium::Path path;
                    if ( Reflect::Serializer::GetValue( field->CreateSerializer( element ), path ) )
                    {
                        m_Dependencies.insert( path );

                        return false;
                    }
                }
                //-----------------------------------------------
                else if ( field->m_SerializerID == Reflect::GetType< Reflect::ArraySerializer >() )
                {
                    const Reflect::ArraySerializer* arraySerializer = Reflect::ConstDangerousCast<Reflect::ArraySerializer>( field->CreateSerializer( element ) );
                    if ( arraySerializer->GetItemType() == Reflect::GetType< Reflect::PathSerializer >() )
                    {
                        if ( (int)arraySerializer->GetSize() < 1 )
                        {
                            return true;
                        }

                        for ( size_t index = 0; index < arraySerializer->GetSize(); ++index )
                        {
                            Helium::Path path;
                            if ( Reflect::Serializer::GetValue( arraySerializer->GetItem( index ), path ) )
                            {
                                m_Dependencies.insert( path );
                            }
                        }

                        return false;
                    }
                }
                //-----------------------------------------------
                else if ( field->m_SerializerID == Reflect::GetType< Reflect::MapSerializer >() )
                {
                    const Reflect::MapSerializer* mapSerializer = Reflect::ConstDangerousCast<Reflect::MapSerializer>( field->CreateSerializer( element ) );
                    if ( mapSerializer->GetValueType() == Reflect::GetType< Reflect::PathSerializer >() )
                    {
                        if ( (int)mapSerializer->GetSize() < 1 )
                        {
                            return true;
                        }

                        Reflect::MapSerializer::V_ConstValueType data;
                        mapSerializer->GetItems( data );

                        Reflect::MapSerializer::V_ConstValueType::const_iterator itr = data.begin();
                        Reflect::MapSerializer::V_ConstValueType::const_iterator end = data.end();
                        for ( ; itr != end; ++itr )
                        {
                            Helium::Path path;
                            if ( Reflect::Serializer::GetValue( itr->second, path ) )
                            {
                                m_Dependencies.insert( path );
                            }
                        }

                        return false;
                    }
                }
                //-----------------------------------------------
                else if ( field->m_SerializerID == Reflect::GetType< Reflect::SetSerializer >() )
                {
                    const Reflect::SetSerializer* setSerializer = Reflect::ConstDangerousCast<Reflect::SetSerializer>( field->CreateSerializer( element ) );
                    if ( setSerializer->GetItemType() == Reflect::GetType< Reflect::PathSerializer >() )
                    {
                        if ( (int)setSerializer->GetSize() < 1 )
                        {
                            return true;
                        }

                        Reflect::V_ConstSerializer data;
                        setSerializer->GetItems( data );

                        Reflect::V_ConstSerializer::const_iterator itr = data.begin();
                        Reflect::V_ConstSerializer::const_iterator end = data.end();
                        for ( ; itr != end; ++itr )
                        {
                            Helium::Path path;
                            if ( Reflect::Serializer::GetValue( (*itr), path ) )
                            {
                                m_Dependencies.insert( path );
                            }
                        }

                        return false;
                    }
                }
                //-----------------------------------------------
                else if ( field->m_SerializerID == Reflect::GetType< Reflect::ElementArraySerializer >() )
                {
                    const Reflect::ElementArraySerializer* arraySerializer = Reflect::ConstDangerousCast< Reflect::ElementArraySerializer >( field->CreateSerializer( element ) );

                    if ( (int)arraySerializer->GetSize() < 1 )
                    {
                        return true;
                    }

                    const Reflect::V_Element& vals = arraySerializer->m_Data.Ref();
                    for ( Reflect::V_Element::const_iterator itr = vals.begin(), end = vals.end(); itr != end; ++itr )
                    {
                        (*itr)->Accept( *this );
                    }

                    return false;
                }
                //-----------------------------------------------
                else if ( field->m_SerializerID == Reflect::GetType< Reflect::ElementMapSerializer >() )
                {
                    const Reflect::ElementMapSerializer* mapSerializer = Reflect::ConstDangerousCast< Reflect::ElementMapSerializer >( field->CreateSerializer( element ) );

                    if ( (int)mapSerializer->GetSize() < 1 )
                    {
                        return true;
                    }

                    Reflect::ElementMapSerializer::V_ConstValueType data;
                    mapSerializer->GetItems( data );

                    Reflect::ElementMapSerializer::V_ConstValueType::const_iterator itr = data.begin();
                    Reflect::ElementMapSerializer::V_ConstValueType::const_iterator end = data.end();
                    for ( ; itr != end; ++itr )
                    {
                        (*itr->second)->Accept( *this );
                    }

                    return false;
                }
                //-----------------------------------------------
                else if ( field->m_SerializerID == Reflect::GetType< Reflect::ElementSetSerializer >() )
                {
                    const Reflect::ElementSetSerializer* setSerializer = Reflect::ConstDangerousCast< Reflect::ElementSetSerializer >( field->CreateSerializer( element ) );

                    if ( (int)setSerializer->GetSize() < 1 )
                    {
                        return true;
                    }

                    const Reflect::ElementSetSerializer::DataType& vals = setSerializer->m_Data.Ref();
                    for ( Reflect::ElementSetSerializer::DataType::const_iterator itr = vals.begin(), end = vals.end(); itr != end; ++itr )
                    {
                        (*itr)->Accept( *this );
                    }

                    return false;
                }

                // continue search
                return true;
            }
        };
    }
}

void AssetClass::GetFileReferences( std::set< Helium::Path >& fileReferences )
{
    AssetDependencyVisitor assetDepVisitor( fileReferences );
    this->Accept( assetDepVisitor );

    //__super::GetFileReferences( fileReferences );

    fileReferences.insert( m_ContentPath );
}

void AssetClass::ComponentChanged( const Component::ComponentBase* component )
{
    __super::ComponentChanged( component );
}

bool AssetClass::SetComponent( const Component::ComponentPtr& component, bool validate, tstring* error )
{
    return __super::SetComponent( component, validate, error );
}

bool AssetClass::RemoveComponent( int32_t typeID )
{
    return __super::RemoveComponent( typeID );
}

bool AssetClass::Serialize()
{
    bool result = Reflect::ToArchive( m_SourcePath, this );

    m_Modified = !result;
    return result;
}

/////////////////////////////////////////////////////////////////////////////
// Override this function to provide logic that makes sure an asset class is
// valid.  Fill out the error message provided so that it can be reported
// to the user.  This function is intended to be used by the UI to make sure
// that an asset is valid before it is built (validation is done during save).
// 
bool AssetClass::ValidateClass( tstring& error ) const
{
    error.clear();
    return true;
}

bool AssetClass::ValidateCompatible( const Component::ComponentPtr &component, tstring& error ) const
{
    if ( component->GetComponentUsage() == Component::ComponentUsages::Instance )
    {
        error = TXT( "The " ) + component->GetClass()->m_UIName + TXT( " component can only be added to an instance of an asset." );
        return false;
    }

    return __super::ValidateCompatible( component, error );
}

void AssetClass::LoadFinished()
{

}

void AssetClass::CopyTo(const Reflect::ElementPtr& destination) 
{
    // Restore the Asset Class ID after performing the copy
    AssetClass* destinationAsset = Reflect::ObjectCast< AssetClass >( destination );
    __super::CopyTo( destination );
}
