#include "Precompile.h"
#include "ThumbnailLoadedEvent.h"

using namespace Luna;

DEFINE_EVENT_TYPE( Luna::ThumbnailLoadedCommandEvent )

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
  SetAssetFile( event.GetAssetFile() );
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

Asset::AssetFile* ThumbnailLoadedEvent::GetAssetFile() const
{
  return m_File;
}

void ThumbnailLoadedEvent::SetAssetFile( const Asset::AssetFilePtr& file )
{
  m_File = file;
}

bool ThumbnailLoadedEvent::Cancelled() const
{
  return m_Cancelled;
}

void ThumbnailLoadedEvent::SetCancelled( bool cancelled )
{
  m_Cancelled = cancelled;
}
