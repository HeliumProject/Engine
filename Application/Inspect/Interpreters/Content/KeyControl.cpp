#include "KeyControl.h"
#include "Foundation/Reflect/ArchiveXML.h"
#include "Application/Inspect/Interpreters/Content/KeyClipboardData.h"
#include "Application/UI/CustomColors.h"
#include "Application/UI/RegistryConfig.h"

#include <wx/dcclient.h>
#include <wx/colordlg.h>
#include <wx/dcmemory.h>
#include <wx/image.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/clipbrd.h>

using namespace Helium;
using namespace Helium::Inspect;

// Event table
BEGIN_EVENT_TABLE( KeyControl, wxPanel )
EVT_PAINT( KeyControl::OnPaint )
EVT_LEFT_DOWN( KeyControl::OnMouseLeftDown )
EVT_LEFT_UP( KeyControl::OnMouseLeftUp )
EVT_LEFT_DCLICK( KeyControl::OnMouseLeftDoubleClick )
EVT_RIGHT_UP( KeyControl::OnMouseRightUp )
EVT_MOUSE_CAPTURE_LOST( KeyControl::OnMouseCaptureLost )
EVT_MOTION( KeyControl::OnMouseMove )
END_EVENT_TABLE()


// Constants
static const f32 s_Border = 5.0f; // Decrease control width by this amount
static const i32 s_Range = 2;     // Number of pixels you have to be within when clicking a key

// Context menu strings
static const tchar* s_CutKey = TXT( "Cut Key" );
static const tchar* s_CopyKey = TXT( "Copy Key" );
static const tchar* s_CopyAllKeys = TXT( "Copy All Keys" );
static const tchar* s_PasteKeys = TXT( "Paste Keys" );
static const tchar* s_ClobberKeys = TXT( "Paste Keys (clobber)" );
static const tchar* s_DeleteKey = TXT( "Delete Key" );
static const tchar* s_DeleteAllKeys = TXT( "Delete All Keys" );


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
KeyControl::KeyControl( wxWindow* parent )
: wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER, wxT( "KeyControl Panel" ) )
, m_KeyArray( new KeyArray() )
, m_DraggingKey( Key::InvalidKey )
, m_DragOffset( 0 )
{
  // Context menu
  wxMenuItem* menuItemCut = new wxMenuItem( &m_PopupMenu, wxID_ANY, s_CutKey );
  m_PopupMenu.Append( menuItemCut );
  Connect( menuItemCut->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( KeyControl::OnCutKey ), NULL, this );

  wxMenuItem* menuItemCopy = new wxMenuItem( &m_PopupMenu, wxID_ANY, s_CopyKey );
  m_PopupMenu.Append( menuItemCopy );
  Connect( menuItemCopy->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( KeyControl::OnCopyKey ), NULL, this );

  wxMenuItem* menuItemCopyAll = new wxMenuItem( &m_PopupMenu, wxID_ANY, s_CopyAllKeys );
  m_PopupMenu.Append( menuItemCopyAll );
  Connect( menuItemCopyAll->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( KeyControl::OnCopyAllKeys ), NULL, this );

  wxMenuItem* menuItemPaste = new wxMenuItem( &m_PopupMenu, wxID_ANY, s_PasteKeys );
  m_PopupMenu.Append( menuItemPaste );
  Connect( menuItemPaste->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( KeyControl::OnPasteKeys ), NULL, this );

  wxMenuItem* menuItemClobber = new wxMenuItem( &m_PopupMenu, wxID_ANY, s_ClobberKeys );
  m_PopupMenu.Append( menuItemClobber );
  Connect( menuItemClobber->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( KeyControl::OnClobberKeys ), NULL, this );

  m_PopupMenu.AppendSeparator();

  wxMenuItem* menuItemDelete = new wxMenuItem( &m_PopupMenu, wxID_ANY, s_DeleteKey );
  m_PopupMenu.Append( menuItemDelete );
  Connect( menuItemDelete->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( KeyControl::OnDeleteKey ), NULL, this );

  wxMenuItem* menuItemDeleteAll = new wxMenuItem( &m_PopupMenu, wxID_ANY, s_DeleteAllKeys );
  m_PopupMenu.Append( menuItemDeleteAll );  
  Connect( menuItemDeleteAll->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( KeyControl::OnDeleteAllKeys ), NULL, this );

  // Events
  m_KeyArray->AddKeyCreatedListener( KeySignature::Delegate( this, &KeyControl::Changed ) );
  m_KeyArray->AddKeyDeletedListener( KeySignature::Delegate( this, &KeyControl::Changed ) );
  m_KeyArray->AddKeyMovingListener( KeySignature::Delegate( this, &KeyControl::Changed ) );
  m_KeyArray->AddKeyMovedListener( KeySignature::Delegate( this, &KeyControl::Changed ) );
  m_KeyArray->AddKeyColorChangedListener( KeySignature::Delegate( this, &KeyControl::Changed ) );
  m_KeyArray->AddKeySelectionChangedListener( KeySignature::Delegate( this, &KeyControl::Changed ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
KeyControl::~KeyControl()
{
  m_KeyArray->RemoveKeyCreatedListener( KeySignature::Delegate( this, &KeyControl::Changed ) );
  m_KeyArray->RemoveKeyDeletedListener( KeySignature::Delegate( this, &KeyControl::Changed ) );
  m_KeyArray->RemoveKeyMovingListener( KeySignature::Delegate( this, &KeyControl::Changed ) );
  m_KeyArray->RemoveKeyMovedListener( KeySignature::Delegate( this, &KeyControl::Changed ) );
  m_KeyArray->RemoveKeySelectionChangedListener( KeySignature::Delegate( this, &KeyControl::Changed ) );
  m_KeyArray->RemoveKeyColorChangedListener( KeySignature::Delegate( this, &KeyControl::Changed ) );
}

///////////////////////////////////////////////////////////////////////////////
// Use this pointer to manipulate the control.
// 
KeyArray* KeyControl::GetKeyArray()
{
  return m_KeyArray;
}

///////////////////////////////////////////////////////////////////////////////
// Gets the screen position that corresponds to the specified float value.
// 
i32 KeyControl::GetPos( float location ) const
{
  Math::Clamp( location, 0.0f, 1.0f );
  return Math::Round( ( GetSize().x - s_Border ) * location ) + s_Range;
}

///////////////////////////////////////////////////////////////////////////////
// Returns a value between 0.0 and 1.0 that specifies the relative location of
// the specified absolute position on this control.
// 
float KeyControl::GetLocation( i32 pos ) const
{
  float result = (f32)( pos - s_Range ) / (f32)( GetSize().x - s_Border );
  return Math::Clamp( result, 0.0f, 1.0f );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the color of this control at the specified horizontal pixel location.
// 
Math::Color3 KeyControl::GetColor( i32 pos ) const
{
  Math::Color3 color;
  i32 width = GetSize().x - s_Border;
  if ( width > 0 && pos >= 0 && pos < width )
  {
    wxBitmap bmp( width, 1 );
    wxMemoryDC dc( bmp );

    if ( dc.IsOk() )
    {
      DrawGradient( dc );
      dc.SelectObject( wxNullBitmap );

      wxImage img = bmp.ConvertToImage();
      
      color.r = img.GetRed( pos, 0 );
      color.g = img.GetGreen( pos, 0 );
      color.b = img.GetBlue( pos, 0 );
    }
  }

  return color;
}

///////////////////////////////////////////////////////////////////////////////
// Prompts the user to edit a particular key (indicated by index into the 
// array).  The Windows color picker dialog is used.
// 
bool KeyControl::EditKey( u32 index )
{
  bool ok = false;

  if ( index != Key::InvalidKey )
  {
    const Key* key = m_KeyArray->GetKey( index );
    Math::Color3 initialColor = key->GetColor();

    // Restore custom colors from the registry
    tstring info;
    RegistryConfig::GetInstance()->Read( TXT( "" ), CustomColors::GetDefaultRegistryKey(), info );

    // Show the dialog
    wxColourData colorData;
    colorData.SetChooseFull( true );
    colorData.SetColour( wxColour( initialColor.r, initialColor.g, initialColor.b ) );
    CustomColors::Load( colorData, info );
    wxColourDialog dlg( this, &colorData );
    if ( dlg.ShowModal() == wxID_OK )
    {
      wxColor c = dlg.GetColourData().GetColour();
      Math::Color3 color( c.Red(), c.Green(), c.Blue() );
      m_KeyArray->ChangeKeyColor( index, color );
      ok = true;
    }

    // Save custom colors to the registry
    info = CustomColors::Save( dlg.GetColourData() );
    RegistryConfig::GetInstance()->Write( TXT( "" ), CustomColors::GetDefaultRegistryKey(), info );
  }

  return ok;
}

///////////////////////////////////////////////////////////////////////////////
// Finds the closest key (within s_Range pixels) of the specified screen 
// position.
// 
u32 KeyControl::PickKey( i32 pos ) const
{
  u32 result = Key::InvalidKey;
  const u32 numKeys = m_KeyArray->GetCount();
  for ( u32 keyIndex = 0; keyIndex < numKeys; ++keyIndex )
  {
    const Key* key = m_KeyArray->GetKey( keyIndex );
    if ( ::abs( pos - GetPos( key->GetLocation() ) ) <= s_Range )
    {
      result = keyIndex;
    }
  }
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Draws the background of this control.
// 
void KeyControl::DrawGradient( wxDC& dc ) const
{
  const wxSize size = dc.GetSize();

  const u32 numKeys = m_KeyArray->GetCount();
  if ( numKeys > 1 )
  {
    // Draw gradients
    u32 keyIndex = 0;
    while ( keyIndex < numKeys )
    {
      const Key* keyA = m_KeyArray->GetKey( keyIndex++ );

      if ( keyIndex >= numKeys )
      {
        break;
      }

      const Key* keyB = m_KeyArray->GetKey( keyIndex );

      const Math::Color3& colorA = keyA->GetColor();
      const Math::Color3& colorB = keyB->GetColor();

      i32 posA = GetPos( keyA->GetLocation() );
      i32 width = ::abs( GetPos( keyB->GetLocation() ) - posA );

      dc.GradientFillLinear( wxRect( posA, 0, width, size.y ), wxColor( colorA.r, colorA.g, colorA.b ), wxColor( colorB.r, colorB.g, colorB.b ) );
    }
  }
  else if ( numKeys == 1 )
  {
    // No gradient, solid fill
    const Key* key = m_KeyArray->GetKey( 0 );
    dc.SetBrush( wxBrush( wxColor( key->GetColor().r, key->GetColor().g, key->GetColor().b ) ) );
    dc.DrawRectangle( 0, 0, size.x, size.y );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Draws a key at the specified relative location on this control.
// 
void KeyControl::DrawKey( wxDC& dc, float location, bool selected ) const
{
  const wxSize size = GetSize();
  i32 pos = GetPos( location );

  wxPen fillPen( wxColor( wxT( "BLACK" ) ), 3 );
  dc.SetPen( fillPen );
  dc.DrawLine( pos, 0, pos, size.y );

  if ( selected )
  {
    dc.SetPen( wxPen( wxColor( wxT( "YELLOW" ) ), 1, wxDOT ) );
    dc.DrawLine( pos - 1,  0,  pos - 1,  size.y );
    dc.DrawLine( pos,      1,  pos,      size.y );
    dc.DrawLine( pos + 1,  2,  pos + 1,  size.y );
  }
  else
  {
    wxPen dotPen( wxColor( wxT( "WHITE" ) ), 1, wxDOT );
    dotPen.SetJoin( wxJOIN_MITER );
    dc.SetPen( dotPen );
    dc.DrawLine( pos - 1,  0,  pos - 1,  size.y );
    dc.DrawLine( pos,      1,  pos,      size.y );
    dc.DrawLine( pos + 1,  0,  pos + 1,  size.y );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Saves the specified keys to the clipboard.
// 
bool KeyControl::ToClipboard( const V_KeyPtr& keys )
{
  KeyClipboardDataPtr clipboardData = new KeyClipboardData();
  clipboardData->m_Keys = keys;
  tstring xml;
  try
  {
    Reflect::ArchiveXML::ToString( clipboardData, xml );
  }
  catch ( const Helium::Exception& e )
  {
    tstring error = TXT( "Failed to copy keys to clipboard: " ) + e.Get();
    wxMessageBox( error.c_str(), TXT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, this );
    return false;
  }
  
  if ( wxTheClipboard->Open() )
  {
      tstring temp;
      bool converted = Helium::ConvertString( xml, temp );
      HELIUM_ASSERT( converted );

      wxTheClipboard->SetData( new wxTextDataObject( temp ) );
      wxTheClipboard->Close();
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Attempts to retrieve keys from the clipboard and returns them in the 
// specified parameter if any were found.
// 
bool KeyControl::FromClipboard( V_KeyPtr& keys )
{
    tstring xml;
    if (wxTheClipboard->Open())
    {
        if (wxTheClipboard->IsSupported( wxDF_TEXT ))
        {
            wxTextDataObject data;
            wxTheClipboard->GetData( data );
            const wxChar* str = data.GetText().c_str();
            bool converted = Helium::ConvertString( str, xml );
            HELIUM_ASSERT( converted );
        }  
        wxTheClipboard->Close();
    }

  if ( xml.empty() )
  {
    return false;
  }

  Reflect::V_Element spool;
  KeyClipboardDataPtr clipboardData;
  try
  {
    Reflect::ArchiveXML::FromString( xml, spool );
  }
  catch ( const Helium::Exception& )
  {
    return false;
  }

  if ( spool.empty() )
  {
    return false;
  }

  Reflect::V_Element::const_iterator spoolItr = spool.begin();
  Reflect::V_Element::const_iterator spoolEnd = spool.end();
  for ( ; spoolItr != spoolEnd && !clipboardData.ReferencesObject(); ++spoolItr )
  {
    clipboardData = Reflect::ObjectCast< KeyClipboardData >( *spoolItr );
  }

  if ( clipboardData.ReferencesObject() )
  {
    keys.reserve( clipboardData->m_Keys.size() );
    V_KeyPtr::const_iterator keyItr = clipboardData->m_Keys.begin();
    V_KeyPtr::const_iterator keyEnd = clipboardData->m_Keys.end();
    for ( ; keyItr != keyEnd; ++keyItr )
    {
      keys.push_back( *keyItr );
    }
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Makes sure that there are keys at the beginning and end of the control.  
// Creates new keys if necessary.
// 
void KeyControl::EnsureEndCaps()
{
  u32 firstKey = m_KeyArray->FindKeyAtLocation( 0.0f, 0.0f );
  u32 lastKey = m_KeyArray->FindKeyAtLocation( 1.0f, 0.0f );

  // If either the first or last keys are missing...
  if ( firstKey == Key::InvalidKey || lastKey == Key::InvalidKey )
  {
    m_KeyArray->Freeze();
    
    // Default key color is black.
    Math::Color3 color( 0 );

    // If the first key is missing, create a new one.  Make
    // its color match the key closest to 0.0.
    if ( firstKey == Key::InvalidKey )
    {
      u32 closest = m_KeyArray->FindKeyAtLocation( 0.0f, 1.0f );
      if ( closest != Key::InvalidKey )
      {
        const Key* key = m_KeyArray->GetKey( closest );
        color = key->GetColor();
      }
      m_KeyArray->CreateKey( color, 0.0f );
    }

    // If the last key is missing, create a new one.  Make
    // its color match the key closest to 1.0.
    if ( lastKey == Key::InvalidKey )
    {
      u32 closest = m_KeyArray->FindKeyAtLocation( 1.0f, 1.0f );
      if ( closest != Key::InvalidKey )
      {
        const Key* key = m_KeyArray->GetKey( closest );
        color = key->GetColor();
      }
      m_KeyArray->CreateKey( color, 1.0f );
    }

    m_KeyArray->Thaw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Custom drawing code.
// 
void KeyControl::OnPaint( wxPaintEvent& args )
{
  wxPaintDC dc( this );

  // Clear
  dc.SetBrush( wxBrush( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) ) );
  dc.Clear();

  // Fill
  DrawGradient( dc );

  // Draw unselected keys
  const u32 numKeys = m_KeyArray->GetCount();
  for ( u32 keyIndex = 0; keyIndex < numKeys; ++keyIndex )
  {
    const Key* key = m_KeyArray->GetKey( keyIndex );
    if ( m_KeyArray->GetSelectedKey() != keyIndex )
    {
      DrawKey( dc, key->GetLocation(), false );
    }
  }

  // Draw selected key (always on top of unselected keys)
  const Key* selected = m_KeyArray->GetKey( m_KeyArray->GetSelectedKey() );
  if ( selected )
  {
    DrawKey( dc, selected->GetLocation(), true );
  }

  // Draw Border
  const wxSize size = GetSize();
  dc.SetPen( wxPen( wxSystemSettings::GetColour( wxSYS_COLOUR_3DDKSHADOW ) ) );
  dc.SetBrush( *wxTRANSPARENT_BRUSH );
  dc.DrawRectangle( 0, 0, size.x, size.y );
}

///////////////////////////////////////////////////////////////////////////////
// Changes the selected key, begins drag events as necessary.
// 
void KeyControl::OnMouseLeftDown( wxMouseEvent& args )
{
  if ( !HasCapture() )
  {
    CaptureMouse();
  }

  m_DraggingKey = PickKey( args.GetPosition().x );
  m_KeyArray->SelectKey( m_DraggingKey );
  if ( m_DraggingKey != Key::InvalidKey )
  {
    m_DragOffset = args.GetPosition().x - GetPos( m_KeyArray->GetKey( m_DraggingKey )->GetLocation() );
  }
  else
  {
    m_DragOffset = 0;
  }

  args.Skip();
}

///////////////////////////////////////////////////////////////////////////////
// Ends a drag event if necessary.
// 
void KeyControl::OnMouseLeftUp( wxMouseEvent& args )
{
  if ( m_DraggingKey != Key::InvalidKey )
  {
    // You can't drag the first or last keys
    if ( m_DraggingKey > 0 && m_DraggingKey < m_KeyArray->GetCount() - 1 )
    {
      // Finish drag
      const Key* dragging = m_KeyArray->GetKey( m_DraggingKey );
      float location = GetLocation( args.GetPosition().x - m_DragOffset );
      m_DraggingKey = m_KeyArray->MoveKey( m_DraggingKey, location, false );

      const i32 newPos = GetPos( location );

      // Clobber any keys that we land on top of
      for ( u32 keyIndex = 0; keyIndex < m_KeyArray->GetCount(); ++keyIndex )
      {
        const Key* key = m_KeyArray->GetKey( keyIndex );
        if ( ( key != dragging ) && ( GetPos( key->GetLocation() ) == newPos ) )
        {
          m_KeyArray->DeleteKey( keyIndex-- );
        }
      }
    }
  }

  m_DraggingKey = Key::InvalidKey;
  m_DragOffset = 0;

  if ( HasCapture() )
  {
    ReleaseMouse();
  }

  args.Skip();
}

///////////////////////////////////////////////////////////////////////////////
// Creates a key at the position that was clicked on, or edits the color value
// of an existing key at that location.
// 
void KeyControl::OnMouseLeftDoubleClick( wxMouseEvent& args )
{
  if ( m_KeyArray->GetSelectedKey() != Key::InvalidKey )
  {
    EditKey( m_KeyArray->GetSelectedKey() );
  }
  else
  {
    f32 location = 0.0f;
    if ( m_KeyArray->GetCount() == 0 )
    {
      location = 0.0f;
    }
    else if ( m_KeyArray->GetCount() == 1 )
    {
      location = 1.0f;
    }
    else
    {
      location = GetLocation( args.GetPosition().x );
    }

    u32 keyIndex = m_KeyArray->CreateKey( GetColor( args.GetPosition().x ), location );
    if ( !EditKey( keyIndex ) )
    {
      m_KeyArray->DeleteKey( keyIndex );
    }
  }
  args.Skip();
}

///////////////////////////////////////////////////////////////////////////////
// Drags a key as necessary.
// 
void KeyControl::OnMouseMove( wxMouseEvent& args )
{
  if ( m_DraggingKey != Key::InvalidKey )
  {
    // You can't drag the first or last keys
    if ( m_DraggingKey > 0 && m_DraggingKey < m_KeyArray->GetCount() - 1 )
    {
      const Key* dragging = m_KeyArray->GetKey( m_DraggingKey );
      float location = GetLocation( args.GetPosition().x - m_DragOffset );
      i32 newPos = GetPos( location );
      if ( newPos != GetPos( dragging->GetLocation() ) )
      {
        m_DraggingKey = m_KeyArray->MoveKey( m_DraggingKey, location, true );
      }
    }
  }

  args.Skip();
}

///////////////////////////////////////////////////////////////////////////////
// Displays the context menu for this control.  Also selects the key under the
// mouse if there is one.
// 
void KeyControl::OnMouseRightUp( wxMouseEvent& args )
{
  // Select the key
  u32 key = PickKey( args.GetPosition().x );
  m_KeyArray->SelectKey( key );

  // Enable/disable menu items as necessary
  i32 deleteKeyID = m_PopupMenu.FindItem( s_DeleteKey );
  m_PopupMenu.Enable( deleteKeyID, key != Key::InvalidKey && key > 0 && key < m_KeyArray->GetCount() - 1 );

  i32 cutKeyID = m_PopupMenu.FindItem( s_CutKey );
  m_PopupMenu.Enable( cutKeyID, key != Key::InvalidKey );

  i32 copyKeyID = m_PopupMenu.FindItem( s_CopyKey );
  m_PopupMenu.Enable( copyKeyID, key != Key::InvalidKey );

  V_KeyPtr clipboardKeys;
  i32 pasteKeyID = m_PopupMenu.FindItem( s_PasteKeys );
  bool hasClipboardData = FromClipboard( clipboardKeys );
  m_PopupMenu.Enable( pasteKeyID, hasClipboardData && clipboardKeys.size() > 0 );

  i32 clobberKeyID = m_PopupMenu.FindItem( s_ClobberKeys );
  m_PopupMenu.Enable( clobberKeyID, hasClipboardData && clipboardKeys.size() > 0 ); 

  // Show the menu
  PopupMenu( &m_PopupMenu );

  args.Skip();
}

///////////////////////////////////////////////////////////////////////////////
// This function is required by wxWidgets.
// 
void KeyControl::OnMouseCaptureLost( wxMouseCaptureLostEvent& args )
{
  if ( HasCapture() )
  {
    ReleaseMouse();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for the cut menu item.
// 
void KeyControl::OnCutKey( wxCommandEvent& args )
{
  u32 selectedKey = m_KeyArray->GetSelectedKey();
  // You can't cut the first or last key
  if ( selectedKey != Key::InvalidKey && selectedKey != 0 && selectedKey < ( m_KeyArray->GetCount() - 1 ) )
  {
    const Key* key = m_KeyArray->GetKey( selectedKey );

    V_KeyPtr keys;
    keys.push_back( new Key( key->GetColor(), key->GetLocation() ) );
    if ( ToClipboard( keys ) )
    {
      m_KeyArray->DeleteKey( selectedKey );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for the copy menu item.
// 
void KeyControl::OnCopyKey( wxCommandEvent& args )
{
  u32 selectedKey = m_KeyArray->GetSelectedKey();
  if ( selectedKey != Key::InvalidKey )
  {
    const Key* key = m_KeyArray->GetKey( selectedKey );

    V_KeyPtr keys;
    keys.push_back( new Key( key->GetColor(), key->GetLocation() ) );
    ToClipboard( keys );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for the copy all keys menu item.
// 
void KeyControl::OnCopyAllKeys( wxCommandEvent& args )
{
  const u32 numKeys = m_KeyArray->GetCount();
  if ( numKeys > 0 )
  {
    V_KeyPtr keys;
    keys.reserve( numKeys );
    for ( u32 index = 0; index < numKeys; ++index )
    {
      const Key* key = m_KeyArray->GetKey( index );
      keys.push_back( new Key( key->GetColor(), key->GetLocation() ) );
    }
    ToClipboard( keys );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for the paste menu item.
// 
void KeyControl::OnPasteKeys( wxCommandEvent& args )
{
  V_KeyPtr keys;
  if ( FromClipboard( keys ) && keys.size() > 0 )
  {
    m_KeyArray->Freeze();
    V_KeyPtr::const_iterator keyItr = keys.begin();
    V_KeyPtr::const_iterator keyEnd = keys.end();
    for ( ; keyItr != keyEnd; ++keyItr )
    {
      const Key* key = *keyItr;
      m_KeyArray->CreateKey( key->GetColor(), key->GetLocation() );
    }
    EnsureEndCaps();
    m_KeyArray->Thaw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for a paste operation which should clobber any existing keys in the
// control.
// 
void KeyControl::OnClobberKeys( wxCommandEvent& args )
{
  V_KeyPtr keys;
  if ( FromClipboard( keys ) && keys.size() > 0 )
  {
    m_KeyArray->Freeze();
    m_KeyArray->Clear();
    V_KeyPtr::const_iterator keyItr = keys.begin();
    V_KeyPtr::const_iterator keyEnd = keys.end();
    for ( ; keyItr != keyEnd; ++keyItr )
    {
      const Key* key = *keyItr;
      m_KeyArray->CreateKey( key->GetColor(), key->GetLocation() );
    }
    EnsureEndCaps();
    m_KeyArray->Thaw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for deleting the selected key.
// 
void KeyControl::OnDeleteKey( wxCommandEvent& args )
{
  u32 selectedKey = m_KeyArray->GetSelectedKey();
  // You can't delete the first or last keys
  if ( selectedKey != Key::InvalidKey && selectedKey > 0 && selectedKey < m_KeyArray->GetCount() - 1 )
  {
    m_KeyArray->DeleteKey( selectedKey );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for deleting the all keys.
// 
void KeyControl::OnDeleteAllKeys( wxCommandEvent& args )
{
  m_KeyArray->Clear();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the key array managed by this control is changed.
// 
void KeyControl::Changed( const KeyArgs& args )
{
  Refresh( false );
}
