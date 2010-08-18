#include "Precompile.h"
#include "VaultSettings.h"
#include "VaultFrame.h"

using namespace Helium;
using namespace Helium::Editor;

const static tstring s_WindowSettingsVersion( TXT( "1" ) );

///////////////////////////////////////////////////////////////////////////////
VaultSettings::VaultSettings( const tstring& defaultFolder, ViewOptionID thumbnailMode, u32 thumbnailSize )
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

VaultSettings::~VaultSettings()
{
}

///////////////////////////////////////////////////////////////////////////////
void VaultSettings::GetWindowSettings( VaultFrame* browserFrame, wxAuiManager* manager )
{
  m_WindowSettings->ApplyToWindow( browserFrame, manager, true );
}

///////////////////////////////////////////////////////////////////////////////
void VaultSettings::SetWindowSettings( VaultFrame* browserFrame, wxAuiManager* manager )
{
  m_WindowSettings->SetFromWindow( browserFrame, manager );
}

///////////////////////////////////////////////////////////////////////////////
const ViewOptionID VaultSettings::GetThumbnailMode() const
{
  return m_ThumbnailMode;
}

///////////////////////////////////////////////////////////////////////////////
void VaultSettings::SetThumbnailMode( ViewOptionID thumbnailMode )
{
  m_ThumbnailMode = thumbnailMode;
}

///////////////////////////////////////////////////////////////////////////////
const u32 VaultSettings::GetThumbnailSize() const
{
  return m_ThumbnailSize;
}

///////////////////////////////////////////////////////////////////////////////
void VaultSettings::SetThumbnailSize( u32 thumbnailSize )
{
  m_ThumbnailSize = thumbnailSize;
  Math::Clamp( m_ThumbnailSize, ThumbnailSizes::Small, ThumbnailSizes::Large );
}

///////////////////////////////////////////////////////////////////////////////
bool VaultSettings::DisplayPreviewAxis() const
{
  return m_DisplayPreviewAxis;
}

///////////////////////////////////////////////////////////////////////////////
void VaultSettings::SetDisplayPreviewAxis( bool display )
{
  if ( m_DisplayPreviewAxis != display )
  {
    m_DisplayPreviewAxis = display;
    RaiseChanged( DisplayPreviewAxisField() );
  }
}

///////////////////////////////////////////////////////////////////////////////
const Reflect::Field* VaultSettings::DisplayPreviewAxisField() const
{
  return GetClass()->FindField( &VaultSettings::m_DisplayPreviewAxis );
}

///////////////////////////////////////////////////////////////////////////////
u32 VaultSettings::GetDependencyCollectionRecursionDepth() const
{
  return m_DependencyCollectionRecursionDepth;
}

///////////////////////////////////////////////////////////////////////////////
void VaultSettings::SetDependencyCollectionRecursionDepth( u32 recursionDepth )
{
  if ( m_DependencyCollectionRecursionDepth != recursionDepth )
  {
    m_DependencyCollectionRecursionDepth = recursionDepth;
    Math::Clamp( m_DependencyCollectionRecursionDepth, 0, 1000 );
    RaiseChanged( DependencyCollectionRecursionDepth() );
  }
}

///////////////////////////////////////////////////////////////////////////////
const Reflect::Field* VaultSettings::DependencyCollectionRecursionDepth() const
{
  return GetClass()->FindField( &VaultSettings::m_DependencyCollectionRecursionDepth );
}

///////////////////////////////////////////////////////////////////////////////
u32 VaultSettings::GetUsageCollectionRecursionDepth() const
{
  return m_UsageCollectionRecursionDepth;
}

///////////////////////////////////////////////////////////////////////////////
void VaultSettings::SetUsageCollectionRecursionDepth( u32 recursionDepth )
{
  if ( m_UsageCollectionRecursionDepth != recursionDepth )
  {
    m_UsageCollectionRecursionDepth = recursionDepth;
    Math::Clamp( m_UsageCollectionRecursionDepth, 0, 1000 );
    RaiseChanged( UsageCollectionRecursionDepth() );
  }
}

///////////////////////////////////////////////////////////////////////////////
const Reflect::Field* VaultSettings::UsageCollectionRecursionDepth() const
{
  return GetClass()->FindField( &VaultSettings::m_UsageCollectionRecursionDepth );
}

///////////////////////////////////////////////////////////////////////////////
const tstring& VaultSettings::GetDefaultFolderPath() const
{
  return m_DefaultFolder;
}

///////////////////////////////////////////////////////////////////////////////
void VaultSettings::SetDefaultFolderPath( const tstring& path )
{
  m_DefaultFolder = path;
  Helium::Path::Normalize( m_DefaultFolder );
  Helium::Path::GuaranteeSeparator( m_DefaultFolder );
}
