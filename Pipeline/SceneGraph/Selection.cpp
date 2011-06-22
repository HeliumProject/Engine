#include "PipelinePch.h"
#include "Pipeline/SceneGraph/Selection.h"

#include <algorithm>

using namespace Helium;
using namespace Helium::SceneGraph;

Selection::Selection()
{

}

void Selection::Refresh()
{
    SCENE_GRAPH_SCOPE_TIMER( ("") );

    SimpleTimer timer;

    m_SelectionChanging.Raise(m_Items);

    // do nothing

    m_SelectionChanged.Raise(m_Items);

    Log::Profile( TXT( "Selection Refresh took %fms\n" ), timer.Elapsed());
}

const OS_SceneNodeDumbPtr& Selection::GetItems() const
{
    return m_Items;
}

Undo::CommandPtr Selection::Clear(const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
    if (m_Items.Empty())
    {
        return NULL;
    }

    SCENE_GRAPH_SCOPE_TIMER( ("") );

    SimpleTimer timer;

    Undo::CommandPtr command;

    OS_SceneNodeDumbPtr empty;
    SelectionChangingArgs args ( empty );
    m_SelectionChanging.RaiseWithEmitter( args, emitterChanging );
    if ( !args.m_Veto )
    {
        command = new SelectionChangeCommand( this );

        OS_SceneNodeDumbPtr::Iterator itr = m_Items.Begin();
        OS_SceneNodeDumbPtr::Iterator end = m_Items.End();
        for ( ; itr != end; ++itr )
        {
            (*itr)->SetSelected(false);
        }

        m_Items.Clear();

        m_SelectionChanged.RaiseWithEmitter(m_Items, emitterChanged);
    }

    Log::Profile( TXT( "Selection Clear took %fms\n" ), timer.Elapsed());

    return command;
}

Undo::CommandPtr Selection::SetItem(SceneNode* item, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
    if (item == NULL)
    {
        return Clear(emitterChanging, emitterChanged);
    }

    OS_SceneNodeDumbPtr temp;

    temp.Append(item);

    return SetItems(temp, emitterChanging, emitterChanged);
}

Undo::CommandPtr Selection::SetItems(const OS_SceneNodeDumbPtr& items, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
    if (items.Empty())
    {
        return Clear(emitterChanging, emitterChanged);
    }

    SCENE_GRAPH_SCOPE_TIMER( ("") );

    SimpleTimer timer;

    OS_SceneNodeDumbPtr selectableItems;

    {
        OS_SceneNodeDumbPtr::Iterator itr = items.Begin();
        OS_SceneNodeDumbPtr::Iterator end = items.End();
        for ( ; itr != end; ++itr )
        {
            if ((*itr)->IsSelectable())
            {
                selectableItems.Append(*itr);
            }
        }
    }

    Undo::CommandPtr command;

    SelectionChangingArgs args ( items );
    m_SelectionChanging.RaiseWithEmitter( args , emitterChanging);
    if ( !args.m_Veto )
    {
        command = new SelectionChangeCommand( this );

        {
            OS_SceneNodeDumbPtr::Iterator itr = m_Items.Begin();
            OS_SceneNodeDumbPtr::Iterator end = m_Items.End();
            for ( ; itr != end; ++itr )
            {
                (*itr)->SetSelected(false);
            }
        }

        m_Items = selectableItems;

        {
            OS_SceneNodeDumbPtr::Iterator itr = m_Items.Begin();
            OS_SceneNodeDumbPtr::Iterator end = m_Items.End();
            for ( ; itr != end; ++itr )
            {
                (*itr)->SetSelected(true);
            }
        }

        m_SelectionChanged.RaiseWithEmitter(m_Items, emitterChanged);
    }

    Log::Profile( TXT( "Selection SetItems took %fms\n" ), timer.Elapsed());

    return command;
}

Undo::CommandPtr Selection::AddItem(SceneNode* item, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
    if ( item == NULL )
    {
        return NULL;
    }

    OS_SceneNodeDumbPtr temp;

    temp.Append(item);

    return AddItems(temp, emitterChanging, emitterChanged);
}

Undo::CommandPtr Selection::AddItems(const OS_SceneNodeDumbPtr &items, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
    if ( items.Empty() )
    {
        return NULL;
    }

    SCENE_GRAPH_SCOPE_TIMER( ("") );

    SimpleTimer timer;

    std::vector<SceneNode*> added;
    OS_SceneNodeDumbPtr temp = m_Items;
    OS_SceneNodeDumbPtr::Iterator itr = items.Begin();
    OS_SceneNodeDumbPtr::Iterator end = items.End();
    for ( ; itr != end; ++itr )
    {
        if ( temp.Append(*itr) )
        {
            added.push_back(*itr);
        }
    }

    Undo::CommandPtr command;

    if ( !temp.Empty() )
    {
        SelectionChangingArgs args ( temp );
        m_SelectionChanging.RaiseWithEmitter( args, emitterChanging );
        if ( !args.m_Veto )
        {
            command = new SelectionChangeCommand( this );

            std::vector<SceneNode*>::iterator itr = added.begin();
            std::vector<SceneNode*>::iterator end = added.end();
            for ( ; itr != end; ++itr )
            {
                (*itr)->SetSelected(true);
            }

            m_Items = temp;

            m_SelectionChanged.RaiseWithEmitter(m_Items, emitterChanged);
        }
    }

    Log::Profile( TXT( "Selection AddItems took %fms\n" ), timer.Elapsed());

    return command;
}

Undo::CommandPtr Selection::RemoveItem(SceneNode* item, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
    // no item to remove or no items to remove from
    if ( item == NULL || m_Items.Empty() )
    {
        return NULL;
    }

    OS_SceneNodeDumbPtr temp;

    temp.Append(item);

    return RemoveItems(temp, emitterChanging, emitterChanged);
}

Undo::CommandPtr Selection::RemoveItems(const OS_SceneNodeDumbPtr& items, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
    // no selected items
    if ( m_Items.Empty() )
    {
        return NULL;
    }

    SCENE_GRAPH_SCOPE_TIMER( ("") );

    SimpleTimer timer;

    std::vector<SceneNode*> removed;
    OS_SceneNodeDumbPtr temp = m_Items;
    OS_SceneNodeDumbPtr::Iterator itr = items.Begin();
    OS_SceneNodeDumbPtr::Iterator end = items.End();
    for ( ; itr != end; ++itr )
    {
        if ( temp.Remove(*itr) )
        {
            removed.push_back(*itr);
        }
    }

    Undo::CommandPtr command;

    SelectionChangingArgs args ( temp );
    m_SelectionChanging.RaiseWithEmitter( args, emitterChanging );
    if ( !args.m_Veto )
    {
        command = new SelectionChangeCommand( this );

        std::vector<SceneNode*>::iterator itr = removed.begin();
        std::vector<SceneNode*>::iterator end = removed.end();
        for ( ; itr != end; ++itr )
        {
            (*itr)->SetSelected(false);
        }

        m_Items = temp;

        m_SelectionChanged.RaiseWithEmitter(m_Items, emitterChanged);
    }

    Log::Profile( TXT( "Selection RemoveItems took %fms\n" ), timer.Elapsed());

    return command;
}

bool Selection::Contains(SceneNode* item) const
{
    OS_SceneNodeDumbPtr::Iterator itr = m_Items.Begin();
    OS_SceneNodeDumbPtr::Iterator end = m_Items.End();
    for ( ; itr != end; ++itr )
    {
        if (*itr == item)
        {
            return true;
        }
    }

    return false;
}

void Selection::GetUndo( OS_SceneNodeDumbPtr& outItems ) const
{
    outItems = GetItems();
}

void Selection::SetUndo( const OS_SceneNodeDumbPtr& items )
{
    SetItems( items );
}
