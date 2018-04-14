#include "EditorScenePch.h"
#include "HierarchyNode.h"

#include "Reflect/Object.h"

#include "EditorScene/Colors.h"
#include "EditorScene/Layer.h"
#include "EditorScene/Transform.h"
#include "EditorScene/Scene.h"
#include "EditorScene/SceneSettings.h"
#include "EditorScene/SceneVisitor.h"

HELIUM_DEFINE_ABSTRACT( Helium::Editor::HierarchyNode );

using namespace Helium;
using namespace Helium::Editor;

void HierarchyNode::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &HierarchyNode::m_ParentID, "m_ParentID" );

	Reflect::Field* field = NULL;

	field = comp.AddField( &HierarchyNode::m_Hidden, "m_Hidden" );
	field->SetProperty( "HelpText", "This determines if the node is hidden or not." );

	field = comp.AddField( &HierarchyNode::m_Live, "m_Live" );
	field->SetProperty( "HelpText", "This determines if the node is hit tested when working with live objects only." );
}

HierarchyNode::HierarchyNode() 
	: m_ParentID( Helium::TUID::Null )
	, m_Hidden( false )
	, m_Live( false )
	, m_Parent( NULL )
	, m_Previous( NULL )
	, m_Next( NULL )
	, m_LayerColor( NULL )
	, m_Visible( true )
	, m_Selectable( true )
	, m_Highlighted( false )
	, m_Reactive( false )
{
}

HierarchyNode::~HierarchyNode()
{
}

void HierarchyNode::InitializeHierarchy()
{
	Initialize();

	for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
	{
		Editor::HierarchyNode* child = *itr;
		child->SetOwner( m_Owner );
		child->InitializeHierarchy();
	}
}

void HierarchyNode::Reset()
{
	m_Children.Clear();
}

void HierarchyNode::SetTransient( bool isTransient )
{
	if ( isTransient != IsTransient() )
	{
		Base::SetTransient( isTransient );

		OS_HierarchyNodeDumbPtr::Iterator childItr = m_Children.Begin();
		OS_HierarchyNodeDumbPtr::Iterator childEnd = m_Children.End();
		for ( ; childItr != childEnd; ++childItr )
		{
			Editor::HierarchyNode* child = *childItr;
			child->SetTransient( isTransient );
		}
	}
}

bool HierarchyNode::IsHidden() const
{
	return m_Hidden; 
}

void HierarchyNode::SetHidden(bool hidden)
{
	m_Hidden = hidden;
	Dirty();
}

bool HierarchyNode::IsLive() const
{
	return m_Live;
}

void HierarchyNode::SetLive(bool isLive)
{
	m_Live = isLive;
}

bool HierarchyNode::IsVisible() const
{
	bool isVisible = m_Visible;

	if ( m_Owner->GetRoot().Ptr() == this )
	{
		// The root object is never visible
		isVisible = false;
	}

	return isVisible;
}

bool HierarchyNode::IsSelectable() const
{
	bool isSelectable = m_Selectable;

	if ( m_Owner->GetRoot().Ptr() == this )
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
		Base::SetSelected( value );

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
		Editor::HierarchyNode* child = *childItr;
		child->SetReactive( value );
	}
}

void HierarchyNode::SetName( const std::string& value )
{
	Base::SetName( value );

	// reset path b/c our name changed
	m_Path = "";
}

const std::string& HierarchyNode::GetPath()
{
	if (m_Path == "")
	{
		m_Path = "|" + GetName();
		const Editor::HierarchyNode* p = m_Parent;
		while ( p != NULL && p->GetParent() != NULL )
		{
			m_Path = m_Path.insert( 0, "|" + p->GetName() );
			p = p->GetParent();
		}
	}

	return m_Path;
}

Editor::HierarchyNode* HierarchyNode::GetParent() const
{
	return m_Parent;
}

void HierarchyNode::SetParent( Editor::HierarchyNode* value )
{
	if ( value != m_Parent )
	{
		Editor::HierarchyNode* oldParent = m_Parent;

		ParentChangingArgs args ( this, value );
		m_ParentChanging.Raise( args );
		if( !args.m_Veto )
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
				m_ParentID = 0x0;
			}

			m_Parent = value;
			m_ParentID = value->GetID();

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

void HierarchyNode::SetPrevious( Editor::HierarchyNode* value )
{
	m_Previous = value;
}

void HierarchyNode::SetNext( Editor::HierarchyNode* value )
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
	Editor::HierarchyNode* previous = NULL;
	V_HierarchyNodeSmartPtr::reverse_iterator rItr = children.rbegin();
	V_HierarchyNodeSmartPtr::reverse_iterator rEnd = children.rend();
	for ( ; rItr != rEnd; ++rItr )
	{
		Editor::HierarchyNode* current = *rItr;
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
	// clone the persistent data into a new instance of content data
	HierarchyNodePtr duplicate = Reflect::AssertCast< HierarchyNode > ( Clone() );

	// generate new unique ID 
	TUID::Generate( duplicate->m_ID ); 

	// the duplicate must be a part of the dependency graph to hold the parent/child relationship
	m_Graph->AddNode( duplicate );

	// copy ancestral dependency connections
	for ( S_SceneNodeDumbPtr::const_iterator itr = GetAncestors().begin(), end = GetAncestors().end(); itr != end; ++itr )
	{
		if ( (*itr)->IsA( Reflect::GetMetaClass<Editor::HierarchyNode>() ) )
		{
			continue;
		}

		duplicate->CreateDependency( (*itr) );
	}

	// copy descendant dependency connections
	for ( S_SceneNodeSmartPtr::const_iterator itr = GetDescendants().begin(), end = GetDescendants().end(); itr != end; ++itr )
	{
		if ( (*itr)->IsA( Reflect::GetMetaClass<Editor::HierarchyNode>() ) )
		{
			continue;
		}

		(*itr)->CreateDependency( duplicate );
	}

	// recurse on each child
	for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
	{
		Editor::HierarchyNode* child = *itr;

		// duplicate recursively
		HierarchyNodePtr duplicateChild = child->Duplicate();

		// connect to the duplicated parent
		duplicateChild->SetParent( duplicate );
	}

	return duplicate;
}

AlignedBox HierarchyNode::GetGlobalBounds() const
{
	const Editor::Transform* transform = GetTransform();

	AlignedBox bounds;

	// reset to local HierarchyBounds
	bounds.Merge( m_ObjectBounds );

	if (transform)
	{
		// transform those as sample points into global space
		bounds.Transform( transform->GetGlobalTransform() );
	}

	return bounds;
}

AlignedBox HierarchyNode::GetGlobalHierarchyBounds() const
{
	const Editor::Transform* transform = GetTransform();

	AlignedBox bounds;

	// reset to local HierarchyBounds
	bounds.Merge( m_ObjectHierarchyBounds );

	if (transform)
	{
		// transform those as sample points into global space
		bounds.Transform( transform->GetGlobalTransform() );
	}

	return bounds;
}

void HierarchyNode::AddChild(Editor::HierarchyNode* c)
{
	c->CreateDependency( this );
}

void HierarchyNode::RemoveChild(Editor::HierarchyNode* c)
{
	c->RemoveDependency( this );
}

void HierarchyNode::DisconnectDescendant(Editor::SceneNode* descendant) 
{
	Base::DisconnectDescendant(descendant);

	Editor::HierarchyNode* child = Reflect::SafeCast< Editor::HierarchyNode >( descendant );
	if ( child )
	{
		// sanity check that the parent is really set to this
		HELIUM_ASSERT( child->GetParent() == this );

		// we should not be disconnecting descendant hierarchy nodes that are not our children
		HELIUM_ASSERT( m_Children.Contains(child) );
		Log::Debug( "Removing %s from %s's child list (previous=%s next=%s)\n", child->GetName().c_str(), GetName().c_str(), child->m_Previous ? child->m_Previous->GetName().c_str() : "NULL", child->m_Next ? child->m_Next->GetName().c_str() : "NULL" );

		// fix up linked list
		if ( child->m_Previous )
		{
			Log::Debug( "Setting %s's m_Next to %s\n", child->m_Previous->m_Next ? child->m_Previous->m_Next->GetName().c_str() : "NULL", child->m_Next ? child->m_Next->GetName().c_str() : "NULL" );
			HELIUM_ASSERT( m_Children.Contains( child->m_Previous ) );
			child->m_Previous->m_Next = child->m_Next;
		}

		if ( child->m_Next )
		{
			Log::Debug( "Setting %s's m_Previous to %s\n", child->m_Next->m_Previous ? child->m_Next->m_Previous->GetName().c_str() : "NULL", child->m_Previous ? child->m_Previous->GetName().c_str() : "NULL" );
			HELIUM_ASSERT( m_Children.Contains( child->m_Next ) );
			child->m_Next->m_Previous = child->m_Previous;
		}

		// do erase
		m_Children.Remove( child );
	}
}

void HierarchyNode::ConnectDescendant(Editor::SceneNode* descendant)
{
	Base::ConnectDescendant(descendant);

	Editor::HierarchyNode* child = Reflect::SafeCast< Editor::HierarchyNode >( descendant );
	if ( child )
	{
		// sanity check that the parent is really set to this
		HELIUM_ASSERT( child->GetParent() == this );

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
				Editor::HierarchyNode* back = m_Children.Back();

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

void HierarchyNode::ConnectAncestor( Editor::SceneNode* ancestor )
{
	Base::ConnectAncestor( ancestor );

	if ( ancestor->IsA( Reflect::GetMetaClass< Editor::Layer >() ) )
	{
		m_LayerColor = Reflect::SafeCast< Editor::Layer >( ancestor );
	}
}

void HierarchyNode::DisconnectAncestor( Editor::SceneNode* ancestor )
{
	Base::DisconnectAncestor( ancestor );

	if ( ancestor == m_LayerColor )
	{
		m_LayerColor = NULL;
		S_SceneNodeDumbPtr::const_iterator ancestorItr = m_Ancestors.begin();
		S_SceneNodeDumbPtr::const_iterator ancestorEnd = m_Ancestors.end();
		for ( ; ancestorItr != ancestorEnd; ++ancestorItr )
		{
			Editor::SceneNode* dependNode = (*ancestorItr);

			Editor::Layer* layer = Reflect::SafeCast< Editor::Layer >( dependNode );
			if ( layer )
			{
				m_LayerColor = layer;
				break;
			}
		}
	}
}

Editor::Transform* HierarchyNode::GetTransform()
{
	HierarchyNodePtr node = this;

	while (node.ReferencesObject())
	{
		Editor::Transform* transform = Reflect::SafeCast< Editor::Transform >( node );

		if (transform != NULL)
		{
			return transform;
		}

		node = node->GetParent();
	}

	return NULL;
}

const Editor::Transform* HierarchyNode::GetTransform() const
{
	return const_cast<Editor::HierarchyNode*>(this)->GetTransform();
}

void HierarchyNode::Create()
{
	Base::Create();

	for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
	{
		Editor::HierarchyNode* child = *itr;
		child->Create();
	}
}

void HierarchyNode::Delete()
{
	Base::Delete();

	for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
	{
		Editor::HierarchyNode* child = *itr;
		child->Delete();
	}
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if all the layers that this node is a member of are currently
// visible.
// 
bool HierarchyNode::ComputeVisibility() const
{
	Editor::HierarchyNode* parent = GetParent();

	bool isVisible = !IsHidden();

	isVisible &= (parent && m_Owner && parent != m_Owner->GetRoot()) ? parent->IsVisible() : true;

	S_SceneNodeDumbPtr::const_iterator ancestorItr = m_Ancestors.begin();
	S_SceneNodeDumbPtr::const_iterator ancestorEnd = m_Ancestors.end();
	for ( ; ancestorItr != ancestorEnd && isVisible; ++ancestorItr )
	{
		Editor::SceneNode* dependNode = (*ancestorItr);

		Editor::Layer* layer = Reflect::SafeCast< Editor::Layer >( dependNode );
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
	bool isSelectable = true;

	S_SceneNodeDumbPtr::const_iterator ancestorItr = m_Ancestors.begin();
	S_SceneNodeDumbPtr::const_iterator ancestorEnd = m_Ancestors.end();
	for ( ; ancestorItr != ancestorEnd && isSelectable; ++ancestorItr )
	{
		Editor::SceneNode* dependNode = (*ancestorItr);

		Editor::Layer* layer = Reflect::SafeCast< Editor::Layer >( dependNode );
		if ( layer )
		{
			isSelectable &= layer->IsSelectable();
		}
	}

	return isSelectable;
}

uint32_t HierarchyNode::Dirty()
{
	uint32_t count = 0;

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
	Editor::Transform* transform = GetTransform();

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
				Editor::HierarchyNode* child = *itr;

				// get the hierarchical bounds for that child
				AlignedBox bounds = child->GetObjectHierarchyBounds();

				// get the child's transform
				Editor::Transform* childTransform = child->GetTransform();

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

	Base::Evaluate(direction);
}

bool HierarchyNode::BoundsCheck(const Matrix4& instanceMatrix) const
{
	Editor::Camera* camera = m_Owner->GetViewport()->GetCamera();

	AlignedBox bounds (m_ObjectHierarchyBounds);

	bounds.Transform( instanceMatrix );

	if (camera->IsViewFrustumCulling() && !camera->GetViewFrustum().IntersectsBox(bounds))
	{
		return false;
	}

	return true;
}

Helium::Color HierarchyNode::GetMaterialColor( Helium::Color defaultMaterial ) const
{
	Editor::Viewport* view = m_Owner->GetViewport();

	Helium::Color material = defaultMaterial;

	switch ( view->GetSettingsManager()->GetSettings< ViewportSettings >()->GetColorMode() )
	{
	case ViewColorMode::Layer:
		if ( m_LayerColor )
		{
			material = m_LayerColor->GetColor();
		}
		break;

	case ViewColorMode::Scene:
		{
			material = m_Owner->GetColor();
		}
		break;

	case ViewColorMode::Type:
		break;
	}

	if ( m_Owner->IsFocused() )
	{
		if ( IsSelectable() )
		{
			if ( IsHighlighted() && view->IsHighlighting() )
			{
				material = Editor::Viewport::s_HighlightedMaterial;
			}
			else if ( IsSelected() )
			{
				material = Editor::Viewport::s_SelectedMaterial;
			}
			else if ( IsReactive() )
			{
				material = Editor::Viewport::s_ReactiveMaterial;
			}
			else if ( IsLive() )
			{
				material = Editor::Viewport::s_LiveMaterial;
			}

			material.SetA( defaultMaterial.GetA() );;
		}
		else
		{
			material = Editor::Viewport::s_UnselectableMaterial;
		}
	}
	else
	{
		material.SetA( defaultMaterial.GetA() );
	}

	return material;
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
				Editor::HierarchyNode* child = *itr;

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

	case TraversalActions::Abort:
		break;
	}

	return TraversalActions::Abort;
}

void HierarchyNode::Render( RenderVisitor* render )
{
	Editor::Transform* transform = GetTransform();

#ifdef VIEWPORT_REFACTOR
	if ( transform && IsSelected() && m_Owner->IsFocused() && render->GetViewport()->IsBoundsVisible() )
	{
		V_Vector3 vertices;
		V_Vector3 lineList;

		D3DMATERIAL9 material;
		ZeroMemory(&material, sizeof(material));

		m_Owner->GetViewport()->GetDevice()->SetFVF( ElementFormats[ ElementTypes::Position ] );


		//
		// Local draw
		//

		{
			Matrix4 matrix = render->State().m_Matrix;

			m_Owner->GetViewport()->GetDevice()->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&(matrix) );

			vertices.clear();
			m_ObjectBounds.GetVertices(vertices);
			AlignedBox::GetWireframe( vertices, lineList );
			material.Ambient.r = 255;
			material.Ambient.g = 0;
			material.Ambient.b = 0;
			material.Ambient.a = 1;
			m_Owner->GetViewport()->GetDevice()->SetMaterial(&material);
			m_Owner->GetViewport()->GetDevice()->DrawPrimitiveUP( D3DPT_LINELIST, (UINT)lineList.size() / 2, &lineList.front(), sizeof(Vector3));
		}


		//
		// Global draw
		//

		{
			Matrix4 matrix = render->ParentState().m_Matrix;

			m_Owner->GetViewport()->GetDevice()->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&(matrix) );

			vertices.clear();
			GetGlobalBounds().GetVertices(vertices);
			AlignedBox::GetWireframe( vertices, lineList );
			material.Ambient.r = 255;
			material.Ambient.g = 128;
			material.Ambient.b = 128;
			material.Ambient.a = 1;
			m_Owner->GetViewport()->GetDevice()->SetMaterial(&material);
			m_Owner->GetViewport()->GetDevice()->DrawPrimitiveUP( D3DPT_LINELIST, (UINT)lineList.size() / 2, &lineList.front(), sizeof(Vector3));
		}


		//
		// Local Hierarchy draw
		//

		{
			Matrix4 matrix = render->State().m_Matrix;

			m_Owner->GetViewport()->GetDevice()->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&(matrix) );

			vertices.clear();
			m_ObjectHierarchyBounds.GetVertices(vertices);
			AlignedBox::GetWireframe( vertices, lineList );
			material.Ambient.r = 0;
			material.Ambient.g = 0;
			material.Ambient.b = 255;
			material.Ambient.a = 1;
			m_Owner->GetViewport()->GetDevice()->SetMaterial(&material);
			m_Owner->GetViewport()->GetDevice()->DrawPrimitiveUP( D3DPT_LINELIST, (UINT)lineList.size() / 2, &lineList.front(), sizeof(Vector3));
		}


		//
		// Global Hierarchy draw
		//

		{
			Matrix4 matrix = render->ParentState().m_Matrix;

			m_Owner->GetViewport()->GetDevice()->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&(matrix) );

			vertices.clear();
			GetGlobalHierarchyBounds().GetVertices(vertices);
			AlignedBox::GetWireframe( vertices, lineList );
			material.Ambient.r = 128;
			material.Ambient.g = 128;
			material.Ambient.b = 255;
			material.Ambient.a = 1;
			m_Owner->GetViewport()->GetDevice()->SetMaterial(&material);
			m_Owner->GetViewport()->GetDevice()->DrawPrimitiveUP( D3DPT_LINELIST, (UINT)lineList.size() / 2, &lineList.front(), sizeof(Vector3));
		}

		m_Owner->GetViewport()->GetResources()->ResetState();
	}
#endif
}

bool HierarchyNode::Pick(PickVisitor* pick)
{
	return false;
}

Editor::HierarchyNode* HierarchyNode::Find( const std::string& targetName )
{
	if ( targetName.empty() )
		return NULL;

	if ( CaseInsensitiveCompareString( GetName().c_str(), targetName.c_str() ) == 0 )
	{
		return this;
	}

	Editor::HierarchyNode* found = NULL;

	for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
	{
		Editor::HierarchyNode* child = *itr;

		const std::string& currentName = child->GetName();

		// Case-insensitive comparison to see if the name matches the target
		if ( !currentName.empty() && ( CaseInsensitiveCompareString( currentName.c_str(), targetName.c_str() ) == 0 ) )
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

Editor::HierarchyNode* HierarchyNode::FindFromPath( std::string path )
{
	if ( path.empty() )
		return NULL;

	if ( GetPath() == path )
	{
		return this;
	}

	Editor::HierarchyNode* found = NULL;

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
		Editor::HierarchyNode* child = *itr;

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

}

bool HierarchyNode::IsSimilar(const HierarchyNodePtr& node) const
{
	return false;
}

std::string HierarchyNode::GetDescription() const
{
	return std::string ();
}

void HierarchyNode::ConnectManipulator(ManiuplatorAdapterCollection* collection)
{
	Editor::Transform* transform = GetTransform();

	if (transform != m_Owner->GetRoot())
	{
		// for non-transform types, just forward on the call
		transform->ConnectManipulator(collection);
	}
}
