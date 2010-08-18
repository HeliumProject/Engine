#include "Precompile.h"
#include "ThumbnailView.h"
#include "SearchResults.h"
#include "Vault.h"
#include "VaultSearch.h"
#include "DetailsFrame.h"
#include "ThumbnailLoadedEvent.h"

#include "Foundation/File/Path.h"
#include "Foundation/String/Utilities.h"
#include "Editor/UpdateStatusEvent.h"
#include "Editor/DragDrop/DropSource.h"
#include "Foundation/Undo/Command.h"
#include "Editor/ArtProvider.h"

#include "Core/Scene/Color.h"  // BARF! Should we move Color.h to Editor?
#include "Core/Scene/Render.h" // BARF! Should we move Render.h to Editor?

#include <wx/dnd.h>
#include <shellapi.h>

using namespace Helium;
using namespace Helium::Core;
using namespace Helium::Editor;

// Statics
const float ThumbnailView::s_NearClipDistance( 0.05f );
const float ThumbnailView::s_FarClipDistance( 10000.0f );
const DWORD ThumbnailView::s_TextColorDefault( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
const DWORD ThumbnailView::s_TextColorBGSelected( D3DCOLOR_ARGB( 255, 49, 106, 197 ) );
const DWORD ThumbnailView::s_TextColorDark( D3DCOLOR_ARGB( 255, 15, 15, 15 ) );
const Math::Vector2 ThumbnailView::s_GapBetweenTiles( 0.06f, 0.2f );
const float ThumbnailView::s_SpaceBetweenTileAndLabel( 0.05f );
const float ThumbnailView::s_ThumbnailSize( 1.0f );
const float ThumbnailView::s_MinThumbnailSize( 16 );
const float ThumbnailView::s_MaxThumbnailSize( 256 );
const u32 s_ScrollBarIncrement = 5; // in pixels
const u32 s_MouseTolerance = 3; // in pixels, how far the mouse must move before causing a drag

static inline bool FloatIsEqual( float f0, float f1, float tolerance )
{
    return fabsf( f0 - f1 ) <= tolerance;
}


// Event Table
BEGIN_EVENT_TABLE( Editor::ThumbnailView, wxScrolledWindow )
EVT_SIZE( ThumbnailView::OnSize )
EVT_KEY_DOWN( ThumbnailView::OnKeyDown )
EVT_PAINT( ThumbnailView::OnPaint )
EVT_MOUSEWHEEL( ThumbnailView::OnMouseWheel )
EVT_SCROLLWIN( ThumbnailView::OnScrollEvent )
EVT_MOTION( ThumbnailView::OnMouseMove )
EVT_LEFT_DOWN( ThumbnailView::OnMouseLeftDown )
EVT_LEFT_UP( ThumbnailView::OnMouseLeftUp )
EVT_LEFT_DCLICK( ThumbnailView::OnMouseLeftDoubleClick )
EVT_RIGHT_DOWN( ThumbnailView::OnMouseRightDown )
EVT_LEAVE_WINDOW( ThumbnailView::OnMouseLeave )
EVT_MENU( ID_Rename, ThumbnailView::OnRename )
EVT_MENU( ID_SelectAll, ThumbnailView::OnSelectAll )
EVT_MENU( ID_SortByName, ThumbnailView::OnSortAlphabetical )
EVT_MENU( ID_SortByType, ThumbnailView::OnSortByType )
EVT_MENU( ID_Sort, ThumbnailView::OnSort )
EVT_MENU( ID_Properties, ThumbnailView::OnFileProperties )
EDITOR_EVT_THUMBNAIL_LOADED( wxID_ANY, ThumbnailView::OnThumbnailLoaded )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ThumbnailView::ThumbnailView( const tstring& thumbnailDirectory, VaultFrame *browserFrame, wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxScrolledWindow( parent, id, pos, size, style, name )
, m_ThumbnailDirectory( thumbnailDirectory)
, m_LabelFontHeight( 14 )
, m_LabelFont( NULL )
, m_TypeFont( NULL )
, m_EditCtrl( NULL )
, m_TileCreator( this )
, m_MouseDown( false )
, m_MouseDownTile( NULL )
, m_RangeSelectTile( NULL )
, m_CtrlOnMouseDown( false )
, m_Scale( 128.0f )
, m_VaultFrame( browserFrame )
{
    m_ThumbnailManager = new ThumbnailManager( this, &m_DeviceManager, m_ThumbnailDirectory );

    // Don't erase background
    SetBackgroundStyle( wxBG_STYLE_CUSTOM );

    // We are handling all scrolling ourself, don't let wxWidgets move our pixels around
    EnableScrolling( false, false );

    // Set up camera orientation (orthographic)
    const Math::Vector3 dir = Math::Vector3::BasisZ * -1.f;
    const Math::Vector3 up = Math::Vector3::BasisY;
    m_Orientation.SetBasis( Math::SingleAxes::Y, Math::Vector4( up ) );
    m_Orientation.SetBasis( Math::SingleAxes::X, Math::Vector4( dir.Cross( up ) ) );
    m_Orientation.SetBasis( Math::SingleAxes::Z, Math::Vector4( Math::Vector3::Zero - dir ) );
    m_Orientation.Invert();

    m_World = Math::Matrix4( Math::Scale( m_Scale, m_Scale, m_Scale ) );

    // Set up camera view matrix
    const Math::Vector3 pivot( 0, 0, 0 );
    m_ViewMatrix = Math::Matrix4( pivot * -1 ) * m_Orientation * Math::Matrix4( Math::Vector3::BasisZ * ( -s_FarClipDistance / 2.0f ) );

    m_DeviceManager.Init( GetHwnd(), 64, 64 );
    m_DeviceManager.AddDeviceFoundListener( Core::Render::DeviceStateSignature::Delegate( this, &ThumbnailView::OnAllocateResources ) );
    m_DeviceManager.AddDeviceLostListener( Core::Render::DeviceStateSignature::Delegate( this, &ThumbnailView::OnReleaseResources ) );
    CreateResources();

    m_TileCreator.SetDefaultThumbnails( m_TextureError, m_TextureLoading, m_TextureFolder );

    m_EditCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    m_EditCtrl->Hide();

    CalculateTotalItemSize();

    // Setup Ribbon colors and FileType Icons
    m_FileTypeColors.insert( M_FileTypeColors::value_type( TXT( "*.entity.*" ), D3DCOLOR_ARGB( 0xff, 0, 180, 253 ) ) );
    m_FileTypeColors.insert( M_FileTypeColors::value_type( TXT( "*.scene.*" ), D3DCOLOR_ARGB( 0xff, 142, 234, 251 ) ) );
    m_FileTypeColors.insert( M_FileTypeColors::value_type( TXT( "*.shader.*" ), D3DCOLOR_ARGB( 0xff, 57, 143, 202 ) ) );

    m_FileTypeColors.insert( M_FileTypeColors::value_type( TXT( "*.fbx" ), D3DCOLOR_ARGB( 0xff, 215, 15, 10 ) ) );
    m_FileTypeColors.insert( M_FileTypeColors::value_type( TXT( "*.nrb" ), D3DCOLOR_ARGB( 0xff, 0, 180, 253 ) ) );
    m_FileTypeColors.insert( M_FileTypeColors::value_type( TXT( "*.tga" ), D3DCOLOR_ARGB( 0xff, 0, 130, 132 ) ) ); 

    IDirect3DDevice9* device = m_DeviceManager.GetD3DDevice();

    InsertFileTypeIcon( device, m_FileTypeIcons, TXT( "*.entity.*" ), TXT( "moon" ) );
    InsertFileTypeIcon( device, m_FileTypeIcons, TXT( "*.scene.*" ), TXT( "enginetype_level" ) );
    InsertFileTypeIcon( device, m_FileTypeIcons, TXT( "*.shader.*" ), TXT( "enginetype_shader" ) );

    InsertFileTypeIcon( device, m_FileTypeIcons, TXT( "*.fbx" ), TXT( "maya" ) );
    InsertFileTypeIcon( device, m_FileTypeIcons, TXT( "*.nrb" ), TXT( "moon" ) );
    InsertFileTypeIcon( device, m_FileTypeIcons, TXT( "*.tga" ), TXT( "fileType_tga" ) );

    // Connect Listeners
    m_EditCtrl->Connect( m_EditCtrl->GetId(), wxEVT_KILL_FOCUS, wxFocusEventHandler( ThumbnailView::OnEditBoxLostFocus ), NULL, this );
    m_EditCtrl->Connect( m_EditCtrl->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ThumbnailView::OnEditBoxPressEnter ), NULL, this );
    m_VaultFrame->Connect( m_VaultFrame->GetId(), wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ThumbnailView::OnVaultFrameClosing ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ThumbnailView::~ThumbnailView()
{
    m_VaultFrame->Disconnect( m_VaultFrame->GetId(), wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ThumbnailView::OnVaultFrameClosing ), NULL, this );
    m_EditCtrl->Disconnect( m_EditCtrl->GetId(), wxEVT_KILL_FOCUS, wxFocusEventHandler( ThumbnailView::OnEditBoxLostFocus ), NULL, this );
    m_EditCtrl->Disconnect( m_EditCtrl->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ThumbnailView::OnEditBoxPressEnter ), NULL, this );

    m_DeviceManager.RemoveDeviceFoundListener( Render::DeviceStateSignature::Delegate( this, &ThumbnailView::OnAllocateResources ) );
    m_DeviceManager.RemoveDeviceLostListener( Render::DeviceStateSignature::Delegate( this, &ThumbnailView::OnReleaseResources ) );

    if ( m_LabelFont )
    {
        m_LabelFont->Release();
        m_LabelFont = NULL;
    }

    if ( m_TypeFont )
    {
        m_TypeFont->Release();
        m_TypeFont = NULL;
    }

    DeleteResources();

    m_FileTypeColors.clear();

    // This must be called before UnregisterWindow, otherwise the background thread might
    // try to access a NULL device.  Probably no longer necessary since we cancel when the
    // browser frame is about to shut down, but keeping it here just in case.
    m_ThumbnailManager->Cancel();
    delete m_ThumbnailManager;
}

///////////////////////////////////////////////////////////////////////////////
void ThumbnailView::InsertFileTypeIcon( IDirect3DDevice9* device, M_FileTypeIcons& fileTypeIcons, const tstring& type, const tchar* fileName )
{
#pragma TODO( "reimplement icons as resources" )
    tstring file = fileName;

    Helium::Insert<M_FileTypeIcons>::Result inserted = fileTypeIcons.insert( M_FileTypeIcons::value_type( type, new Thumbnail( &m_DeviceManager, LoadTexture( device, file ) ) ) );
    HELIUM_ASSERT( inserted.second && inserted.first->second && inserted.first->second->GetTexture() );
}

///////////////////////////////////////////////////////////////////////////////
// Change the results that are currently displayed in this view.
// 
void ThumbnailView::SetResults( SearchResults* results )
{
    if ( m_Results.Ptr() != results )
    {
        m_Results = results;

        m_Sorter.Clear();

        m_MouseDownTile = NULL;
        m_RangeSelectTile = NULL;

        m_Tiles.clear();
        m_VisibleTiles.Clear();
        m_MouseOverTiles.Clear();
        m_SelectedTiles.Clear();
        m_CurrentTextureRequests.clear();

        m_VisibleTileCorners.clear();
        m_HighlighedTileCorners.clear();
        m_SelectedTileCorners.clear();
        m_RibbonColorTileCorners.clear();
        m_FileTypeTileCorners.clear();

        m_ThumbnailManager->Cancel();
        m_ThumbnailManager->Reset();

        m_TileCreator.StartThread();

        AdjustScrollBar( false );
        Refresh();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Stop displaying results in this view.
// 
void ThumbnailView::ClearResults()
{
    SetResults( NULL );
}

///////////////////////////////////////////////////////////////////////////////
// Returns a pointer to the results that are currently being displayed.
// 
const SearchResults* ThumbnailView::GetResults() const
{
    return m_Results;
}

///////////////////////////////////////////////////////////////////////////////
// * Finds the path in the current results
// * scrolls down so it's in view in the D3D panel
// * De-selects all
// * Selects the corresponding tile
void ThumbnailView::SelectPath( const tstring& path )
{
    // Figure out where the tile is so we can scroll to it
    u32 count = 0;
    ThumbnailTile* found = NULL;

    for ( ThumbnailIteratorPtr tileItr = m_Sorter.GetIterator(); !tileItr->IsDone() && !found; tileItr->Next(), ++count )
    {
        ThumbnailTile* tile = tileItr->GetCurrentTile();
        u32 row = count / m_TotalVisibleItems.x;
        u32 col = count % m_TotalVisibleItems.x;
        tile->SetRowColumn( row, col );
        if ( tile->GetPath().IsFile() && ( tile->GetPath().Get() == path ) )
        {
            found = tile;
        }
    }

    if ( found )
    {
        ClearSelection();
        Select( found );
        if ( !IsVisible( found ) )
        {
            EnsureVisible( found );
        }
        Refresh();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Fills out the specified arrays with the files and folders that are currently
// selected.
// 
void ThumbnailView::GetSelectedPaths( std::set< Helium::Path >& paths )
{
    for ( OS_ThumbnailTiles::Iterator tileItr = m_SelectedTiles.Begin(),
        tileEnd = m_SelectedTiles.End(); tileItr != tileEnd; ++tileItr )
    {
        ThumbnailTile* tile = *tileItr;
        paths.insert( tile->GetPath() );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the path of the item that the mouse is currently over (if any).
// 
tstring ThumbnailView::GetHighlightedPath() const
{
    tstring path;
    if ( !m_MouseOverTiles.Empty() )
    {
        path = m_MouseOverTiles.Front()->GetFullPath();
    }
    return path;
}

///////////////////////////////////////////////////////////////////////////////
// Set the size of the thumbnails.  For example, a value of 128 means that the
// thumbnails will appear as 128x128 in the view.
// The scale factor will be clamped between s_MinThumbnailSize and s_MaxThumbnailSize.
// 
void ThumbnailView::SetZoom( u16 zoom )
{
    float scale = zoom;
    Math::Clamp( scale, s_MinThumbnailSize, s_MaxThumbnailSize );

    if ( scale != m_Scale )
    {
        // Keep track of the first visible item so we can maintain it when the 
        // view size changes.
        ThumbnailTile* firstVisible = NULL;
        if ( !m_VisibleTiles.Empty() )
        {
            firstVisible = m_VisibleTiles.Front();
        }

        // Scale
        m_Scale = scale;
        m_World.x.x = m_Scale;
        m_World.y.y = m_Scale;
        m_World.z.z = m_Scale;

        CalculateTotalItemSize();

        // Maintain scroll position if possible
        AdjustScrollBar( false );
        if ( firstVisible && !IsVisible( firstVisible ) )
        {
            // We just changed the size of the tiles, so we have to go through and 
            // recalculate the row and column for all the tiles up to the one we
            // actually care about.
            bool found = false;
            u32 count = 0;
            for ( ThumbnailIteratorPtr tileItr = m_Sorter.GetIterator(); !tileItr->IsDone() && !found; tileItr->Next(), ++count )
            {
                ThumbnailTile* tile = tileItr->GetCurrentTile();
                u32 row = count / m_TotalVisibleItems.x;
                u32 col = count % m_TotalVisibleItems.x;
                tile->SetRowColumn( row, col );
                found = tile == firstVisible;
            }

            if ( found )
            {
                EnsureVisible( firstVisible );
            }
        }
        else
        {
            Refresh();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the current sorting method.
// 
ThumbnailSortMethod ThumbnailView::GetSortMethod() const
{
    return m_Sorter.GetSortMethod();
}

///////////////////////////////////////////////////////////////////////////////
// Sorts the view and optionally refreshes it.
// 
void ThumbnailView::Sort( ThumbnailSortMethod method, u32 sortOptions )
{
    // Only sort if we are being forced to, or if the sort method is actually changing
    if ( method != GetSortMethod() || ( sortOptions & SortOptions::Force ) )
    {
        wxBusyCursor busyCursor;

        m_Sorter.Clear();
        m_Sorter.SetSortMethod( method );
        m_Sorter.Add( m_Tiles );

        // Only refresh if the option is set
        if ( sortOptions & SortOptions::Refresh )
        {
            Refresh();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Scroll to the given position (in logical coords).  Overridden to actually 
// move the placement of the 3D view.
// 
void ThumbnailView::Scroll( int x, int y )
{
    // NOTE: we ignore the x value, this view only scrolls in the y direction.

    SetScrollPos( wxVERTICAL, y, false );
    AdjustScrollBar( true );
    Refresh();
}

///////////////////////////////////////////////////////////////////////////////
// Called by the thumbnail creator when the various containers of tiles are ready
// after being generated from the current results.  Replaces the view's containers
// with those provided by the loader.
// 
void ThumbnailView::OnTilesCreated( const M_PathToTilePtr& tiles, const ThumbnailSorter& sorter, const std::set< Helium::Path >& textures )
{
    m_Tiles = tiles;

    // Retain the sorting method regardless of the setting specified in the callback
    ThumbnailSortMethod sortMethod = m_Sorter.GetSortMethod();
    m_Sorter = sorter;
    m_Sorter.SetSortMethod( sortMethod );

    m_ThumbnailManager->Request( textures );
    Refresh();
}

///////////////////////////////////////////////////////////////////////////////
// Selects the specified tile and returns true if the selection maintained by
// this control was actually changed.  Returning true means that a Refresh is
// required to cause the control to redraw.
// 
bool ThumbnailView::Select( ThumbnailTile* tile )
{
    tile->SetSelected( true );
    if ( m_SelectedTiles.Append( tile ) )
    {
        m_SelectionChanged.Raise( m_SelectedTiles.Size() );
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Deselects the specified tile and returns true if the selection maintained by
// this control was actually changed.  Returning true means that a Refresh is
// required to cause the control to redraw.
// 
bool ThumbnailView::Deselect( ThumbnailTile* tile )
{
    tile->SetSelected( false );
    if ( m_SelectedTiles.Remove( tile ) )
    {
        m_SelectionChanged.Raise( m_SelectedTiles.Size() );
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// See Select() and Deselect() above.
// 
bool ThumbnailView::SetSelection( ThumbnailTile* tile, bool selected )
{
    if ( selected )
    {
        return Select( tile );
    }
    return Deselect( tile );
}

///////////////////////////////////////////////////////////////////////////////
// Removes all selected items and returns true if there were any to begin with.
// Returning true means that a Refresh is required to cause the control to redraw.
// 
bool ThumbnailView::ClearSelection()
{
    bool selectionChanged = !m_SelectedTiles.Empty();
    for ( OS_ThumbnailTiles::Iterator tileItr = m_SelectedTiles.Begin(), 
        tileEnd = m_SelectedTiles.End(); tileItr != tileEnd; ++tileItr )
    {
        ( *tileItr )->SetSelected( false );
    }
    m_SelectedTiles.Clear();
    if ( selectionChanged )
    {
        m_SelectionChanged.Raise( 0 );
    }
    return selectionChanged;
}

///////////////////////////////////////////////////////////////////////////////
// Selects a range of tiles in this view.
// 
bool ThumbnailView::SelectRange( ThumbnailTile* first, ThumbnailTile* last )
{
    bool selectionChanged = false;

    bool foundLast = false;
    for ( ThumbnailIteratorPtr iterator = m_Sorter.GetIterator( first );
        !iterator->IsDone() && !foundLast; iterator->Next() )
    {
        selectionChanged |= Select( iterator->GetCurrentTile() );
        foundLast = iterator->GetCurrentTile() == last;
    }

    return selectionChanged;
}

///////////////////////////////////////////////////////////////////////////////
// Highlights the specified tile (and refreshes if necessary).  There can only
// be one highlight at a time, so this may un-highlight another tile.
// 
void ThumbnailView::Highlight( ThumbnailTile* tile )
{
    if ( !tile->IsHighlighed() )
    {
        if ( !m_MouseOverTiles.Empty() && m_MouseOverTiles.Front().Ptr() != tile )
        {
            ClearHighlight();
        }
        m_MouseOverTiles.Append( tile );
        tile->SetHighlighted( true );
        Refresh();
        m_HighlightChanged.Raise( ThumbnailHighlightArgs( tile->GetFullPath() ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Un-highlights any tile that might be highlighed (and refreshes if necessary)
// 
void ThumbnailView::ClearHighlight()
{
    if ( !m_MouseOverTiles.Empty() )
    {
        m_MouseOverTiles.Front()->SetHighlighted( false );
        m_MouseOverTiles.Clear();
        Refresh();
        m_HighlightChanged.Raise( ThumbnailHighlightArgs( TXT( "" ) ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified tile is currently within the view.  Assumes
// that the row setting on tile is correct.
// 
bool ThumbnailView::IsVisible( ThumbnailTile* tile )
{
    // If there's no scrollbar, it must be visible
    if ( !HasScrollbar( wxVERTICAL ) )
    {
        return true;
    }

    Math::Vector3 itemSizePixels( 0.0f, m_TotalItemSize.y + s_GapBetweenTiles.y, 0.0f );
    m_World.TransformVertex( itemSizePixels );
    float tilePosY = itemSizePixels.y * ( float )( tile->GetRow() );

    u32 scrollPosY = GetScrollPos( wxVERTICAL );
    float viewTop = scrollPosY * s_ScrollBarIncrement;
    float viewBottom = viewTop + GetSize().y;

    return ( FloatIsEqual( tilePosY, viewTop, 0.1f ) || ( tilePosY > viewTop ) ) && ( tilePosY < viewBottom );
}

///////////////////////////////////////////////////////////////////////////////
// Scroll the view to the selected tile. 
// 
void ThumbnailView::EnsureVisible( ThumbnailTile* tile )
{
    Math::Vector3 itemSizePixels( 0.0f, m_TotalItemSize.y + s_GapBetweenTiles.y, 0.0f );
    m_World.TransformVertex( itemSizePixels );
    float tilePosY = itemSizePixels.y * ( float )( tile->GetRow() );
    u32 scrollPosY = Math::Round( tilePosY / ( float )s_ScrollBarIncrement );
    SetScrollPos( wxVERTICAL, scrollPosY, false );
    AdjustScrollBar( true );
    Refresh();
}

///////////////////////////////////////////////////////////////////////////////
// Updates the projection matrix in response to a change in the control's size.
// 
void ThumbnailView::UpdateProjectionMatrix()
{
    HELIUM_ASSERT( GetSize().x > 0 && GetSize().y > 0 );

    m_Projection = Math::Matrix4::Identity;
    m_Projection.x = Math::Vector3::BasisX;
    m_Projection.y = Math::Vector3::BasisY;
    m_Projection.z = Math::Vector3::BasisZ;
    m_Projection.Invert();

    Math::Matrix4 ortho;
    D3DXMatrixOrthoOffCenterRH( (D3DXMATRIX*)&ortho, 0, GetSize().x, -GetSize().y, 0, s_NearClipDistance, s_FarClipDistance );

    m_Projection *= ortho;
}

///////////////////////////////////////////////////////////////////////////////
// Creates all D3D resources needed by this control.
// 
void ThumbnailView::CreateResources()
{
    IDirect3DDevice9* device = m_DeviceManager.GetD3DDevice();
    if ( !device )
    {
        return;
    }

    if ( !m_LabelFont )
    {
        HRESULT result = D3DXCreateFont(
            device,
            m_LabelFontHeight,
            0,
            600,
            1,
            FALSE,
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            NONANTIALIASED_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            TXT( "Arial" ),
            &m_LabelFont );

        HELIUM_ASSERT( SUCCEEDED( result ) );
    }
    else
    {
        m_LabelFont->OnResetDevice();
    }

    if ( !m_TypeFont )
    {
        HRESULT result = D3DXCreateFont(
            device,
            12, // HEIGHT
            0,
            700,
            1,
            FALSE,
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            NONANTIALIASED_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            TXT( "Arial" ),
            &m_TypeFont );

        HELIUM_ASSERT( SUCCEEDED( result ) );
    }
    else
    {
        m_TypeFont->OnResetDevice();
    }

#pragma TODO( "redo the below as resources" )

    if ( !m_TextureMissing )
    {
        tstring file = TXT( "screenshot_missing.png" );

        m_TextureMissing = new Thumbnail( &m_DeviceManager, LoadTexture( device, file ) );
        HELIUM_ASSERT( m_TextureMissing->GetTexture() );
    }

    if ( !m_TextureError )
    {
        tstring file = TXT( "file_error_256.png" );

        m_TextureError = new Thumbnail( &m_DeviceManager, LoadTexture( device, file ) );
        HELIUM_ASSERT( m_TextureError->GetTexture() );
    }

    if ( !m_TextureLoading )
    {
        tstring file = TXT( "screenshot_loading.png" );

        m_TextureLoading = new Thumbnail( &m_DeviceManager, LoadTexture( device, file ) );
        HELIUM_ASSERT( m_TextureLoading->GetTexture() );
    }

    if ( !m_TextureFolder )
    {
        tstring file = TXT( "folder_256.png" );

        m_TextureFolder = new Thumbnail( &m_DeviceManager, LoadTexture( device, file ) );
        HELIUM_ASSERT( m_TextureFolder->GetTexture() );
    }

    if ( !m_TextureOverlay )
    {
        tstring file = TXT( "thumbnail_overlay.png" );

        m_TextureOverlay = new Thumbnail( &m_DeviceManager, LoadTexture( device, file ) );
        HELIUM_ASSERT( m_TextureOverlay->GetTexture() );
    }

    if ( !m_TextureSelected )
    {
        tstring file = TXT( "thumbnail_overlay_selected.png" );

        m_TextureSelected = new Thumbnail( &m_DeviceManager, LoadTexture( device, file ) );
        HELIUM_ASSERT( m_TextureSelected->GetTexture() );
    }

    if ( !m_TextureHighlighted )
    {
        tstring file = TXT( "thumbnail_overlay_highlighted.png" );

        m_TextureHighlighted = new Thumbnail( &m_DeviceManager, LoadTexture( device, file ) );
        HELIUM_ASSERT( m_TextureHighlighted->GetTexture() );
    }

    if ( !m_TextureBlankFile )
    {
        tstring file = TXT( "blank_file_32.png" );

        m_TextureBlankFile = new Thumbnail( &m_DeviceManager, LoadTexture( device, file ) );
        HELIUM_ASSERT( m_TextureBlankFile->GetTexture() );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Frees all D3D resources that this control has created.
// 
void ThumbnailView::DeleteResources()
{
    if ( m_LabelFont )
    {
        m_LabelFont->OnLostDevice();
    }

    if ( m_TypeFont )
    {
        m_TypeFont->OnLostDevice();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Convert a point in world space to a screen x, y coordinate (useful for 
// drawing text at a location in 3D).
// 
void ThumbnailView::WorldToScreen( const Math::Vector3& p, float& x, float& y )
{
    Math::Vector4 v( p.x, p.y, p.z, 1.f );

    // global to camera
    m_ViewMatrix.Transform( v );

    // camera to projection
    m_Projection.Transform( v );

    // apply projection from w component
    ViewportToScreen( Math::Vector3( v.x / v.w, v.y / v.w, v.z / v.w ), x, y );
}

///////////////////////////////////////////////////////////////////////////////
// Converts a viewport coordinate into screen space.
// 
void ThumbnailView::ViewportToScreen( const Math::Vector3& v, float& x, float& y )
{
    x = ( (v.x + 1) * GetSize().x ) / 2.0f;
    y = ( (-v.y + 1) * GetSize().y ) / 2.0f;
}

///////////////////////////////////////////////////////////////////////////////
// Converts a screen coordinate into the viewport space.
// 
void ThumbnailView::ScreenToViewport( float x, float y, Math::Vector3& v ) const
{
    v.x = (((2.0f * x) / GetSize().x) - 1);
    v.y = -(((2.0f * y) / GetSize().y) - 1);
}

///////////////////////////////////////////////////////////////////////////////
// Converts a viewport coordinate into a point in world space.
// 
void ThumbnailView::ViewportToWorldVertex( float x, float y, Math::Vector3& v ) const
{
    ScreenToViewport( x, y, v );

    // treat this as a x/y coordinate only (at the camera location)
    v.z = 0.0f;

    // unproject our screen space coordinate
    Math::Matrix4 inverseProjection = m_Projection;
    inverseProjection.Invert();
    inverseProjection.TransformVertex( v );

    // orient the view vector
    Math::Matrix4 inverseView = m_ViewMatrix;
    inverseView.Invert();
    inverseView.TransformVertex( v );
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to calculate m_TotalItemSize when the world matrix changes.
// This is necessary because the text is kept at a constant size regardless of
// the scaling that occurs on the screenshot.
// 
void ThumbnailView::CalculateTotalItemSize()
{
    Math::Matrix4 inverseWorld( m_World );
    inverseWorld.Invert();

    Math::Vector3 screenOrigin( 0, 0, 0 );
    Math::Vector3 worldUpperLeft;
    inverseWorld.TransformVertex( screenOrigin );
    ViewportToWorldVertex( screenOrigin.x, screenOrigin.y, worldUpperLeft );

    Math::Vector3 screenFontHeight( 0, m_LabelFontHeight, 0 );
    Math::Vector3 worldFontHeight;
    inverseWorld.TransformVertex( screenFontHeight );
    ViewportToWorldVertex( screenFontHeight.x, screenFontHeight.y, worldFontHeight );

    m_TotalItemSize.x = s_ThumbnailSize;
    m_TotalItemSize.y = s_ThumbnailSize + s_SpaceBetweenTileAndLabel + ( worldUpperLeft.y - worldFontHeight.y );

    CalculateTotalVisibleItems();
}

///////////////////////////////////////////////////////////////////////////////
// Sets m_TotalVisibleItems based upon current view
// 
void ThumbnailView::CalculateTotalVisibleItems()
{
    const wxSize size = GetSize();

    m_TotalVisibleItems.x = ( u32 )( size.x / ( m_Scale * ( m_TotalItemSize.x + s_GapBetweenTiles.x ) ) );
    if ( m_TotalVisibleItems.x == 0 )
    {
        m_TotalVisibleItems.x = 1;
    }

    m_TotalVisibleItems.y = ( u32 )( size.y / ( m_Scale * ( m_TotalItemSize.y + s_GapBetweenTiles.y ) ) );
    if ( m_TotalVisibleItems.y == 0 )
    {
        m_TotalVisibleItems.y = 1;
    }
}

///////////////////////////////////////////////////////////////////////////////
// This control only displays a vertical scroll bar.  This function updates it
// and should be called when the size of the items changes, or the size of the
// control changes.  You must still call Refresh() after calling this function
// to force a redraw.
// 
void ThumbnailView::AdjustScrollBar( bool maintainScrollPos )
{
    // Do the math
    size_t totalItems = m_Results ? m_Results->GetPathsMap().size() : 0;
    u32 totalItemsY = (u32)( ceil( ( float )totalItems / ( float )m_TotalVisibleItems.x ) );
    Math::Vector3 itemSizePixels( 0.0f, m_TotalItemSize.y + s_GapBetweenTiles.y, 0.0f );
    m_World.TransformVertex( itemSizePixels );
    u32 pixelsY = totalItemsY * itemSizePixels.y;
    const u32 extraToPreventLastRowFromBeingChopped = 5;
    u32 logicalStepsY = Math::Round( ( float )pixelsY / ( float )s_ScrollBarIncrement ) + extraToPreventLastRowFromBeingChopped;
    int scrollPosY = 0;
    if ( HasScrollbar( wxVERTICAL ) && maintainScrollPos )
    {
        scrollPosY = GetScrollPos( wxVERTICAL );
    }
    Math::Clamp( scrollPosY, 0, logicalStepsY );

    // Keep the view in line with what we are about to set on the scrollbar
    m_ViewMatrix.t.y = scrollPosY * s_ScrollBarIncrement;

    // Update scrollbars!
    SetScrollbars( 0, s_ScrollBarIncrement, 0, logicalStepsY, 0, scrollPosY, true );
}

///////////////////////////////////////////////////////////////////////////////
// Shows a context menu based on the current selection.
// 
void ThumbnailView::ShowContextMenu( const wxPoint& pos )
{
    // Iterate over the selection so we know what kind of menu to make
    bool inFolder = m_VaultFrame->InFolder();
    size_t numSelected = m_SelectedTiles.Size();

    // Prepare the menu
    wxMenu menu;

    if ( numSelected )
    {
        // Open
        {
            menu.Append( ID_Open, VaultMenu::Label( ID_Open ) );
            menu.AppendSeparator();
        }

        // Perforce
        {
            wxMenu* p4Menu = new wxMenu;
            p4Menu->Append( ID_CheckOut, VaultMenu::Label( ID_CheckOut ) );
            p4Menu->Append( ID_History, VaultMenu::Label( ID_History ) );
            p4Menu->Append( ID_ShowInPerforce, VaultMenu::Label( ID_ShowInPerforce ) );
            wxMenuItem* currentItem = menu.AppendSubMenu( p4Menu, TXT( "Perforce" ) );
            p4Menu->Enable( ID_CheckOut, numSelected > 0 );
            p4Menu->Enable( ID_History, numSelected == 1 );
            p4Menu->Enable( ID_ShowInPerforce, numSelected == 1 );
            menu.Enable( currentItem->GetId(), p4Menu->IsEnabled( ID_CheckOut ) && p4Menu->IsEnabled( ID_History ) && p4Menu->IsEnabled( ID_ShowInPerforce ) );
        }

        // Show In...
        {
            wxMenu* showInMenu = new wxMenu;
            showInMenu->Append( ID_ShowInFolders, VaultMenu::Label( ID_ShowInFolders ) );
            showInMenu->Append( ID_ShowInWindows, VaultMenu::Label( ID_ShowInWindows ) );
            wxMenuItem* currentItem = menu.AppendSubMenu( showInMenu, TXT( "Show In" ) );
            menu.Enable( currentItem->GetId(), numSelected == 1 );
        }

        // Make Collection...
        {
            wxMenu* collectionMenu = new wxMenu;
            collectionMenu->Append( ID_NewCollectionFromSel, VaultMenu::Label( ID_NewCollectionFromSel ) );
            collectionMenu->Append( ID_NewDependencyCollectionFromSel, VaultMenu::Label( ID_NewDependencyCollectionFromSel ) );
            collectionMenu->Append( ID_NewUsageCollectionFromSel, VaultMenu::Label( ID_NewUsageCollectionFromSel ) );
            wxMenuItem* currentItem = menu.AppendSubMenu( collectionMenu, TXT( "Make Collection" ) );
            menu.Enable( currentItem->GetId(), numSelected > 0 );
            collectionMenu->Enable( ID_NewCollectionFromSel, true );
            const bool enableDependencyCollection = numSelected == 1;
            collectionMenu->Enable( ID_NewDependencyCollectionFromSel, enableDependencyCollection );
            collectionMenu->Enable( ID_NewUsageCollectionFromSel, enableDependencyCollection );
        }

        // Copy To Clipboard...
        {
            wxMenu* copyToClipboardMenu = new wxMenu;
            copyToClipboardMenu->Append( ID_CopyPathNative, VaultMenu::Label( ID_CopyPathNative ) );
            copyToClipboardMenu->Append( ID_CopyPath, VaultMenu::Label( ID_CopyPath ) );
            wxMenuItem* currentItem = menu.AppendSubMenu( copyToClipboardMenu, TXT( "Copy To Clipboard" ) );
            menu.Enable( currentItem->GetId(), numSelected > 0 );
        }

    }
    else
    {
        // Thumbnail Size...
        {
            wxMenu* viewMenu = new wxMenu();
            viewMenu->AppendCheckItem( ID_ViewSmall, VaultMenu::Label( ID_ViewSmall ) + TXT( " " ) + ThumbnailSizes::Label( ThumbnailSizes::Small ), VaultMenu::Label( ID_ViewSmall ) );
            viewMenu->AppendCheckItem( ID_ViewMedium, VaultMenu::Label( ID_ViewMedium ) + TXT( " " ) + ThumbnailSizes::Label( ThumbnailSizes::Medium ), VaultMenu::Label( ID_ViewMedium ) );
            viewMenu->AppendCheckItem( ID_ViewLarge, VaultMenu::Label( ID_ViewLarge ) + TXT( " " ) + ThumbnailSizes::Label( ThumbnailSizes::Large ), VaultMenu::Label( ID_ViewLarge ) );
            menu.AppendSubMenu( viewMenu, TXT( "Thumbnail Size" ) );

            // Make sure view option is correct
            menu.Check( ID_ViewLarge, m_Scale == ThumbnailSizes::Large );
            menu.Check( ID_ViewMedium, m_Scale == ThumbnailSizes::Medium );
            menu.Check( ID_ViewSmall, m_Scale == ThumbnailSizes::Small );
        }

        // Sort...
        {
            menu.AppendSeparator();

            wxMenu* sortMenu = new wxMenu();
            sortMenu->AppendCheckItem( ID_SortByName, VaultMenu::Label( ID_SortByName ) );
            sortMenu->AppendCheckItem( ID_SortByType, VaultMenu::Label( ID_SortByType ) );
            sortMenu->AppendSeparator();
            sortMenu->Append( ID_Sort, VaultMenu::Label( ID_Sort ) );
            i32 sortMenuId = menu.AppendSubMenu( sortMenu, TXT( "Arrange Icons By" ) )->GetId();

            sortMenu->Check( ID_SortByName, GetSortMethod() == ThumbnailSortMethods::AlphabeticalByName );
            sortMenu->Check( ID_SortByType, GetSortMethod() == ThumbnailSortMethods::AlphabeticalByType );

            menu.Enable( sortMenuId, m_Results && m_Results->HasResults() );
        }  

        // Refresh
        {
            menu.Append( ID_Refresh, VaultMenu::Label( ID_Refresh ) );
            menu.Enable( ID_Refresh, inFolder );
        }

        // Setting
        {
            menu.AppendSeparator();
            menu.Append( ID_Settings, VaultMenu::Label( ID_Settings ) );
            menu.Enable( ID_Settings, inFolder );
        }

        // New
        {
            menu.AppendSeparator();

            wxMenu* newMenu = m_VaultFrame->GetNewAssetMenu();
            //newMenu->PrependSeparator();
            //newMenu->Prepend( ID_NewFolder, VaultMenu::Label( ID_NewFolder ) );
            //newMenu->Enable( ID_NewFolder, inFolder );

            wxMenuItem* menuItem = new wxMenuItem( &menu, ID_New, VaultMenu::Label( ID_New ), VaultMenu::Label( ID_New ), wxITEM_NORMAL, newMenu );
            menuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_NEW ) );
            menu.Append( menuItem );
            menuItem->Enable( inFolder );
        }

        // Paste
        //{
        //  menu.AppendSeparator();
        //  menu.Append( ID_Paste, VaultMenu::Label( ID_Paste ) + " (Coming Soon!)" );
        //  menu.Enable( ID_Paste, false ); //inFolder ); 
        //}

        // Select All
        {
            menu.AppendSeparator();
            menu.Append( ID_SelectAll, VaultMenu::Label( ID_SelectAll ) );
            menu.Enable( ID_SelectAll, m_Results && m_Results->HasResults() );
        }
    }

    // Properties
    {
        menu.AppendSeparator();
        menu.Append( ID_Properties, VaultMenu::Label( ID_Properties ) );
        menu.Enable( ID_Properties, numSelected > 0 );
    }

    // Show the menu
    PopupMenu( &menu, pos );
}

///////////////////////////////////////////////////////////////////////////////
// Finds the tile that goes with the specified asset file.
// 
ThumbnailTile* ThumbnailView::FindTile( const Helium::Path& path ) const
{
    M_PathToTilePtr::const_iterator found = m_Tiles.find( path );
    if ( found != m_Tiles.end() )
    {
        return found->second;
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Fills out the corners parameter to contain all of the relevant positions
// for the tile.  See the Corners enumeration for values.  Make sure that the
// array is CORNER_COUNT long.  The tile must have a row and column and the
// CalculateTotalItemSize function must have already been called.
// 
void ThumbnailView::GetTileCorners( ThumbnailTile* tile, Math::Vector3 corners[] ) const
{
    corners[ThumbnailTopLeft] = Math::Vector3( tile->GetColumn() * ( m_TotalItemSize.x + s_GapBetweenTiles.x ) + s_GapBetweenTiles.x,  -1.0f * tile->GetRow() * ( m_TotalItemSize.y + s_GapBetweenTiles.y ) - s_GapBetweenTiles.y, 0.0f );
    corners[ThumbnailTopRight] = corners[ThumbnailTopLeft] + Math::Vector3( s_ThumbnailSize, 0.0f, 0.0f );
    corners[ThumbnailBottomLeft] = corners[ThumbnailTopLeft] + Math::Vector3( 0.0f, -s_ThumbnailSize, 0.0f );
    corners[ThumbnailBottomRight] = corners[ThumbnailTopLeft] + Math::Vector3( s_ThumbnailSize, -s_ThumbnailSize, 0.0f );
    corners[LabelTopLeft] = corners[ThumbnailBottomLeft] + Math::Vector3( 0.0f, -s_SpaceBetweenTileAndLabel, 0.0f );
    corners[LabelTopRight] = corners[LabelTopLeft] + Math::Vector3( s_ThumbnailSize, 0.0f, 0.0f );
    corners[LabelBottomLeft] = corners[ThumbnailTopLeft] + Math::Vector3( 0.0f, -m_TotalItemSize.y, 0.0f );
    corners[LabelBottomRight] = corners[LabelBottomLeft] + Math::Vector3( s_ThumbnailSize, 0.0f, 0.0f );
}

///////////////////////////////////////////////////////////////////////////////
// Shows an edit box under the specified thumbnail tile, allowing the user
// to edit the name.
// 
void ThumbnailView::EditTileLabel( ThumbnailTile* tile )
{
    Math::Vector3 corners[CORNER_COUNT];
    GetTileCorners( tile, corners );
    float left = 0.0f;
    float top = 0.0f;
    Math::Vector4 topLeft( corners[LabelTopLeft].x, corners[LabelTopLeft].y, corners[LabelTopLeft].z, 1.0f );
    m_World.Transform( topLeft );
    WorldToScreen( Math::Vector3( topLeft.x, topLeft.y, topLeft.z ), left, top );

    m_EditCtrl->SetValue( tile->GetEditableName() );
    m_EditCtrl->Move( left, top );
    m_EditCtrl->Show();
    m_EditCtrl->SetSelection( -1, -1 );
    m_EditCtrl->SetFocus();
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over the visible tiles and find any that are hit by mousePos1.
// 
void ThumbnailView::Pick( wxPoint mousePos1, wxPoint mousePos2, OS_ThumbnailTiles& hits )
{
    HELIUM_ASSERT( mousePos1 == mousePos2 );
#pragma TODO( "ThumbnailView::Pick - Support rubber-band selection, actually use mousePos2, return more than 1 hit in that case" )

    // Convert mouse position from the screen to object (local space)
    Math::Vector3 localMouse1;
    ViewportToWorldVertex( mousePos1.x, mousePos1.y, localMouse1 );

    Math::Matrix4 inverseWorld( m_World );
    inverseWorld.Invert();
    inverseWorld.TransformVertex( localMouse1 );

    // For each visible tile, get row/column, make bounding box, convert to 
    // world, compare with cursor
    bool done = false;
    OS_ThumbnailTiles::Iterator tileItr = m_VisibleTiles.Begin();
    OS_ThumbnailTiles::Iterator tileEnd = m_VisibleTiles.End();
    for ( ; tileItr != tileEnd && !done; ++tileItr )
    {
        ThumbnailTile* tile = *tileItr;

        Math::Vector3 tileCorners[CORNER_COUNT];
        GetTileCorners( tile, tileCorners );
        if 
            ( 
            tileCorners[TopLeft].x <= localMouse1.x && 
            tileCorners[BottomRight].x >= localMouse1.x &&
            tileCorners[TopLeft].y >= localMouse1.y &&
            tileCorners[BottomRight].y <= localMouse1.y 
            )
        {
            hits.Append( tile );
            done = mousePos1 == mousePos2;
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over the results and draws them.
// 
bool ThumbnailView::Draw()
{
    if ( !m_DeviceManager.TestDeviceReady() )
    {
        return false;
    }

    IDirect3DDevice9* device = m_DeviceManager.GetD3DDevice();

    // Begin Scene
    HRESULT result = S_OK;
    result = device->BeginScene();
    result = device->SetRenderTarget( 0, m_DeviceManager.GetBackBuffer() );
    result = device->SetDepthStencilSurface( m_DeviceManager.GetDepthBuffer() );
    result = device->Clear( NULL, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 255, 80, 80, 80 ), 1.0f, 0 );

    // Camera Transforms
    device->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&m_World );
    device->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)&m_Projection );
    device->SetTransform( D3DTS_VIEW, (D3DMATRIX*)&m_ViewMatrix );

    // Set render state
    device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    device->SetRenderState( D3DRS_LIGHTING, FALSE );
    device->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    device->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
    device->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
    device->SetPixelShader( NULL );
    device->SetVertexShader( NULL );

    // Vertex format
    result = device->SetFVF( ElementFormats[ ElementTypes::PositionUV ] );

    // Visibility testing
    m_VisibleTiles.Clear();
    m_ViewFrustum = Math::Frustum( m_World * m_ViewMatrix * m_Projection );

    m_VisibleTileCorners.clear();
    m_HighlighedTileCorners.clear();
    m_SelectedTileCorners.clear();
    m_RibbonColorTileCorners.clear();
    m_FileTypeTileCorners.clear();

    // If there are any tiles, draw the visible ones
    u32 count = 0;
    for ( ThumbnailIteratorPtr tileItr = m_Sorter.GetIterator(); !tileItr->IsDone(); tileItr->Next(), ++count )
    {
        ThumbnailTile* tile = tileItr->GetCurrentTile();
        u32 row = count / m_TotalVisibleItems.x;
        u32 col = count % m_TotalVisibleItems.x;
        tile->SetRowColumn( row, col );
        DrawTile( device, tile );
    }

    // Ribbons Overlay
    for ( M_RibbonColorTileCorners::iterator itr = m_RibbonColorTileCorners.begin(), end = m_RibbonColorTileCorners.end();
        itr != end; ++itr )
    {
        DWORD ribbonColor = itr->first;
        V_TileCorners& tileCorners = itr->second;
        DrawTileRibbons( device, tileCorners, ribbonColor );
    }

    // Emboss Overlay
    DrawTileOverlays( device, m_VisibleTileCorners, m_TextureOverlay );

    // Highlight Overlay
    DrawTileOverlays( device, m_HighlighedTileCorners, m_TextureHighlighted );

    // Selected Overlay
    DrawTileOverlays( device, m_SelectedTileCorners, m_TextureSelected );

    // FileType Overlay
    for ( M_FileTypeTileCorners::iterator itr = m_FileTypeTileCorners.begin(), end = m_FileTypeTileCorners.end();
        itr != end; ++itr )
    {
        Thumbnail* thumbnail = itr->first;
        V_TileCorners& tileCorners = itr->second;

        DrawTileFileType( device, tileCorners, thumbnail );
    }

    // Request some textures to be loaded
    if ( !m_CurrentTextureRequests.empty() )
    {
        m_ThumbnailManager->Request( m_CurrentTextureRequests );
        m_CurrentTextureRequests.clear();
    }

    device->SetRenderState( D3DRS_LIGHTING, TRUE );
    device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    device->EndScene();

    if ( m_DeviceManager.Display( GetHwnd() ) == D3DERR_DEVICELOST )
    {
        // Device needs to be reset, so tell the caller not to validate this window
        m_DeviceManager.SetDeviceLost();
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Draws a single result tile at the specified position.  Returns true if the
// tile was actually drawn, false if the file is outside of the view frustum.
// 
void ThumbnailView::DrawTile( IDirect3DDevice9* device, ThumbnailTile* tile, bool overlayOnly )
{
    // Calculate position
    Math::Vector3 tileCorners[CORNER_COUNT];
    GetTileCorners( tile, tileCorners );
    PositionUV corners[4] =
    {
        PositionUV( tileCorners[ThumbnailBottomLeft],   Math::Vector2( 0, 1 ) ),
        PositionUV( tileCorners[ThumbnailTopLeft],      Math::Vector2( 0, 0 ) ),
        PositionUV( tileCorners[ThumbnailBottomRight],  Math::Vector2( 1, 1 ) ),
        PositionUV( tileCorners[ThumbnailTopRight],     Math::Vector2( 1, 0 ) ),
    };

    // Decide if we really need to draw this tile
    Math::AlignedBox box( corners[0].m_Position, corners[3].m_Position );
    if ( m_ViewFrustum.IntersectsBox( box ) )
    {
        // Draw screenshot
        device->SetTexture( 0, tile->GetThumbnail()->GetTexture() );
        HRESULT result = device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, &corners[0], ElementSizes[ ElementTypes::PositionUV ] );
        //HELIUM_ASSERT( SUCCEEDED( result ) );
        device->SetTexture( 0, NULL );

        // Keep track of which tiles are visible
        m_VisibleTiles.Append( tile );

        // Add to overlay lists
        m_VisibleTileCorners.push_back( tileCorners[ThumbnailTopLeft] );
        if ( tile->IsHighlighed() )
        {
            m_HighlighedTileCorners.push_back( tileCorners[ThumbnailTopLeft] );
        }
        else if ( tile->IsSelected() )
        {
            m_SelectedTileCorners.push_back( tileCorners[ThumbnailTopLeft] );
        }

        // Get the ribbon color for files
        if ( tile->GetPath().IsFile() )
        {
            DWORD ribbonColor = 0;
            if ( tile->GetTypeColor( ribbonColor ) )
            {
                Helium::Insert<M_RibbonColorTileCorners>::Result inserted = m_RibbonColorTileCorners.insert( M_RibbonColorTileCorners::value_type( ribbonColor, V_TileCorners() ) );
                inserted.first->second.push_back( tileCorners[ThumbnailTopLeft] );
            }
            else
            {
                M_FileTypeColors::iterator findColor = m_FileTypeColors.find( tile->GetPath().FullExtension() );
                if ( findColor != m_FileTypeColors.end() )
                {
                    Helium::Insert<M_RibbonColorTileCorners>::Result inserted = m_RibbonColorTileCorners.insert( M_RibbonColorTileCorners::value_type( findColor->second, V_TileCorners() ) );
                    inserted.first->second.push_back( tileCorners[ThumbnailTopLeft] );
                }
            }

            // FileType Overlay
                M_FileTypeIcons::iterator findIcon = m_FileTypeIcons.find( tile->GetPath().FullExtension() );
                if ( findIcon != m_FileTypeIcons.end() )
                {
                    Helium::Insert<M_FileTypeTileCorners>::Result inserted = m_FileTypeTileCorners.insert( M_FileTypeTileCorners::value_type( findIcon->second, V_TileCorners() ) );
                    inserted.first->second.push_back( tileCorners[ThumbnailTopLeft] );
                }
                else if ( tile->GetPath().Extension() == Reflect::Archive::GetExtension( Reflect::ArchiveTypes::Binary )
                    && ( findIcon = m_FileTypeIcons.find( Reflect::Archive::GetExtension( Reflect::ArchiveTypes::Binary ) ) ) != m_FileTypeIcons.end() )
                {
                    Helium::Insert<M_FileTypeTileCorners>::Result inserted = m_FileTypeTileCorners.insert( M_FileTypeTileCorners::value_type( findIcon->second, V_TileCorners() ) );
                    inserted.first->second.push_back( tileCorners[ThumbnailTopLeft] );
                }
            }

        if ( tile->GetThumbnail() == m_TextureLoading )
        {
            // Keep track of which textures that need to be loaded
            m_CurrentTextureRequests.insert( tile->GetPath() );
        }

        // Draw label
        tstring label = tile->GetLabel();
        if ( !label.empty() )
        {
            float left = 0.0f;
            float top = 0.0f;
            Math::Vector4 topLeft( tileCorners[LabelTopLeft].x, tileCorners[LabelTopLeft].y, tileCorners[LabelTopLeft].z, 1.0f );
            m_World.Transform( topLeft );
            WorldToScreen( Math::Vector3( topLeft.x, topLeft.y, topLeft.z ), left, top );

            float right = 0.0f;
            float unused = 0.0f;
            Math::Vector4 bottomRight( tileCorners[LabelBottomRight].x, tileCorners[LabelBottomRight].y, tileCorners[LabelBottomRight].z, 1.0f );
            m_World.Transform( bottomRight );
            WorldToScreen( Math::Vector3( bottomRight.x, bottomRight.y, bottomRight.z ), right, unused );

            RECT rect;
            rect.top = top;
            rect.left = left;
            rect.right = right;
            rect.bottom = top + m_LabelFontHeight;
            LPD3DXSPRITE sprite = NULL;
#pragma TODO( "ThumbnailView::DrawTile - Create a sprite object to improve efficiency (see DirectX docs)" )
            RECT calcRect( rect );

            if ( tile->IsSelected() )
            {
                device->ColorFill( m_DeviceManager.GetBackBuffer(), &rect, s_TextColorBGSelected );
            }

            //tile->IsSelected() ? s_TextColorBGSelected : s_TextColorDefault
            m_LabelFont->DrawText( sprite, label.c_str(), -1, &calcRect, DT_CALCRECT, s_TextColorDefault );
            u32 flags = 0;
            if ( calcRect.right <= rect.right )
            {
                flags = DT_CENTER;
            }
            result = m_LabelFont->DrawText( sprite, label.c_str(), -1, &rect, flags, s_TextColorDefault );
            //HELIUM_ASSERT( SUCCEEDED( result ) );
        }

        // Draw type name on top of the thumbnail image
        tstring typeLabel = tile->GetTypeLabel();
        if ( !typeLabel.empty() )
        {
            float left = 0.0f;
            float top = 0.0f;
            Math::Vector4 topLeft( tileCorners[ThumbnailTopLeft].x, tileCorners[ThumbnailTopLeft].y, tileCorners[ThumbnailTopLeft].z, 1.0f );
            m_World.Transform( topLeft );
            WorldToScreen( Math::Vector3( topLeft.x, topLeft.y, topLeft.z ), left, top );

            float right = 0.0f;
            float bottom = 0.0f;
            Math::Vector4 bottomRight( tileCorners[ThumbnailBottomRight].x, tileCorners[ThumbnailBottomRight].y, tileCorners[ThumbnailBottomRight].z, 1.0f );
            m_World.Transform( bottomRight );
            WorldToScreen( Math::Vector3( bottomRight.x, bottomRight.y, bottomRight.z ), right, bottom );

            RECT rect;
            rect.top = top + 2;
            rect.left = left;
            rect.right = right;
            rect.bottom = bottom;
            LPD3DXSPRITE sprite = NULL;

            DWORD color = s_TextColorDefault;
            if ( tile->GetThumbnail()->IsFromIcon() )
            {
                color = s_TextColorDark;
            }
            result = m_LabelFont->DrawText( sprite, typeLabel.c_str(), -1, &rect, DT_CENTER, color );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void ThumbnailView::DrawTileOverlays( IDirect3DDevice9* device, V_TileCorners& tileCorners, Thumbnail* thumbnail )
{
    HRESULT result = S_OK;

    device->SetTexture( 0, thumbnail->GetTexture() );
    for ( V_TileCorners::iterator cornerItr = tileCorners.begin(), cornerEnd = tileCorners.end();
        cornerItr != cornerEnd; ++cornerItr )
    {
        Math::Vector3 thumbnailTopLeft = (*cornerItr);

        PositionUV corners[4] =
        {
            PositionUV( thumbnailTopLeft + Math::Vector3( 0.0f, -s_ThumbnailSize, 0.0f ), Math::Vector2( 0, 1 ) ),            // BottomLeft
            PositionUV( thumbnailTopLeft, Math::Vector2( 0, 0 ) ),                                                            // TopLeft
            PositionUV( thumbnailTopLeft + Math::Vector3( s_ThumbnailSize, -s_ThumbnailSize, 0.0f ), Math::Vector2( 1, 1 ) ), // BottomRight
            PositionUV( thumbnailTopLeft + Math::Vector3( s_ThumbnailSize, 0.0f, 0.0f ), Math::Vector2( 1, 0 ) ),             // TopRight
        };
        result = device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, &corners[0], ElementSizes[ ElementTypes::PositionUV ] );
        //HELIUM_ASSERT( SUCCEEDED( result ) );
    }
    device->SetTexture( 0, NULL );
}

///////////////////////////////////////////////////////////////////////////////
void ThumbnailView::DrawTileRibbons( IDirect3DDevice9* device, V_TileCorners& tileCorners, DWORD color )
{
    HRESULT result = S_OK;

    result = device->SetFVF( ElementFormats[ ElementTypes::PositionColored ] );

    for ( V_TileCorners::iterator cornerItr = tileCorners.begin(), cornerEnd = tileCorners.end();
        cornerItr != cornerEnd; ++cornerItr )
    {
        Math::Vector3 thumbnailTopLeft = (*cornerItr);

        PositionColored vertices[3] =
        {
            PositionColored( thumbnailTopLeft + Math::Vector3( 0.0f, -s_ThumbnailSize, 0.0f ) + Math::Vector3( s_ThumbnailSize * 0.75f, 0.0f, 0.0f ), color ), // BottomLeft
            PositionColored( thumbnailTopLeft + Math::Vector3( s_ThumbnailSize, 0.0f, 0.0f ) + Math::Vector3( 0.0f, -s_ThumbnailSize * 0.75f, 0.0f ), color ), // TopRight
            PositionColored( thumbnailTopLeft + Math::Vector3( s_ThumbnailSize, -s_ThumbnailSize, 0.0f ), color ), // BottomRight
        };
        result = device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 1, &vertices[0], ElementSizes[ ElementTypes::PositionColored ] );
        //HELIUM_ASSERT( SUCCEEDED( result ) );
    }
    device->SetTexture( 0, NULL );

    result = device->SetFVF( ElementFormats[ ElementTypes::PositionUV ] );
}

///////////////////////////////////////////////////////////////////////////////
void ThumbnailView::DrawTileFileType( IDirect3DDevice9* device, V_TileCorners& tileCorners, Thumbnail* thumbnail )
{
    if ( m_Scale < 35.0f )
        return;

    HRESULT result = S_OK;



    // conver hieght
    Math::Matrix4 inverseWorld( m_World );
    inverseWorld.Invert();

    Math::Vector3 paddingWidth( 3.0f, 0.0f, 0.0f );
    inverseWorld.TransformVertex( paddingWidth );

    float padding = s_ThumbnailSize * 0.05f;
    Math::Vector3 bottomRightOffset = Math::Vector3( s_ThumbnailSize, -s_ThumbnailSize, 0.0f ) + Math::Vector3( -padding, padding, 0.0f ) + Math::Vector3( paddingWidth.x, 0.0f, 0.0f );

    // File Overlay
    {
        Math::Vector3 emptyFileSize( 32.0f, 32.0f, 0.0f );
        inverseWorld.TransformVertex( emptyFileSize );

        device->SetTexture( 0, m_TextureBlankFile->GetTexture() );
        for ( V_TileCorners::iterator cornerItr = tileCorners.begin(), cornerEnd = tileCorners.end();
            cornerItr != cornerEnd; ++cornerItr )
        {
            Math::Vector3 thumbnailTopLeft = (*cornerItr);
            Math::Vector3 thumbnailBottomRight = thumbnailTopLeft + bottomRightOffset;

            // we're going to calculate everything from the bottom right:
            PositionUV corners[4] =
            {
                PositionUV( thumbnailBottomRight + Math::Vector3( -emptyFileSize.x, 0.0f, 0.0f ), Math::Vector2( 0, 1 ) ), // BottomLeft
                PositionUV( thumbnailBottomRight + Math::Vector3( -emptyFileSize.x, emptyFileSize.y, 0.0f ), Math::Vector2( 0, 0 ) ), // TopLeft
                PositionUV( thumbnailBottomRight, Math::Vector2( 1, 1 ) ), // BottomRight
                PositionUV( thumbnailBottomRight + Math::Vector3( 0.0f, emptyFileSize.y, 0.0f ), Math::Vector2( 1, 0 ) ), // TopRight
            };

            result = device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, &corners[0], ElementSizes[ ElementTypes::PositionUV ] );
            //HELIUM_ASSERT( SUCCEEDED( result ) );
        }
        device->SetTexture( 0, NULL );
    }


    // FileType Icon Overlay
    {
        Math::Vector3 fileTypeHeight( 16.0f, 16.0f, 0.0f );
        inverseWorld.TransformVertex( fileTypeHeight );

        Math::Vector3 fileTypeBottomRightOffset = bottomRightOffset + Math::Vector3( -fileTypeHeight.x * 0.5f, fileTypeHeight.y * 0.5f, 0.0f );

        device->SetTexture( 0, thumbnail->GetTexture() );
        for ( V_TileCorners::iterator cornerItr = tileCorners.begin(), cornerEnd = tileCorners.end();
            cornerItr != cornerEnd; ++cornerItr )
        {
            Math::Vector3 thumbnailTopLeft = (*cornerItr);
            Math::Vector3 thumbnailBottomRight = thumbnailTopLeft + fileTypeBottomRightOffset;

            // we're going to calculate everything from the bottom right:
            PositionUV corners[4] =
            {
                PositionUV( thumbnailBottomRight + Math::Vector3( -fileTypeHeight.x, 0.0f, 0.0f ), Math::Vector2( 0, 1 ) ), // BottomLeft
                PositionUV( thumbnailBottomRight + Math::Vector3( -fileTypeHeight.x, fileTypeHeight.y, 0.0f ), Math::Vector2( 0, 0 ) ), // TopLeft
                PositionUV( thumbnailBottomRight, Math::Vector2( 1, 1 ) ), // BottomRight
                PositionUV( thumbnailBottomRight + Math::Vector3( 0.0f, fileTypeHeight.y, 0.0f ), Math::Vector2( 1, 0 ) ), // TopRight
            };

            result = device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, &corners[0], ElementSizes[ ElementTypes::PositionUV ] );
            //HELIUM_ASSERT( SUCCEEDED( result ) );
        }
        device->SetTexture( 0, NULL );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Draw to the D3D device during paint messages.
// 
void ThumbnailView::OnPaint( wxPaintEvent& args )
{
    if ( Draw() )
    {
        ::ValidateRect( ( HWND )GetHandle(), NULL );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Resize the D3D device when a size event occurrs.
// 
void ThumbnailView::OnSize( wxSizeEvent& args )
{
    if ( !m_DeviceManager.GetD3DDevice() )
    {
        return;
    }

    if ( args.GetSize().x > 0 && args.GetSize().y > 0 )
    {
        UpdateProjectionMatrix();
        CalculateTotalVisibleItems();
        AdjustScrollBar( true );
        m_DeviceManager.Resize( args.GetSize().x, args.GetSize().y );
    }

    Refresh();
}

///////////////////////////////////////////////////////////////////////////////
// Handle key presses for hotkeys.
// 
void ThumbnailView::OnKeyDown( wxKeyEvent& args )
{
    switch ( args.GetKeyCode() )
    {
    case wxT('A'):
        if ( args.GetModifiers() == wxMOD_CONTROL )
        {
            // Seletct All
            wxCommandEvent evt( wxEVT_COMMAND_MENU_SELECTED, ID_SelectAll );
            evt.SetEventObject( this );
            GetEventHandler()->ProcessEvent( evt );
        }
        else
        {
            args.Skip();
        }
        break;

    default:
        args.Skip();
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Handle the mouse wheel
// 
void ThumbnailView::OnMouseWheel( wxMouseEvent& args )
{
    if ( args.ControlDown() )
    {
        SetZoom( m_Scale + ( ( float )args.GetWheelRotation() / ( float )args.GetWheelDelta() * 2.0f ) );
    }
    else
    {
        args.Skip();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Shared logic in the mouse move and mouse up events.  Looks at the tile that
// was clicked at and determines if any additional selection changes are needed
// based upon the state of any modifier keys (ctrl), and whether or not the 
// item that was clicked on was already selected.
// 
void ThumbnailView::MouseSelectionHelper()
{
    ThumbnailTile* onlySelectedTile = NULL;
    if ( m_SelectedTiles.Size() == 1 )
    {
        onlySelectedTile = m_SelectedTiles.Front();
    }

    bool selectionChanged = false;

    // If the control key was not down and the tile is not the only one that is 
    // selected, clear selection
    if ( !m_CtrlOnMouseDown && m_MouseDownTile != onlySelectedTile )
    {
        selectionChanged |= ClearSelection();
    }

    // Toggle the selection if the control key was down, otherwise just 
    // select the tile
    selectionChanged |= SetSelection( m_MouseDownTile, m_CtrlOnMouseDown ? !m_MouseDownTile->IsSelected() : true );

    if ( selectionChanged )
    {
        Refresh();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Mouse movement - track mouse, drag items, rubber-band select (eventually)
// 
void ThumbnailView::OnMouseMove( wxMouseEvent& args )
{
    args.Skip();

    if ( m_MouseDown )
    {
        // Determine if a drag operation is starting based on amount of movement
        wxPoint diff = args.GetPosition() - m_MouseDownLoc;
        if ( abs( diff.x ) > s_MouseTolerance || abs( diff.y ) > s_MouseTolerance )
        {
            // Handle any last minute selection before starting the drag
            if ( m_MouseDownTile )
            {
                if ( !m_MouseDownTile->IsSelected() )
                {
                    MouseSelectionHelper();
                }
            }

            // Build the list of items to drag and drop
            bool doDrag = false;
            wxFileDataObject clipboardData;
            for ( OS_ThumbnailTiles::Iterator tileItr = m_SelectedTiles.Begin(),
                tileEnd = m_SelectedTiles.End(); tileItr != tileEnd; ++tileItr )
            {
                ThumbnailTile* tile = *tileItr;
                if ( !tile->GetPath().empty() )
                {
                    clipboardData.AddFile( tile->GetPath().Get() );
                    doDrag = true;
                }
            }

            // Drag and drop time
            if ( doDrag )
            {
                Inspect::DropSource source( clipboardData, this );
                source.SetAutoRaise( true );
                source.DoDragDrop( wxDrag_DefaultMove );
                m_MouseDown = false;
            }
        }
    }

    if ( !m_MouseDown || m_SelectedTiles.Empty() )
    {
        // The mouse is not down, just track the item to highlight
        OS_ThumbnailTiles hits;
        Pick( args.GetPosition(), args.GetPosition(), hits );
        if ( hits.Size() > 0 )
        {
            ThumbnailTile* hit = hits.Front();
            Highlight( hit );
        }
        else
        {
            ClearHighlight();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Track the items that was clicked on.  Most selection actually takes place
// during mouse up.
// 
void ThumbnailView::OnMouseLeftDown( wxMouseEvent& args )
{
    args.Skip();

    SetFocusIgnoringChildren();

    m_MouseDown = true;
    m_MouseDownLoc = args.GetPosition();
    m_MouseDownTile = NULL;
    m_CtrlOnMouseDown = args.ControlDown();

    OS_ThumbnailTiles hits;
    ThumbnailTile* hit = NULL;
    Pick( args.GetPosition(), args.GetPosition(), hits );
    if ( hits.Size() > 0 )
    {
        m_MouseDownTile = hits.Front();

        if ( args.ShiftDown() && !m_CtrlOnMouseDown )
        {
            ThumbnailTile* first = m_RangeSelectTile;
            ThumbnailTile* second = m_MouseDownTile;

            if ( first )
            {
                if ( m_Sorter.Compare( first, second ) > 0 )
                {
                    first = m_MouseDownTile;
                    second = m_RangeSelectTile;
                }
            }

            ClearSelection();
            SelectRange( first, second );
            Refresh();

            m_MouseDownTile = NULL;
        }
    }
    else if ( !m_CtrlOnMouseDown )
    {
        if ( ClearSelection() )
        {
            Refresh();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Stop tracking the mouse for drag and drop and handle selection changes.
// 
void ThumbnailView::OnMouseLeftUp( wxMouseEvent& args )
{
    args.Skip();
    m_MouseDown = false;

    if ( m_MouseDownTile )
    {
        wxPoint diff = args.GetPosition() - m_MouseDownLoc;
        if ( abs( diff.x ) <= s_MouseTolerance || abs( diff.y ) <= s_MouseTolerance )
        {
            MouseSelectionHelper();
        }
    }

    if ( m_SelectedTiles.Size() == 1 )
    {
        m_RangeSelectTile = m_SelectedTiles.Front();
    }

    m_MouseDownTile = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Open folders when they are double-clicked on.
// 
void ThumbnailView::OnMouseLeftDoubleClick( wxMouseEvent& args )
{
    args.Skip();

    SetFocusIgnoringChildren();

    OS_ThumbnailTiles hits;
    ThumbnailTile* hit = NULL;
    Pick( args.GetPosition(), args.GetPosition(), hits );
    if ( !hits.Empty() )
    {
        hit = hits.Front();
        if ( hit->GetPath().IsDirectory() )
        {
            m_VaultFrame->Search( hit->GetPath().Get() );
        }
        else
        {
            wxCommandEvent evt( wxEVT_COMMAND_MENU_SELECTED, ID_Open );
            evt.SetEventObject( this );
            wxPostEvent( GetEventHandler(), evt );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Show context menu options on right-clicks.
// 
void ThumbnailView::OnMouseRightDown( wxMouseEvent& args )
{
    args.Skip();

    OS_ThumbnailTiles hits;
    Pick( args.GetPosition(), args.GetPosition(), hits );

    if ( hits.Empty() )
    {
        ClearSelection();
    }
    else
    {
        HELIUM_ASSERT( hits.Size() == 1 );
        ThumbnailTile* hit = hits.Front().Ptr();
        if ( !args.ControlDown() && !args.ShiftDown() && !hit->IsSelected() )
        {
            ClearSelection();
        }
        Select( hit );
    }

    if ( m_SelectedTiles.Size() == 1 )
    {
        m_RangeSelectTile = m_SelectedTiles.Front();
    }

    // This will cause a Refresh as well
    ClearHighlight();

    ShowContextMenu( args.GetPosition() );
}

///////////////////////////////////////////////////////////////////////////////
// Clear highlights and restore status text.
// 
void ThumbnailView::OnMouseLeave( wxMouseEvent& args )
{
    args.Skip();

    ClearHighlight();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for any scroll events.
// 
void ThumbnailView::OnScrollEvent( wxScrollWinEvent& args )
{
    // We are going to handle this event
    args.Skip( false ); 

    SetFocusIgnoringChildren();

    if ( args.GetOrientation() != wxVERTICAL )
    {
        // We are assuming that there is only vertical scrolling in this function
        return;
    }

    // Figure out how much to scroll by and scroll
    i32 amount = GetScrollHelper()->CalcScrollInc( args );
    if ( amount != 0 )
    {
        i32 current = GetScrollPos( wxVERTICAL );
        amount += current;
        Math::Clamp( amount, 0, amount );
        Scroll( 0, amount );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Selects all the results that are visible.
// 
void ThumbnailView::OnSelectAll( wxCommandEvent& args )
{
    ThumbnailIteratorPtr iterator = m_Sorter.GetIterator();
    if ( SelectRange( iterator->GetCurrentTile(), iterator->GetLastTile() ) )
    {
        Refresh();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Sort the results alphabetically by label (or path if labels are the same).
// 
void ThumbnailView::OnSortAlphabetical( wxCommandEvent& args )
{
    Sort( ThumbnailSortMethods::AlphabeticalByName );
}

///////////////////////////////////////////////////////////////////////////////
// Sort the results in alphabetical order by file type.
// 
void ThumbnailView::OnSortByType( wxCommandEvent& args )
{
    Sort( ThumbnailSortMethods::AlphabeticalByType );
}

///////////////////////////////////////////////////////////////////////////////
// Sort the results right now using the current sort method.
// 
void ThumbnailView::OnSort( wxCommandEvent& args )
{
    Sort( GetSortMethod() );
}

///////////////////////////////////////////////////////////////////////////////
// Show a window with additional information on the selected file(s).
// 
void ThumbnailView::OnFileProperties( wxCommandEvent& args )
{
    std::set< Helium::Path > paths;
    GetSelectedPaths( paths );
    if ( !paths.empty() )
    {
        if ( paths.size() > 5 )
        {
            tstringstream message;
            message << TXT( "Are you sure that you want to show the properties for all " ) << paths.size() << TXT( " selected paths?" );
            i32 result = wxMessageBox( message.str(), TXT( "Show Details?" ), wxCENTER | wxYES_NO | wxICON_QUESTION, this );
            if ( result != wxYES )
            {
                return;
            }
        }

        for ( std::set< Helium::Path >::const_iterator fileItr = paths.begin(), fileEnd = paths.end();
            fileItr != fileEnd; ++fileItr )
        {
            DetailsFrame* detailsWindow = new DetailsFrame( m_VaultFrame );
            detailsWindow->Populate( *fileItr );
            detailsWindow->Show();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for a rename event from a context menu.
// 
void ThumbnailView::OnRename( wxCommandEvent& args )
{
    if ( m_SelectedTiles.Size() == 1 )
    {
        ThumbnailTile* tile = m_SelectedTiles.Front();

        if ( !tile->GetPath().IsFile() )
        {
            return;
        }

        if ( true )//File::GlobalManager().ValidateCanDeleteFile( tile->GetFile()->GetFilePath() ) )
        {
            EditTileLabel( tile );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the control for editing a name loses focus.  Hides the edit
// control.
// 
void ThumbnailView::OnEditBoxLostFocus( wxFocusEvent& args )
{
    m_EditCtrl->Hide();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the enter button is pressed in the edit box.
// 
void ThumbnailView::OnEditBoxPressEnter( wxCommandEvent& args )
{
    // TODO: actually rename the file/folder
    m_EditCtrl->Hide();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a texture is loaded.  
// 
void ThumbnailView::OnThumbnailLoaded( Editor::ThumbnailLoadedEvent& args )
{
    ThumbnailTile* tile = FindTile( args.GetPath() );
    if ( tile )
    {
        if ( !args.GetThumbnails().empty() )
        {
            tile->SetThumbnail( *args.GetThumbnails().begin() );
        }
        else
        {
            // the extension is used to identify this type of file
            tstring extension = args.GetPath().Extension();
            toLower( extension );

            // look for a cached thumbnail for this extension
            std::map<tstring, ThumbnailPtr>::const_iterator found = m_AssociatedIcons.find( extension );
            if ( found != m_AssociatedIcons.end() )
            {
                // we got it, just share it
                tile->SetThumbnail( found->second );
            }
            else // load the file associated icon from the shell
            {
                WORD index = 0;

                tstring native = args.GetPath().Native();

                // get the icon resource for this example file
                tchar path[MAX_PATH];
                _tcscpy( path, native.c_str() );
                HICON icon = ExtractAssociatedIcon( NULL, path, &index );

                // if we got the resource
                if ( icon )
                {
                    // check to see if its the fallback in shell32.dll
                    tchar file[MAX_PATH];
                    _tsplitpath( path, NULL, NULL, file, NULL );

                    // if its windows' stub icon, don't bother using it
                    if ( _tcsicmp( file, TXT( "SHELL32" ) ) )
                    {
                        // build a thumbnail texture from the icon resource
                        ThumbnailPtr thumb = new Thumbnail( &m_DeviceManager );
                        if ( thumb->FromIcon( icon ) )
                        {
                            // cache it for re-use
                            m_AssociatedIcons[ extension ] = thumb;

                            // set this file's thumb
                            tile->SetThumbnail( thumb );
                        }
                        else
                        {
                            // there is no decent associated icon, so juse use missing
                            m_AssociatedIcons[ extension ] = m_TextureMissing;
                        }
                    }
                }

                // if we don't have a thumbnail use the fallback
                if ( !tile->GetThumbnail() )
                {
                    tile->SetThumbnail( m_TextureMissing );
                }
            }
        }

        if ( IsVisible( tile ) )
        {
            Refresh();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback before the browser frame closes.  Attempts to prevent a race condition
// where the thumbnail loader posts a message to this window while it is closing.
// 
void ThumbnailView::OnVaultFrameClosing( wxCloseEvent& args )
{
    args.Skip();
    m_ThumbnailManager->DetachFromWindow();
    m_ThumbnailManager->Cancel();
}

///////////////////////////////////////////////////////////////////////////////
// If the device is lost, clean up any D3D resources that we created.
// 
void ThumbnailView::OnReleaseResources( const Render::DeviceStateArgs& args )
{
    DeleteResources();

    // Our view is now corrupt, post a paint message and hopefully we can redraw
    // during the next event update.
    wxPostEvent( GetEventHandler(), wxPaintEvent() );
}

///////////////////////////////////////////////////////////////////////////////
// If the device is found, reset any D3D resources that are needed.
// 
void ThumbnailView::OnAllocateResources( const Render::DeviceStateArgs& args )
{
    CreateResources();
}
