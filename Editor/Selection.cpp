#include "Precompile.h"
#include "Selection.h"

#include <algorithm>

using namespace Helium;
using namespace Helium::Editor;

Selection::Selection()
{

}

void Selection::Refresh()
{
  LUNA_CORE_SCOPE_TIMER( ("") );

  Profile::Timer timer;

  m_SelectionChanging.Raise(m_Items);

  // do nothing
  
  m_SelectionChanged.Raise(m_Items);

  Log::Profile( TXT( "Selection Refresh took %fms\n" ), timer.Elapsed());
}

const OS_SelectableDumbPtr& Selection::GetItems() const
{
  return m_Items;
}

Undo::CommandPtr Selection::Clear(const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
  if (m_Items.Empty())
  {
    return NULL;
  }

  LUNA_CORE_SCOPE_TIMER( ("") );

  Profile::Timer timer;

  Undo::CommandPtr command;

  if ( m_SelectionChanging.RaiseWithReturn(OS_SelectableDumbPtr (), emitterChanging) )
  {
    command = new SelectionChangeCommand( this );

    OS_SelectableDumbPtr::Iterator itr = m_Items.Begin();
    OS_SelectableDumbPtr::Iterator end = m_Items.End();
    for ( ; itr != end; ++itr )
    {
      (*itr)->SetSelected(false);
    }

    m_Items.Clear();

    m_SelectionChanged.Raise(m_Items, emitterChanged);
  }

  Log::Profile( TXT( "Selection Clear took %fms\n" ), timer.Elapsed());

  return command;
}

Undo::CommandPtr Selection::SetItem(Selectable* selection, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
  if (selection == NULL)
  {
    return Clear(emitterChanging, emitterChanged);
  }

  OS_SelectableDumbPtr temp;

  temp.Append(selection);

  return SetItems(temp, emitterChanging, emitterChanged);
}

Undo::CommandPtr Selection::SetItems(const OS_SelectableDumbPtr& items, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
  if (items.Empty())
  {
    return Clear(emitterChanging, emitterChanged);
  }

  LUNA_CORE_SCOPE_TIMER( ("") );

  Profile::Timer timer;

  OS_SelectableDumbPtr selectableItems;

  {
    OS_SelectableDumbPtr::Iterator itr = items.Begin();
    OS_SelectableDumbPtr::Iterator end = items.End();
    for ( ; itr != end; ++itr )
    {
      if ((*itr)->IsSelectable())
      {
        selectableItems.Append(*itr);
      }
    }
  }

  Undo::CommandPtr command;

  if ( m_SelectionChanging.RaiseWithReturn(items, emitterChanging) )
  {
    command = new SelectionChangeCommand( this );

    {
      OS_SelectableDumbPtr::Iterator itr = m_Items.Begin();
      OS_SelectableDumbPtr::Iterator end = m_Items.End();
      for ( ; itr != end; ++itr )
      {
        (*itr)->SetSelected(false);
      }
    }

    m_Items = selectableItems;

    {
      OS_SelectableDumbPtr::Iterator itr = m_Items.Begin();
      OS_SelectableDumbPtr::Iterator end = m_Items.End();
      for ( ; itr != end; ++itr )
      {
        (*itr)->SetSelected(true);
      }
    }

    m_SelectionChanged.Raise(m_Items, emitterChanged);
  }

  Log::Profile( TXT( "Selection SetItems took %fms\n" ), timer.Elapsed());

  return command;
}

Undo::CommandPtr Selection::AddItem(Selectable* selection, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
  if ( selection == NULL )
  {
    return NULL;
  }

  OS_SelectableDumbPtr temp;

  temp.Append(selection);

  return AddItems(temp, emitterChanging, emitterChanged);
}

Undo::CommandPtr Selection::AddItems(const OS_SelectableDumbPtr &items, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
  if ( items.Empty() )
  {
    return NULL;
  }

  LUNA_CORE_SCOPE_TIMER( ("") );

  Profile::Timer timer;

  std::vector<Selectable*> added;
  OS_SelectableDumbPtr temp = m_Items;
  OS_SelectableDumbPtr::Iterator itr = items.Begin();
  OS_SelectableDumbPtr::Iterator end = items.End();
  for ( ; itr != end; ++itr )
  {
    if ( temp.Append(*itr) )
    {
      added.push_back(*itr);
    }
  }

  Undo::CommandPtr command;

  if ( !temp.Empty() && m_SelectionChanging.RaiseWithReturn(temp, emitterChanging) )
  {
    command = new SelectionChangeCommand( this );
 
    std::vector<Selectable*>::iterator itr = added.begin();
    std::vector<Selectable*>::iterator end = added.end();
    for ( ; itr != end; ++itr )
    {
      (*itr)->SetSelected(true);
    }

    m_Items = temp;

    m_SelectionChanged.Raise(m_Items, emitterChanged);
  }

  Log::Profile( TXT( "Selection AddItems took %fms\n" ), timer.Elapsed());

  return command;
}

Undo::CommandPtr Selection::RemoveItem(Selectable* selection, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
  // no item to remove or no items to remove from
  if ( selection == NULL || m_Items.Empty() )
  {
    return NULL;
  }

  OS_SelectableDumbPtr temp;

  temp.Append(selection);

  return RemoveItems(temp, emitterChanging, emitterChanged);
}

Undo::CommandPtr Selection::RemoveItems(const OS_SelectableDumbPtr& items, const SelectionChangingSignature::Delegate& emitterChanging, const SelectionChangedSignature::Delegate& emitterChanged)
{
  // no selected items
  if ( m_Items.Empty() )
  {
    return NULL;
  }

  LUNA_CORE_SCOPE_TIMER( ("") );

  Profile::Timer timer;

  std::vector<Selectable*> removed;
  OS_SelectableDumbPtr temp = m_Items;
  OS_SelectableDumbPtr::Iterator itr = items.Begin();
  OS_SelectableDumbPtr::Iterator end = items.End();
  for ( ; itr != end; ++itr )
  {
    if ( temp.Remove(*itr) )
    {
      removed.push_back(*itr);
    }
  }

  Undo::CommandPtr command;

  if ( m_SelectionChanging.RaiseWithReturn(temp, emitterChanging) )
  {
    command = new SelectionChangeCommand( this );

    std::vector<Selectable*>::iterator itr = removed.begin();
    std::vector<Selectable*>::iterator end = removed.end();
    for ( ; itr != end; ++itr )
    {
      (*itr)->SetSelected(false);
    }

    m_Items = temp;

    m_SelectionChanged.Raise(m_Items, emitterChanged);
  }

  Log::Profile( TXT( "Selection RemoveItems took %fms\n" ), timer.Elapsed());

  return command;
}

bool Selection::Contains(Selectable* selection) const
{
  OS_SelectableDumbPtr::Iterator itr = m_Items.Begin();
  OS_SelectableDumbPtr::Iterator end = m_Items.End();
  for ( ; itr != end; ++itr )
  {
    if (*itr == selection)
    {
      return true;
    }
  }

  return false;
}

void Selection::GetUndo( OS_SelectableDumbPtr& outItems ) const
{
  outItems = GetItems();
}

void Selection::SetUndo( const OS_SelectableDumbPtr& items )
{
  SetItems( items );
}
