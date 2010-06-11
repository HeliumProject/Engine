#include "stdafx.h"
#include "Control.h"
#include "Container.h"
#include "Interpreter.h"
#include "Canvas.h"
#include "StringData.h"
#include <wx/dnd.h>

using namespace Inspect;

#ifdef PROFILE_ACCUMULATION
Profile::Accumulator Inspect::g_RealizeAccumulator( "Inspect Realize Accumulator" );
#endif

Control::Control()
: m_Canvas (NULL)
, m_Parent (NULL)
, m_Interpreter (NULL)
, m_Writing (false)
, m_ForeColor (0)
, m_BackColor (0)
, m_Enabled (true)
, m_ReadOnly (false)
, m_FixedWidth (false)
, m_FixedHeight (false)
, m_ProportionalWidth (0.f)
, m_ProportionalHeight (0.f)
, m_DropTarget (NULL)
, m_Window (NULL)
, m_Realized (false)
{

}

Control::~Control()
{
  if (m_BoundData)
  {
    m_BoundData->RemoveChangedListener( DataChangedSignature::Delegate ( this, &Control::DataChanged ) );
  }

  m_Realized = false;

  m_Window = NULL;

  delete m_DropTarget;
  m_DropTarget = NULL;
}

void Control::Create()
{

}

int Control::GetDepth()
{
  int depth = 0;
  Control* parent = m_Parent;

  while (parent != NULL)
  {
    depth++;
    parent = parent->m_Parent;
  }

  return depth;
}

const DataPtr& Control::GetData()
{
  return m_BoundData;
}

void Control::Bind(const DataPtr& data)
{
  if ( !m_BoundData.ReferencesObject() || !data.ReferencesObject() )
  {
    if ( m_BoundData.ReferencesObject() )
    {
      m_BoundData->RemoveChangedListener( DataChangedSignature::Delegate ( this, &Control::DataChanged ) );
    }

    m_BoundData = data;

    if ( m_BoundData.ReferencesObject() )
    {
      m_BoundData->AddChangedListener( DataChangedSignature::Delegate ( this, &Control::DataChanged ) );
    }
  }
}

bool Control::IsBound() const
{
  return m_BoundData.ReferencesObject();
}

void Control::SetDefault(const std::string& def)
{
  m_Default = def;
}

bool Control::SetDefault()
{
  if (!m_Default.empty())
  {
    return WriteData(m_Default);
  }
  else
  {
    return false;
  }
}

bool Control::IsDefault() const
{
  if (m_Default.empty() || m_BoundData == NULL)
  {
    return false;
  }

  StringData* data = CastData<StringData, DataTypes::String>( m_BoundData );
  if ( data )
  {
    std::string val;
    data->Get(val);
    return m_Default == val;
  }

  NOC_BREAK(); // you need to NOC_OVERRIDE this, your control is using custom data
  return false;
}

void Control::SetDefaultAppearance(bool def)
{

}


bool Control::Process(const std::string& key, const std::string& value)
{
  if ( key == ATTR_TOOLTIP )
  {
    SetToolTip( value );

    return true;
  }

  return false;
}

int Control::GetForeColor()
{
  return m_ForeColor;
}

void Control::SetForeColor(int color)
{
  m_ForeColor = color;

  if (m_ForeColor != 0x0 && m_Window != NULL)
  {
    m_Window->SetForegroundColour(color);
  }
}

int Control::GetBackColor()
{
  return m_BackColor;
}

void Control::SetBackColor(int color)
{
  m_BackColor = color;

  if (m_BackColor != 0x0 && m_Window != NULL)
  {
    m_Window->SetBackgroundColour(color);
  }
}

bool Control::IsEnabled() const
{
  return m_Enabled;
}

void Control::SetEnabled(bool enabled)
{
  m_Enabled = enabled;

  if (m_Window != NULL)
  {
    m_Window->Enable(m_Enabled && !GetCanvas()->IsLocked());
  }
}

bool Control::IsReadOnly() const
{
  return m_ReadOnly;
}

void Control::SetReadOnly(bool readOnly)
{
  m_ReadOnly = readOnly;

  if (m_Window != NULL)
  {
    m_Window->Enable(m_Enabled && !m_ReadOnly && !GetCanvas()->IsLocked());
  }
}

const ContextMenuPtr& Control::GetContextMenu()
{
  return m_ContextMenu;
}

void Control::SetContextMenu(const ContextMenuPtr& contextMenu)
{
  m_ContextMenu = contextMenu;
}

wxDropTarget* Control::GetDropTarget() const
{
  wxDropTarget* dropTarget = m_DropTarget;
  if ( m_Window )
  {
    dropTarget = m_Window->GetDropTarget();
  }
  return dropTarget;
}

void Control::SetDropTarget(wxDropTarget* dropTarget)
{
  if ( m_Window )
  {
    m_Window->SetDropTarget( dropTarget );
  }
  else
  {
    delete m_DropTarget;
    m_DropTarget = dropTarget;
  }
}

int Control::GetStringWidth(const std::string& str)
{
  wxClientDC dc (m_Window);

  int x, y;
  wxString wxStr (str.c_str());
  dc.GetTextExtent(wxStr, &x, &y, NULL, NULL, &m_Window->GetFont());

  return x;
}

bool Control::TrimString(std::string& str, int width)
{
  wxClientDC dc (m_Window);

  int x, y;
  wxString wxStr (str.c_str());
  dc.GetTextExtent(wxStr, &x, &y, NULL, NULL, &m_Window->GetFont());

  if (x <= width)
  {
    return false;
  }

  size_t count = str.size();
  for ( size_t i = count; i>0; i-- )
  {
    wxStr = (str.substr(0, i-1) + "...").c_str();

    dc.GetTextExtent(wxStr, &x, &y, NULL, NULL, &m_Window->GetFont());

    if (x < width)
    {
      str = wxStr.c_str();
      return true;
    }
  }

  str = "...";
  return true;
}

bool Control::IsRealized()
{
  return m_Realized;
}

void Control::Realize(Container* parent)
{
  PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );

  // allocate your window first, and call up to the base class AST
  NOC_ASSERT( m_Window != NULL );

  SetForeColor(m_ForeColor);
  SetBackColor(m_BackColor);

  if (m_ContextMenu.ReferencesObject())
  {
    SetContextMenu(m_ContextMenu);
  }

  m_Parent = parent;

  m_Realized = true;

  if (!HasType(Reflect::GetType<Container>()))
  {
    // populate choices
    Populate();
  }

  m_Window->SetToolTip( m_ToolTip.c_str() );
  m_Window->SetDropTarget( m_DropTarget );
  m_DropTarget = NULL;

  // Set initial read-only and enable state.
  if ( IsReadOnly() )
  {
    SetReadOnly( true );
  }
  else if ( !IsEnabled() )
  {
    SetEnabled( false );
  }

  m_RealizeEvent.Raise(this);
}

void Control::UnRealize()
{
  if ( m_Parent )
  {
    // If you hit this, you are trying to UnRealize a control that still belongs to
    // its parent.  Remove the control from its parent first, before calling this function.
    NOC_ASSERT( std::find( m_Parent->GetControls().begin(), m_Parent->GetControls().end(), ControlPtr(this) ) == m_Parent->GetControls().end() );
  }

  delete m_Window;
  m_Window = NULL;

  m_Realized = false;
}

void Control::Invalidate()
{
  if (m_Window != NULL)
  {
    m_Window->Refresh();
  }
}

void Control::Freeze()
{
  if( m_Window && !m_Window->IsFrozen() )
  {
    m_Window->Freeze();
  }
}

void Control::Thaw()
{
  if( m_Window && m_Window->IsFrozen() )
  {
    m_Window->Thaw();
  }
}

void Control::Show()
{
  if( m_Window )
  {
    m_Window->Show(true);
  }
}

void Control::Hide()
{
  if( m_Window )
  {
    m_Window->Show(false);
  }
}

void Control::DataChanged(const DataChangedArgs& args)
{
  if (!m_Writing)
  {
    Read();
  }

  m_BoundDataChanged.Raise( this );
}

void Control::Read()
{
  SetDefaultAppearance(IsDefault());
}

bool Control::ReadData(std::string& str) const
{
  StringData* data = CastData<StringData, DataTypes::String>( m_BoundData );
  if (data)
  {
    str.clear();
    data->Get( str );
    return true;
  }

  NOC_BREAK(); // you should not call this, your control is using custom data
  return false;
}

bool Control::ReadAll(V_string& strs) const
{
  StringData* data = CastData<StringData, DataTypes::String>( m_BoundData );
  if ( data )
  {
    strs.clear();
    data->GetAll( strs );
    return true;
  }

  NOC_BREAK(); // you should not call this, your control is using custom data
  return false;
}

bool Control::PreWrite( const Reflect::SerializerPtr& newValue, bool preview )
{
  // check to see if a event handler bound to this control bypasses the write
  if (!RaiseBoundDataChanging( newValue, preview ))
  {
    return false;
  }

  // callback to our interpreter that we changed
  if (m_Interpreter && !m_Interpreter->RaisePropertyChanging( ChangingArgs (this, newValue, preview) ))
  {
    return false;
  }

  return true;
}

bool Control::Write()
{
  return true;
}

bool Control::WriteData(const std::string& str, bool preview)
{
  StringData* data = CastData<StringData, DataTypes::String>( m_BoundData );

  return WriteTypedData(str, data, preview);
}

bool Control::WriteAll(const V_string& strs, bool preview)
{
  StringData* data = CastData<StringData, DataTypes::String>( m_BoundData );
  if (data)
  {
    V_string currentValues;
    data->GetAll( currentValues );

    if ( strs == currentValues )
    {
      return true;
    }

    Reflect::SerializerPtr serializer = Reflect::AssertCast< Reflect::Serializer >( Reflect::Serializer::Create< V_string >() );
    serializer->ConnectData( const_cast< V_string* >( &strs ) );
    if ( !PreWrite( serializer, preview ) )
    {
      Read();
      return false;
    }

    m_Writing = true;
    bool result = data->SetAll( strs );
    m_Writing = false;

    if (result)
    {
      PostWrite();
      return true;
    }
  }

  NOC_BREAK(); // you should not call this, your control is using custom data
  return false;
}

void Control::PostWrite()
{
  SetDefaultAppearance(IsDefault());

  // callback to our interpreter that we changed
  if (m_Interpreter != NULL)
  {
    m_Interpreter->RaisePropertyChanged( this );
  }

  // data validator could change our value, so re-read the value
  Read();
}

const std::string& Control::GetToolTip()
{
  return m_ToolTip;
}

void Control::SetToolTip( const std::string& toolTip )
{
  m_ToolTip = toolTip;

  if ( IsRealized() )
  {
    m_Window->SetToolTip( m_ToolTip.c_str() );
  }
}

#ifdef INSPECT_DEBUG_LAYOUT_LOGIC

void Control::PrintLayout()
{
  if (m_Window)
    Log::Print("%p Type: '%s'\n  Size: (%d/%d, %d/%d) Position: (%d/%d, %d/%d)\n", this, typeid(*this).name(),
    m_Size.x, m_Window->GetSize().x,
    m_Size.y, m_Window->GetSize().y,
    m_Position.x, m_Window->GetPosition().x,
    m_Position.y, m_Window->GetPosition().y );
}

#endif
