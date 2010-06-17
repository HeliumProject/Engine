#include "Precompile.h"
#include "BrowserPreferences.h"
#include "BrowserFrame.h"

#include "Application/Preferences.h"
#include "Pipeline/Asset/AssetFlags.h"
#include "Foundation/Environment.h"
#include "Finder/LunaSpecs.h"
#include "Reflect/Serializer.h"
#include "Reflect/Serializers.h"

#include <wx/msgdlg.h>

using namespace Luna;

const static std::string s_PreferencesVersion( "2" );

///////////////////////////////////////////////////////////////////////////////
REFLECT_DEFINE_CLASS( BrowserPreferences )
void BrowserPreferences::EnumerateClass( Reflect::Compositor< BrowserPreferences >& comp )
{
  comp.GetComposite().m_UIName = "Vault Preferences";

  Reflect::ElementField* elemWindowSettings = comp.AddField( &BrowserPreferences::m_WindowSettings, "m_WindowSettings", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldDefaultFolder = comp.AddField( &BrowserPreferences::m_DefaultFolder, "m_DefaultFolder", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldThumbnailMode = comp.AddEnumerationField( &BrowserPreferences::m_ThumbnailMode, "m_ThumbnailMode" );
  Reflect::Field* fieldThumbnailSize = comp.AddField( &BrowserPreferences::m_ThumbnailSize, "m_ThumbnailSize" );
  fieldThumbnailSize->SetProperty( "UIScript", "UI[.[slider{min=16.0; max=256.0} value{}].]" );
  Reflect::Field* fieldDisplayPreviewAxis = comp.AddField( &BrowserPreferences::m_DisplayPreviewAxis, "m_DisplayPreviewAxis" );

  Reflect::Field* fieldCollectionManager = comp.AddField( &BrowserPreferences::m_CollectionManager, "m_CollectionManager", Reflect::FieldFlags::Hide | Reflect::FieldFlags::Share );

  Reflect::Field* fieldSearchHistory = comp.AddField( &BrowserPreferences::m_SearchHistory, "m_SearchHistory", Reflect::FieldFlags::Hide | Reflect::FieldFlags::Share );

  Reflect::Field* fieldDependencyRecursion = comp.AddField( &BrowserPreferences::m_DependencyCollectionRecursionDepth, "DependencySearchDepth" );
  fieldDependencyRecursion->SetProperty( "UIScript", "UI[.[slider{min=0; max=100} value{}].]" );
  Reflect::Field* fieldUsageRecursion = comp.AddField( &BrowserPreferences::m_UsageCollectionRecursionDepth, "UsageSearchDepth" );
  fieldUsageRecursion->SetProperty( "UIScript", "UI[.[slider{min=0; max=100} value{}].]" );
}

///////////////////////////////////////////////////////////////////////////////
BrowserPreferences::BrowserPreferences( const std::string& defaultFolder, ViewOptionID thumbnailMode, u32 thumbnailSize )
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

BrowserPreferences::~BrowserPreferences()
{
}

///////////////////////////////////////////////////////////////////////////////
const std::string& BrowserPreferences::GetCurrentVersion() const
{
  return s_PreferencesVersion;
}

///////////////////////////////////////////////////////////////////////////////
std::string BrowserPreferences::GetPreferencesPath() const
{
    Nocturnal::Path prefsDir;
    if ( !Application::GetPreferencesDirectory( prefsDir ) )
    {
        throw Nocturnal::Exception( "Could not get preferences directory." );
    }
    return FinderSpecs::Luna::BROWSER_PREFS.GetFile( prefsDir.Get() );
}

///////////////////////////////////////////////////////////////////////////////
void BrowserPreferences::GetWindowSettings( BrowserFrame* browserFrame, wxAuiManager* manager )
{
  m_WindowSettings->ApplyToWindow( browserFrame, manager, true );
}

///////////////////////////////////////////////////////////////////////////////
void BrowserPreferences::SetWindowSettings( BrowserFrame* browserFrame, wxAuiManager* manager )
{
  m_WindowSettings->SetFromWindow( browserFrame, manager );
}

///////////////////////////////////////////////////////////////////////////////
const ViewOptionID BrowserPreferences::GetThumbnailMode() const
{
  return m_ThumbnailMode;
}

///////////////////////////////////////////////////////////////////////////////
void BrowserPreferences::SetThumbnailMode( ViewOptionID thumbnailMode )
{
  m_ThumbnailMode = thumbnailMode;
}

///////////////////////////////////////////////////////////////////////////////
const u32 BrowserPreferences::GetThumbnailSize() const
{
  return m_ThumbnailSize;
}

///////////////////////////////////////////////////////////////////////////////
void BrowserPreferences::SetThumbnailSize( u32 thumbnailSize )
{
  m_ThumbnailSize = thumbnailSize;
  Math::Clamp( m_ThumbnailSize, ThumbnailSizes::Small, ThumbnailSizes::Large );
}

///////////////////////////////////////////////////////////////////////////////
bool BrowserPreferences::DisplayPreviewAxis() const
{
  return m_DisplayPreviewAxis;
}

///////////////////////////////////////////////////////////////////////////////
void BrowserPreferences::SetDisplayPreviewAxis( bool display )
{
  if ( m_DisplayPreviewAxis != display )
  {
    m_DisplayPreviewAxis = display;
    RaiseChanged( DisplayPreviewAxisField() );
  }
}

///////////////////////////////////////////////////////////////////////////////
const Reflect::Field* BrowserPreferences::DisplayPreviewAxisField() const
{
  return GetClass()->FindField( &BrowserPreferences::m_DisplayPreviewAxis );
}

///////////////////////////////////////////////////////////////////////////////
u32 BrowserPreferences::GetDependencyCollectionRecursionDepth() const
{
  return m_DependencyCollectionRecursionDepth;
}

///////////////////////////////////////////////////////////////////////////////
void BrowserPreferences::SetDependencyCollectionRecursionDepth( u32 recursionDepth )
{
  if ( m_DependencyCollectionRecursionDepth != recursionDepth )
  {
    m_DependencyCollectionRecursionDepth = recursionDepth;
    Math::Clamp( m_DependencyCollectionRecursionDepth, 0, 1000 );
    RaiseChanged( DependencyCollectionRecursionDepth() );
  }
}

///////////////////////////////////////////////////////////////////////////////
const Reflect::Field* BrowserPreferences::DependencyCollectionRecursionDepth() const
{
  return GetClass()->FindField( &BrowserPreferences::m_DependencyCollectionRecursionDepth );
}

///////////////////////////////////////////////////////////////////////////////
u32 BrowserPreferences::GetUsageCollectionRecursionDepth() const
{
  return m_UsageCollectionRecursionDepth;
}

///////////////////////////////////////////////////////////////////////////////
void BrowserPreferences::SetUsageCollectionRecursionDepth( u32 recursionDepth )
{
  if ( m_UsageCollectionRecursionDepth != recursionDepth )
  {
    m_UsageCollectionRecursionDepth = recursionDepth;
    Math::Clamp( m_UsageCollectionRecursionDepth, 0, 1000 );
    RaiseChanged( UsageCollectionRecursionDepth() );
  }
}

///////////////////////////////////////////////////////////////////////////////
const Reflect::Field* BrowserPreferences::UsageCollectionRecursionDepth() const
{
  return GetClass()->FindField( &BrowserPreferences::m_UsageCollectionRecursionDepth );
}

///////////////////////////////////////////////////////////////////////////////
const std::string& BrowserPreferences::GetDefaultFolderPath() const
{
  return m_DefaultFolder;
}

///////////////////////////////////////////////////////////////////////////////
void BrowserPreferences::SetDefaultFolderPath( const std::string& path )
{
  m_DefaultFolder = path;
  Nocturnal::Path::Normalize( m_DefaultFolder );
  Nocturnal::Path::GuaranteeSlash( m_DefaultFolder );
}
