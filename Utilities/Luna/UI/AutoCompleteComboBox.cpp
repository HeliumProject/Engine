#include "Precompile.h"
#include "Platform/Assert.h"

#include "Luna/UI/AutoCompleteComboBox.h"

#include <algorithm>

using namespace Luna;

namespace Luna
{
  BEGIN_EVENT_TABLE( AutoCompleteComboBox, wxComboBox )
    EVT_TEXT( wxID_ANY, AutoCompleteComboBox::OnTextChanged )
    EVT_KEY_DOWN( AutoCompleteComboBox::OnKeyDown )
    END_EVENT_TABLE()

    AutoCompleteComboBox::AutoCompleteComboBox(
    wxWindow *parent,
    wxWindowID id, 
    const wxString &value, 
    const wxPoint &pos, 
    const wxSize &size, 
    int n, const wxString choices[], 
    long style, 
    const wxValidator &validator, 
    const wxString &name )
    : wxComboBox( parent, id, value, pos, size, n, choices, style, validator, name )
    , m_UsedDeletion( false )
  {

  }

  AutoCompleteComboBox::AutoCompleteComboBox(
    wxWindow *parent,
    wxWindowID id, 
    const wxString &value, 
    const wxPoint &pos, 
    const wxSize &size, 
    const wxArrayString &choices, 
    long style, 
    const wxValidator &validator, 
    const wxString &name )
    : wxComboBox( parent, id, value, pos, size, choices, style, validator, name )
  {

  }

  AutoCompleteComboBox::~AutoCompleteComboBox()
  {

  }

  void AutoCompleteComboBox::OnTextChanged( wxCommandEvent& event )
  {
    // we have used deletion, then do not do anything
    // this is standard behavior in most other UIs
    if( m_UsedDeletion ) 
    {
      m_UsedDeletion = false;
      return;
    }

    bool isReadOnly = ( (GetWindowStyle() & wxCB_READONLY) != 0 );

    // get the current value and make sure it is not empty
    wxString current( GetValue() );
    u32 currentLength = (u32)current.length();

    if( currentLength < 0 )
    {
      return;
    }

    // get the current insertion point
    u32 pos = GetInsertionPoint(); 

    // if we are inserting into the middle of the string, don't do anything
    if ( currentLength > pos )
    {
      return;
    }

    // get the string before that insertion point
    current = current.substr( 0, pos );

    //get the best partial match given the current sub string
    wxString match;
    bool foundMatches = GetBestPartialMatch( current, match );
    if( foundMatches )
    {
      if ( isReadOnly )
      {
        SetValue( match );
      }
      else
      {
        wxString newValue = current;
        newValue += match.substr( currentLength, match.length() );
        SetValue( newValue );
      }

      // if we have a match, then we need to set its value
      // and hide the caret(because Microsoft hates us)
      HideCaret( (HWND)GetEditHWND() );
    }
    else
    {
      // if there is no match, then the user is crazy and 
      // is doing their own thing.  Better give back the caret
      ShowCaret( (HWND)GetEditHWND() );
    }

    // retrieve the last position (whether having made the match
    // or not) and set the selection
    u32 lastPos = GetLastPosition();

    // if our insertion point is equal to our last position, show the caret 
    // again
    if( pos == lastPos )
    {
      ShowCaret( (HWND)GetEditHWND() );
    }

    // set the highlight
    SetSelection( lastPos, pos );

    event.Skip();
  }

  void AutoCompleteComboBox::OnKeyDown( wxKeyEvent& event )
  {
    if ( !(GetWindowStyle() & wxCB_READONLY) )
    {
      switch( event.m_keyCode )
      {
      case WXK_RETURN:
        {
          // TODO: fire event for this?
          break;
        }
      case WXK_BACK:
      case WXK_DELETE:
        {     
          //if we are deleting, then show the caret
          m_UsedDeletion = true;
          ShowCaret( (HWND)GetEditHWND() );
          break;
        }
      case WXK_SPACE:
        {
          ShowCaret( (HWND)GetEditHWND() );
          break;
        }

      default:
        {
          break;
        }
      }
    }

    event.Skip();
  }

  void AutoCompleteComboBox::Clear()
  {
    m_Choices.clear();
    __super::Clear();
  }

  void AutoCompleteComboBox::Delete( unsigned int n )
  {
    m_Choices.erase( m_Choices.begin() + n );
    __super::Delete( n );
  }

  int AutoCompleteComboBox::DoAppend( const wxString& item )
  {
    m_Choices.push_back( std::string( item ) );

    if ( !(GetWindowStyle() & wxCB_SORT) )
    {
      std::sort( m_Choices.begin(), m_Choices.end() );
    }

    return __super::DoAppend( item );
  }

  int AutoCompleteComboBox::DoInsert( const wxString& item, unsigned int pos )
  {
    m_Choices.push_back( std::string( item ) );

    if ( !(GetWindowStyle() & wxCB_SORT) )
    {
      std::sort( m_Choices.begin(), m_Choices.end() );
    }

    return __super::DoInsert( item, pos );
  }

  bool AutoCompleteComboBox::GetBestPartialMatch( const wxString& value, wxString& match )
  {
    //simple linear search
    m_Matches.clear();
    wxArrayString::iterator itr = m_Choices.begin();
    wxArrayString::iterator end = m_Choices.end();
    for( ; itr != end; ++itr )
    {
      wxString& choice (*itr);

      //test against the subset of the given choice 
      //case insensitivity in the search
      if( stricmp( value.c_str(), choice.substr( 0, value.size() ).c_str() ) == 0 )
      {
        m_Matches.push_back( choice );
      }
    }

    if ( !m_Matches.empty() )
    {
      match = (*m_Matches.begin());
      return true;
    }

    return false;
  }

  void AutoCompleteComboBox::UpdateList( wxArrayString& items )
  {
    __super::Clear();
    wxArrayString::const_iterator itr = items.begin();
    wxArrayString::const_iterator end = items.end();
    for( ; itr != end; ++itr )
    {
      __super::DoAppend( *itr );
    }
  }
}

