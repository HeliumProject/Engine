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

#include "Pipeline/Asset/Classes/Entity.h"
#include "Foundation/Component/ComponentHandle.h"
#include "Pipeline/Asset/Components/BoundingBoxComponent.h"

using namespace Luna;

LUNA_DEFINE_TYPE(Luna::EntityAssetSet);

void EntityAssetSet::InitializeType()
{
    Reflect::RegisterClass< Luna::EntityAssetSet >( TXT( "Luna::EntityAssetSet" ) );
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

    m_Class = Asset::AssetClass::LoadAssetClass<Asset::Entity>( m_AssetPath );

    if ( !m_Class.ReferencesObject() )
    {
        m_Name = m_AssetPath.Basename();
    }
    else
    {
        m_Name = m_Class->GetFullName();

        m_ArtFile = m_Class->GetPath().Get();

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

            Asset::BoundingBoxComponentPtr boundingBox = m_Class->GetComponent< Asset::BoundingBoxComponent >();
            if ( boundingBox.ReferencesObject() )
            {
                if ( boundingBox->GetMinima() != Math::Vector3::Zero || boundingBox->GetMaxima() != Math::Vector3::Zero )
                {
                    cube->SetBounds( boundingBox->GetMinima(), boundingBox->GetMaxima() );
                    cube->Update();
                }
                else if ( boundingBox->GetExtents() != Math::Vector3::Zero )
                {
                    Math::Vector3 minima = -(boundingBox->GetExtents() / 2.f) + boundingBox->GetOffset();
                    minima.y += boundingBox->GetExtents().y / 2.f;
                    Math::Vector3 maxima = (boundingBox->GetExtents() / 2.f) + boundingBox->GetOffset();
                    maxima.y += boundingBox->GetExtents().y / 2.f;
                    cube->SetBounds( minima, maxima );
                    cube->Update();
                }
            }
            else if ( Nocturnal::Path( m_ArtFile ).Exists() )
            {
                try
                {
                    m_Manifest = Reflect::Archive::FromFile<Asset::EntityManifest>( m_ArtFile );
                }
                catch ( const Reflect::Exception& e )
                {
                    Log::Error( TXT( "Error loading %s (%s)\n" ), m_ArtFile.c_str(), e.What());
                }

                if (m_Manifest.ReferencesObject())
                {
                    cube->SetBounds( m_Manifest->m_BoundingBoxMin, m_Manifest->m_BoundingBoxMax );
                    cube->Update();
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
