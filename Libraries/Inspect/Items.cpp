#include "stdafx.h"
#include "Items.h"
#include "Container.h"

using namespace Inspect;

Items::Items()
: m_Required (false)
, m_Highlight(false)
{

}

bool Items::Process(const std::string& key, const std::string& value)
{
  if (__super::Process(key, value))
    return true;

  if (key == ITEMS_ATTR_REQUIRED)
  {
    if (value == ATTR_VALUE_TRUE)
    {
      m_Required = true;
      return true;
    }
    else if (value == ATTR_VALUE_FALSE)
    {
      m_Required = false;
      return true;
    }
  }
  else if (key == ITEMS_ATTR_ITEM)
  {
    size_t delim = value.find_first_of(ITEMS_ATTR_ITEM_DELIM);

    if (delim != std::string::npos)
    {
      m_Statics.push_back(Item (value.substr(0, delim), value.substr(delim+1)));
    }

    return true;
  }
  else if (key == ITEMS_ATTR_PREFIX)
  {
    m_Prefix = value;
    return true;
  }

  return false;
}

void Items::SetDefaultAppearance(bool def)
{
  SetHighlight(def);
}

void Items::SetToDefault(const ContextMenuEventArgs& event)
{
  event.m_Control->SetDefault();

  event.m_Control->Read();
}

void Items::Realize(Container* parent)
{
  PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );

  if ( IsRealized() )
    return;

  __super::Realize(parent);

  if (!m_Value.empty())
  {
    SetValue( m_Value );
  }

  SetHighlight( m_Highlight );
}

void Items::Clear()
{
  m_Items.clear();

  if ( IsRealized() )
  {
    Control::Cast<wxControlWithItems>(this)->Clear();
  }
}

const V_Item& Items::GetItems() const
{
  return m_Items;
}

void Items::SetItems(const V_Item& items)
{
  if ( IsRealized() )
  {
    m_Items = items;

    Control::Cast<wxControlWithItems>(this)->Clear();

    if( !m_Items.empty() ) 
    {
      m_Window->Freeze();

      V_Item::iterator itr = m_Items.begin();
      V_Item::iterator end = m_Items.end();
      for ( ; itr != end; ++itr )
      {
        if (m_Prefix.length() && itr->m_Key.find(m_Prefix) != itr->m_Key.npos)
        {
          itr->m_Key = itr->m_Key.substr(m_Prefix.length());
        }

        Control::Cast<wxControlWithItems>(this)->Append(itr->m_Key.c_str(), const_cast<std::string*>(&itr->m_Data));
      }

      m_Window->Thaw();
    }
  }
  else
  {
    m_Statics = items;
  }
}

std::string Items::GetValue()
{
  static std::string empty;

  if ( IsRealized() )
  {
    wxControlWithItems* cwi = Control::Cast<wxControlWithItems>(this);

    int selection = cwi->GetSelection();
    if ( selection != wxNOT_FOUND )
    {
      return *static_cast<const std::string*>( cwi->GetClientData( selection ) );
    }
  }
  else
  {
    return m_Value;
  }

  return empty;
}

void Items::SetValue(const std::string& data)
{
  m_Value = data;

  if ( IsRealized() )
  {
    V_Item::const_iterator itr = m_Items.begin();
    V_Item::const_iterator end = m_Items.end();
    for ( ; itr != end; ++itr )
    {
      if ( itr->m_Data == data )
      {
        if (!Control::Cast<wxControlWithItems>(this)->SetStringSelection(itr->m_Key.c_str()))
        {
          NOC_BREAK();
        }
        break;
      }
    }
  }
}

bool Items::Contains(const std::string& item)
{
  if ( IsRealized() )
  {
    V_Item::const_iterator itr = m_Items.begin();
    V_Item::const_iterator end = m_Items.end();
    for ( ; itr != end; ++itr )
    {
      if ( itr->m_Data == item )
      {
        return true;
      }
    }
  }

  return false;
}

void Items::SetHighlight(bool highlighted)
{
  m_Highlight = highlighted;

  if ( IsRealized() )
  {
    if ( m_Highlight )
    {
      if ( m_Required )
      {
        m_Window->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
        m_Window->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
      }
      else
      {
        m_Window->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_Window->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));
      }
    }
    else
    {
      m_Window->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
      m_Window->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    }
  }
}
