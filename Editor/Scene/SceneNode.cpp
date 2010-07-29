#include "Precompile.h"
#include "SceneNode.h"

#include "Pipeline/Content/Nodes/SceneNode.h"

#include "SceneNodeType.h"
#include "SceneNodePanel.h"
#include "SceneGraph.h"

#include "Scene.h"
#include "Layer.h"
#include "Transform.h"

#include "Statistics.h"
#include "Application/UI/ArtProvider.h"

using namespace Editor;
using namespace Helium;

LUNA_DEFINE_TYPE( SceneNode );

void SceneNode::InitializeType()
{
  Reflect::RegisterClass< Editor::SceneNode >( TXT( "Editor::SceneNode" ) );

  PropertiesGenerator::InitializePanel( TXT( "Scene Node" ), CreatePanelSignature::Delegate( &SceneNode::CreatePanel ));
}

void SceneNode::CleanupType()
{
  Reflect::UnregisterClass< Editor::SceneNode >();
}

SceneNode::SceneNode( Editor::Scene* scene, Content::SceneNode* data )
: Persistent( data )
, m_IsInitialized ( false )
, m_NodeType( NULL )
, m_Graph( NULL )
, m_Scene( scene )
, m_VisitedID( 0 )
, m_IsTransient( false )
{
  m_NodeStates[ GraphDirections::Downstream ] = NodeStates::Dirty;
  m_NodeStates[ GraphDirections::Upstream ] = NodeStates::Dirty;
}

SceneNode::~SceneNode()
{

}

const TUID& SceneNode::GetID() const
{
  return GetPackage<Content::SceneNode>()->m_ID;
}

void SceneNode::SetID( const TUID& id )
{
  GetPackage<Content::SceneNode>()->m_ID = id;
}

tstring SceneNode::GenerateName() const
{
  tstring name = GetPackage()->GetClass()->m_UIName;
  name[0] = tolower( name[0] );
  name += TXT( "1" );

  return name;
}

const tstring& SceneNode::GetName() const
{
  return GetPackage<Content::SceneNode>()->GetName();
}

void SceneNode::SetName(const tstring& value)
{
  Content::SceneNode* node = GetPackage<Content::SceneNode>();

  m_NameChanging.Raise( SceneNodeChangeArgs( this ) );

  if ( node->m_UseGivenName )
  {
    node->m_GivenName = value;
  }
  else
  {
    node->m_DefaultName = value;
  }

  m_NameChanged.Raise( SceneNodeChangeArgs( this ) );
}

bool SceneNode::UseGivenName() const
{
  return GetPackage<Content::SceneNode>()->m_UseGivenName;
}

void SceneNode::SetUseGivenName(bool use)
{
  Content::SceneNode* node = GetPackage<Content::SceneNode>();

  tstring oldName = node->GetName();

  node->m_UseGivenName = use;

  m_Scene->Rename( this, use ? node->GetName() : GenerateName(), oldName );
}

void SceneNode::SetGivenName(const tstring& newName)
{
  Content::SceneNode* node = GetPackage<Content::SceneNode>();

  tstring oldName = node->GetName();

  // we are setting the given name so mark it so
  // in order to get the m_GivenName used in SetName
  node->m_UseGivenName = true; 

  // this may generate us a name if we conflict with someone else
  // however, it will still be the given name, and we will still be
  // marked m_UseGivenName = true
  m_Scene->Rename( this, newName, oldName );  

}

void SceneNode::Rename(const tstring& newName)
{
  m_Scene->Rename( this, newName );
}

void SceneNode::PopulateManifest( Asset::SceneManifest* manifest ) const
{
  // by default we reference no other assets
}

void SceneNode::Reset()
{
  m_Ancestors.clear();
  m_Descendants.clear();
}

void SceneNode::Initialize()
{
  // we start out dirty, of course
  Dirty();

  // check type for this node, inserting into appropriate runtime type
  CheckNodeType();

  // we are now initialized
  m_IsInitialized = true;
}

void SceneNode::ConnectDescendant(Editor::SceneNode* descendant)
{
  if (m_Graph == NULL)
  {
    m_Graph = descendant->GetGraph();
  }

  m_Descendants.insert( descendant );

  m_Graph->Classify( this );
  m_Graph->Classify( descendant );

  // our graph will change here during undo/redo without CreateDependency/RemoveDependency being called
  Dirty();
}

void SceneNode::DisconnectDescendant(Editor::SceneNode* descendant)
{
  m_Descendants.erase( descendant );

  m_Graph->Classify( this );
  m_Graph->Classify( descendant );

  // our graph will change here during undo/redo without CreateDependency/RemoveDependency being called
  Dirty();
}

void SceneNode::ConnectAncestor( Editor::SceneNode* ancestor )
{
  if (m_Graph == NULL)
  {
    m_Graph = ancestor->GetGraph();
  }

  m_Ancestors.insert( ancestor );

  m_Graph->Classify( this );
  m_Graph->Classify( ancestor );
}

void SceneNode::DisconnectAncestor( Editor::SceneNode* ancestor )
{
  m_Ancestors.erase(ancestor);

  m_Graph->Classify( this );
  m_Graph->Classify( ancestor );
}

void SceneNode::CreateDependency(Editor::SceneNode* ancestor)
{
  ancestor->ConnectDescendant( this );

  ConnectAncestor( ancestor );

  if (m_Graph == NULL)
  {
    m_Graph = ancestor->GetGraph();
  }
}

void SceneNode::RemoveDependency(Editor::SceneNode* ancestor)
{
  ancestor->DisconnectDescendant( this );

  DisconnectAncestor( ancestor );
}

void SceneNode::Insert(SceneGraph* graph, V_SceneNodeDumbPtr& insertedNodes )
{
  graph->AddNode(this);

  if (m_Ancestors.empty() && m_Descendants.empty())
  {
    return;
  }

  u32 id = m_Graph->AssignVisitedID();

  std::stack<Editor::SceneNode*> stack;

  // reconnect this to all ancestors' descendant lists
  for ( S_SceneNodeDumbPtr::const_iterator itr = m_Ancestors.begin(), end = m_Ancestors.end(); itr != end; ++itr )
  {
    SceneNode* ancestor = *itr;
    ancestor->ConnectDescendant(this);
  }

  // prime the stack with pruned node's children
  for ( S_SceneNodeSmartPtr::const_iterator itr = m_Descendants.begin(), end = m_Descendants.end(); itr != end; ++itr )
  {
    SceneNode* descendant = *itr;
    stack.push(descendant);
    insertedNodes.push_back( descendant );
  }

  // nodes that are visitedDown are members of the pruned branch,
  //  so their ancestor's descendants never got removed
  SetVisitedID(id);

  while (!stack.empty())
  {
    Editor::SceneNode* n = stack.top();

    // re-add it back to the graph
    m_Graph->AddNode(n);

    stack.pop();

    // remove decendant reference to n from unvisited parents
    for ( S_SceneNodeDumbPtr::const_iterator itr = n->m_Ancestors.begin(), end = n->m_Ancestors.end(); itr != end; ++itr )
    {
      SceneNode* ancestor = *itr;

      // don't duplicate connections
      if (ancestor->GetVisitedID() != id)
      {
        ancestor->ConnectDescendant(n);
      }
    }

    // push each child onto the stack
    for ( S_SceneNodeSmartPtr::const_iterator itr = n->m_Descendants.begin(), end = n->m_Descendants.end(); itr != end; ++itr )
    {
      SceneNode* descendant = *itr;
      stack.push(descendant);
      insertedNodes.push_back( descendant );
    }

    n->SetVisitedID(id);
  }
}

void SceneNode::Prune( V_SceneNodeDumbPtr& prunedNodes )
{
  u32 id = m_Graph->AssignVisitedID();

  if (!m_Ancestors.empty() || !m_Descendants.empty())
  {
    std::stack<Editor::SceneNode*> stack;

    // prune this from all ancestors' descendants list
    for ( S_SceneNodeDumbPtr::const_iterator itr = m_Ancestors.begin(), end = m_Ancestors.end(); itr != end; ++itr )
    {
      SceneNode* ancestor = *itr;
      HELIUM_ASSERT( ancestor->GetGraph() );
      ancestor->DisconnectDescendant(this);
    }

    // prime the stack with pruned node's children
    for ( S_SceneNodeSmartPtr::const_iterator itr = m_Descendants.begin(), end = m_Descendants.end(); itr != end; ++itr )
    {
      SceneNode* descendant = *itr;
      stack.push( descendant );
      prunedNodes.push_back( descendant );
    }

    // nodes that are visitedDown are members of the pruned branch,
    //  so preserve their ancestor's descendants
    SetVisitedID(id);

    while (!stack.empty())
    {
      Editor::SceneNode* n = stack.top();
      stack.pop();

      // remove decendant reference to n from unvisited ancestors
      for ( S_SceneNodeDumbPtr::const_iterator itr = n->m_Ancestors.begin(), end = n->m_Ancestors.end(); itr != end; ++itr )
      {
        Editor::SceneNode* ancestor = *itr;

        // preserve pruned branch connections
        if (ancestor->GetVisitedID() != id)
        {
          ancestor->DisconnectDescendant(n);
        }
      }

      // push each child onto the stack
      for ( S_SceneNodeSmartPtr::const_iterator itr = n->m_Descendants.begin(), end = n->m_Descendants.end(); itr != end; ++itr )
      {
        Editor::SceneNode* decendant = *itr;
        stack.push(decendant);
        prunedNodes.push_back( decendant );
      }

      n->SetVisitedID(id);

      // fully remove the child descendants from the graph, too
      m_Graph->RemoveNode(n);
    }
  }

  m_Graph->RemoveNode(this);
}

void SceneNode::DoEvaluate(GraphDirection direction)
{
  LUNA_SCENE_EVALUATE_SCOPE_TIMER( ("Evaluate %s", GetClass()->m_ShortName.c_str()) );

  m_NodeStates[direction] = NodeStates::Evaluating;

  Evaluate(direction);

  m_NodeStates[direction] = NodeStates::Clean;
}

u32 SceneNode::Dirty()
{
  u32 count = 0;

  if ( m_Graph )
  {
    // by default, normal dependency nodes only dirty downstream
    count += m_Graph->DirtyNode( this, GraphDirections::Downstream );
  }

  return count;
}

void SceneNode::Evaluate(GraphDirection direction)
{

}

i32 SceneNode::GetImageIndex() const
{
  return Helium::GlobalFileIconsTable().GetIconID( TXT( "null" ) );
}

tstring SceneNode::GetApplicationTypeName() const
{
  return GetClass()->m_UIName;
}

SceneNodeTypePtr SceneNode::CreateNodeType( Editor::Scene* scene ) const
{
  SceneNodeTypePtr nodeType = new Editor::SceneNodeType( scene, GetType() );

  nodeType->SetImageIndex( GetImageIndex() );
  
  return nodeType;
}

void SceneNode::ChangeNodeType( Editor::SceneNodeType* type )
{
  if (m_NodeType != type)
  {
    if (m_NodeType != NULL)
    {
      m_NodeType->RemoveInstance(this);
    }

    m_NodeType = type;

    type->AddInstance(this);
  }
}

Editor::SceneNodeType* SceneNode::DeduceNodeType()
{
  SceneNodeTypePtr nodeType;

  // this string will be the encoded type information for this node
  const tstring name = GetApplicationTypeName();

  // attempt to find a "natural" simple type for this object in the scene (matches compile-time type)
  const HM_StrToSceneNodeTypeSmartPtr& nodeTypes = m_Scene->GetNodeTypesByName();
  HM_StrToSceneNodeTypeSmartPtr::const_iterator found = nodeTypes.find( name );

  // did we find it?
  if ( found != nodeTypes.end() )
  {
    // this is our type
    nodeType = found->second;
  }

  // second attempt failed, this must be the first object of a new type, ask the object to create a type for it
  if (!nodeType)
  {
    // create it
    nodeType = CreateNodeType( m_Scene );

    // set its name
    nodeType->SetName( name );

    // add it to the scene
    m_Scene->AddNodeType( nodeType );
  }

  return nodeType;
}

void SceneNode::CheckNodeType()
{
  ChangeNodeType( DeduceNodeType() );
}

void SceneNode::Create()
{

}

void SceneNode::Delete()
{

}

void SceneNode::Execute(bool interactively)
{
  // make me dirty
  Dirty();

  // update and render
  m_Scene->Execute(interactively);
}

bool SceneNode::ValidatePanel(const tstring& name)
{
  if (name == TXT( "Scene Node" ) )
  {
    return true;
  }

  return false;
}

void SceneNode::CreatePanel(CreatePanelArgs& args)
{
  SceneNodePanel* panel = new SceneNodePanel ( args.m_Generator, args.m_Selection );

  args.m_Generator->Push( panel );
  {
    panel->SetCanvas( args.m_Generator->GetContainer()->GetCanvas() );
    panel->Create();
  }
  args.m_Generator->Pop();
}

tstring SceneNode::GetMembership() const
{
  std::set< tstring > layerNames;
  S_SceneNodeDumbPtr::const_iterator itr = m_Ancestors.begin();
  S_SceneNodeDumbPtr::const_iterator end = m_Ancestors.end();
  for ( ; itr != end; ++itr )
  {
    Editor::SceneNode* node = *itr;
    if ( node->HasType( Reflect::GetType<Editor::Layer>() ) )
    {
      Editor::Layer* layer = Reflect::DangerousCast< Editor::Layer >( node );
      if ( layer->GetScene()->GetNodes().find( layer->GetID() ) != layer->GetScene()->GetNodes().end() )
      {
        layerNames.insert( node->GetName() );
      }
    }
  }

  tstring layers;
  std::set< tstring >::const_iterator layerItr = layerNames.begin();
  std::set< tstring >::const_iterator layerEnd = layerNames.end();
  for ( ; layerItr != layerEnd; ++layerItr )
  {
    if ( !layers.empty() )
    {
      layers += Reflect::s_ContainerItemDelimiter;
    }
    layers += *layerItr;
  }

  return layers;
}

void SceneNode::SetMembership( const tstring& layers )
{
  // This function is required to generate the UI that lists the layer membership.
  // It doesn't do anything and you shouldn't be calling it.
  HELIUM_BREAK();
}
