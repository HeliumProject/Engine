#include "Precompile.h"
#include "VaultPreferences.h"
#include "VaultFrame.h"

#include "Application/Preferences.h"
#include "Foundation/Reflect/Serializer.h"
#include "Foundation/Reflect/Serializers.h"

#include <wx/msgdlg.h>

using namespace Helium;
using namespace Helium::Editor;

const static tstring s_WindowSettingsVersion( TXT( "1" ) );

///////////////////////////////////////////////////////////////////////////////
VaultPreferences::VaultPreferences( const tstring& defaultFolder, ViewOptionID thumbnailMode, u32 thumbnailSize )
 : m_WindowSettings( new WindowSettings(s_WindowSettingsVersion) )
 , m_DefaultFolder( defaultFolder )
 , m_ThumbnailMode( thumbnailMode )
 , m_ThumbnailSize( thumbnailSize )
 , m_DisplayPreviewAxis( false )
 , m_CollectionManager( new CollectionManager() )
 , m_SearchHistory( new SearchHistory() )
 , m_DependencyCollectionRecursionDepth( 0 )
 , m_UsageCollectionRecursionDepth( 0 )
{
  Helium::Path::Normalize( m_DefaultFolder );
  Helium::Path::GuaranteeSeparator( m_DefaultFolder );
}

VaultPreferences::~VaultPreferences()
{
}

///////////////////////////////////////////////////////////////////////////////
void VaultPreferences::GetWindowSettings( VaultFrame* browserFrame, wxAuiManager* manager )
{
  m_WindowSettings->ApplyToWindow( browserFrame, manager, true );
}

///////////////////////////////////////////////////////////////////////////////
void VaultPreferences::SetWindowSettings( VaultFrame* browserFrame, wxAuiManager* manager )
{
  m_WindowSettings->SetFromWindow( browserFrame, manager );
}

///////////////////////////////////////////////////////////////////////////////
const ViewOptionID VaultPreferences::GetThumbnailMode() const
{
  return m_ThumbnailMode;
}

///////////////////////////////////////////////////////////////////////////////
void VaultPreferences::SetThumbnailMode( ViewOptionID thumbnailMode )
{
  m_ThumbnailMode = thumbnailMode;
}

///////////////////////////////////////////////////////////////////////////////
const u32 VaultPreferences::GetThumbnailSize() const
{
  return m_ThumbnailSize;
}

///////////////////////////////////////////////////////////////////////////////
void VaultPreferences::SetThumbnailSize( u32 thumbnailSize )
{
  m_ThumbnailSize = thumbnailSize;
  Math::Clamp( m_ThumbnailSize, ThumbnailSizes::Small, ThumbnailSizes::Large );
}

///////////////////////////////////////////////////////////////////////////////
bool VaultPreferences::DisplayPreviewAxis() const
{
  return m_DisplayPreviewAxis;
}

///////////////////////////////////////////////////////////////////////////////
void VaultPreferences::SetDisplayPreviewAxis( bool display )
{
  if ( m_DisplayPreviewAxis != display )
  {
    m_DisplayPreviewAxis = display;
    RaiseChanged( DisplayPreviewAxisField() );
  }
}

///////////////////////////////////////////////////////////////////////////////
const Reflect::Field* VaultPreferences::DisplayPreviewAxisField() const
{
  return GetClass()->FindField( &VaultPreferences::m_DisplayPreviewAxis );
}

///////////////////////////////////////////////////////////////////////////////
u32 VaultPreferences::GetDependencyCollectionRecursionDepth() const
{
  return m_DependencyCollectionRecursionDepth;
}

///////////////////////////////////////////////////////////////////////////////
void VaultPreferences::SetDependencyCollectionRecursionDepth( u32 recursionDepth )
{
  if ( m_DependencyCollectionRecursionDepth != recursionDepth )
  {
    m_DependencyCollectionRecursionDepth = recursionDepth;
    Math::Clamp( m_DependencyCollectionRecursionDepth, 0, 1000 );
    RaiseChanged( DependencyCollectionRecursionDepth() );
  }
}

///////////////////////////////////////////////////////////////////////////////
const Reflect::Field* VaultPreferences::DependencyCollectionRecursionDepth() const
{
  return GetClass()->FindField( &VaultPreferences::m_DependencyCollectionRecursionDepth );
}

///////////////////////////////////////////////////////////////////////////////
u32 VaultPreferences::GetUsageCollectionRecursionDepth() const
{
  return m_UsageCollectionRecursionDepth;
}

///////////////////////////////////////////////////////////////////////////////
void VaultPreferences::SetUsageCollectionRecursionDepth( u32 recursionDepth )
{
  if ( m_UsageCollectionRecursionDepth != recursionDepth )
  {
    m_UsageCollectionRecursionDepth = recursionDepth;
    Math::Clamp( m_UsageCollectionRecursionDepth, 0, 1000 );
    RaiseChanged( UsageCollectionRecursionDepth() );
  }
}

///////////////////////////////////////////////////////////////////////////////
const Reflect::Field* VaultPreferences::UsageCollectionRecursionDepth() const
{
  return GetClass()->FindField( &VaultPreferences::m_UsageCollectionRecursionDepth );
}

///////////////////////////////////////////////////////////////////////////////
const tstring& VaultPreferences::GetDefaultFolderPath() const
{
  return m_DefaultFolder;
}

///////////////////////////////////////////////////////////////////////////////
void VaultPreferences::SetDefaultFolderPath( const tstring& path )
{
  m_DefaultFolder = path;
  Helium::Path::Normalize( m_DefaultFolder );
  Helium::Path::GuaranteeSeparator( m_DefaultFolder );
}
