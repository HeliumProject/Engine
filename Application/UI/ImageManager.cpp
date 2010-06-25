#include "ImageManager.h"

#include "Foundation/Exception.h"
#include "Foundation/Container/Insert.h" 
#include "Foundation/File/Directory.h"
#include "Foundation/File/Path.h"
using Nocturnal::Insert; 

#include "Foundation/Log.h"

#include <sstream>

#include <wx/image.h>

namespace Nocturnal
{
    static ImageManager* g_GlobalImageManager = NULL;

    int g_InitCount = 0;

    const int g_IconSizes[IconSizes::Count] = { 16, 32, 64, 128 };

    const tchar* g_IconSizeSpecs[IconSizes::Count] = 
    { 
        TXT( "*_16.png" ),
        TXT( "*_32.png" ),
        TXT( "*_64.png" ),
        TXT( "*_128.png" ),
    };

    IconSize GetIconSize( const tstring& fileName )
    {
        if ( fileName.find( TXT( "16" ) ) )
        {
            return IconSizes::Size16;
        }
        else if ( fileName.find( TXT( "32" ) ) )
        {
            return IconSizes::Size32;
        }
        else if ( fileName.find( TXT( "64" ) ) )
        {
            return IconSizes::Size64;
        }
        else if ( fileName.find( TXT( "128" ) ) )
        {
            return IconSizes::Size128;
        }

        return IconSizes::Size16;
    }

    bool ImageManagerInit( const tstring& defaultFolder, const tstring& themeFolder )
    {
        if ( ++g_InitCount == 1 ) 
        {
            // ImageHandlers:
            //    wxWidgets normally doesn't link in image handlers for all types of images,
            //    in order to be a bit more efficient.  Consequently, you have to initialize
            //    and add image handlers.  You can see how it is done for each type in the
            //    demo in MyApp.OnInit.  Or you can call wxInitAllImageHandlers() to do them
            //    all.  However, there is a limitation to doing them all.  TGA files may be
            //    handled by the wxCURHandler instead of the wxTGAHandler, simply because that
            //    handler appears in the list before TGA when you init them all at once.
            //wxImage::AddHandler( new wxJPEGHandler );
            //wxImage::AddHandler( new wxPNGHandler );
            wxInitAllImageHandlers();

            wxImageHandler* curHandler = wxImage::FindHandler( wxBITMAP_TYPE_CUR );
            if ( curHandler )
            {
                // Force the cursor handler to the end of the list so that it doesn't try to
                // open TGA files.
                wxImage::RemoveHandler( curHandler->GetName() );
                curHandler = NULL;
                wxImage::AddHandler( new wxCURHandler );
            }

            g_GlobalImageManager = new ImageManager( defaultFolder, themeFolder );
        }
        return true;
    }

    void ImageManagerCleanup()
    {
        if ( --g_InitCount == 0 )
        {

            delete g_GlobalImageManager;
            g_GlobalImageManager = NULL;

            wxImage::CleanUpHandlers(); 
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // Returns a reference to the static global Manager theGlobalManager.
    //
    ImageManager &GlobalImageManager()
    {
        if ( !g_GlobalImageManager )
        {
            throw Nocturnal::Exception( TXT( "GlobalImageManager is not initialized, must call Nocturnal::ImageManagerInit()" ) );
        }

        return *g_GlobalImageManager;
    }



    ///////////////////////////////////////////////////////////////////////////////
    //   
    ImageManager::ImageManager( const tstring& defaultFolder, const tstring& themeFolder )
        : m_DefaultFolder( defaultFolder )
        , m_ThemeFolder( themeFolder )
    {
        m_ExtensionType.insert( M_ExtensionType::value_type( TXT( ".bmp" ), wxBITMAP_TYPE_BMP ) );
        m_ExtensionType.insert( M_ExtensionType::value_type( TXT( ".gif" ), wxBITMAP_TYPE_GIF ) );
        m_ExtensionType.insert( M_ExtensionType::value_type( TXT( ".jpg" ), wxBITMAP_TYPE_JPEG ) );
        m_ExtensionType.insert( M_ExtensionType::value_type( TXT( ".png" ), wxBITMAP_TYPE_PNG ) );
        m_ExtensionType.insert( M_ExtensionType::value_type( TXT( ".tga" ), wxBITMAP_TYPE_TGA ) );
    }

    ///////////////////////////////////////////////////////////////////////////////
    //   
    ImageManager::~ImageManager()
    {
        // we don't need to "clear" anything here because 
        // our member destructors will do it for us. 
    }



    ///////////////////////////////////////////////////////////////////////////////
    // 
    // wxBitmap Functions
    //
    ///////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////////
    // Gets the full path to an image file from a relative path
    // 
    void ImageManager::GetFullImagePath( const tstring& partialPath, tstring& fullPath )
    {
        // they may have passed in a full path
        if ( partialPath.length() > 1 && partialPath[1] == ':' )
        {
            fullPath = partialPath;
        }
        else
        {
            if ( !m_ThemeFolder.empty() )
            {
                fullPath = Nocturnal::Path( m_ThemeFolder + '/' + partialPath ).Get();
            }

            // then use the default path
            if ( !Nocturnal::Path( fullPath ).Exists() )
            {
                fullPath = Nocturnal::Path( m_DefaultFolder + '/' + partialPath ).Get();
            }
        }
    }



    ///////////////////////////////////////////////////////////////////////////////
    // Returns a reference to the bitmap with the specified name.  If the bitmap
    // with that name has not yet been loaded, it will be loaded by calling this
    // function.
    // 
    const wxBitmap& ImageManager::GetBitmap( const tstring& fileName, long type )
    {
        long tryType = type;
        if ( tryType == wxBITMAP_TYPE_ANY )
        {
            tryType = GetBitmapLoadType( fileName );
        }

        bool exists = false;
        M_Bitmap::iterator found = m_Bitmaps.find( fileName );
        if ( found != m_Bitmaps.end() )
        {
            exists = true;
            if ( !found->second.m_Path.ChangedSince( found->second.m_PathLastUpdated ) )
            {
                return found->second.m_Bitmap;
            }
        }

        tstring fullPath;
        GetFullImagePath( fileName, fullPath );
        if ( !Nocturnal::Path( fullPath ).Exists() )
        {
            Log::Warning( TXT( "Bitmap file does not exist %s\n" ), fullPath.c_str() );
        }

        if ( !exists )
        {
            BitmapFileInfo info( fullPath );

            Insert<M_Bitmap>::Result inserted = m_Bitmaps.insert( M_Bitmap::value_type ( fileName, info) );
            if ( !inserted.first->second.m_Bitmap.LoadFile( fullPath.c_str(), tryType ) )
            {
                Log::Warning( TXT( "Unable to load GUI image %s\n" ), fileName.c_str() );
                return inserted.first->second.m_Bitmap;
            }

            return inserted.first->second.m_Bitmap;
        }
        else
        {
            found->second.m_Bitmap.LoadFile( found->second.m_Path.c_str(), tryType );
            return found->second.m_Bitmap;
        }
    }


    ///////////////////////////////////////////////////////////////////////////////
    // Returns a reference to the bitmap with the specified name.  If the bitmap
    // with that name has not yet been loaded, it will be loaded by calling this
    // function.
    // 
    const wxBitmap& ImageManager::GetScaledBitmap( const tstring& fileName, int maxSize, long type )
    {
        Nocturnal::Path keyFile( fileName );
        tstring ext = keyFile.Extension();

        keyFile.ReplaceExtension( TXT( "" ) );

        tstringstream keyStream;
        keyStream << keyFile.Get() << maxSize << ext;
        keyFile.Set( keyStream.str() );

        bool exists = false;
        M_Bitmap::iterator found = m_Bitmaps.find( keyFile.Get() );
        if ( found != m_Bitmaps.end() )
        {
            exists = true;
            if( !found->second.m_Path.ChangedSince( found->second.m_PathLastUpdated ) )
            {
                return found->second.m_Bitmap;
            }
        }

        const wxBitmap& origBitmap = GetBitmap( fileName, type );
        if ( !origBitmap.IsOk() )
        {
            return origBitmap;
        }

        // determine how to scale the image
        float ratioHToW = ( (float) origBitmap.GetHeight() / (float) origBitmap.GetWidth() );

        int width, height;
        if ( ratioHToW >= 1.0 )
        {
            ratioHToW = 1.0 / ratioHToW;
            height = maxSize;
            width  = (int) ( ratioHToW * (float) maxSize );
        }
        else
        {
            width = maxSize;
            height  = (int) ( ratioHToW * (float) maxSize );
        }

        // store the scaled image
        wxImage image = origBitmap.ConvertToImage();
        image.Rescale( width, height );

        if ( !exists )
        {
            BitmapFileInfo info( fileName );
            info.m_Bitmap = wxBitmap( image );

            Insert<M_Bitmap>::Result inserted = m_Bitmaps.insert( M_Bitmap::value_type ( keyFile.Get(), info ) );
            if ( !inserted.second )
            {
                Log::Warning( TXT( "Failed to store scaled GUI image %s\n" ), fileName.c_str() );
            }

            return inserted.first->second.m_Bitmap;
        }
        else
        {
            found->second.m_Bitmap = wxBitmap( image );
            return found->second.m_Bitmap;
        }

    }

    const wxAnimation& ImageManager::GetAnimation( const tstring& fileName, wxAnimationType type )
    {
        M_Animation::iterator found = m_Animations.find( fileName );
        if ( found != m_Animations.end() )
        {
            return found->second;
        }

        tstring fullPath;
        GetFullImagePath( fileName, fullPath );
        if ( !Nocturnal::Path( fullPath ).Exists() )
        {
            Log::Warning( TXT( "Animated image file does not exist %s\n" ), fullPath.c_str() );
        }

        Insert<M_Animation>::Result inserted = m_Animations.insert( M_Animation::value_type( fileName, wxAnimation() ) );
        if ( !inserted.first->second.LoadFile( fullPath.c_str(), type ) )
        {
            Log::Warning( TXT( "Unable to load animated image %s\n" ), fileName.c_str() );
            inserted.first->second = wxNullAnimation;
        }

        return inserted.first->second;
    }



    ///////////////////////////////////////////////////////////////////////////////
    // 
    // wxImageList Functions
    //
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    // Searches the list of bitmaps and returns the name of the bitmaps with the
    // specified image index (into the image list).  Returns an empty string if
    // the bitmap was not found in the image list.
    // 
    const tstring& ImageManager::GetNameFromImageIndex( i32 index ) const
    {
        static const tstring emptyString( TXT( "" ) );

        M_StrI32::const_iterator itr = m_PathNameToIndex.begin();
        M_StrI32::const_iterator end = m_PathNameToIndex.end();
        for ( ; itr != end; ++itr )
        {
            const tstring& currentName = itr->first;
            const i32 currentIndex = itr->second;
            if ( currentIndex == index )
            {
                return currentName;
            }
        }

        return emptyString;
    }


    ///////////////////////////////////////////////////////////////////////////////
    // Returns an imagelist of sizeXsize pixel images for use by the GUI.
    // 
    wxImageList* ImageManager::GetGuiImageList( const IconSize size )
    {
        return &m_GuiImageLists[size];
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Loads all the images in the art directory that are needed.  Places the 
    // images in image lists as appropriate.  Maintains a mapping of fileName to 
    // image list index.
    //
    bool ImageManager::LoadGuiArt( const IconSize size )
    {
        bool isOk = true;

        std::set< Nocturnal::Path > artFiles;
        Nocturnal::Directory::GetFiles( m_DefaultFolder, artFiles, g_IconSizeSpecs[size], true );

        const int numImages = static_cast< int >( artFiles.size() );
        if ( numImages > 0 )
        {
            m_GuiImageLists[size].Create( g_IconSizes[size], g_IconSizes[size], true, numImages );
            std::set< Nocturnal::Path >::const_iterator fileItr = artFiles.begin();
            std::set< Nocturnal::Path >::const_iterator fileEnd = artFiles.end();
            for ( ; fileItr != fileEnd; ++fileItr )
            {
                tstring currentFile;
                GetFullImagePath( (*fileItr).Get(), currentFile );
                isOk = LoadImage(currentFile, size);
            }
        }
        else
        {
            isOk = false; // There should be some images to load, but they are missing
        }

        return isOk;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Load Images into the correct imagelist
    // 
    bool ImageManager::LoadImage( const tstring& fileName, const IconSize size )
    {
        wxImage image;

        if ( !image.LoadFile( fileName.c_str(), wxBITMAP_TYPE_PNG ) )
        {
            Log::Warning( TXT( "Unable to load GUI image %s\n" ), fileName.c_str() );
            return false;
        }
        else
        {
            // The file was loaded, add it to the image list and store a mapping of file name to index
            u32 index = m_GuiImageLists[size].Add( wxBitmap( image ) );

            m_PathNameToIndex.insert( M_StrI32::value_type( fileName, index ) );

            return true;
        }
    }


    ///////////////////////////////////////////////////////////////////////////////
    // Returns a zero-based index into an image list for the file that has the 
    // specified name.  If the file does not exist, -1 is returned.  No information
    // is provided about which image list this index belongs to, but that should
    // should be obvious because of our naming conventions.  Image lists must contain
    // images that are all the same size and we name our images with _16, _32, etc.
    // at the end of the name to indicate the dimensions of the image.  So if you 
    // request "foo_16.png", it should be in the image list returned by 
    // GetGuiImageList.
    // 
    // fileName is not the full path, but it does include the extension
    // example: "foo_16.png"
    // 
    int ImageManager::GetImageIndex( const tstring& fileName )
    {
        int index = -1;

        if ( !fileName.empty() )
        {
            tstring fullPath;
            GetFullImagePath( fileName, fullPath );
            M_StrI32::const_iterator found = m_PathNameToIndex.find( fullPath );
            if ( found != m_PathNameToIndex.end() )
            {
                index = found->second;
            }
            else
            {
                if (!LoadImage( fullPath, GetIconSize( fileName ) ))
                {
                    m_PathNameToIndex[ fullPath ] = -1;
                }
                else
                {
                    M_StrI32::const_iterator found = m_PathNameToIndex.find( fullPath );
                    NOC_ASSERT( found != m_PathNameToIndex.end() );
                    index = found->second;
                }
            }
        }

        return index;
    }


    ///////////////////////////////////////////////////////////////////////////////
    // Returns the best bitmap type to use when loading an image, based on extension
    //
    // See: x:\rcf\code\tech\sdk\wxwidgets\include\wx\gdicmn.h
    //
    long ImageManager::GetBitmapLoadType( const tstring& fileName )
    {    
        Nocturnal::Path path( fileName );

        M_ExtensionType::iterator findType = m_ExtensionType.find( path.Extension() );
        if ( findType != m_ExtensionType.end() )
        {
            return findType->second;
        }
        else
        {
            return wxBITMAP_TYPE_ANY;
        }
    }

} // namespace Nocturnal
