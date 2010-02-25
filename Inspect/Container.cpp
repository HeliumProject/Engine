#include "stdafx.h"
#include "Container.h"
#include "Canvas.h"

using namespace Inspect;

Container::Container()
{

}

Container::~Container()
{
  Clear();
  m_Window = NULL;
}

const V_Control& Container::GetControls() const
{
  return m_Controls;
}

void Container::SetControls(const V_Control& controls)
{
  m_Controls.clear();

  V_Control::const_iterator itr = controls.begin();
  V_Control::const_iterator end = controls.end();
  for( ; itr != end; ++itr )
  {
    AddControl(*itr);
  }
}

void Container::AddControl(Control* control)
{
  if ( IsReadOnly() )
  {
    control->SetReadOnly( true );
  }
  else if ( !IsEnabled() )
  {
    control->SetEnabled( false );
  }

  m_Controls.push_back(control);
}

void Container::InsertControl(int index, Control* control)
{
  if ( IsReadOnly() )
  {
    control->SetReadOnly( true );
  }
  else if ( !IsEnabled() )
  {
    control->SetEnabled( false );
  }

  m_Controls.insert(m_Controls.begin() + index, control);
}

void Container::RemoveControl(Control* control)
{
  // This is annoying... it looks like there might be a bug in wx having
  // to do with wxButtons being the "TmpDefaultItem".  The destructor of
  // wxButton usually takes care of this, but because we remove the button
  // before destroying it, the top level window is left holding a dead
  // pointer.  Therefore, we have to clean up that pointer manually.
  // Update: Apparently the same problem occurs with wxTopLevelWindowMSW::m_winLastFocused.
  // Therefore, we will also store and clear it ourselves.
  wxTopLevelWindow* topWindow = wxDynamicCast( wxGetTopLevelParent( m_Window ), wxTopLevelWindow );
  wxWindow* defaultItem = NULL;
  wxWindow* lastFocus = NULL;
  if ( topWindow )
  {
    defaultItem = topWindow->GetTmpDefaultItem();
    lastFocus = topWindow->GetLastFocus();
  }

  // our child's internal window
  wxWindow* window = NULL;

  // hold a reference on the stack
  ControlPtr referenceHolder = control;

  // if the child is realized
  if (control->GetWindow())
  {
    // If the child is a container, clear it before we break the window hierarchy
    if ( control->HasType( Reflect::GetType<Container>() ) )
    {
      Container* container = static_cast< Container* >( control );
      container->Clear();
    }

    // save the child window pointer
    window = control->GetWindow();

    // The item we are about to remove is stored on the frame, so 
    // clear it (see comments at top of this function).
    if ( defaultItem && defaultItem == window )
    {
      topWindow->SetTmpDefaultItem( NULL );
    }

    // The item we are about to remove is stored on the frame (as
    // the item last having focus), so clear it (see comments at 
    // top of this function).
    if ( lastFocus && lastFocus == window )
    {
      topWindow->SetLastFocus( NULL );
    }

    // unhook the child from the nested control
    if ( window->GetParent() == m_Window )
    {
      m_Window->RemoveChild(window);
    }
  }

  // remove our reference to the control
  const i32 numControls = static_cast< i32 >( m_Controls.size() ) - 1;
  for ( i32 controlIndex = numControls; controlIndex > -1; --controlIndex )
  {
    if ( control == m_Controls.at( controlIndex ) )
    {
      // remove control from our list
      m_Controls.erase( m_Controls.begin() + controlIndex );
      break;
    }
  }

  // unrealize the control
  control->UnRealize();
}

void Container::Clear()
{
  while (!m_Controls.empty())
  {
    // get the current control to clear
    ControlPtr control = m_Controls.back();

    // do the remove work
    RemoveControl( control );
  }
}

int Container::GetIndent()
{
  return (GetDepth() - 1) * m_Canvas->GetBorder();
}

void Container::Bind(const DataPtr& data)
{
  V_Control::iterator itr = m_Controls.begin();
  V_Control::iterator end = m_Controls.end();
  for( ; itr != end; ++itr )
  {
    (*itr)->Bind(data);
  }
}

void Container::SetDefault(const std::string& def)
{
  V_Control::iterator itr = m_Controls.begin();
  V_Control::iterator end = m_Controls.end();
  for( ; itr != end; ++itr )
  {
    (*itr)->SetDefault(def);
  }
}

void Container::Realize(Container* parent)
{
  if ( IsRealized() )
  {
    for ( V_Control::iterator itr = m_Controls.begin(), end = m_Controls.end(); itr != end; ++itr )
    {
      (*itr)->Realize( this );
    }
    return;
  }

  if ( m_Window == NULL )
  {
    INSPECT_SCOPE_TIMER( ( "" ) );
    m_Window = new wxPanel( parent->GetWindow(), wxID_ANY );
  }
  
  if ( m_Window->GetSizer() == NULL )
  {
    m_Window->SetSizer( new wxBoxSizer( wxHORIZONTAL ) );
    wxSizer* sizer = m_Window->GetSizer();

    V_Sizer sizerList;
    V_i32 proportionList;
    
    int proportionMultiplier = 1000;
    int remainingProportion = proportionMultiplier;
    int numRemainingProportions = 0;

    wxSizer* unboundedProportionSizer = NULL;

    V_Control::const_iterator itr = m_Controls.begin();
    V_Control::const_iterator end = m_Controls.end();
    for( ; itr != end; ++itr )
    {
      Control* c = *itr;
      c->Realize( this );
      
      if ( c->GetProportionalWidth() > 0.0f )
      {
        int proportion = (int) ( c->GetProportionalWidth() * (f32) proportionMultiplier );
        remainingProportion -= proportion;

        sizerList.push_back( new wxBoxSizer( wxHORIZONTAL ) );
        proportionList.push_back( proportion );
        unboundedProportionSizer = NULL;
      }
      else
      {
        if ( unboundedProportionSizer == NULL )
        {
          unboundedProportionSizer = new wxBoxSizer( wxHORIZONTAL );
          ++numRemainingProportions;
        }
        
        sizerList.push_back( unboundedProportionSizer );
        proportionList.push_back( -1 );
      }
    }
    
    if ( numRemainingProportions > 1 )
    {
      remainingProportion = (int) ( (f32) remainingProportion / (f32) numRemainingProportions + 0.5f );
    }
    
    int index = 0;
    int spacing = GetCanvas()->GetPad();

    itr = m_Controls.begin();
    end = m_Controls.end();
    for( ; itr != end; ++index, ++itr )
    {
      Control* c = *itr;
      
      int proportion = proportionList[ index ];
      wxSizer* currentSizer = sizerList[ index ];
      
      if ( sizer->GetItem( currentSizer ) == NULL )
      {
        sizer->Add( currentSizer, proportion > 0 ? proportion : remainingProportion, wxEXPAND | wxTOP | wxBOTTOM, spacing );
      }

      int flags = wxALIGN_CENTER_VERTICAL;
      proportion = 0;
      if ( !c->IsFixedWidth() )
      {
        proportion = 1;
        flags |= wxEXPAND;
      }
      
      currentSizer->Add( spacing, 0, 0 );
      currentSizer->Add( c->GetWindow(), proportion, flags );
    }
    sizer->Add(spacing, 0, 0);

    m_Window->Layout();
  }

  __super::Realize(parent);
}


void Container::Populate()
{
  V_Control::iterator itr = m_Controls.begin();
  V_Control::iterator end = m_Controls.end();
  for( ; itr != end; ++itr )
  {
    (*itr)->Populate();
  }

  __super::Populate();
}

void Container::Read()
{
  V_Control::iterator itr = m_Controls.begin();
  V_Control::iterator end = m_Controls.end();
  for( ; itr != end; ++itr )
  {
    (*itr)->Read();
  }

  __super::Read();
}

bool Container::Write()
{
  bool result = true;

  V_Control::iterator itr = m_Controls.begin();
  V_Control::iterator end = m_Controls.end();
  for( ; itr != end; ++itr )
  {
    result &= (*itr)->Write();
  }

  return result && __super::Write();
}

void Container::Freeze()
{
  __super::Freeze();

  V_Control::iterator itr = m_Controls.begin();
  V_Control::iterator end = m_Controls.end();
  for( ; itr != end; ++itr )
  {
    (*itr)->Freeze();
  }
}

void Container::Thaw()
{
  __super::Thaw();

  V_Control::iterator itr = m_Controls.begin();
  V_Control::iterator end = m_Controls.end();
  for( ; itr != end; ++itr )
  {
    (*itr)->Thaw();
  }
}

void Container::Show()
{
  __super::Show();

  V_Control::iterator itr = m_Controls.begin();
  V_Control::iterator end = m_Controls.end();
  for( ; itr != end; ++itr )
  {
    (*itr)->Show();
  }
}

void Container::Hide()
{
  __super::Hide();

  V_Control::iterator itr = m_Controls.begin();
  V_Control::iterator end = m_Controls.end();
  for( ; itr != end; ++itr )
  {
    (*itr)->Hide();
  }
}

void Container::SetEnabled(bool enabled)
{
  __super::SetEnabled( enabled );

  V_Control::iterator itr = m_Controls.begin();
  V_Control::iterator end = m_Controls.end();
  for( ; itr != end; ++itr )
  {
    (*itr)->SetEnabled( enabled );
  }
}

void Container::SetReadOnly(bool readOnly)
{
  __super::SetReadOnly( readOnly );

  V_Control::iterator itr = m_Controls.begin();
  V_Control::iterator end = m_Controls.end();
  for( ; itr != end; ++itr )
  {
    (*itr)->SetReadOnly( readOnly );
  }
}

#ifdef INSPECT_DEBUG_LAYOUT_LOGIC

void Container::PrintLayout()
{
  V_Control::iterator itr = m_Controls.begin();
  V_Control::iterator end = m_Controls.end();
  for( ; itr != end; ++itr )
  {
    (*itr)->PrintLayout();
  }
  
  __super::PrintLayout();
}

#endif
