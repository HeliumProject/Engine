#include "SceneGraphPch.h"
#include "Layer.h"

#include "Foundation/Natural.h"
#include "Foundation/Log.h"

#include "SceneGraph/Scene.h"

#include "Reflect/TranslatorDeduction.h"

HELIUM_DEFINE_CLASS( Helium::SceneGraph::Layer );

using namespace Helium;
using namespace Helium::SceneGraph;

void Layer::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &Layer::m_Visible,     TXT( "m_Visible" ) );
	comp.AddField( &Layer::m_Selectable,  TXT( "m_Selectable" ) );
	comp.AddField( &Layer::m_Members,     TXT( "m_Members" ) );
	comp.AddField( &Layer::m_Color,       TXT( "m_Color" ) );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Layer::Layer()
	: m_Visible( true )
	, m_Selectable( true )
	, m_Color( 255 )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
Layer::~Layer()
{
}

///////////////////////////////////////////////////////////////////////////////
// Called after deserialization of the entire scene.  Builds the layer's 
// dependency graph (representing what objects are members of the layer) from
// the list of ids stored on the persistent data.
// 
void Layer::Initialize()
{
	Base::Initialize();

	m_Descendants.clear();

	// this we be the list of valid members, which will be trimmed to not include missing objects after the following loop
	std::set<TUID> memberIDs;

	for ( std::set<TUID>::const_iterator itr = m_Members.begin(), end = m_Members.end(); itr != end; ++itr )
	{
		SceneGraph::SceneNode* node = m_Owner->FindNode( *itr );
		if ( node )
		{
			node->CreateDependency( this );

			// The ID might not be the same as what we requested (like
			// in the case of copy/paste), so save off all the IDs
			// and update the persistent data when we are done.
			memberIDs.insert( node->GetID() );
		}
		else
		{
			std::string idStr;
			(*itr).ToString( idStr );
			Log::Debug( TXT( "Layer %s: Unable to reconnect layer member with ID %s (it is no longer in the scene).\n" ), GetName().c_str(), idStr.c_str() );
		}
	}

	m_Members = memberIDs;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this layer is marked as being visible.
// 
bool Layer::IsVisible() const
{
	return m_Visible; 
}

///////////////////////////////////////////////////////////////////////////////
// Set whether the layer is visible or not.  Members of this layer will not
// be visible if the layer is not visible.
// 
void Layer::SetVisible( bool visible )
{
	m_Visible = visible; 
	m_VisibilityChanged.Raise( SceneNodeChangeArgs( this ) );
	Dirty();
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this layer is marked as selectable.  If a layer is selectable,
// it's members can be picked in the 3D view.
// 
bool Layer::IsSelectable() const
{
	return m_Selectable;
}

///////////////////////////////////////////////////////////////////////////////
// Set whether this member should be selectable or not.  See IsSelectable().
// 
void Layer::SetSelectable( bool selectable )
{
	m_Selectable = selectable;
	Dirty();
}

const Color3& Layer::GetColor() const
{
	return m_Color;
}

void Layer::SetColor( const Color3& color )
{
	if ( color != m_Color )
	{
		m_Color = color;
		Dirty();
	}
}

OS_SceneNodeDumbPtr Layer::GetMembers()
{
	OS_SceneNodeDumbPtr members;

	for ( S_SceneNodeSmartPtr::const_iterator itr = m_Descendants.begin(), end = m_Descendants.end(); itr != end; ++itr )
	{
		members.Append( *itr );
	}

	return members;
}

bool Layer::ContainsMember( SceneGraph::SceneNode* node ) const
{
	return m_Descendants.find( node ) != m_Descendants.end();
}

void Layer::ConnectDescendant( SceneGraph::SceneNode* descendant )
{
	Base::ConnectDescendant( descendant );

	HELIUM_ASSERT( m_Members.find( descendant->GetID() ) == m_Members.end() );
	m_Members.insert( descendant->GetID() );
}

void Layer::DisconnectDescendant( SceneGraph::SceneNode* descendant )
{
	Base::DisconnectDescendant( descendant );

	HELIUM_ASSERT( m_Members.find( descendant->GetID() ) != m_Members.end() );
	m_Members.erase( descendant->GetID() );
}

///////////////////////////////////////////////////////////////////////////////
// Inserting a layer needs to restore its members that were previously pruned.
// 
void Layer::Insert(Graph* g, V_SceneNodeDumbPtr& insertedNodes )
{
	// Let the base class put the layer back into the graph
	Base::Insert( g, insertedNodes );

	// Fetch the persistent data, dereference each object and add it back
	// as a member of the layer.
	if ( IsInitialized() )
	{
		HELIUM_ASSERT( m_Descendants.empty() );
		for ( std::set<TUID>::const_iterator itr = m_Members.begin(), end = m_Members.end(); itr != end; ++itr )
		{
			const TUID& id = *itr;
			SceneGraph::SceneNode* node = m_Owner->FindNode( id );
			if ( node )
			{
				ConnectDescendant( node );
			}
			else
			{
				std::string idStr;
				id.ToString( idStr );
				Log::Debug( TXT( "Layer %s: Unable to reconnect layer member with ID %s (it is no longer in the scene).\n" ), GetName().c_str(), idStr.c_str() );
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Pruning a layer does not prune the layer's members.
// 
void Layer::Prune( V_SceneNodeDumbPtr& prunedNodes )
{
	std::set<TUID> members = m_Members;

	while ( !m_Descendants.empty() )
	{
		DisconnectDescendant( *m_Descendants.begin() );
	}

	m_Members = members;

	Base::Prune( prunedNodes );
}
