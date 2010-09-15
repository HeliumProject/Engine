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
REFLECT_DEFINE_ABSTRACT(Core::EntityInstance);

void EntityInstance::InitializeType()
{
    Reflect::RegisterClassType< Core::EntityInstance >( TXT( "Core::EntityInstance" ) );
    PropertiesGenerator::InitializePanel( TXT( "EntityInstance" ), CreatePanelSignature::Delegate( &EntityInstance::CreatePanel ) );
}

void EntityInstance::CleanupType()
{
    Reflect::UnregisterClassType< Core::EntityInstance >();
}

EntityInstance::EntityInstance(Core::Scene* scene)
: Core::Instance (scene, new Content::EntityInstance())
{
    ConstructorInit();
}

EntityInstance::EntityInstance(Core::Scene* scene, Content::EntityInstance* entity)
: Core::Instance ( scene, entity )
{
    ConstructorInit();
}

EntityInstance::~EntityInstance()
{
    Content::EntityInstance* pkg = GetPackage< Content::EntityInstance >();

    pkg->RemoveComponentAddedListener( Component::ComponentCollectionChangedSignature::Delegate( this, &EntityInstance::OnComponentAdded ) );
    pkg->RemoveComponentRemovedListener( Component::ComponentCollectionChangedSignature::Delegate( this, &EntityInstance::OnComponentRemoved ) );
}

void EntityInstance::ConstructorInit()
{
    m_ClassSet = NULL;
    m_Scene = NULL;

    Content::EntityInstance* pkg = GetPackage< Content::EntityInstance >();

    pkg->AddComponentAddedListener( Component::ComponentCollectionChangedSignature::Delegate( this, &EntityInstance::OnComponentAdded ) );
    pkg->AddComponentRemovedListener( Component::ComponentCollectionChangedSignature::Delegate( this, &EntityInstance::OnComponentRemoved ) );
}

tstring EntityInstance::GenerateName() const
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

tstring EntityInstance::GetApplicationTypeName() const
{
    return TXT( "EntityInstance" );
}

SceneNodeTypePtr EntityInstance::CreateNodeType( Core::Scene* scene ) const
{
    // Overridden to create an entity-specific type
    Core::EntityType* nodeType = new Core::EntityType( scene, GetType() );

    // Set the image index (usually this is handled by the base class, but we aren't calling the base)
    nodeType->SetImageIndex( GetImageIndex() );

    return nodeType;
}

Core::Scene* EntityInstance::GetNestedScene( GeometryMode mode, bool load_on_demand ) const
{
    if (m_ClassSet->GetEntity())
    {
        ResolveSceneArgs args ( m_ClassSet->GetContentFile() );
        m_Owner->ResolveSceneDelegate().Invoke( args );
        m_Scene = args.m_Scene;
    }

    return m_Scene;
}

bool EntityInstance::IsPointerVisible() const
{
    HELIUM_ASSERT(m_VisibilityData); 
    return m_VisibilityData->GetShowPointer(); 
}

void EntityInstance::SetPointerVisible(bool visible)
{
    HELIUM_ASSERT(m_VisibilityData); 
    m_VisibilityData->SetShowPointer(visible); 

    // we need to dirty to cause our bounds needs to be re-computed
    Dirty();
}

bool EntityInstance::IsBoundsVisible() const
{
    HELIUM_ASSERT(m_VisibilityData); 
    return m_VisibilityData->GetShowBounds(); 
}

void EntityInstance::SetBoundsVisible(bool visible)
{
    HELIUM_ASSERT(m_VisibilityData); 
    m_VisibilityData->SetShowBounds(visible); 

    // we need to dirty to cause our bounds needs to be re-computed
    Dirty();
}

bool EntityInstance::IsGeometryVisible() const
{
    HELIUM_ASSERT(m_VisibilityData); 
    return m_VisibilityData->GetShowGeometry(); 
}

void EntityInstance::SetGeometryVisible(bool visible)
{
    HELIUM_ASSERT(m_VisibilityData); 
    m_VisibilityData->SetShowGeometry(visible); 

    // we need to dirty to cause our bounds needs to be re-computed
    Dirty();
}

Core::EntitySet* EntityInstance::GetClassSet()
{
    return m_ClassSet;
}

const Core::EntitySet* EntityInstance::GetClassSet() const
{
    return m_ClassSet;
}

void EntityInstance::SetClassSet(Core::EntitySet* classSet)
{
    m_ClassSet = classSet;
}

void EntityInstance::PopulateManifest( Asset::SceneManifest* manifest ) const
{
}

void EntityInstance::Evaluate(GraphDirection direction)
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

void EntityInstance::Render( RenderVisitor* render )
{
    const Content::EntityInstance* package = GetPackage< Content::EntityInstance >();

    if (IsPointerVisible())
    {
        // entity pointer is drawn normalized
        RenderEntry* entry = render->Allocate(this);
        entry->m_Location = render->State().m_Matrix.Normalized();
        entry->m_Center = m_ObjectBounds.Center();
        entry->m_Draw = &EntityInstance::DrawPointer;
    }

    if (IsBoundsVisible() && m_ClassSet && m_ClassSet->GetShape())
    {
        // bounds are drawn non-normalized
        RenderEntry* entry = render->Allocate(this);
        entry->m_Location = render->State().m_Matrix;
        entry->m_Center = m_ObjectBounds.Center();
        entry->m_Draw = &EntityInstance::DrawBounds;

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

void EntityInstance::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const Core::EntityInstance* entity = Reflect::ConstAssertCast<Core::EntityInstance>( object );

    const Core::EntityType* type = Reflect::ConstAssertCast<Core::EntityType>( entity->GetNodeType() );

    entity->SetMaterial( type->GetMaterial() );

    // draw type pointer
    type->GetPointer()->Draw( args );
}

void EntityInstance::DrawBounds( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const Core::EntityInstance* entity = Reflect::ConstAssertCast<Core::EntityInstance>( object );

    const Core::EntityType* type = Reflect::ConstAssertCast<Core::EntityType>( entity->GetNodeType() );

    const Core::EntitySet* classSet = entity->GetClassSet();

    const Content::EntityInstance* package = entity->GetPackage< Content::EntityInstance >();

    entity->SetMaterial( type->GetMaterial() );

    // draw class shape
    classSet->GetShape()->Draw( args, package->m_SolidOverride ? &package->m_Solid : NULL, package->m_TransparentOverride ? &package->m_Transparent : NULL );
}

bool EntityInstance::Pick( PickVisitor* pick )
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
// Returns true if the specified panel is supported by Core::EntityInstance.
//
bool EntityInstance::ValidatePanel(const tstring& name)
{
    if ( name == TXT( "EntityInstance" ) )
        return true;

    if ( name == TXT( "Instance" ) )
        return false;

    return __super::ValidatePanel( name );
}

///////////////////////////////////////////////////////////////////////////////
// Static function for creating the UI panel that allows users to edit Core::EntityInstance.
//
void EntityInstance::CreatePanel( CreatePanelArgs& args )
{
    EntityPanel* panel = new EntityPanel ( args.m_Generator, args.m_Selection );

    args.m_Generator->Push( panel );
    {
        panel->SetCanvas( args.m_Generator->GetContainer()->GetCanvas() );
    }
    args.m_Generator->Pop();
}

tstring EntityInstance::GetEntityAssetPath() const
{
    Asset::Entity* entity = GetPackage< Content::EntityInstance >()->GetEntity();
    if ( entity )
    {
        return entity->GetPath().Get();
    }
    return TXT("");
}

void EntityInstance::SetEntityAssetPath( const tstring& entityClass )
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

void EntityInstance::OnComponentAdded( const Component::ComponentCollectionChanged& args )
{
}

void EntityInstance::OnComponentRemoved( const Component::ComponentCollectionChanged& args )
{
}
