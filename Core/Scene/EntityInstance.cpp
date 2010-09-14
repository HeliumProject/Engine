/*#include "Precompile.h"*/
#include "EntityInstance.h"
#include "EntityInstanceType.h"
#include "EntityInstancePanel.h"
#include "EntitySet.h"

#include "Core/Scene/Scene.h"
#include "Core/Scene/SceneManager.h"

#include "Core/Scene/PropertiesGenerator.h"
#include "PrimitiveCube.h"
#include "PrimitivePointer.h"

#include "Foundation/Component/ComponentHandle.h"
#include "Core/Asset/Manifests/SceneManifest.h"
#include "Core/Asset/Classes/Entity.h"

#include "Foundation/Log.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Core;
using namespace Helium::Component;

// RTTI
SCENE_DEFINE_TYPE(Core::Entity);

void Entity::InitializeType()
{
    Reflect::RegisterClassType< Core::Entity >( TXT( "Core::Entity" ) );
    PropertiesGenerator::InitializePanel( TXT( "Entity" ), CreatePanelSignature::Delegate( &Entity::CreatePanel ) );
}

void Entity::CleanupType()
{
    Reflect::UnregisterClassType< Core::Entity >();
}

Entity::Entity(Core::Scene* scene)
: Core::Instance (scene, new Content::EntityInstance())
{
    ConstructorInit();
}

Entity::Entity(Core::Scene* scene, Content::EntityInstance* entity)
: Core::Instance ( scene, entity )
{
    ConstructorInit();
}

Entity::~Entity()
{
    Content::EntityInstance* pkg = GetPackage< Content::EntityInstance >();

    pkg->RemoveComponentAddedListener( Component::ComponentCollectionChangedSignature::Delegate( this, &Entity::OnComponentAdded ) );
    pkg->RemoveComponentRemovedListener( Component::ComponentCollectionChangedSignature::Delegate( this, &Entity::OnComponentRemoved ) );
}

void Entity::ConstructorInit()
{
    m_ClassSet = NULL;
    m_Scene = NULL;

    Content::EntityInstance* pkg = GetPackage< Content::EntityInstance >();

    pkg->AddComponentAddedListener( Component::ComponentCollectionChangedSignature::Delegate( this, &Entity::OnComponentAdded ) );
    pkg->AddComponentRemovedListener( Component::ComponentCollectionChangedSignature::Delegate( this, &Entity::OnComponentRemoved ) );
}

tstring Entity::GenerateName() const
{
    const Content::EntityInstance* entity = GetPackage<Content::EntityInstance>();
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

SceneNodeTypePtr Entity::CreateNodeType( Core::Scene* scene ) const
{
    // Overridden to create an entity-specific type
    Core::EntityType* nodeType = new Core::EntityType( scene, GetType() );

    // Set the image index (usually this is handled by the base class, but we aren't calling the base)
    nodeType->SetImageIndex( GetImageIndex() );

    return nodeType;
}

Core::Scene* Entity::GetNestedScene( GeometryMode mode, bool load_on_demand ) const
{
    if (m_ClassSet->GetEntity())
    {
        ResolveSceneArgs args ( m_ClassSet->GetContentFile() );
        m_Owner->ResolveSceneDelegate().Invoke( args );
        m_Scene = args.m_Scene;
    }

    return m_Scene;
}

bool Entity::IsPointerVisible() const
{
    HELIUM_ASSERT(m_VisibilityData); 
    return m_VisibilityData->GetShowPointer(); 
}

void Entity::SetPointerVisible(bool visible)
{
    HELIUM_ASSERT(m_VisibilityData); 
    m_VisibilityData->SetShowPointer(visible); 

    // we need to dirty to cause our bounds needs to be re-computed
    Dirty();
}

bool Entity::IsBoundsVisible() const
{
    HELIUM_ASSERT(m_VisibilityData); 
    return m_VisibilityData->GetShowBounds(); 
}

void Entity::SetBoundsVisible(bool visible)
{
    HELIUM_ASSERT(m_VisibilityData); 
    m_VisibilityData->SetShowBounds(visible); 

    // we need to dirty to cause our bounds needs to be re-computed
    Dirty();
}

bool Entity::IsGeometryVisible() const
{
    HELIUM_ASSERT(m_VisibilityData); 
    return m_VisibilityData->GetShowGeometry(); 
}

void Entity::SetGeometryVisible(bool visible)
{
    HELIUM_ASSERT(m_VisibilityData); 
    m_VisibilityData->SetShowGeometry(visible); 

    // we need to dirty to cause our bounds needs to be re-computed
    Dirty();
}

Core::EntitySet* Entity::GetClassSet()
{
    return m_ClassSet;
}

const Core::EntitySet* Entity::GetClassSet() const
{
    return m_ClassSet;
}

void Entity::SetClassSet(Core::EntitySet* classSet)
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
                Core::EntityType* type = Reflect::AssertCast<Core::EntityType>(m_NodeType);

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
                const Core::Scene* nested = GetNestedScene( m_Scene->GetViewport()->GetGeometryMode() );

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
    const Content::EntityInstance* package = GetPackage< Content::EntityInstance >();

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

        if ( package->m_TransparentOverride ? package->m_Transparent : Reflect::AssertCast<Core::InstanceType>( m_NodeType )->IsTransparent() )
        {
            entry->m_Flags |= RenderFlags::DistanceSort;
        }
    }

    if (IsGeometryVisible())
    {
        Core::Scene* nested = GetNestedScene( render->GetViewport()->GetGeometryMode() );

        VisitorState state ( render->State().m_Matrix,
            render->State().m_Highlighted || (m_Scene->IsFocused() && IsHighlighted()),
            render->State().m_Selected || (m_Scene->IsFocused() && IsSelected()),
            render->State().m_Live || (m_Scene->IsFocused() && IsLive()),
            render->State().m_Selectable || (m_Scene->IsFocused() && IsSelectable()) );

        if (nested)
        {
            render->PushState( state );
            nested->Render( render );
            render->PopState();
        }
    }

    // don't call __super here, it will draw big ass axes
    Core::HierarchyNode::Render( render );
}

void Entity::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const Core::Entity* entity = Reflect::ConstAssertCast<Core::Entity>( object );

    const Core::EntityType* type = Reflect::ConstAssertCast<Core::EntityType>( entity->GetNodeType() );

    entity->SetMaterial( type->GetMaterial() );

    // draw type pointer
    type->GetPointer()->Draw( args );
}

void Entity::DrawBounds( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const Core::Entity* entity = Reflect::ConstAssertCast<Core::Entity>( object );

    const Core::EntityType* type = Reflect::ConstAssertCast<Core::EntityType>( entity->GetNodeType() );

    const Core::EntitySet* classSet = entity->GetClassSet();

    const Content::EntityInstance* package = entity->GetPackage< Content::EntityInstance >();

    entity->SetMaterial( type->GetMaterial() );

    // draw class shape
    classSet->GetShape()->Draw( args, package->m_SolidOverride ? &package->m_Solid : NULL, package->m_TransparentOverride ? &package->m_Transparent : NULL );
}

bool Entity::Pick( PickVisitor* pick )
{
    bool result = false;

    Core::EntityType* type = Reflect::AssertCast<Core::EntityType>(m_NodeType);

    const Content::EntityInstance* package = GetPackage< Content::EntityInstance >();

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
        const Core::Scene* scene = GetNestedScene(GetOwner()->GetViewport()->GetGeometryMode());

        // hit test the entire nested scene
        if (scene && scene->Pick(pick))
        {
            // verify that our hits are in there
            HELIUM_ASSERT( pick->GetHits().size() > high );

            // process nested hits into hits in this scene
            V_PickHitSmartPtr::const_iterator itr = pick->GetHits().begin() + high;
            V_PickHitSmartPtr::const_iterator end = pick->GetHits().end();
            for ( ; itr != end; ++itr )
            {
                // take ownership
                (*itr)->SetHitObject(this);
            }

            // success!
            result = true;
        }

        pick->PopState();
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Core::Entity.
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
// Static function for creating the UI panel that allows users to edit Core::Entity.
//
void Entity::CreatePanel( CreatePanelArgs& args )
{
    EntityPanel* panel = new EntityPanel ( args.m_Generator, args.m_Selection );

    args.m_Generator->Push( panel );
    {
        panel->SetCanvas( args.m_Generator->GetContainer()->GetCanvas() );
    }
    args.m_Generator->Pop();
}

tstring Entity::GetEntityAssetPath() const
{
    Asset::Entity* entity = GetPackage< Content::EntityInstance >()->GetEntity();
    if ( entity )
    {
        return entity->GetPath().Get();
    }
    return TXT("");
}

void Entity::SetEntityAssetPath( const tstring& entityClass )
{
    Content::EntityInstance* entity = GetPackage< Content::EntityInstance >();

    Helium::Path oldPath = entity->GetEntity()->GetPath();
    Helium::Path newPath( entityClass );

    m_ClassChanging.Raise( EntityAssetChangeArgs( this, oldPath, newPath ) );

    entity->GetEntity()->SetPath( newPath );

    // since our entity class is criteria used for deducing object type,
    //  ensure we are a member of the correct type
    CheckNodeType();

    m_ClassChanged.Raise( EntityAssetChangeArgs( this, oldPath, newPath ) );

    Dirty();
}

void Entity::OnComponentAdded( const Component::ComponentCollectionChanged& args )
{
}

void Entity::OnComponentRemoved( const Component::ComponentCollectionChanged& args )
{
}
