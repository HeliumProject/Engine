#include "Precompile.h"
#include "HierarchyNode.h"

#include "Reflect/Object.h"
#include "Foundation/Container/Insert.h" 

#include "Color.h"
#include "SceneGraph.h"

#include "Entity.h"
#include "EntityAssetSet.h"
#include "HierarchyNodeType.h"
#include "Layer.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "ScenePreferences.h"
#include "SceneVisitor.h"

using namespace Luna;
using namespace Nocturnal;

LUNA_DEFINE_TYPE( Luna::HierarchyNode );

void HierarchyNode::InitializeType()
{
  Reflect::RegisterClass< Luna::HierarchyNode >( "Luna::HierarchyNode" );
  Enumerator::InitializePanel( "Hierarchy", CreatePanelSignature::Delegate( &HierarchyNode::CreatePanel ) );
}

void HierarchyNode::CleanupType()
{
  Reflect::UnregisterClass< Luna::HierarchyNode >();
}

HierarchyNode::HierarchyNode(Luna::Scene* scene, Content::HierarchyNode* data) 
: Luna::SceneNode(scene, data)
, m_Parent( NULL )
, m_Previous( NULL )
, m_Next( NULL )
, m_LayerColor( NULL )
, m_Visible( true )
, m_Selectable( true )
, m_Highlighted( false )
, m_Reactive( false )
{

  m_VisibilityData = scene->GetVisibility( data->m_ID ); 

}

HierarchyNode::~HierarchyNode()
{

}

SceneNodeTypePtr HierarchyNode::CreateNodeType( Luna::Scene* scene ) const
{
  Luna::HierarchyNodeType* nodeType = new Luna::HierarchyNodeType( scene, GetType() );

  nodeType->SetImageIndex( GetImageIndex() );

  return nodeType;
}

void HierarchyNode::InitializeHierarchy()
{
  Initialize();

  for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
  {
    Luna::HierarchyNode* child = *itr;
    child->InitializeHierarchy();
  }
}

void HierarchyNode::Reset()
{
  m_Children.Clear();
}

void HierarchyNode::Pack()
{
  __super::Pack();

  UID::TUID parentID( UID::TUID::Null );

  if ( GetParent() != NULL && GetParent() != m_Scene->GetRoot() )
  {
    parentID = GetParent()->GetID();
  }

  Content::HierarchyNode* node = GetPackage<Content::HierarchyNode>();

  node->m_ParentID = parentID;
}

void HierarchyNode::Unpack()
{
  __super::Unpack();
}

void HierarchyNode::SetTransient( bool isTransient )
{
  if ( isTransient != IsTransient() )
  {
    __super::SetTransient( isTransient );

    OS_HierarchyNodeDumbPtr::Iterator childItr = m_Children.Begin();
    OS_HierarchyNodeDumbPtr::Iterator childEnd = m_Children.End();
    for ( ; childItr != childEnd; ++childItr )
    {
      Luna::HierarchyNode* child = *childItr;
      child->SetTransient( isTransient );
    }
  }
}

bool HierarchyNode::IsHidden() const
{
  NOC_ASSERT(m_VisibilityData); 
  return m_VisibilityData->GetHiddenNode(); 
}

void HierarchyNode::SetHidden(bool isHidden)
{
  NOC_ASSERT(m_VisibilityData); 
  m_VisibilityData->SetHiddenNode(isHidden); 

  Dirty();
}

bool HierarchyNode::IsLive() const
{
  return GetPackage< Content::HierarchyNode >()->m_Live;
}

void HierarchyNode::SetLive(bool isLive)
{
  GetPackage< Content::HierarchyNode >()->m_Live = isLive;
}

bool HierarchyNode::IsVisible() const
{
  bool isVisible = m_Visible;

  if ( m_Scene->GetRoot().Ptr() == this )
  {
    // The root object is never visible
    isVisible = false;
  }

  return isVisible;
}

bool HierarchyNode::IsSelectable() const
{
  bool isSelectable = m_Selectable;

  if ( m_Scene->GetRoot().Ptr() == this )
  {
    // The root object is never selectable
    isSelectable = false;
  }

  return isSelectable;
}

void HierarchyNode::SetSelected( bool value )
{
  if ( value != IsSelected() )
  {
    __super::SetSelected( value );

    SetReactive( value );
  }
}

bool HierarchyNode::IsHighlighted() const
{
  return m_Highlighted;
}

void HierarchyNode::SetHighlighted(bool value)
{
  m_Highlighted = value;
}

bool HierarchyNode::IsReactive() const
{
  return m_Reactive;
}

void HierarchyNode::SetReactive( bool value )
{
  m_Reactive = value;
  OS_HierarchyNodeDumbPtr::Iterator childItr = m_Children.Begin();
  OS_HierarchyNodeDumbPtr::Iterator childEnd = m_Children.End();
  for ( ; childItr != childEnd; ++childItr )
  {
    Luna::HierarchyNode* child = *childItr;
    child->SetReactive( value );
  }
}

void HierarchyNode::SetName( const std::string& value )
{
  __super::SetName( value );

  // reset path b/c our name changed
  m_Path = "";
}

const std::string& HierarchyNode::GetPath()
{
  if (m_Path == "")
  {
    m_Path = std::string( "|" ) + GetName();
    const Luna::HierarchyNode* p = m_Parent;
    while ( p != NULL && p->GetParent() != NULL )
    {
      m_Path = m_Path.insert( 0, "|" + p->GetName() );
      p = p->GetParent();
    }
  }

  return m_Path;
}

Luna::HierarchyNode* HierarchyNode::GetParent() const
{
  return m_Parent;
}

void HierarchyNode::SetParent( Luna::HierarchyNode* value )
{
  if ( value != m_Parent )
  {
    Luna::HierarchyNode* oldParent = m_Parent;

    ParentChangingArgs changing( this, value );
    if( m_ParentChanging.RaiseWithReturn( changing ) )
    {
      if (value != NULL)
      {
        if (m_Graph != NULL && m_Graph != value->GetGraph())
        {
          m_Graph->RemoveNode(this);
        }

        if (m_Graph == NULL)
        {
          value->GetGraph()->AddNode(this);
        }
      }

      if (m_Parent != NULL)
      {
        m_Parent->RemoveChild(this);
        m_Parent = NULL;
      }

      m_Parent = value;

      if ( m_Parent )
      {
        m_Parent->AddChild(this);
      }

      m_Path = "";

      ParentChangedArgs parentChanged( this, oldParent );
      m_ParentChanged.Raise( parentChanged );
    }
  }
}

void HierarchyNode::SetPrevious( Luna::HierarchyNode* value )
{
  m_Previous = value;
}

void HierarchyNode::SetNext( Luna::HierarchyNode* value )
{
  m_Next = value;
}

void HierarchyNode::ReverseChildren()
{
  V_HierarchyNodeSmartPtr children;
  children.reserve( m_Children.Size() );
  OS_HierarchyNodeDumbPtr::Iterator childItr = m_Children.Begin();
  OS_HierarchyNodeDumbPtr::Iterator childEnd = m_Children.End();
  for ( ; childItr != childEnd; ++childItr )
  {
    children.push_back( *childItr );
  }

  m_Children.Clear();
  Luna::HierarchyNode* previous = NULL;
  V_HierarchyNodeSmartPtr::reverse_iterator rItr = children.rbegin();
  V_HierarchyNodeSmartPtr::reverse_iterator rEnd = children.rend();
  for ( ; rItr != rEnd; ++rItr )
  {
    Luna::HierarchyNode* current = *rItr;
    current->m_Previous = previous;
    if ( previous )
    {
      previous->m_Next = current;
    }
    m_Children.Append( current );

    previous = current;
  }

  Dirty();
}

HierarchyNodePtr HierarchyNode::Duplicate()
{
  // update persistent data of this object
  Pack();

  // clone the persistent data into a new instance of content data
  Content::HierarchyNodePtr data = Reflect::DangerousCast< Content::HierarchyNode > ( GetPackage< Content::HierarchyNode > ()->Clone() );

  // generate new unique ID 
  UID::TUID::Generate( data->m_ID ); 

  // have the scene create the correct application object for this data
  HierarchyNodePtr duplicate = Reflect::ObjectCast< Luna::HierarchyNode > ( m_Scene->CreateNode( data ) );

  // the duplicate must be a part of the dependency graph to hold the parent/child relationship
  m_Graph->AddNode( duplicate );

  // copy ancestral dependency connections
  for each (Luna::SceneNode* ancestor in GetAncestors())
  {
    if ( ancestor->HasType( Reflect::GetType<Luna::HierarchyNode>() ) )
    {
      continue;
    }

    duplicate->CreateDependency( ancestor );
  }

  // copy descendant dependency connections
  for each (Luna::SceneNode* descendant in GetDescendants())
  {
    if ( descendant->HasType( Reflect::GetType<Luna::HierarchyNode>() ) )
    {
      continue;
    }

    descendant->CreateDependency( duplicate );
  }

  // recurse on each child
  for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
  {
    Luna::HierarchyNode* child = *itr;

    // duplicate recursively
    HierarchyNodePtr duplicateChild = child->Duplicate();

    // connect to the duplicated parent
    duplicateChild->SetParent( duplicate );
  }

  Content::NodeVisibilityPtr visibilityData = m_Scene->GetVisibility( duplicate->GetID() ); 
  m_Scene->GetVisibility( GetID() )->CopyTo( visibilityData );
  
  return duplicate;
}

Math::AlignedBox HierarchyNode::GetGlobalBounds() const
{
  const Luna::Transform* transform = GetTransform();

  Math::AlignedBox bounds;

  // reset to local HierarchyBounds
  bounds.Merge( m_ObjectBounds );

  if (transform)
  {
    // transform those as sample points into global space
    bounds.Transform( transform->GetGlobalTransform() );
  }

  return bounds;
}

Math::AlignedBox HierarchyNode::GetGlobalHierarchyBounds() const
{
  const Luna::Transform* transform = GetTransform();

  Math::AlignedBox bounds;

  // reset to local HierarchyBounds
  bounds.Merge( m_ObjectHierarchyBounds );

  if (transform)
  {
    // transform those as sample points into global space
    bounds.Transform( transform->GetGlobalTransform() );
  }

  return bounds;
}

void HierarchyNode::AddParentChangingListener( ParentChangingSignature::Delegate listener )
{
  m_ParentChanging.Add( listener );
}

void HierarchyNode::RemoveParentChangingListener( ParentChangingSignature::Delegate listener )
{
  m_ParentChanging.Remove( listener );
}

void HierarchyNode::AddParentChangedListener( ParentChangedSignature::Delegate listener )
{
  m_ParentChanged.Add( listener );
}

void HierarchyNode::RemoveParentChangedListener( ParentChangedSignature::Delegate listener )
{
  m_ParentChanged.Remove( listener );
}

void HierarchyNode::AddChild(Luna::HierarchyNode* c)
{
  c->CreateDependency( this );
}

void HierarchyNode::RemoveChild(Luna::HierarchyNode* c)
{
  c->RemoveDependency( this );
}

void HierarchyNode::DisconnectDescendant(Luna::SceneNode* descendant) 
{
  __super::DisconnectDescendant(descendant);

  if (descendant->HasType(Reflect::GetType<Luna::HierarchyNode>()))
  {
    Luna::HierarchyNode* child = Reflect::DangerousCast<Luna::HierarchyNode>(descendant);

    // sanity check that the parent is really set to this
    NOC_ASSERT( child->GetParent() == this );

    // we should not be disconnecting descendant hierarchy nodes that are not our children
    NOC_ASSERT( m_Children.Contains(child) );
    Log::Debug("Removing %s from %s's child list (previous=%s next=%s)\n", child->GetName().c_str(), GetName().c_str(), child->m_Previous ? child->m_Previous->GetName().c_str() : "NULL" , child->m_Next ? child->m_Next->GetName().c_str() : "NULL");

    // fix up linked list
    if ( child->m_Previous )
    {
      Log::Debug("Setting %s's m_Next to %s\n", child->m_Previous->m_Next ? child->m_Previous->m_Next->GetName().c_str() : "NULL", child->m_Next ? child->m_Next->GetName().c_str() : "NULL");
      NOC_ASSERT( m_Children.Contains( child->m_Previous ) );
      child->m_Previous->m_Next = child->m_Next;
    }

    if ( child->m_Next )
    {
      Log::Debug("Setting %s's m_Previous to %s\n", child->m_Next->m_Previous ? child->m_Next->m_Previous->GetName().c_str() : "NULL", child->m_Previous ? child->m_Previous->GetName().c_str() : "NULL");
      NOC_ASSERT( m_Children.Contains( child->m_Next ) );
      child->m_Next->m_Previous = child->m_Previous;
    }

    // do erase
    m_Children.Remove( child );
  }
}

void HierarchyNode::ConnectDescendant(Luna::SceneNode* descendant)
{
  __super::ConnectDescendant(descendant);

  if (descendant->HasType(Reflect::GetType<Luna::HierarchyNode>()))
  {
    Luna::HierarchyNode* child = Reflect::DangerousCast<Luna::HierarchyNode>(descendant);

    // sanity check that the parent is really set to this
    NOC_ASSERT( child->GetParent() == this );

    // if we had a previous child, connect that previous child's next pointer to the child
    if ( child->m_Previous )
    {
      if ( m_Children.Contains( child->m_Previous ) )
      {
        child->m_Previous->m_Next = child;
      }
      else
      {
        child->m_Previous = NULL;
      }
    }

    // look for the next child after the one we want to insert, we will insert the child before that one
    OS_HierarchyNodeDumbPtr::Iterator insertBefore = m_Children.End();
    if ( child->m_Next )
    {
      if ( m_Children.Contains( child->m_Next ) )
      {
        child->m_Next->m_Previous = child;
      }
      else
      {
        child->m_Next = NULL;
      }
    }

    // do insertion
    bool inserted = false;

    if ( insertBefore != m_Children.End() )
    {
      inserted = m_Children.Insert( child, *insertBefore );
    }
    else
    {
      if ( m_Children.Size() > 0 )
      {
        Luna::HierarchyNode* back = m_Children.Back();
        
        back->m_Next = child;

        if ( child != back )
        {
          child->m_Previous = back;
        }
      }

      inserted = m_Children.Append(child);
    }
  }
}

void HierarchyNode::ConnectAncestor( Luna::SceneNode* ancestor )
{
  __super::ConnectAncestor( ancestor );

  if ( ancestor->HasType( Reflect::GetType< Luna::Layer >() ) )
  {
    m_LayerColor = Reflect::ObjectCast< Luna::Layer >( ancestor );
  }
}

void HierarchyNode::DisconnectAncestor( Luna::SceneNode* ancestor )
{
  __super::DisconnectAncestor( ancestor );

  if ( ancestor == m_LayerColor )
  {
    m_LayerColor = NULL;
    S_SceneNodeDumbPtr::const_iterator ancestorItr = m_Ancestors.begin();
    S_SceneNodeDumbPtr::const_iterator ancestorEnd = m_Ancestors.end();
    for ( ; ancestorItr != ancestorEnd; ++ancestorItr )
    {
      Luna::SceneNode* dependNode = (*ancestorItr);

      Luna::Layer* layer = Reflect::ObjectCast< Luna::Layer >( dependNode );
      if ( layer )
      {
        m_LayerColor = layer;
        break;
      }
    }
  }
}

Luna::Transform* HierarchyNode::GetTransform()
{
  HierarchyNodePtr node = this;

  while (node != NULL)
  {
    Luna::Transform* transform = Reflect::ObjectCast< Luna::Transform >( node );

    if (transform != NULL)
    {
      return transform;
    }

    node = node->GetParent();
  }

  return NULL;
}

const Luna::Transform* HierarchyNode::GetTransform() const
{
  return const_cast<Luna::HierarchyNode*>(this)->GetTransform();
}

void HierarchyNode::Create()
{
  __super::Create();

  for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
  {
    Luna::HierarchyNode* child = *itr;
    child->Create();
  }
}

void HierarchyNode::Delete()
{
  __super::Delete();

  for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
  {
    Luna::HierarchyNode* child = *itr;
    child->Delete();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if all the layers that this node is a member of are currently
// visible.
// 
bool HierarchyNode::ComputeVisibility() const
{
  Luna::HierarchyNode* parent = GetParent();

  bool isVisible = !IsHidden();

  isVisible &= (parent && m_Scene && parent != m_Scene->GetRoot()) ? parent->IsVisible() : true;

  isVisible &= m_NodeType ? Reflect::AssertCast<Luna::HierarchyNodeType>(m_NodeType)->IsVisible() : true;

  S_SceneNodeDumbPtr::const_iterator ancestorItr = m_Ancestors.begin();
  S_SceneNodeDumbPtr::const_iterator ancestorEnd = m_Ancestors.end();
  for ( ; ancestorItr != ancestorEnd && isVisible; ++ancestorItr )
  {
    Luna::SceneNode* dependNode = (*ancestorItr);

    Luna::Layer* layer = Reflect::ObjectCast< Luna::Layer >( dependNode );
    if ( layer )
    {
      isVisible &= layer->IsVisible();
    }
  }

  return isVisible;
}

///////////////////////////////////////////////////////////////////////////////
// Determines if this node should be selectable based upon its membership
// in any layers.  Returns true if all the layers that this node is a member
// of are currently selectable.
// 
bool HierarchyNode::ComputeSelectability() const
{
  bool isSelectable = m_NodeType ? Reflect::AssertCast<Luna::HierarchyNodeType>(m_NodeType)->IsSelectable() : true;

  S_SceneNodeDumbPtr::const_iterator ancestorItr = m_Ancestors.begin();
  S_SceneNodeDumbPtr::const_iterator ancestorEnd = m_Ancestors.end();
  for ( ; ancestorItr != ancestorEnd && isSelectable; ++ancestorItr )
  {
    Luna::SceneNode* dependNode = (*ancestorItr);

    Luna::Layer* layer = Reflect::ObjectCast< Luna::Layer >( dependNode );
    if ( layer )
    {
      isSelectable &= layer->IsSelectable();
    }
  }

  return isSelectable;
}

u32 HierarchyNode::Dirty()
{
  u32 count = 0;

  if (m_Graph)
  {
    // hierarchy nodes keep a bounding volume hierarchy up to date, and require both directions become dirty and evaluate
    count += m_Graph->DirtyNode(this, GraphDirections::Downstream);
    count += m_Graph->DirtyNode(this, GraphDirections::Upstream);
  }

  return count;
}

void HierarchyNode::Evaluate(GraphDirection direction)
{
  Luna::Transform* transform = GetTransform();

  switch (direction)
  {
  case GraphDirections::Downstream:
    {
      bool previousVisiblity = m_Visible;
      m_Visible = ComputeVisibility();
      if ( previousVisiblity != m_Visible )
      {
        m_VisibilityChanged.Raise( SceneNodeChangeArgs( this ) );
      }

      m_Selectable = ComputeSelectability();

      break;
    }

  case GraphDirections::Upstream:
    {
      // reset to singular
      m_ObjectHierarchyBounds.Reset();

      // start out with local bounds (your minimal bounds)
      m_ObjectHierarchyBounds.Merge( m_ObjectBounds );

      // for each hierarchy node child
      for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
      {
        Luna::HierarchyNode* child = *itr;

        // get the hierarchical bounds for that child
        Math::AlignedBox bounds = child->GetObjectHierarchyBounds();

        // get the child's transform
        Luna::Transform* childTransform = child->GetTransform();

        // check transform
        if (childTransform && childTransform != transform)
        {
          // if the transform isn't the same as our transform (don't double up), then transform that child's bounds into our space
          bounds.Transform( childTransform->GetGlobalTransform() * transform->GetInverseGlobalTransform() );
        }

        // and merge that result (now in our local space) into the accumulated hierarchical bounding volume
        m_ObjectHierarchyBounds.Merge( bounds );
      }

      break;
    }
  }

  __super::Evaluate(direction);
}

bool HierarchyNode::BoundsCheck(const Math::Matrix4& instanceMatrix) const
{
  Luna::Camera* camera = m_Scene->GetView()->GetCamera();

  Math::AlignedBox bounds (m_ObjectHierarchyBounds);

  bounds.Transform( instanceMatrix );

  if (camera->IsViewFrustumCulling() && !camera->GetViewFrustum().IntersectsBox(bounds))
  {
    return false;
  }

  return true;
}

void HierarchyNode::SetMaterial( const D3DMATERIAL9& defaultMaterial ) const
{
  Luna::View* view = m_Scene->GetView();

  IDirect3DDevice9* device = view->GetResources()->GetDevice();

  D3DMATERIAL9 material = defaultMaterial;

  switch ( SceneEditorPreferences()->GetViewPreferences()->GetColorMode() )
  {
  case ViewColorModes::Layer:
    if ( m_LayerColor )
    {
      const Math::Color3& color = m_LayerColor->GetColor();
      material.Ambient = Luna::Color::ColorToColorValue( defaultMaterial.Ambient.a, color.r, color.g, color.b );
    }
    break;

  case ViewColorModes::Zone:
    {
      const Math::Color3& color = m_Scene->GetColor();
      material.Ambient = Luna::Color::ColorToColorValue( defaultMaterial.Ambient.a, color.r, color.g, color.b );
    }
    break;
  
  case ViewColorModes::AssetType:
    {
      Luna::HierarchyNode* hierarchyNode = (Luna::HierarchyNode*) this;
      Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( hierarchyNode );
      if ( entity )
      {
        Luna::EntityAssetSet* entityClassSet = entity->GetClassSet();
        if ( entityClassSet )
        {
          Asset::EntityAsset* entityClass = entityClassSet->GetEntityAsset();
          if ( entityClass )
          {
            Asset::AssetType assetType = entityClass->GetAssetType();
            material = Luna::View::s_AssetTypeMaterials[ assetType ];
          }
        }
      }
    }
    break;
  
  case ViewColorModes::Scale:
    {
      HierarchyNode* hierarchyNode = (HierarchyNode*) this;
      material.Ambient = hierarchyNode->GetTransform()->GetScaleColor();
    }
    break;

  case ViewColorModes::ScaleGradient:
    {
      HierarchyNode* hierarchyNode = (HierarchyNode*) this;
      material.Ambient = hierarchyNode->GetTransform()->GetScaleColorGradient();
    }
    break;
  }

  if ( m_Scene->IsCurrent() )
  {
    if ( IsSelectable() )
    {
      if ( IsHighlighted() && view->IsHighlighting() )
      {
        material = Luna::View::s_HighlightedMaterial;
      }
      else if ( IsSelected() )
      {
        material = Luna::View::s_SelectedMaterial;
      }
      else if ( IsReactive() )
      {
        material = Luna::View::s_ReactiveMaterial;
      }
      else if ( IsLive() )
      {
        material = Luna::View::s_LiveMaterial;
      }

      material.Ambient.a = defaultMaterial.Ambient.a;
      material.Diffuse.a = defaultMaterial.Diffuse.a;
      material.Specular.a = defaultMaterial.Specular.a;
      material.Emissive.a = defaultMaterial.Emissive.a;
    }
    else
    {
      material = Luna::View::s_UnselectableMaterial;
    }
  }
  else
  {
    material.Ambient.a = defaultMaterial.Ambient.a;
    material.Diffuse.a = defaultMaterial.Diffuse.a;
    material.Specular.a = defaultMaterial.Specular.a;
    material.Emissive.a = defaultMaterial.Emissive.a;
  }

  device->SetMaterial( &material );
}

TraversalAction HierarchyNode::TraverseHierarchy( HierarchyTraverser* traverser )
{
  TraversalAction result = traverser->VisitHierarchyNode( this );

  switch (result)
  {
  case TraversalActions::Continue:
    {
      for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
      {
        Luna::HierarchyNode* child = *itr;
        
        TraversalAction childResult = child->TraverseHierarchy( traverser );

        if (childResult == TraversalActions::Abort)
        {
          return TraversalActions::Abort;
        }
      }
    }

  case TraversalActions::Prune:
    {
      return TraversalActions::Continue;
    }
  }

  return TraversalActions::Abort;
}

void HierarchyNode::Render( RenderVisitor* render )
{
  Luna::Transform* transform = GetTransform();

  if ( transform && IsSelected() && m_Scene->IsCurrent() && render->GetView()->IsBoundsVisible() )
  {
    Math::V_Vector3 vertices;
    Math::V_Vector3 lineList;

    D3DMATERIAL9 material;
    ZeroMemory(&material, sizeof(material));

    m_Scene->GetView()->GetDevice()->SetFVF( ElementFormats[ ElementTypes::Position ] );


    //
    // Local draw
    //

    {
      Math::Matrix4 matrix = render->State().m_Matrix;

      m_Scene->GetView()->GetDevice()->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&(matrix) );

      vertices.clear();
      m_ObjectBounds.GetVertices(vertices);
      Math::AlignedBox::GetWireframe( vertices, lineList );
      material.Ambient = Luna::Color::ColorToColorValue( 1, 255, 0, 0 );
      m_Scene->GetView()->GetDevice()->SetMaterial(&material);
      m_Scene->GetView()->GetDevice()->DrawPrimitiveUP( D3DPT_LINELIST, (UINT)lineList.size() / 2, &lineList.front(), sizeof(Math::Vector3));
    }


    //
    // Global draw
    //

    {
      Math::Matrix4 matrix = render->ParentState().m_Matrix;

      m_Scene->GetView()->GetDevice()->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&(matrix) );

      vertices.clear();
      GetGlobalBounds().GetVertices(vertices);
      Math::AlignedBox::GetWireframe( vertices, lineList );
      material.Ambient = Luna::Color::ColorToColorValue( 1, 255, 128, 128 );
      m_Scene->GetView()->GetDevice()->SetMaterial(&material);
      m_Scene->GetView()->GetDevice()->DrawPrimitiveUP( D3DPT_LINELIST, (UINT)lineList.size() / 2, &lineList.front(), sizeof(Math::Vector3));
    }


    //
    // Local Hierarchy draw
    //

    {
      Math::Matrix4 matrix = render->State().m_Matrix;

      m_Scene->GetView()->GetDevice()->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&(matrix) );

      vertices.clear();
      m_ObjectHierarchyBounds.GetVertices(vertices);
      Math::AlignedBox::GetWireframe( vertices, lineList );
      material.Ambient = Luna::Color::ColorToColorValue( 1, 0, 0, 255 );
      m_Scene->GetView()->GetDevice()->SetMaterial(&material);
      m_Scene->GetView()->GetDevice()->DrawPrimitiveUP( D3DPT_LINELIST, (UINT)lineList.size() / 2, &lineList.front(), sizeof(Math::Vector3));
    }


    //
    // Global Hierarchy draw
    //

    {
      Math::Matrix4 matrix = render->ParentState().m_Matrix;

      m_Scene->GetView()->GetDevice()->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&(matrix) );

      vertices.clear();
      GetGlobalHierarchyBounds().GetVertices(vertices);
      Math::AlignedBox::GetWireframe( vertices, lineList );
      material.Ambient = Luna::Color::ColorToColorValue( 1, 128, 128, 255 );
      m_Scene->GetView()->GetDevice()->SetMaterial(&material);
      m_Scene->GetView()->GetDevice()->DrawPrimitiveUP( D3DPT_LINELIST, (UINT)lineList.size() / 2, &lineList.front(), sizeof(Math::Vector3));
    }

    m_Scene->GetView()->GetResources()->ResetState();
  }
}

bool HierarchyNode::Pick(PickVisitor* pick)
{
  return false;
}

Luna::HierarchyNode* HierarchyNode::Find( const std::string& targetName )
{
  if ( targetName.empty() )
    return NULL;

  if ( _stricmp( GetName().c_str(), targetName.c_str() ) == 0 )
  {
    return this;
  }

  Luna::HierarchyNode* found = NULL;

  for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
  {
    Luna::HierarchyNode* child = *itr;

    const std::string& currentName = child->GetName();

    // Case-insensitive comparison to see if the name matches the target
    if ( !currentName.empty() && ( _stricmp( currentName.c_str(), targetName.c_str() ) == 0 ) )
    {
      found = child; // stopping case, breaks out of the loop
    }
    else
    {
      found = child->Find( targetName ); // Search the child's children
    }

    if (found)
    {
      break;
    }
  }

  return found;
}

Luna::HierarchyNode* HierarchyNode::FindFromPath( std::string path )
{
  if ( path.empty() )
    return NULL;

  if ( GetPath() == path )
  {
    return this;
  }

  Luna::HierarchyNode* found = NULL;

  if (path[0] == '|')
  {
    path.erase( 0, 1 );
  }

  std::string childName = path;
  std::string childPath;

  // if our path is longer than one item, pick the first one
  const size_t pathSeparatorIndex = path.find_first_of( '|' );
  if ( pathSeparatorIndex != std::string::npos )
  {
    childName = path.substr( 0, pathSeparatorIndex );
    childPath = path.substr( pathSeparatorIndex + 1 );
  }

  // search our children, matching the childName
  for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
  {
    Luna::HierarchyNode* child = *itr;

    // if the name exists, and it matches
    const std::string& currentName = child->GetName();

    if ( !currentName.empty() && currentName == childName )
    {
      if ( childPath.empty() )
      {
        found = child; // stopping case, breaks out of the loop
      }
      else
      {
        found = child->FindFromPath( childPath );
      }

      if (found)
      {
        break;
      }
    }
  }

  return found;
}

void HierarchyNode::FindSimilar(V_HierarchyNodeDumbPtr& similar) const
{
  HM_SceneNodeSmartPtr::const_iterator itr = m_NodeType->GetInstances().begin();
  HM_SceneNodeSmartPtr::const_iterator end = m_NodeType->GetInstances().end();
  for ( ; itr != end; ++itr )
  {
    similar.push_back( Reflect::AssertCast<Luna::HierarchyNode>(itr->second) );
  }
}

bool HierarchyNode::IsSimilar(const HierarchyNodePtr& node) const
{
  return ( GetNodeType() == node->GetNodeType() );
}

std::string HierarchyNode::GetDescription() const
{
  return std::string ();
}

void HierarchyNode::ConnectManipulator(ManiuplatorAdapterCollection* collection)
{
  Luna::Transform* transform = GetTransform();

  if (transform != m_Scene->GetRoot())
  {
    // for non-transform types, just forward on the call
    transform->ConnectManipulator(collection);
  }
}

bool HierarchyNode::ValidatePanel(const std::string& name)
{
  if ( name == "Hierarchy" )
    return true;

  return __super::ValidatePanel( name );
}

void HierarchyNode::CreatePanel(CreatePanelArgs& args)
{
  args.m_Enumerator->PushPanel("Hierarchy Node", true);
  {
    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel( "Hidden" );
      args.m_Enumerator->AddCheckBox<Luna::HierarchyNode, bool>( args.m_Selection, &HierarchyNode::IsHidden, &HierarchyNode::SetHidden, false );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel( "Live" );   
      args.m_Enumerator->AddCheckBox<Luna::HierarchyNode, bool>( args.m_Selection, &HierarchyNode::IsLive, &HierarchyNode::SetLive );
    }
    args.m_Enumerator->Pop();
  }
  args.m_Enumerator->Pop();
}

