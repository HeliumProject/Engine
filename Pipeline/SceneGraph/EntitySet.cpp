/*#include "Precompile.h"*/
#include "EntitySet.h"

#include "Foundation/Log.h"
#include "Foundation/Reflect/Archive.h"

#include "Foundation/Component/ComponentHandle.h"

#include "Pipeline/Asset/Classes/Entity.h"
#include "Pipeline/Asset/Components/BoundingBoxComponent.h"

#include "Pipeline/SceneGraph/Scene.h"
#include "Pipeline/SceneGraph/EntityInstance.h"
#include "Pipeline/SceneGraph/EntityInstanceType.h"
#include "Pipeline/SceneGraph/PrimitiveCube.h"
#include "Pipeline/SceneGraph/PrimitiveSphere.h"
#include "Pipeline/SceneGraph/PrimitiveCylinder.h"
#include "Pipeline/SceneGraph/PrimitiveCapsule.h"
#include "Pipeline/SceneGraph/Mesh.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT(SceneGraph::EntitySet);

void EntitySet::InitializeType()
{
    Reflect::RegisterClassType< SceneGraph::EntitySet >( TXT( "SceneGraph::EntitySet" ) );
}

void EntitySet::CleanupType()
{
    Reflect::UnregisterClassType< SceneGraph::EntitySet >();
}

EntitySet::EntitySet( SceneGraph::EntityInstanceType* type, const Helium::Path& assetPath )
: SceneGraph::InstanceSet (type)
, m_AssetPath( assetPath )
, m_ClassMissing (false)
, m_Shape( NULL )
, m_Class( NULL )
{
    LoadAssetClass();
}

EntitySet::~EntitySet()
{
    delete m_Shape;
}

void EntitySet::LoadAssetClass()
{
    m_Class = m_AssetPath.empty() ? NULL : Asset::AssetClass::LoadAssetClass<Asset::Entity>( m_AssetPath );

    if ( !m_Class.ReferencesObject() )
    {
        m_Name = m_AssetPath.Basename();
    }
    else
    {
        m_Name = m_Class->GetFullName();

        if ( !m_Class->GetContentPath().empty() )
        {
            SceneGraph::PrimitiveCube* cube;
            if ( !m_Shape )
            {
                cube = new SceneGraph::PrimitiveCube (m_Type->GetScene()->GetViewport()->GetResources());
                cube->Update();

                m_Shape = cube;
            }
            else
            {
                cube = dynamic_cast< SceneGraph::PrimitiveCube* >( m_Shape );
            }

            Asset::BoundingBoxComponentPtr boundingBox = m_Class->GetComponent< Asset::BoundingBoxComponent >();
            if ( boundingBox.ReferencesObject() )
            {
                if ( boundingBox->GetMinima() != Vector3::Zero || boundingBox->GetMaxima() != Vector3::Zero )
                {
                    cube->SetBounds( boundingBox->GetMinima(), boundingBox->GetMaxima() );
                    cube->Update();
                }
                else if ( boundingBox->GetExtents() != Vector3::Zero )
                {
                    Vector3 minima = -(boundingBox->GetExtents() / 2.f) + boundingBox->GetOffset();
                    minima.y += boundingBox->GetExtents().y / 2.f;
                    Vector3 maxima = (boundingBox->GetExtents() / 2.f) + boundingBox->GetOffset();
                    maxima.y += boundingBox->GetExtents().y / 2.f;
                    cube->SetBounds( minima, maxima );
                    cube->Update();
                }
            }
            else
            {
                Path meshPath = m_Class->GetContentPath().GetAbsolutePath( m_Class->GetSourcePath() );
                meshPath.ReplaceExtension( TXT( "HeliumMesh" ) );

                if ( meshPath.Exists() )
                {
                    SceneGraph::MeshPtr mesh;
                    try
                    {
                        mesh = Reflect::FromArchive< SceneGraph::Mesh >( meshPath );
                    }
                    catch ( const Reflect::Exception& e )
                    {
                        Log::Error( TXT( "Error loading %s (%s)\n" ), meshPath.c_str(), e.What());
                    }

                    if ( mesh )
                    {
                        AlignedBox box;
                        mesh->GetAlignedBoundingBox( box );
                        cube->SetBounds( box );
                        cube->Update();
                    }
                }
            }
        }
    }

    m_ClassLoaded.Raise( EntitySetChangeArgs( this ) );
}

void EntitySet::Create()
{
    if (m_Shape)
    {
        m_Shape->Create();
    }
}

void EntitySet::Delete()
{
    if (m_Shape)
    {
        m_Shape->Delete();
    }
}

void EntitySet::AddInstance(SceneGraph::Instance* i)
{
    // set class link (must be done before calling base class)
    Reflect::AssertCast<SceneGraph::EntityInstance>(i)->SetClassSet(this);

    __super::AddInstance(i);
}

void EntitySet::RemoveInstance(SceneGraph::Instance* i)
{
    __super::RemoveInstance(i);

    // remove class link (must be done after calling base class)
    Reflect::AssertCast<SceneGraph::EntityInstance>(i)->SetClassSet(NULL);
}
