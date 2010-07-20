#include "Precompile.h"
#include "Entity.h"
#include "EntityType.h"
#include "EntityPanel.h"
#include "EntityAssetSet.h"

#include "Scene.h"
#include "SceneManager.h"
#include "SceneEditor.h"

#include "PropertiesGenerator.h"
#include "PrimitiveCube.h"
#include "PrimitivePointer.h"

#include "Foundation/Component/ComponentHandle.h"
#include "Pipeline/Asset/Manifests/SceneManifest.h"
#include "Pipeline/Asset/Classes/Entity.h"

#include "Foundation/Log.h"

// Using
using namespace Math;
using namespace Luna;
using namespace Component;

// RTTI
LUNA_DEFINE_TYPE(Luna::Entity);

void Entity::InitializeType()
{
    Reflect::RegisterClass< Luna::Entity >( TXT( "Luna::Entity" ) );
    PropertiesGenerator::InitializePanel( TXT( "Entity" ), CreatePanelSignature::Delegate( &Entity::CreatePanel ) );
}

void Entity::CleanupType()
{
    Reflect::UnregisterClass< Luna::Entity >();
}

Entity::Entity(Luna::Scene* scene)
: Luna::Instance (scene, new Asset::EntityInstance())
{
    ConstructorInit();
}

Entity::Entity(Luna::Scene* scene, Asset::EntityInstance* entity)
: Luna::Instance ( scene, entity )
{
    ConstructorInit();
}

Entity::~Entity()
{
    if ( m_NestedSceneArt )
    {
        m_Scene->GetManager()->ReleaseNestedScene( m_NestedSceneArt );
    }

    if ( m_NestedSceneCollision )
    {
        m_Scene->GetManager()->ReleaseNestedScene( m_NestedSceneCollision );
    }

    if ( m_NestedScenePathfinding )
    {
        m_Scene->GetManager()->ReleaseNestedScene( m_NestedScenePathfinding );
    }

    Asset::Entity* pkg = GetPackage< Asset::Entity >();

    pkg->RemoveComponentAddedListener( Component::ComponentCollectionChangedSignature::Delegate( this, &Entity::OnComponentAdded ) );
    pkg->RemoveComponentRemovedListener( Component::ComponentCollectionChangedSignature::Delegate( this, &Entity::OnComponentRemoved ) );
}

void Entity::ConstructorInit()
{
    m_ClassSet = NULL;
    m_NestedSceneArt = NULL;
    m_NestedSceneCollision = NULL;
    m_NestedScenePathfinding = NULL;

    Asset::Entity* pkg = GetPackage< Asset::Entity >();

    pkg->AddComponentAddedListener( Component::ComponentCollectionChangedSignature::Delegate( this, &Entity::OnComponentAdded ) );
    pkg->AddComponentRemovedListener( Component::ComponentCollectionChangedSignature::Delegate( this, &Entity::OnComponentRemoved ) );
}

tstring Entity::GenerateName() const
{
    const Asset::EntityInstance* entity = GetPackage<Asset::EntityInstance>();
    Asset::EntityPtr entityClass = entity->GetEntity();

    tstring name = entityClass ? entityClass->GetShortName() : TXT( "" );

    if (name.empty())
    {
        name = __super::GenerateName();
    }
    else
    {
        if ( isdigit( *name.rbegin() ) )
        {
            name += TXT( "_" );
        }

        name += TXT( "1" );
    }

    return name;
}

tstring Entity::GetApplicationTypeName() const
{
    return TXT( "Entity" );
}

SceneNodeTypePtr Entity::CreateNodeType( Luna::Scene* scene ) const
{
    // Overridden to create an entity-specific type
    Luna::EntityType* nodeType = new Luna::EntityType( scene, GetType() );

    // Set the image index (usually this is handled by the base class, but we aren't calling the base)
    nodeType->SetImageIndex( GetImageIndex() );

    return nodeType;
}

Luna::Scene* Entity::GetNestedScene( GeometryMode mode, bool load_on_demand ) const
{
    Luna::Scene* nestedScene = NULL;

    if (m_ClassSet->GetEntity())
    {
#pragma TODO( "Support the various rendering modes.  This used to load different files for art, collision, etc." )
        tstring nestedFile = m_ClassSet->GetContentFile();
        m_NestedSceneArt = m_Scene->GetManager()->AllocateNestedScene( nestedFile, m_Scene );
    }

    return m_NestedSceneArt;
}

bool Entity::IsPointerVisible() const
{
    NOC_ASSERT(m_VisibilityData); 
    return m_VisibilityData->GetShowPointer(); 
}

void Entity::SetPointerVisible(bool visible)
{
    NOC_ASSERT(m_VisibilityData); 
    m_VisibilityData->SetShowPointer(visible); 

    // we need to dirty to cause our bounds needs to be re-computed
    Dirty();
}

bool Entity::IsBoundsVisible() const
{
    NOC_ASSERT(m_VisibilityData); 
    return m_VisibilityData->GetShowBounds(); 
}

void Entity::SetBoundsVisible(bool visible)
{
    NOC_ASSERT(m_VisibilityData); 
    m_VisibilityData->SetShowBounds(visible); 

    // we need to dirty to cause our bounds needs to be re-computed
    Dirty();
}

bool Entity::IsGeometryVisible() const
{
    NOC_ASSERT(m_VisibilityData); 
    return m_VisibilityData->GetShowGeometry(); 
}

void Entity::SetGeometryVisible(bool visible)
{
    NOC_ASSERT(m_VisibilityData); 
    m_VisibilityData->SetShowGeometry(visible); 

    // we need to dirty to cause our bounds needs to be re-computed
    Dirty();
}

Luna::EntityAssetSet* Entity::GetClassSet()
{
    return m_ClassSet;
}

const Luna::EntityAssetSet* Entity::GetClassSet() const
{
    return m_ClassSet;
}

void Entity::SetClassSet(Luna::EntityAssetSet* classSet)
{
    m_ClassSet = classSet;
}

void Entity::PopulateManifest( Asset::SceneManifest* manifest ) const
{
}

void Entity::Evaluate(GraphDirection direction)
{
    __super::Evaluate(direction);

    switch (direction)
    {
    case GraphDirections::Downstream:
        {
            // start the box from scratch
            m_ObjectBounds.Reset();

            // merge type pointer into our bounding box
            if ( IsPointerVisible() )
            {
                Luna::EntityType* type = Reflect::AssertCast<Luna::EntityType>(m_NodeType);

                if ( type && type->GetPointer() )
                {
                    Math::AlignedBox box (type->GetPointer()->GetBounds());

                    Math::Scale scale;
                    Math::Matrix3 rotate;
                    Math::Vector3 translate;
                    m_InverseGlobalTransform.Decompose (scale, rotate, translate);

                    // this will compensate for the normalized render of the pointer
                    box.Transform (Math::Matrix4 (scale));

                    m_ObjectBounds.Merge( box );
                }
            }

            // merge class shape into our bounding box
            if ( IsBoundsVisible() )
            {
                if ( m_ClassSet && m_ClassSet->GetShape() )
                {
                    m_ObjectBounds.Merge( m_ClassSet->GetShape()->GetBounds() );
                }
            }

            // merge geometry bounds into our bounding box
            if ( IsGeometryVisible() )
            {
                // merge nested scene into our bounding box
                const Luna::Scene* nested = GetNestedScene( m_Scene->GetViewport()->GetGeometryMode() );

                if (nested)
                {
                    m_ObjectBounds.Merge( nested->GetRoot()->GetObjectHierarchyBounds() );
                }
            }

            break;
        }
    }
}

void Entity::Render( RenderVisitor* render )
{
    const Asset::EntityInstance* package = GetPackage< Asset::EntityInstance >();

    if (IsPointerVisible())
    {
        // entity pointer is drawn normalized
        RenderEntry* entry = render->Allocate(this);
        entry->m_Location = render->State().m_Matrix.Normalized();
        entry->m_Center = m_ObjectBounds.Center();
        entry->m_Draw = &Entity::DrawPointer;
    }

    if (IsBoundsVisible() && m_ClassSet && m_ClassSet->GetShape())
    {
        // bounds are drawn non-normalized
        RenderEntry* entry = render->Allocate(this);
        entry->m_Location = render->State().m_Matrix;
        entry->m_Center = m_ObjectBounds.Center();
        entry->m_Draw = &Entity::DrawBounds;

        if ( package->m_TransparentOverride ? package->m_Transparent : Reflect::AssertCast<Luna::InstanceType>( m_NodeType )->IsTransparent() )
        {
            entry->m_Flags |= RenderFlags::DistanceSort;
        }
    }

    if (IsGeometryVisible())
    {
        const Luna::Scene* nested = GetNestedScene( render->GetViewport()->GetGeometryMode() );

        VisitorState state ( render->State().m_Matrix,
            render->State().m_Highlighted || (m_Scene->IsCurrent() && IsHighlighted()),
            render->State().m_Selected || (m_Scene->IsCurrent() && IsSelected()),
            render->State().m_Live || (m_Scene->IsCurrent() && IsLive()),
            render->State().m_Selectable || (m_Scene->IsCurrent() && IsSelectable()) );

        if (nested)
        {
            render->PushState( state );
            nested->Render( render );
            render->PopState();
        }
    }

    // don't call __super here, it will draw big ass axes
    Luna::HierarchyNode::Render( render );
}

void Entity::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const Luna::Entity* entity = Reflect::ConstAssertCast<Luna::Entity>( object );

    const Luna::EntityType* type = Reflect::ConstAssertCast<Luna::EntityType>( entity->GetNodeType() );

    entity->SetMaterial( type->GetMaterial() );

    // draw type pointer
    type->GetPointer()->Draw( args );
}

void Entity::DrawBounds( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const Luna::Entity* entity = Reflect::ConstAssertCast<Luna::Entity>( object );

    const Luna::EntityType* type = Reflect::ConstAssertCast<Luna::EntityType>( entity->GetNodeType() );

    const Luna::EntityAssetSet* classSet = entity->GetClassSet();

    const Asset::EntityInstance* package = entity->GetPackage< Asset::EntityInstance >();

    entity->SetMaterial( type->GetMaterial() );

    // draw class shape
    classSet->GetShape()->Draw( args, package->m_SolidOverride ? &package->m_Solid : NULL, package->m_TransparentOverride ? &package->m_Transparent : NULL );
}

bool Entity::Pick( PickVisitor* pick )
{
    bool result = false;

    Luna::EntityType* type = Reflect::AssertCast<Luna::EntityType>(m_NodeType);

    const Asset::EntityInstance* package = GetPackage< Asset::EntityInstance >();

    pick->SetFlag( PickFlags::IgnoreVertex, true );

    if (IsPointerVisible() || m_Selected)
    {
        pick->SetCurrentObject (this, pick->State().m_Matrix.Normalized());
        result |= type->GetPointer()->Pick (pick);
    }

    pick->SetFlag( PickFlags::IgnoreVertex, false );

    if ((IsBoundsVisible() || m_Selected) && m_ClassSet && m_ClassSet->GetShape())
    {
        pick->SetCurrentObject (this, pick->State().m_Matrix);
        result |= m_ClassSet->GetShape()->Pick (pick, package->m_SolidOverride ? &package->m_Solid : NULL);
    }

    if (IsGeometryVisible())
    {
        // setup pick
        pick->SetCurrentObject (this, pick->State().m_Matrix);

        // save our current scene's hits
        size_t high = pick->GetHitCount();

        // recurse into our nested coordinate frame
        pick->PushState( VisitorState (pick->State().m_Matrix, IsHighlighted(), IsSelected(), IsLive(), IsSelectable()) );

        // retrieve nested scene
        const Luna::Scene* scene = GetNestedScene(GetScene()->GetViewport()->GetGeometryMode());

        // hit test the entire nested scene
        if (scene && scene->Pick(pick))
        {
            // verify that our hits are in there
            NOC_ASSERT( pick->GetHits().size() > high );

            // process nested hits into hits in this scene
            V_PickHitSmartPtr::const_iterator itr = pick->GetHits().begin() + high;
            V_PickHitSmartPtr::const_iterator end = pick->GetHits().end();
            for ( ; itr != end; ++itr )
            {
                // take ownership
                (*itr)->SetObject(this);
            }

            // success!
            result = true;
        }

        pick->PopState();
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Luna::Entity.
//
bool Entity::ValidatePanel(const tstring& name)
{
    if ( name == TXT( "Entity" ) )
        return true;

    if ( name == TXT( "Instance" ) )
        return false;

    return __super::ValidatePanel( name );
}

///////////////////////////////////////////////////////////////////////////////
// Static function for creating the UI panel that allows users to edit Luna::Entity.
//
void Entity::CreatePanel( CreatePanelArgs& args )
{
    EntityPanel* panel = new EntityPanel ( args.m_Generator, args.m_Selection );

    args.m_Generator->Push( panel );
    {
        panel->SetCanvas( args.m_Generator->GetContainer()->GetCanvas() );
        panel->Create();
    }
    args.m_Generator->Pop();
}

tstring Entity::GetEntityAssetPath() const
{
    return GetPackage< Asset::EntityInstance >()->GetEntity()->GetPath().Get();
}

void Entity::SetEntityAssetPath( const tstring& entityClass )
{
    Asset::EntityInstance* entity = GetPackage< Asset::EntityInstance >();

    Nocturnal::Path oldPath = entity->GetEntity()->GetPath();
    Nocturnal::Path newPath( entityClass );

    m_ClassChanging.Raise( EntityAssetChangeArgs( this, oldPath, newPath ) );

    entity->GetEntity()->SetPath( newPath );

    // since our entity class is criteria used for deducing object type,
    //  ensure we are a member of the correct type
    CheckNodeType();

    // release leased nested scene pointers
    if ( m_NestedSceneArt )
    {
        m_Scene->GetManager()->ReleaseNestedScene( m_NestedSceneArt );
    }

    if ( m_NestedSceneCollision )
    {
        m_Scene->GetManager()->ReleaseNestedScene( m_NestedSceneCollision );
    }

    if ( m_NestedScenePathfinding )
    {
        m_Scene->GetManager()->ReleaseNestedScene( m_NestedScenePathfinding );
    }

    m_ClassChanged.Raise( EntityAssetChangeArgs( this, oldPath, newPath ) );

    Dirty();
}

void Entity::OnComponentAdded( const Component::ComponentCollectionChanged& args )
{
}

void Entity::OnComponentRemoved( const Component::ComponentCollectionChanged& args )
{
}
