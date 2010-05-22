#include "stdafx.h"
#include "FileInfo.h"

#include "Attribute/AttributeHandle.h"
#include "Asset/ArtFileAttribute.h"
#include "Asset/ColorMapAttribute.h"
#include "Asset/AssetClass.h"

#include "Common/String/Utilities.h"

#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/FinderSpec.h"
#include "Finder/ProjectSpecs.h"

using namespace File;

REFLECT_DEFINE_CLASS( FileInfo );

void FileInfo::EnumerateClass( Reflect::Compositor<FileInfo>& comp )
{
}


///////////////////////////////////////////

static const i32 s_DisplayTimeSize = 32;
static const u32 InvalidLegacyID = -1;


///////////////////////////////////////////////////////////////////////////////
// Ctor/Dtor
//

FileInfo::FileInfo( const std::string& path )
: Reflect::Element()
{
    m_File.SetPath( path );
}

FileInfo::FileInfo( const File::Reference& fileRef )
: Reflect::Element()
{
    m_File = fileRef.GetFile();
}

FileInfo::~FileInfo()
{
}



/////////////////////////////////////////////////////////////////////////////
// Overloaded = operator only compares the file paths
bool FileInfo::operator==( const FileInfo &rhs )
{
    return ( m_File.GetPath().Get().compare( rhs.m_File.GetPath().Get() ) == 0 );
}

/////////////////////////////////////////////////////////////////////////////
// Pretty prints time
static std::string TimeAsString( u64 time )
{
    std::string result;

    // try to get a printer friendly version of the datetime
    __time64_t timeT  = ( __time64_t ) ( time / 1000 );

    char timePrint[s_DisplayTimeSize];
    if ( _ctime64_s( timePrint, s_DisplayTimeSize, &timeT ) == 0 )
    {
        // timeT
        result = timePrint;
        result = result.substr( 0, result.length() - 1 );
    }
    else
    {
        sprintf( timePrint, "%ld", time );
        result = timePrint;
    }    
    return result;
}

// Pretty prints time
std::string FileInfo::GetCreatedAsString()
{
    return TimeAsString( m_File.GetCreatedTime() );
}

// Pretty prints time
std::string FileInfo::GetModifiedAsString()
{
    return TimeAsString( m_File.GetModifiedTime() );
}

/////////////////////////////////////////////////////////////////////////////
std::string FileInfo::GetSizeAsString()
{
    u64 size = m_File.Size();

    std::stringstream printSize;
    if ( size == 0 )
    {
        printSize << "0 KB";
    }
    else if ( size <= 1024 )
    {
        printSize << "1 KB";
    }
    else
    {
        size = size / 1024;
        printSize << size << " KB";
    }

    return printSize.str();
}

/////////////////////////////////////////////////////////////////////////////
// Gets the path to the thumbnail from the art file path
//
// Example:
//   x:/rcf/assets/devel/maya/entities/shrubs/shrub0/shrub.mb
// becomes...
//   x:/rcf/assets/devel/maya/entities/shrubs/shrub0/shrub.jpg
//
std::string FileInfo::GetThumbnailPath( )
{
    if ( m_ThumbnailPath.empty() )
    {
        std::string extension = m_File.GetPath().Extension();

        if ( extension == FinderSpecs::Extension::MAYA_BINARY.GetExtension() )
        {
            m_ThumbnailPath = m_File.GetPath();
            m_ThumbnailPath.ReplaceExtension( "jpg" );
        }
        else if ( extension == FinderSpecs::Extension::PNG.GetExtension()
            || extension == FinderSpecs::Extension::TGA.GetExtension()
            || extension == FinderSpecs::Extension::JPG.GetExtension()
            || extension == FinderSpecs::Extension::GIF.GetExtension() )
        {
            m_ThumbnailPath = m_File.GetPath();
        }
        //else if ( extension == FinderSpecs::Asset::ENTITY_DECORATION.GetDecoration() )
        //{
        //    GetArtFile();

        //    if ( !m_ArtFile.empty() )
        //    {
        //        m_ThumbnailPath = m_ArtFile;
        //        FileSystem::StripExtension( m_ThumbnailPath );
        //        m_ThumbnailPath += ".jpg";
        //    }
        //}
        //else if ( extension == FinderSpecs::Asset::SHADER_DECORATION.GetDecoration() )
        //{
        //    Asset::AssetClassPtr assetClass = NULL;
        //    try
        //    {
        //        assetClass = Asset::AssetClass::FindAssetClass( GetID() );

        //        if ( assetClass.ReferencesObject() )
        //        {
        //            Asset::ColorMapAttributePtr colorMapFile = Reflect::ObjectCast< Asset::ColorMapAttribute >( assetClass->GetAttribute( Reflect::GetType< Asset::ColorMapAttribute >() ) );

        //            if ( colorMapFile.ReferencesObject() )
        //            {
        //                m_ThumbnailPath = colorMapFile->GetFilePath();
        //            }
        //        }       
        //    }
        //    catch( const Nocturnal::Exception& )
        //    {
        //        // do nothing
        //    }
        //}
    }

    return m_ThumbnailPath;
}


