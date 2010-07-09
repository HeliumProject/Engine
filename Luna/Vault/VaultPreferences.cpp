#include "Precompile.h"
#include "VaultPreferences.h"
#include "VaultFrame.h"

#include "Application/Preferences.h"
#include "Foundation/Reflect/Serializer.h"
#include "Foundation/Reflect/Serializers.h"

#include <wx/msgdlg.h>

using namespace Luna;

const static tstring s_PreferencesVersion( TXT( "2" ) );

///////////////////////////////////////////////////////////////////////////////
REFLECT_DEFINE_CLASS( VaultPreferences )
void VaultPreferences::EnumerateClass( Reflect::Compositor< VaultPreferences >& comp )
{
  comp.GetComposite().m_UIName = TXT( "Vault Preferences" );

  Reflect::ElementField* elemWindowSettings = comp.AddField( &VaultPreferences::m_WindowSettings, "m_WindowSettings", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldDefaultFolder = comp.AddField( &VaultPreferences::m_DefaultFolder, "m_DefaultFolder", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldThumbnailMode = comp.AddEnumerationField( &VaultPreferences::m_ThumbnailMode, "m_ThumbnailMode" );
  Reflect::Field* fieldThumbnailSize = comp.AddField( &VaultPreferences::m_ThumbnailSize, "m_ThumbnailSize" );
  fieldThumbnailSize->SetProperty( TXT( "UIScript" ), TXT( "UI[.[slider{min=16.0; max=256.0} value{}].]" ) );
  Reflect::Field* fieldDisplayPreviewAxis = comp.AddField( &VaultPreferences::m_DisplayPreviewAxis, "m_DisplayPreviewAxis" );

  Reflect::Field* fieldCollectionManager = comp.AddField( &VaultPreferences::m_CollectionManager, "m_CollectionManager", Reflect::FieldFlags::Hide | Reflect::FieldFlags::Share );

  Reflect::Field* fieldSearchHistory = comp.AddField( &VaultPreferences::m_SearchHistory, "m_SearchHistory", Reflect::FieldFlags::Hide | Reflect::FieldFlags::Share );

  Reflect::Field* fieldDependencyRecursion = comp.AddField( &VaultPreferences::m_DependencyCollectionRecursionDepth, "DependencySearchDepth" );
  fieldDependencyRecursion->SetProperty( TXT( "UIScript" ), TXT( "UI[.[slider{min=0; max=100} value{}].]" ) );
  Reflect::Field* fieldUsageRecursion = comp.AddField( &VaultPreferences::m_UsageCollectionRecursionDepth, "UsageSearchDepth" );
  fieldUsageRecursion->SetProperty( TXT( "UIScript" ), TXT( "UI[.[slider{min=0; max=100} value{}].]" ) );
}

///////////////////////////////////////////////////////////////////////////////
VaultPreferences::VaultPreferences( const tstring& defaultFolder, ViewOptionID thumbnailMode, u32 thumbnailSize )
 : m_WindowSettings( new WindowSettings() )
 , m_DefaultFolder( defaultFolder )
 , m_ThumbnailMode( thumbnailMode )
 , m_ThumbnailSize( thumbnailSize )
 , m_DisplayPreviewAxis( false )
 , m_CollectionManager( new CollectionManager() )
 , m_SearchHistory( new SearchHistory() )
 , m_DependencyCollectionRecursionDepth( 0 )
 , m_UsageCollectionRecursionDepth( 0 )
{
  Nocturnal::Path::Normalize( m_DefaultFolder );
  Nocturnal::Path::GuaranteeSlash( m_DefaultFolder );
}

VaultPreferences::~VaultPreferences()
{
}

///////////////////////////////////////////////////////////////////////////////
const tstring& VaultPreferences::GetCurrentVersion() const
{
  return s_PreferencesVersion;
}

///////////////////////////////////////////////////////////////////////////////
tstring VaultPreferences::GetPreferencesPath() const
{
    Nocturnal::Path prefsDir;
    if ( !Application::GetPreferencesDirectory( prefsDir ) )
    {
        throw Nocturnal::Exception( TXT( "Could not get preferences directory." ) );
    }
    return prefsDir.Get() + TXT( "LunaVaultPrefs.nrb" );
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
  Nocturnal::Path::Normalize( m_DefaultFolder );
  Nocturnal::Path::GuaranteeSlash( m_DefaultFolder );
}
