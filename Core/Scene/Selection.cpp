/*#include "Precompile.h"*/
#include "Core/Scene/Selection.h"

#include <algorithm>

using namespace Helium;
using namespace Helium::Core;

Selection::Selection()
{

}

void Selection::Refresh()
{
    CORE_SCOPE_TIMER( ("") );

    Timer timer;

    m_SelectionChanging.Raise(m_Items);

    // do nothing

    m_SelectionChanged.Raise(m_Items);

    Log::Profile( TXT( "Selection Refresh took %fms\n" ), timer.Elapsed());
}

const OS_PersistentDumbPtr& Selection::GetItems() const
{
    return m_Items;
}

Undo::CommandPtr Selection::Clear(const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
    if (m_Items.Empty())
    {
        return NULL;
    }

    CORE_SCOPE_TIMER( ("") );

    Timer timer;

    Undo::CommandPtr command;

    OS_PersistentDumbPtr empty;
    SelectionChangingArgs args ( empty );
    m_SelectionChanging.RaiseWithEmitter( args, emitterChanging );
    if ( !args.m_Veto )
    {
        command = new SelectionChangeCommand( this );

        OS_PersistentDumbPtr::Iterator itr = m_Items.Begin();
        OS_PersistentDumbPtr::Iterator end = m_Items.End();
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

Undo::CommandPtr Selection::SetItem(Persistent* item, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
    if (item == NULL)
    {
        return Clear(emitterChanging, emitterChanged);
    }

    OS_PersistentDumbPtr temp;

    temp.Append(item);

    return SetItems(temp, emitterChanging, emitterChanged);
}

Undo::CommandPtr Selection::SetItems(const OS_PersistentDumbPtr& items, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
    if (items.Empty())
    {
        return Clear(emitterChanging, emitterChanged);
    }

    CORE_SCOPE_TIMER( ("") );

    Timer timer;

    OS_PersistentDumbPtr selectableItems;

    {
        OS_PersistentDumbPtr::Iterator itr = items.Begin();
        OS_PersistentDumbPtr::Iterator end = items.End();
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
            OS_PersistentDumbPtr::Iterator itr = m_Items.Begin();
            OS_PersistentDumbPtr::Iterator end = m_Items.End();
            for ( ; itr != end; ++itr )
            {
                (*itr)->SetSelected(false);
            }
        }

        m_Items = selectableItems;

        {
            OS_PersistentDumbPtr::Iterator itr = m_Items.Begin();
            OS_PersistentDumbPtr::Iterator end = m_Items.End();
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

Undo::CommandPtr Selection::AddItem(Persistent* item, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
    if ( item == NULL )
    {
        return NULL;
    }

    OS_PersistentDumbPtr temp;

    temp.Append(item);

    return AddItems(temp, emitterChanging, emitterChanged);
}

Undo::CommandPtr Selection::AddItems(const OS_PersistentDumbPtr &items, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
    if ( items.Empty() )
    {
        return NULL;
    }

    CORE_SCOPE_TIMER( ("") );

    Timer timer;

    std::vector<Persistent*> added;
    OS_PersistentDumbPtr temp = m_Items;
    OS_PersistentDumbPtr::Iterator itr = items.Begin();
    OS_PersistentDumbPtr::Iterator end = items.End();
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

            std::vector<Persistent*>::iterator itr = added.begin();
            std::vector<Persistent*>::iterator end = added.end();
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

Undo::CommandPtr Selection::RemoveItem(Persistent* item, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
    // no item to remove or no items to remove from
    if ( item == NULL || m_Items.Empty() )
    {
        return NULL;
    }

    OS_PersistentDumbPtr temp;

    temp.Append(item);

    return RemoveItems(temp, emitterChanging, emitterChanged);
}

Undo::CommandPtr Selection::RemoveItems(const OS_PersistentDumbPtr& items, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
    // no selected items
    if ( m_Items.Empty() )
    {
        return NULL;
    }

    CORE_SCOPE_TIMER( ("") );

    Timer timer;

    std::vector<Persistent*> removed;
    OS_PersistentDumbPtr temp = m_Items;
    OS_PersistentDumbPtr::Iterator itr = items.Begin();
    OS_PersistentDumbPtr::Iterator end = items.End();
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

        std::vector<Persistent*>::iterator itr = removed.begin();
        std::vector<Persistent*>::iterator end = removed.end();
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

bool Selection::Contains(Persistent* item) const
{
    OS_PersistentDumbPtr::Iterator itr = m_Items.Begin();
    OS_PersistentDumbPtr::Iterator end = m_Items.End();
    for ( ; itr != end; ++itr )
    {
        if (*itr == item)
        {
            return true;
        }
    }

    return false;
}

void Selection::GetUndo( OS_PersistentDumbPtr& outItems ) const
{
    outItems = GetItems();
}

void Selection::SetUndo( const OS_PersistentDumbPtr& items )
{
    SetItems( items );
}
