#include "Precompile.h"
#include "Entity.h"
#include "EntityType.h"
#include "EntityPanel.h"
#include "EntityAssetSet.h"

#include "Scene.h"
#include "SceneManager.h"
#include "SceneEditor.h"

#include "Core/Enumerator.h"
#include "PrimitiveCube.h"
#include "PrimitivePointer.h"

#include "Attribute/AttributeHandle.h"
#include "Asset/ArtFileAttribute.h"
#include "Asset/SceneManifest.h"
#include "Asset/Entity.h"

#include "Finder/ContentSpecs.h"
#include "File/Manager.h"
#include "Console/Console.h"

// Using
using namespace Math;
using namespace Luna;
using namespace Attribute;

// RTTI
LUNA_DEFINE_TYPE(Luna::Entity);

void Entity::InitializeType()
{
  Reflect::RegisterClass< Luna::Entity >( "Luna::Entity" );
  Enumerator::InitializePanel( "Entity", CreatePanelSignature::Delegate( &Entity::CreatePanel ) );
}

void Entity::CleanupType()
{
  Reflect::UnregisterClass< Luna::Entity >();
}

Entity::Entity(Luna::Scene* scene)
: Luna::Instance (scene, new Asset::Entity ())
{
  ConstructorInit();
}

Entity::Entity(Luna::Scene* scene, Asset::Entity* entity)
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

  pkg->RemoveAttributeAddedListener( Attribute::AttributeCollectionChangedSignature::Delegate( this, &Entity::OnAttributeAdded ) );
  pkg->RemoveAttributeRemovedListener( Attribute::AttributeCollectionChangedSignature::Delegate( this, &Entity::OnAttributeRemoved ) );

}

void Entity::ConstructorInit()
{
  m_ClassSet = NULL;
  m_NestedSceneArt = NULL;
  m_NestedSceneCollision = NULL;
  m_NestedScenePathfinding = NULL;

  Asset::Entity* pkg = GetPackage< Asset::Entity >();

  pkg->AddAttributeAddedListener( Attribute::AttributeCollectionChangedSignature::Delegate( this, &Entity::OnAttributeAdded ) );
  pkg->AddAttributeRemovedListener( Attribute::AttributeCollectionChangedSignature::Delegate( this, &Entity::OnAttributeRemoved ) );
}

std::string Entity::GenerateName() const
{
  Asset::EntityAssetPtr entityClass;

  const Asset::Entity* entity = GetPackage<Asset::Entity>();
  try
  {
    if ( entity->GetEntityAssetID() )
    {
      entityClass = entity->GetEntityAsset();
    }
  }
  catch ( Nocturnal::Exception& ex )
  {
    Console::Error( "%s", ex.what() );
  }

  std::string name = entityClass ? entityClass->GetShortName() : "";
  
  if (name.empty())
  {
    name = __super::GenerateName();
  }
  else
  {
    if ( isdigit( *name.rbegin() ) )
    {
      name += "_";
    }

    name += "1";
  }

  return name;
}

std::string Entity::GetApplicationTypeName() const
{
  return "Entity";
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

  if (m_ClassSet->GetEntityAsset())
  {
    switch ( mode )
    {
    case GeometryModes::Render:
      if ( !m_NestedSceneArt && load_on_demand )
      {
        Attribute::AttributeViewer< Asset::ArtFileAttribute > artFile( m_ClassSet->GetEntityAsset(), true );
        std::string nestedFile = FinderSpecs::Content::STATIC_DECORATION.GetExportFile( m_ClassSet->GetContentFile(), artFile->m_FragmentNode );
        m_NestedSceneArt = m_Scene->GetManager()->AllocateNestedScene( nestedFile, m_Scene );
      }
      nestedScene = m_NestedSceneArt;
      break;

    case GeometryModes::Collision:
      if ( !m_NestedSceneCollision  && load_on_demand)
      {
        Attribute::AttributeViewer< Asset::ArtFileAttribute > artFile( m_ClassSet->GetEntityAsset(), true );
        std::string nestedFile = FinderSpecs::Content::COLLISION_DECORATION.GetExportFile( m_ClassSet->GetContentFile(), artFile->m_FragmentNode );
        m_NestedSceneCollision = m_Scene->GetManager()->AllocateNestedScene( nestedFile, m_Scene );
      }
      nestedScene = m_NestedSceneCollision;
      break;

    case GeometryModes::Pathfinding:
      if ( !m_NestedScenePathfinding && load_on_demand)
      {
        Attribute::AttributeViewer< Asset::ArtFileAttribute > artFile( m_ClassSet->GetEntityAsset(), true );
        std::string nestedFile = FinderSpecs::Content::PATHFINDING_DECORATION.GetExportFile( m_ClassSet->GetContentFile(), artFile->m_FragmentNode );
        m_NestedScenePathfinding = m_Scene->GetManager()->AllocateNestedScene( nestedFile, m_Scene );
      }
      nestedScene = m_NestedScenePathfinding;
      break;
    }
  }

  return nestedScene;
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
        const Luna::Scene* nested = GetNestedScene( m_Scene->GetView()->GetGeometryMode() );

        if (nested)
        {
          m_ObjectBounds.Merge( nested->GetRoot()->GetObjectHierarchyBounds() );
        }

        if (m_Scene->GetView()->IsPathfindingVisible())
        {
          nested = GetNestedScene( GeometryModes::Pathfinding );

          if (nested)
          {
            m_ObjectBounds.Merge( nested->GetRoot()->GetObjectHierarchyBounds() );
          }
        }
      }

      break;
    }
  }
}

void Entity::Render( RenderVisitor* render )
{
  const Asset::Entity* package = GetPackage< Asset::Entity >();

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
    const Luna::Scene* nested = GetNestedScene( render->GetView()->GetGeometryMode() );

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

    nested = render->GetView()->IsPathfindingVisible() ? GetNestedScene( GeometryModes::Pathfinding ) : NULL;

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

  const Asset::Entity* package = entity->GetPackage< Asset::Entity >();

  entity->SetMaterial( type->GetMaterial() );

  // draw class shape
  classSet->GetShape()->Draw( args, package->m_SolidOverride ? &package->m_Solid : NULL, package->m_TransparentOverride ? &package->m_Transparent : NULL );
}

bool Entity::Pick( PickVisitor* pick )
{
  bool result = false;

  Luna::EntityType* type = Reflect::AssertCast<Luna::EntityType>(m_NodeType);

  const Asset::Entity* package = GetPackage< Asset::Entity >();

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
    const Luna::Scene* scene = GetNestedScene(GetScene()->GetView()->GetGeometryMode());

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

    // retrieve pathfinding scene
    scene = GetScene()->GetView()->IsPathfindingVisible() ? GetNestedScene(GeometryModes::Pathfinding) : NULL;

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
bool Entity::ValidatePanel(const std::string& name)
{
  if ( name == "Entity" )
    return true;

  if ( name == "Instance" )
    return false;

  return __super::ValidatePanel( name );
}

///////////////////////////////////////////////////////////////////////////////
// Static function for creating the UI panel that allows users to edit Luna::Entity.
//
void Entity::CreatePanel( CreatePanelArgs& args )
{
  EntityPanel* panel = new EntityPanel ( args.m_Enumerator, args.m_Selection );

  args.m_Enumerator->Push( panel );
  {
    panel->SetCanvas( args.m_Enumerator->GetContainer()->GetCanvas() );
    panel->Create();
  }
  args.m_Enumerator->Pop();
}

std::string Entity::GetEntityAssetPath() const
{
  try
  {
    return File::GlobalManager().GetPath( GetPackage< Asset::Entity >()->GetEntityAssetID() );
  }
  catch ( const File::Exception& e )
  {
    Console::Error( "%s\n", e.what() );
  }

  return std::string ();
}

void Entity::SetEntityAssetPath( const std::string& entityClass )
{
  Asset::Entity* entity = GetPackage< Asset::Entity >();

  tuid fileID = TUID::Null;

  try
  {
    fileID = File::GlobalManager().GetID( entityClass );
  }
  catch ( const File::Exception& e )
  {
    Console::Error( "%s\n", e.what() );
    return;
  }

  tuid oldID = entity->GetEntityAssetID();
  if ( fileID != oldID )
  {
    m_ClassChanging.Raise( EntityAssetChangeArgs( this, oldID, fileID ) );

    entity->SetEntityAssetID(fileID);

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

    m_ClassChanged.Raise( EntityAssetChangeArgs( this, oldID, fileID ) );

    Dirty();
  }
}

std::string Entity::GetEngineTypeName() const
{
  if ( GetClassSet()->GetEntityAsset() )
  {
    return GetClassSet()->GetEntityAsset()->GetEngineTypeName();
  }
  else
  {
    return "Unknown";
  }
}

void Entity::SetEngineTypeName( const std::string& type )
{
  NOC_BREAK();
}

void Entity::OnInstanceCollisionAttributeModified( const Reflect::ElementChangeArgs& args )
{
}

void Entity::OnAttributeAdded( const Attribute::AttributeCollectionChanged& args )
{
}

void Entity::OnAttributeRemoved( const Attribute::AttributeCollectionChanged& args )
{
}
