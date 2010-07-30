#include "Precompile.h"
#include "ThumbnailLoadedEvent.h"

using namespace Helium;
using namespace Helium::Editor;

DEFINE_EVENT_TYPE( Editor::ThumbnailLoadedCommandEvent )

ThumbnailLoadedEvent::ThumbnailLoadedEvent( wxEventType commandType, int id )
: wxCommandEvent( commandType, id )
, m_Cancelled( false )
{ 
}

// You *must* copy here the data to be transported
ThumbnailLoadedEvent::ThumbnailLoadedEvent( const ThumbnailLoadedEvent& event )
: wxCommandEvent( event ) 
{ 
  SetThumbnails( event.GetThumbnails() );
  SetPath( event.GetPath() );
  SetCancelled( event.Cancelled() );
}

// Required for sending with wxPostEvent()
wxEvent* ThumbnailLoadedEvent::Clone() const 
{ 
  return new ThumbnailLoadedEvent( *this ); 
}

const V_ThumbnailPtr& ThumbnailLoadedEvent::GetThumbnails() const
{
  return m_Thumbnails;
}

void ThumbnailLoadedEvent::SetThumbnails( const V_ThumbnailPtr& thumbnails )
{
  m_Thumbnails = thumbnails;
}

const Helium::Path& ThumbnailLoadedEvent::GetPath() const
{
  return m_Path;
}

void ThumbnailLoadedEvent::SetPath( const Helium::Path& path )
{
  m_Path = path;
}

bool ThumbnailLoadedEvent::Cancelled() const
{
  return m_Cancelled;
}

void ThumbnailLoadedEvent::SetCancelled( bool cancelled )
{
  m_Cancelled = cancelled;
}
