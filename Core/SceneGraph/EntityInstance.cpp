/*#include "Precompile.h"*/
#include "EntityInstance.h"
#include "EntityInstanceType.h"
#include "EntityInstancePanel.h"
#include "EntitySet.h"

#include "Core/SceneGraph/Scene.h"
#include "Core/SceneGraph/SceneManager.h"

#include "Core/SceneGraph/PropertiesGenerator.h"
#include "PrimitiveCube.h"
#include "PrimitivePointer.h"

#include "Foundation/Component/ComponentHandle.h"
#include "Core/Asset/Manifests/SceneManifest.h"
#include "Core/Asset/Classes/Entity.h"

#include "Foundation/Log.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::SceneGraph;
using namespace Helium::Component;

REFLECT_DEFINE_CLASS(EntityInstance);

void EntityInstance::EnumerateClass( Reflect::Compositor< EntityInstance >& comp )
{
    comp.AddField( &EntityInstance::m_Path,         "m_Path" );
    comp.AddField( &EntityInstance::m_ShowPointer,  "m_ShowPointer" );
    comp.AddField( &EntityInstance::m_ShowBounds,   "m_ShowBounds" );
    comp.AddField( &EntityInstance::m_ShowGeometry, "m_ShowGeometry" );
}

void EntityInstance::InitializeType()
{
    Reflect::RegisterClassType< EntityInstance >( TXT( "EntityInstance" ) );
    PropertiesGenerator::InitializePanel( TXT( "EntityInstance" ), CreatePanelSignature::Delegate( &EntityInstance::CreatePanel ) );
}

void EntityInstance::CleanupType()
{
    Reflect::UnregisterClassType< EntityInstance >();
}

EntityInstance::EntityInstance()
: m_ShowPointer (true)
, m_ShowBounds (true)
, m_ShowGeometry (false)
, m_ClassSet( NULL )
{
    AddComponentAddedListener( Component::ComponentCollectionChangedSignature::Delegate( this, &EntityInstance::OnComponentAdded ) );
    AddComponentRemovedListener( Component::ComponentCollectionChangedSignature::Delegate( this, &EntityInstance::OnComponentRemoved ) );
}

EntityInstance::~EntityInstance()
{
    RemoveComponentAddedListener( Component::ComponentCollectionChangedSignature::Delegate( this, &EntityInstance::OnComponentAdded ) );
    RemoveComponentRemovedListener( Component::ComponentCollectionChangedSignature::Delegate( this, &EntityInstance::OnComponentRemoved ) );
}

bool EntityInstance::ValidatePersistent( const Component::ComponentPtr& attr ) const
{
    Asset::EntityPtr entityClass = GetEntity();

    if ( entityClass.ReferencesObject() )
    {
        // if the value of the attribute we are setting to is the default value, don't set the attribute, and attempt to remove it if it exists in the Entity
        if ( attr->GetComponentUsage() == ComponentUsages::Overridable )
        {
            ComponentPtr classAttr = entityClass->GetComponent( attr->GetType() );

            if ( attr->Equals( classAttr ) )
            {
                return false;
            }
        }
    }

    return __super::ValidatePersistent(attr);
}

const ComponentPtr& EntityInstance::GetComponent(i32 typeID) const
{
    // try to get the attribute from the Entity
    const ComponentPtr &instAttr = __super::GetComponent( typeID );

    if ( instAttr )
    {
        ComponentUsage usage = instAttr->GetComponentUsage();

        // NOTE: this handles the case where the Overridable attribute has been removed from the AssetClass
        if ( usage == ComponentUsages::Overridable )
        {
            Asset::EntityPtr entityClass = GetEntity();

            if ( entityClass.ReferencesObject() )
            {
                const ComponentPtr &classAttr = entityClass->GetComponent( typeID );

                if ( classAttr == NULL )
                {
                    // Fan-fucking-tastic
                    const_cast<EntityInstance*>(this)->RemoveComponent( typeID );
                    return classAttr;
                }
            }
        }

        // it's either an Overridable attribute that has been over-riden in the Entity or it's an Instance attribute.
        return instAttr;
    }

    Asset::EntityPtr entityClass = GetEntity();

    if ( entityClass.ReferencesObject() )
    {
        return entityClass->GetComponent( typeID );
    }

    return instAttr;
}

bool EntityInstance::SetComponent( const ComponentPtr& attr, bool validate, tstring* error )
{
    // NOTE: GetAssetClass should ensure that the AssetClass is valid, and throw an exception otherwise
    Asset::EntityPtr entityClass = GetEntity();

    if ( entityClass.ReferencesObject() )
    {
        // find the attribute of the corresponding type in the asset class
        ComponentPtr classAttr = entityClass->GetComponent( attr->GetType() );

        // make sure we aren't attempting to pass in an attribute with the same address as the corresponding attribute in the EntityAsset attr
        if ( classAttr == attr )
        {
            return true;
        }
    }

    return __super::SetComponent( attr, validate, error );
}

tstring EntityInstance::GenerateName() const
{
    Asset::EntityPtr entityClass = GetEntity();

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

SceneNodeTypePtr EntityInstance::CreateNodeType( Scene* scene ) const
{
    // Overridden to create an entity-specific type
    EntityInstanceType* nodeType = new EntityInstanceType( scene, GetType() );

    // Set the image index (usually this is handled by the base class, but we aren't calling the base)
    nodeType->SetImageIndex( GetImageIndex() );

    return nodeType;
}

void EntityInstance::CheckNodeType()
{
    Base::CheckNodeType();

    CheckSets();
}

void EntityInstance::CheckSets()
{
    const Helium::Path& path = GetEntityPath();

    Asset::EntityPtr entity;
    try
    {
        entity = GetEntity();
    }
    catch ( const Helium::Exception& ex )
    {
        Log::Warning( TXT("%s\n"), ex.What() );
    }

    if ( !entity && !path.empty() )
    {
        tstringstream str;
        str << TXT("Failed to load entity class for entity ") << GetName() << TXT(".") << std::endl;
        str << TXT("The entity '") << path.c_str() << TXT("' has moved or been deleted.") << std::endl;
        Log::Error( str.str().c_str() );
    }

    const tstring& currentClassSet = path.Get();

    EntityInstanceType* type = Reflect::AssertCast<EntityInstanceType>( m_NodeType );
    if (type)
    {
        // find the set
        M_InstanceSetSmartPtr::const_iterator found = type->GetSets().find(currentClassSet);

        // if we found it, and it contains us, and we are using it
        if (found != type->GetSets().end() && found->second->ContainsInstance(this) && m_ClassSet == found->second.Ptr())
        {
            // we are GTG
            return;
        }

        // the set we are entering
        EntitySet* newClassSet = NULL;

        // create new class object if it does not already exist
        if (found == type->GetSets().end())
        {
            // create
            newClassSet = new EntitySet (type, path);

            // save
            type->AddSet( newClassSet );
        }
        else
        {
            // existing
            newClassSet = Reflect::AssertCast<EntitySet>( found->second );
        }

        // check previous membership
        if (m_ClassSet)
        {
            m_ClassSet->RemoveInstance(this);
        }

        // add to the new class collection
        newClassSet->AddInstance(this);
    }
}

Scene* EntityInstance::GetNestedScene( GeometryMode mode, bool load_on_demand ) const
{
    if (m_ClassSet->GetEntity())
    {
        ResolveSceneArgs args ( m_ClassSet->GetContentFile() );
        m_Owner->ResolveSceneDelegate().Invoke( args );
        m_Scene = args.m_Scene;
    }

    return m_Scene;
}

tstring EntityInstance::GetEntityPath() const
{
    return m_Path.Get();
}

void EntityInstance::SetEntityPath( const tstring& path )
{
    Helium::Path oldPath = m_Path;
    Helium::Path newPath = path;

    m_ClassChanging.Raise( EntityAssetChangeArgs( this, oldPath, newPath ) );

    m_Path = path;

    // since our entity class is criteria used for deducing object type,
    //  ensure we are a member of the correct type
    CheckNodeType();

    m_ClassChanged.Raise( EntityAssetChangeArgs( this, oldPath, newPath ) );

    Dirty();
}

Asset::EntityPtr EntityInstance::GetEntity() const
{
    return Asset::AssetClass::LoadAssetClass< Asset::Entity >( m_Path );
}

bool EntityInstance::IsPointerVisible() const
{
    return m_ShowPointer; 
}

void EntityInstance::SetPointerVisible(bool visible)
{
    m_ShowPointer = visible;

    // we need to dirty to cause our bounds needs to be re-computed
    Dirty();
}

bool EntityInstance::IsBoundsVisible() const
{
    return m_ShowBounds; 
}

void EntityInstance::SetBoundsVisible(bool visible)
{
    m_ShowBounds = visible;

    // we need to dirty to cause our bounds needs to be re-computed
    Dirty();
}

bool EntityInstance::IsGeometryVisible() const
{
    return m_ShowGeometry; 
}

void EntityInstance::SetGeometryVisible(bool visible)
{
    m_ShowGeometry = visible;

    // we need to dirty to cause our bounds needs to be re-computed
    Dirty();
}

EntitySet* EntityInstance::GetClassSet()
{
    return m_ClassSet;
}

const EntitySet* EntityInstance::GetClassSet() const
{
    return m_ClassSet;
}

void EntityInstance::SetClassSet(EntitySet* classSet)
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
                EntityInstanceType* type = Reflect::AssertCast<EntityInstanceType>(m_NodeType);

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
                const Scene* nested = GetNestedScene( m_Scene->GetViewport()->GetGeometryMode() );

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

        if ( m_TransparentOverride ? m_Transparent : Reflect::AssertCast<InstanceType>( m_NodeType )->IsTransparent() )
        {
            entry->m_Flags |= RenderFlags::DistanceSort;
        }
    }

    if (IsGeometryVisible())
    {
        Scene* nested = GetNestedScene( render->GetViewport()->GetGeometryMode() );

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
    HierarchyNode::Render( render );
}

void EntityInstance::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const EntityInstance* entity = Reflect::ConstAssertCast<EntityInstance>( object );

    const EntityInstanceType* type = Reflect::ConstAssertCast<EntityInstanceType>( entity->GetNodeType() );

    entity->SetMaterial( type->GetMaterial() );

    // draw type pointer
    type->GetPointer()->Draw( args );
}

void EntityInstance::DrawBounds( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const EntityInstance* entity = Reflect::ConstAssertCast<EntityInstance>( object );

    const EntityInstanceType* type = Reflect::ConstAssertCast<EntityInstanceType>( entity->GetNodeType() );

    const EntitySet* classSet = entity->GetClassSet();

    entity->SetMaterial( type->GetMaterial() );

    // draw class shape
    classSet->GetShape()->Draw( args, entity->m_SolidOverride ? &entity->m_Solid : NULL, entity->m_TransparentOverride ? &entity->m_Transparent : NULL );
}

bool EntityInstance::Pick( PickVisitor* pick )
{
    bool result = false;

    EntityInstanceType* type = Reflect::AssertCast<EntityInstanceType>(m_NodeType);

    pick->SetFlag( PickFlags::IgnoreVertex, true );

    if (IsPointerVisible() || m_IsSelected)
    {
        pick->SetCurrentObject (this, pick->State().m_Matrix.Normalized());
        result |= type->GetPointer()->Pick (pick);
    }

    pick->SetFlag( PickFlags::IgnoreVertex, false );

    if ((IsBoundsVisible() || m_IsSelected) && m_ClassSet && m_ClassSet->GetShape())
    {
        pick->SetCurrentObject (this, pick->State().m_Matrix);
        result |= m_ClassSet->GetShape()->Pick(pick, m_SolidOverride ? &m_Solid : NULL);
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
        const Scene* scene = GetNestedScene(GetOwner()->GetViewport()->GetGeometryMode());

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
// Returns true if the specified panel is supported by EntityInstance.
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
// Static function for creating the UI panel that allows users to edit EntityInstance.
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

void EntityInstance::OnComponentAdded( const Component::ComponentCollectionChanged& args )
{
}

void EntityInstance::OnComponentRemoved( const Component::ComponentCollectionChanged& args )
{
}
