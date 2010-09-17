/*#include "Precompile.h"*/
#include "Core/Scene/SceneNode.h"

#include "Core/Scene/SceneNodeType.h"
#include "Core/Scene/SceneNodePanel.h"
#include "Core/Scene/SceneGraph.h"
#include "Core/Scene/Scene.h"
#include "Core/Scene/Layer.h"
#include "Core/Scene/Transform.h"
#include "Core/Scene/Statistics.h"

using namespace Helium;
using namespace Helium::Core;

REFLECT_DEFINE_ABSTRACT( SceneNode );

void SceneNode::EnumerateClass( Reflect::Compositor<SceneNode>& comp )
{
    comp.AddField( &SceneNode::m_ID,            "m_ID",             Reflect::FieldFlags::ReadOnly );
    comp.AddField( &SceneNode::m_DefaultName,   "m_DefaultName",    Reflect::FieldFlags::Hide );
    comp.AddField( &SceneNode::m_GivenName,     "m_GivenName",      Reflect::FieldFlags::Hide );
    comp.AddField( &SceneNode::m_UseGivenName,  "m_UseGivenName",   Reflect::FieldFlags::Hide );
}

void SceneNode::InitializeType()
{
    Reflect::RegisterClassType< Core::SceneNode >();

    PropertiesGenerator::InitializePanel( TXT( "Scene Node" ), CreatePanelSignature::Delegate( &SceneNode::CreatePanel ));
}

void SceneNode::CleanupType()
{
    Reflect::UnregisterClassType< Core::SceneNode >();
}

SceneNode::SceneNode()
: m_ID ( TUID::Generate() )
, m_UseGivenName( false )
, m_IsInitialized( false )
, m_IsSelected( false )
, m_IsTransient( false )
, m_Owner( NULL )
, m_Graph( NULL )
, m_NodeType( NULL )
, m_VisitedID( 0 )
{
    m_NodeStates[ GraphDirections::Downstream ] = NodeStates::Dirty;
    m_NodeStates[ GraphDirections::Upstream ] = NodeStates::Dirty;
}

SceneNode::~SceneNode()
{

}

const TUID& SceneNode::GetID() const
{
    return m_ID;
}

void SceneNode::SetID( const TUID& id )
{
    m_ID = id;
}

tstring SceneNode::GenerateName() const
{
    tstring name = GetClass()->m_UIName;
    name[0] = tolower( name[0] );
    name += TXT( "1" );
    return name;
}

const tstring& SceneNode::GetName() const
{
    return (!m_GivenName.empty() && m_UseGivenName) ? m_GivenName : m_DefaultName;
}

void SceneNode::SetName(const tstring& value)
{
    m_NameChanging.Raise( SceneNodeChangeArgs( this ) );

    if ( m_UseGivenName )
    {
        m_GivenName = value;
    }
    else
    {
        m_DefaultName = value;
    }

    m_NameChanged.Raise( SceneNodeChangeArgs( this ) );
}

bool SceneNode::UseGivenName() const
{
    return m_UseGivenName;
}

void SceneNode::SetUseGivenName(bool use)
{
    tstring oldName = GetName();

    m_UseGivenName = use;

    m_Owner->Rename( this, use ? GetName() : GenerateName(), oldName );
}

void SceneNode::SetGivenName(const tstring& newName)
{
    tstring oldName = GetName();

    // we are setting the given name so mark it so
    // in order to get the m_GivenName used in SetName
    m_UseGivenName = true; 

    // this may generate us a name if we conflict with someone else
    // however, it will still be the given name, and we will still be
    // marked m_UseGivenName = true
    m_Owner->Rename( this, newName, oldName );  

}

void SceneNode::Rename(const tstring& newName)
{
    m_Owner->Rename( this, newName );
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

void SceneNode::ConnectDescendant(Core::SceneNode* descendant)
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

void SceneNode::DisconnectDescendant(Core::SceneNode* descendant)
{
    m_Descendants.erase( descendant );

    m_Graph->Classify( this );
    m_Graph->Classify( descendant );

    // our graph will change here during undo/redo without CreateDependency/RemoveDependency being called
    Dirty();
}

void SceneNode::ConnectAncestor( Core::SceneNode* ancestor )
{
    if (m_Graph == NULL)
    {
        m_Graph = ancestor->GetGraph();
    }

    m_Ancestors.insert( ancestor );

    m_Graph->Classify( this );
    m_Graph->Classify( ancestor );
}

void SceneNode::DisconnectAncestor( Core::SceneNode* ancestor )
{
    m_Ancestors.erase(ancestor);

    m_Graph->Classify( this );
    m_Graph->Classify( ancestor );
}

void SceneNode::CreateDependency(Core::SceneNode* ancestor)
{
    ancestor->ConnectDescendant( this );

    ConnectAncestor( ancestor );

    if (m_Graph == NULL)
    {
        m_Graph = ancestor->GetGraph();
    }
}

void SceneNode::RemoveDependency(Core::SceneNode* ancestor)
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

    std::stack<Core::SceneNode*> stack;

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
        Core::SceneNode* n = stack.top();

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
        std::stack<Core::SceneNode*> stack;

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
            Core::SceneNode* n = stack.top();
            stack.pop();

            // remove decendant reference to n from unvisited ancestors
            for ( S_SceneNodeDumbPtr::const_iterator itr = n->m_Ancestors.begin(), end = n->m_Ancestors.end(); itr != end; ++itr )
            {
                Core::SceneNode* ancestor = *itr;

                // preserve pruned branch connections
                if (ancestor->GetVisitedID() != id)
                {
                    ancestor->DisconnectDescendant(n);
                }
            }

            // push each child onto the stack
            for ( S_SceneNodeSmartPtr::const_iterator itr = n->m_Descendants.begin(), end = n->m_Descendants.end(); itr != end; ++itr )
            {
                Core::SceneNode* decendant = *itr;
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
    CORE_EVALUATE_SCOPE_TIMER( ("Evaluate %s", GetClass()->m_ShortName.c_str()) );

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
    return -1; // Helium::GlobalFileIconsTable().GetIconID( TXT( "null" ) );
}

tstring SceneNode::GetApplicationTypeName() const
{
    return GetClass()->m_UIName;
}

SceneNodeTypePtr SceneNode::CreateNodeType( Core::Scene* scene ) const
{
    SceneNodeTypePtr nodeType = new Core::SceneNodeType( scene, GetType() );

    nodeType->SetImageIndex( GetImageIndex() );

    return nodeType;
}

void SceneNode::ChangeNodeType( Core::SceneNodeType* type )
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

Core::SceneNodeType* SceneNode::DeduceNodeType()
{
    SceneNodeTypePtr nodeType;

    // this string will be the encoded type information for this node
    const tstring name = GetApplicationTypeName();

    // attempt to find a "natural" simple type for this object in the scene (matches compile-time type)
    const HM_StrToSceneNodeTypeSmartPtr& nodeTypes = m_Owner->GetNodeTypesByName();
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
        nodeType = CreateNodeType( m_Owner );

        // set its name
        nodeType->SetName( name );

        // add it to the scene
        m_Owner->AddNodeType( nodeType );
    }

    return nodeType;
}

void SceneNode::CheckNodeType()
{
    ChangeNodeType( DeduceNodeType() );
}

void SceneNode::PopulateManifest( Asset::SceneManifest* manifest ) const
{
    // by default we reference no other assets
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
    m_Owner->Execute(interactively);
}

#pragma TODO("Remove this constness")

void SceneNode::GetState( Reflect::ElementPtr& state ) const
{
    const_cast<SceneNode*>(this)->Pack();

    state = const_cast<SceneNode*>(this)->Clone();
}

void SceneNode::SetState( const Reflect::ElementPtr& state )
{
    if ( !state->Equals( this ) )
    {
        state->CopyTo( this );
        Unpack();
        RaiseChanged();
    }
}

Undo::CommandPtr SceneNode::SnapShot( Reflect::Element* newState )
{
    if ( newState == NULL )
    {
        return new Undo::PropertyCommand<Reflect::ElementPtr>( new Helium::MemberProperty<SceneNode, Reflect::ElementPtr> (this, &SceneNode::GetState, &SceneNode::SetState) );
    }

    return new Undo::PropertyCommand<Reflect::ElementPtr>( new Helium::MemberProperty<SceneNode, Reflect::ElementPtr> (this, &SceneNode::GetState, &SceneNode::SetState), Reflect::ElementPtr( newState ) );
}

bool SceneNode::IsSelectable() const
{
    return true;
}

bool SceneNode::IsSelected() const
{
    return m_IsSelected;
}

void SceneNode::SetSelected(bool selected)
{
    m_IsSelected = selected;
}

void SceneNode::ConnectProperties(EnumerateElementArgs& args)
{

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
        Core::SceneNode* node = *itr;
        if ( node->HasType( Reflect::GetType<Core::Layer>() ) )
        {
            Core::Layer* layer = Reflect::DangerousCast< Core::Layer >( node );
            if ( layer->GetOwner()->GetNodes().find( layer->GetID() ) != layer->GetOwner()->GetNodes().end() )
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
