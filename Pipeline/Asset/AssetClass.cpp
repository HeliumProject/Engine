#include "PipelinePch.h"
#include "AssetClass.h"

#include "Foundation/Log.h"
#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/Version.h"
#include "Foundation/Reflect/Visitor.h"
#include "Foundation/Reflect/Data/DataDeduction.h"
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
void AssetClass::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &AssetClass::m_Description,  TXT( "m_Description" ) );
    comp.AddField( &AssetClass::m_Tags,         TXT( "m_Tags" ) );
    comp.AddField( &AssetClass::m_ContentPath,  TXT( "m_ContentPath" ) );
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

    Base::GatherSearchableProperties( properties );
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

            virtual bool VisitField(void* instance, const Field* field) HELIUM_OVERRIDE
            {
                if ( field->m_DataClass == Reflect::GetClass< Reflect::PathData >() )
                {
                    Helium::Path path;
                    if ( Reflect::Data::GetValue( field->CreateData( instance ), path ) )
                    {
                        m_Dependencies.insert( path );

                        return false;
                    }
                }
                //-----------------------------------------------
                else if ( field->m_DataClass == Reflect::GetClass< Reflect::StlVectorData >() )
                {
                    Reflect::StlVectorData* arrayData = Reflect::AssertCast<Reflect::StlVectorData>( field->CreateData( instance ) );
                    if ( arrayData->GetItemClass() == Reflect::GetClass< Reflect::PathData >() )
                    {
                        if ( (int)arrayData->GetSize() < 1 )
                        {
                            return true;
                        }

                        for ( size_t index = 0; index < arrayData->GetSize(); ++index )
                        {
                            Helium::Path path;
                            if ( Reflect::Data::GetValue( arrayData->GetItem( index ), path ) )
                            {
                                m_Dependencies.insert( path );
                            }
                        }

                        return false;
                    }
                }
                //-----------------------------------------------
                else if ( field->m_DataClass == Reflect::GetClass< Reflect::StlMapData >() )
                {
                    Reflect::StlMapData* mapData = Reflect::AssertCast<Reflect::StlMapData>( field->CreateData( instance ) );
                    if ( mapData->GetValueClass() == Reflect::GetClass< Reflect::PathData >() )
                    {
                        if ( (int)mapData->GetSize() < 1 )
                        {
                            return true;
                        }

                        Reflect::StlMapData::V_ValueType data;
                        mapData->GetItems( data );

                        Reflect::StlMapData::V_ValueType::const_iterator itr = data.begin();
                        Reflect::StlMapData::V_ValueType::const_iterator end = data.end();
                        for ( ; itr != end; ++itr )
                        {
                            Helium::Path path;
                            if ( Reflect::Data::GetValue( itr->second, path ) )
                            {
                                m_Dependencies.insert( path );
                            }
                        }

                        return false;
                    }
                }
                //-----------------------------------------------
                else if ( field->m_DataClass == Reflect::GetClass< Reflect::StlSetData >() )
                {
                    Reflect::StlSetData* setData = Reflect::AssertCast<Reflect::StlSetData>( field->CreateData( instance ) );
                    if ( setData->GetItemClass() == Reflect::GetClass< Reflect::PathData >() )
                    {
                        if ( (int)setData->GetSize() < 1 )
                        {
                            return true;
                        }

                        std::vector< Reflect::DataPtr > data;
                        setData->GetItems( data );

                        std::vector< Reflect::DataPtr >::const_iterator itr = data.begin();
                        std::vector< Reflect::DataPtr >::const_iterator end = data.end();
                        for ( ; itr != end; ++itr )
                        {
                            Helium::Path path;
                            if ( Reflect::Data::GetValue( (*itr), path ) )
                            {
                                m_Dependencies.insert( path );
                            }
                        }

                        return false;
                    }
                }
                //-----------------------------------------------
                else if ( field->m_DataClass == Reflect::GetClass< Reflect::ObjectStlVectorData >() )
                {
                    Reflect::ObjectStlVectorData* arrayData = Reflect::AssertCast< Reflect::ObjectStlVectorData >( field->CreateData( instance ) );

                    if ( (int)arrayData->GetSize() < 1 )
                    {
                        return true;
                    }

                    std::vector< Reflect::ObjectPtr >& vals = *arrayData->m_Data;
                    for ( std::vector< Reflect::ObjectPtr >::const_iterator itr = vals.begin(), end = vals.end(); itr != end; ++itr )
                    {
                        (*itr)->Accept( *this );
                    }

                    return false;
                }
                //-----------------------------------------------
                else if ( field->m_DataClass == Reflect::GetClass< Reflect::ObjectStlMapData >() )
                {
                    Reflect::ObjectStlMapData* mapData = Reflect::AssertCast< Reflect::ObjectStlMapData >( field->CreateData( instance ) );

                    if ( (int)mapData->GetSize() < 1 )
                    {
                        return true;
                    }

                    Reflect::ObjectStlMapData::V_ValueType data;
                    mapData->GetItems( data );

                    Reflect::ObjectStlMapData::V_ValueType::const_iterator itr = data.begin();
                    Reflect::ObjectStlMapData::V_ValueType::const_iterator end = data.end();
                    for ( ; itr != end; ++itr )
                    {
                        (*itr->second)->Accept( *this );
                    }

                    return false;
                }
                //-----------------------------------------------
                else if ( field->m_DataClass == Reflect::GetClass< Reflect::ObjectStlSetData >() )
                {
                    Reflect::ObjectStlSetData* setData = Reflect::AssertCast< Reflect::ObjectStlSetData >( field->CreateData( instance ) );

                    if ( (int)setData->GetSize() < 1 )
                    {
                        return true;
                    }

                    Reflect::ObjectStlSetData::DataType& vals = *setData->m_Data;
                    for ( Reflect::ObjectStlSetData::DataType::const_iterator itr = vals.begin(), end = vals.end(); itr != end; ++itr )
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

    //Base::GetFileReferences( fileReferences );

    fileReferences.insert( m_ContentPath );
}

void AssetClass::ComponentChanged( const Component::ComponentBase* component )
{
    Base::ComponentChanged( component );
}

bool AssetClass::SetComponent( const Component::ComponentPtr& component, bool validate, tstring* error )
{
    return Base::SetComponent( component, validate, error );
}

bool AssetClass::RemoveComponent( const Reflect::Class* type )
{
    return Base::RemoveComponent( type );
}

void AssetClass::ConnectDocument( Document* document )
{
    document->d_Save.Set( this, &AssetClass::OnDocumentSave );

    e_HasChanged.AddMethod( document, &Document::OnObjectChanged );
}

void AssetClass::DisconnectDocument( const Document* document )
{
    document->d_Save.Clear();

    e_HasChanged.RemoveMethod( document, &Document::OnObjectChanged );
}

void AssetClass::OnDocumentSave( const DocumentEventArgs& args )
{
    const Document* document = static_cast< const Document* >( args.m_Document );
    HELIUM_ASSERT( document );
    HELIUM_ASSERT( !m_SourcePath.empty() && document->GetPath() == m_SourcePath )

    args.m_Result = Serialize();
}

bool AssetClass::Serialize()
{
    HELIUM_ASSERT( !m_SourcePath.empty() );
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
        error = TXT( "The " );
        error += *component->GetClass()->m_Name;
        error += TXT( " component can only be added to an instance of an asset." );
        return false;
    }

    return Base::ValidateCompatible( component, error );
}

void AssetClass::LoadFinished()
{

}
