#include "Precompile.h"
#include "VaultSettings.h"
#include "Editor/Vault/VaultPanel.h"
#include "Foundation/Math/Utils.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
VaultSettings::VaultSettings( const tstring& defaultFolder, VaultViewMode viewVaultMode, u32 thumbnailSize )
 : m_WindowSettings( new WindowSettings() )
 //, m_DefaultFolder( defaultFolder )
 , m_VaultViewMode( viewVaultMode )
 , m_ThumbnailSize( thumbnailSize )
 , m_DisplayPreviewAxis( false )
{
  //Helium::Path::Normalize( m_DefaultFolder );
  //Helium::Path::GuaranteeSeparator( m_DefaultFolder );
}

VaultSettings::~VaultSettings()
{
}

///////////////////////////////////////////////////////////////////////////////
void VaultSettings::GetWindowSettings( VaultPanel* vaultPanel, wxAuiManager* manager )
{
  m_WindowSettings->ApplyToWindow( vaultPanel, manager, true );
}

///////////////////////////////////////////////////////////////////////////////
void VaultSettings::SetWindowSettings( VaultPanel* vaultPanel, wxAuiManager* manager )
{
  m_WindowSettings->SetFromWindow( vaultPanel, manager );
}

///////////////////////////////////////////////////////////////////////////////
const VaultViewMode VaultSettings::GetThumbnailMode() const
{
  return m_VaultViewMode;
}

///////////////////////////////////////////////////////////////////////////////
void VaultSettings::SetViewMode( VaultViewMode vaultViewMode )
{
  m_VaultViewMode = vaultViewMode;
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
  Math::Clamp( m_ThumbnailSize, VaultThumbnailsSizes::Small, VaultThumbnailsSizes::Large );
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

/////////////////////////////////////////////////////////////////////////////////
//const tstring& VaultSettings::GetDefaultFolderPath() const
//{
//  return m_DefaultFolder;
//}
//
/////////////////////////////////////////////////////////////////////////////////
//void VaultSettings::SetDefaultFolderPath( const tstring& path )
//{
//  m_DefaultFolder = path;
//  Helium::Path::Normalize( m_DefaultFolder );
//  Helium::Path::GuaranteeSeparator( m_DefaultFolder );
//}
