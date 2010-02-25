#include "Windows.h"
#include "Clipboard.h"

using namespace Windows;

bool Windows::CopyToClipboard( HWND owner, const std::string& data, std::string& error )
{
  bool isOk = true;

  if ( !data.empty() )
  {
    // Allocate zeroed global moveable memory to put the string into
    size_t size = data.size() + 1;
    HGLOBAL mem = GlobalAlloc( GHND, size );
    if ( mem == NULL )
    {
      error = "Unable to allocate memory for copying data to the clipboard.";
      isOk = false;
    }
    else 
    {
      // Copy string to memory buffer
      char* buffer = (char*)GlobalLock( mem );
      {
        strcpy( buffer, data.c_str() );
      }
      GlobalUnlock( mem );

      // Move buffer to the clipboard
      if ( OpenClipboard( owner ) )
      {
        EmptyClipboard();
        SetClipboardData( CF_TEXT, buffer );
        CloseClipboard();
      }
      else
      {
        error = "Unable to open clipboard.";
        isOk = false;
      }
    }
  }
  else
  {
    error = "Illegal attempt to copy an empty string to the clipboard.";
    isOk = false;
  }

  return isOk;
}

bool Windows::RetrieveFromClipboard( HWND owner, std::string& data, std::string& error )
{
  bool isOk = true;

  if ( IsClipboardFormatAvailable( CF_TEXT ) )
  {
    if ( OpenClipboard( owner ) )
    {
      HGLOBAL mem = GetClipboardData( CF_TEXT );
      if ( mem )
      {
        LPTSTR str = (LPTSTR)( GlobalLock( mem ) );
        if ( str )
        {
          data = str;
          GlobalUnlock( mem );
        }
      }
      else
      {
        error = "Unexpected error while trying to fetch clipboard data.";
        isOk = false;
      }
      CloseClipboard();
    }
    else
    {
      error = "Unable to open clipboard.";
      isOk = false;
    }
  }
  else
  {
    error = "Clipboard does not contain a text item.";
    isOk = false;
  }

  return isOk;
}
