#include "Precompile.h"
#include "EditorScene/Selection.h"

#include "Foundation/Log.h"

#include <algorithm>

using namespace Helium;
using namespace Helium::Editor;

Selection::Selection()
{

}

void Selection::Refresh()
{
	HELIUM_EDITOR_SCENE_SCOPE_TIMER( "" );

	SimpleTimer timer;

	m_SelectionChanging.Raise(m_Items);

	// do nothing

	m_SelectionChanged.Raise(m_Items);

	Log::Profile( "Selection Refresh took %fms\n", timer.Elapsed());
}

const OS_ObjectDumbPtr& Selection::GetItems() const
{
	return m_Items;
}

UndoCommandPtr Selection::Clear(const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
	if (m_Items.Empty())
	{
		return NULL;
	}

	HELIUM_EDITOR_SCENE_SCOPE_TIMER( "" );

	SimpleTimer timer;

	UndoCommandPtr command;

	OS_ObjectDumbPtr empty;
	SelectionChangingArgs args ( empty );
	m_SelectionChanging.RaiseWithEmitter( args, emitterChanging );
	if ( !args.m_Veto )
	{
		command = new SelectionChangeCommand( this );

		OS_ObjectDumbPtr::Iterator itr = m_Items.Begin();
		OS_ObjectDumbPtr::Iterator end = m_Items.End();
		for ( ; itr != end; ++itr )
		{
			SceneNode *pSceneNode = Reflect::SafeCast<SceneNode>(*itr);
			if (pSceneNode)
			{
				pSceneNode->SetSelected(false);
			}
		}

		m_Items.Clear();

		m_SelectionChanged.RaiseWithEmitter(m_Items, emitterChanged);
	}

	Log::Profile( "Selection Clear took %fms\n", timer.Elapsed());

	return command;
}

UndoCommandPtr Selection::SetItem(Reflect::Object* item, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
	if (item == NULL)
	{
		return Clear(emitterChanging, emitterChanged);
	}

	OS_ObjectDumbPtr temp;

	temp.Append(item);

	return SetItems(temp, emitterChanging, emitterChanged);
}

UndoCommandPtr Selection::SetItems(const OS_ObjectDumbPtr& items, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
	if (items.Empty())
	{
		return Clear(emitterChanging, emitterChanged);
	}

	HELIUM_EDITOR_SCENE_SCOPE_TIMER( "" );

	SimpleTimer timer;

	OS_ObjectDumbPtr selectableItems;

	{
		OS_ObjectDumbPtr::Iterator itr = items.Begin();
		OS_ObjectDumbPtr::Iterator end = items.End();
		for ( ; itr != end; ++itr )
		{
			SceneNode *pSceneNode = Reflect::SafeCast<SceneNode>(*itr);

			if (!pSceneNode || pSceneNode->IsSelectable())
			{
				selectableItems.Append(*itr);
			}
		}
	}

	UndoCommandPtr command;

	SelectionChangingArgs args ( items );
	m_SelectionChanging.RaiseWithEmitter( args , emitterChanging);
	if ( !args.m_Veto )
	{
		command = new SelectionChangeCommand( this );

		{
			OS_ObjectDumbPtr::Iterator itr = m_Items.Begin();
			OS_ObjectDumbPtr::Iterator end = m_Items.End();
			for ( ; itr != end; ++itr )
			{
				SceneNode *pSceneNode = Reflect::SafeCast<SceneNode>(*itr);

				if (pSceneNode)
				{
					pSceneNode->SetSelected(false);
				}
			}
		}

		m_Items = selectableItems;

		{
			OS_ObjectDumbPtr::Iterator itr = m_Items.Begin();
			OS_ObjectDumbPtr::Iterator end = m_Items.End();
			for ( ; itr != end; ++itr )
			{
				SceneNode *pSceneNode = Reflect::SafeCast<SceneNode>(*itr);

				if (pSceneNode)
				{
					pSceneNode->SetSelected(true);
				}
			}
		}

		m_SelectionChanged.RaiseWithEmitter(m_Items, emitterChanged);
	}

	Log::Profile( "Selection SetItems took %fms\n", timer.Elapsed());

	return command;
}

UndoCommandPtr Selection::SetItems(const OS_SceneNodeDumbPtr& items, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
	OS_ObjectDumbPtr objects;
	OS_SceneNodeDumbPtr::Iterator itr = items.Begin();
	OS_SceneNodeDumbPtr::Iterator end = items.End();
	for ( ; itr != end; ++itr )
	{
		objects.Append( *itr );
	}

	return SetItems( objects, emitterChanging, emitterChanged );
}

UndoCommandPtr Selection::AddItem(Reflect::Object* item, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
	if ( item == NULL )
	{
		return NULL;
	}

	OS_ObjectDumbPtr temp;

	temp.Append(item);

	return AddItems(temp, emitterChanging, emitterChanged);
}

UndoCommandPtr Selection::AddItems(const OS_ObjectDumbPtr &items, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
	if ( items.Empty() )
	{
		return NULL;
	}

	HELIUM_EDITOR_SCENE_SCOPE_TIMER( "" );

	SimpleTimer timer;

	std::vector<Reflect::Object*> added;
	OS_ObjectDumbPtr temp = m_Items;
	OS_ObjectDumbPtr::Iterator itr = items.Begin();
	OS_ObjectDumbPtr::Iterator end = items.End();
	for ( ; itr != end; ++itr )
	{
		if ( temp.Append(*itr) )
		{
			added.push_back(*itr);
		}
	}

	UndoCommandPtr command;

	if ( !temp.Empty() )
	{
		SelectionChangingArgs args ( temp );
		m_SelectionChanging.RaiseWithEmitter( args, emitterChanging );
		if ( !args.m_Veto )
		{
			command = new SelectionChangeCommand( this );

			std::vector<Reflect::Object*>::iterator itr = added.begin();
			std::vector<Reflect::Object*>::iterator end = added.end();
			for ( ; itr != end; ++itr )
			{
				SceneNode *pSceneNode = Reflect::SafeCast<SceneNode>(*itr);
				if (pSceneNode)
				{
					pSceneNode->SetSelected(true);
				}
			}

			m_Items = temp;

			m_SelectionChanged.RaiseWithEmitter(m_Items, emitterChanged);
		}
	}

	Log::Profile( "Selection AddItems took %fms\n", timer.Elapsed());

	return command;
}

UndoCommandPtr Selection::RemoveItem(Reflect::Object* item, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
	// no item to remove or no items to remove from
	if ( item == NULL || m_Items.Empty() )
	{
		return NULL;
	}

	OS_ObjectDumbPtr temp;

	temp.Append(item);

	return RemoveItems(temp, emitterChanging, emitterChanged);
}

UndoCommandPtr Selection::RemoveItems(const OS_ObjectDumbPtr& items, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
	// no selected items
	if ( m_Items.Empty() )
	{
		return NULL;
	}

	HELIUM_EDITOR_SCENE_SCOPE_TIMER( "" );

	SimpleTimer timer;

	std::vector<Reflect::Object*> removed;
	OS_ObjectDumbPtr temp = m_Items;
	OS_ObjectDumbPtr::Iterator itr = items.Begin();
	OS_ObjectDumbPtr::Iterator end = items.End();
	for ( ; itr != end; ++itr )
	{
		if ( temp.Remove(*itr) )
		{
			removed.push_back(*itr);
		}
	}

	UndoCommandPtr command;

	SelectionChangingArgs args ( temp );
	m_SelectionChanging.RaiseWithEmitter( args, emitterChanging );
	if ( !args.m_Veto )
	{
		command = new SelectionChangeCommand( this );

		std::vector<Reflect::Object*>::iterator itr = removed.begin();
		std::vector<Reflect::Object*>::iterator end = removed.end();
		for ( ; itr != end; ++itr )
		{
			SceneNode *pSceneNode = Reflect::SafeCast<SceneNode>( *itr );
			if (pSceneNode)
			{
				pSceneNode->SetSelected(false);
			}
		}

		m_Items = temp;

		m_SelectionChanged.RaiseWithEmitter(m_Items, emitterChanged);
	}

	Log::Profile( "Selection RemoveItems took %fms\n", timer.Elapsed());

	return command;
}

bool Selection::Contains(Reflect::Object* item) const
{
	OS_ObjectDumbPtr::Iterator itr = m_Items.Begin();
	OS_ObjectDumbPtr::Iterator end = m_Items.End();
	for ( ; itr != end; ++itr )
	{
		if (*itr == item)
		{
			return true;
		}
	}

	return false;
}

void Selection::GetUndo( OS_ObjectDumbPtr& outItems ) const
{
	outItems = GetItems();
}

void Selection::SetUndo( const OS_ObjectDumbPtr& items )
{
	SetItems( items );
}
