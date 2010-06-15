#include "Precompile.h"
#include "EntityAssetSet.h"

#include "Scene.h"
#include "SceneEditor.h"
#include "Entity.h"
#include "EntityType.h"

#include "Editor/SharedFileManager.h"
#include "PrimitiveCube.h"
#include "PrimitiveSphere.h"
#include "PrimitiveCylinder.h"
#include "PrimitiveCapsule.h"

#include "Foundation/Log.h"

#include "Asset/EntityAsset.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/ArtFileAttribute.h"

using namespace Luna;

LUNA_DEFINE_TYPE(Luna::EntityAssetSet);

void EntityAssetSet::InitializeType()
{
    Reflect::RegisterClass< Luna::EntityAssetSet >( "Luna::EntityAssetSet" );
}

void EntityAssetSet::CleanupType()
{
    Reflect::UnregisterClass< Luna::EntityAssetSet >();
}

EntityAssetSet::EntityAssetSet( Luna::EntityType* type, const Nocturnal::Path& assetPath )
: Luna::InstanceSet (type)
, m_AssetPath( assetPath )
, m_ClassMissing (false)
, m_Shape (NULL)
{
    LoadAssetClass();

    SharedFileManager::GetInstance()->AddFileListener( m_AssetPath, SharedFileChangedSignature::Delegate( this, &EntityAssetSet::FileChanged ) );
}

EntityAssetSet::~EntityAssetSet()
{
    delete m_Shape;

    SharedFileManager::GetInstance()->RemoveFileListener( m_AssetPath, SharedFileChangedSignature::Delegate( this, &EntityAssetSet::FileChanged ) );
    delete m_AssetPath;
}

void EntityAssetSet::LoadAssetClass()
{

    m_Class = Asset::AssetClass::LoadAssetClass<Asset::EntityAsset>( m_AssetPath );

    if ( !m_Class.ReferencesObject() )
    {
        m_Name = m_AssetPath.Basename();
    }
    else
    {
        m_Name = m_Class->GetFullName();

        Attribute::AttributeViewer< Asset::ArtFileAttribute > model (m_Class);

        if (model.Valid())
        {
            m_ArtFile = model->GetPath().Get();

            if (!m_ArtFile.empty())
            {
                Luna::PrimitiveCube* cube;
                if ( !m_Shape )
                {
                    cube = new Luna::PrimitiveCube (m_Type->GetScene()->GetView()->GetResources());
                    cube->Update();

                    m_Shape = cube;
                }
                else
                {
                    cube = dynamic_cast< Luna::PrimitiveCube* >( m_Shape );
                }

                if ( model->m_Minima != Math::Vector3::Zero || model->m_Maxima != Math::Vector3::Zero )
                {
                    cube->SetBounds( model->m_Minima, model->m_Maxima );
                    cube->Update();
                }
                else if (model->m_Extents != Math::Vector3::Zero)
                {
                    Math::Vector3 minima = -(model->m_Extents / 2.f) + model->m_Offset;
                    minima.y += model->m_Extents.y / 2.f;
                    Math::Vector3 maxima = (model->m_Extents / 2.f) + model->m_Offset;
                    maxima.y += model->m_Extents.y / 2.f;
                    cube->SetBounds( minima, maxima );
                    cube->Update();
                }
                else if ( Nocturnal::Path( m_ArtFile ).Exists() )
                {
                    try
                    {
                        m_Manifest = Reflect::Archive::FromFile<Asset::EntityManifest>( m_ArtFile );
                    }
                    catch ( const Reflect::Exception& e )
                    {
                        Log::Error("Error loading %s (%s)\n", m_ArtFile.c_str(), e.what());
                    }

                    if (m_Manifest.ReferencesObject())
                    {
                        cube->SetBounds( m_Manifest->m_BoundingBoxMin, m_Manifest->m_BoundingBoxMax );
                        cube->Update();
                    }
                }
            }
        }


        if (m_Shape && m_Type->GetConfiguration().ReferencesObject())
        {
            m_Shape->SetSolid(m_Type->GetConfiguration()->m_Solid);
            m_Shape->SetTransparent(m_Type->GetConfiguration()->m_Transparent);

            if (m_Type->GetConfiguration()->m_Solid)
            {
                m_Shape->SetUsingCameraShadingMode(true);
            }

            m_Shape->Update();
        }
    }

    m_ClassLoaded.Raise( EntityAssetSetChangeArgs( this ) );
}

void EntityAssetSet::FileChanged( const SharedFileChangeArgs& args )
{
    LoadAssetClass();
}

void EntityAssetSet::Create()
{
    if (m_Shape)
    {
        m_Shape->Create();
    }
}

void EntityAssetSet::Delete()
{
    if (m_Shape)
    {
        m_Shape->Delete();
    }
}

void EntityAssetSet::AddInstance(Luna::Instance* i)
{
    // set class link (must be done before calling base class)
    Reflect::AssertCast<Luna::Entity>(i)->SetClassSet(this);

    __super::AddInstance(i);
}

void EntityAssetSet::RemoveInstance(Luna::Instance* i)
{
    __super::RemoveInstance(i);

    // remove class link (must be done after calling base class)
    Reflect::AssertCast<Luna::Entity>(i)->SetClassSet(NULL);
}
