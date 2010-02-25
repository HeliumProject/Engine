#pragma once

#include "API.h"

#include "Common/Types.h"
#include "Common/File/File.h"
#include <wx/animate.h>

// Forwards
class wxImageList;

namespace UIToolKit
{
  bool UITOOLKIT_API ImageManagerInit(const std::string& folder, const std::string& themeFolder = std::string( "" ));
  void UITOOLKIT_API ImageManagerCleanup();

  UITOOLKIT_API class ImageManager& GlobalImageManager();


  namespace IconSizes
  {
    enum IconSize
    {
      Size16,
      Size32,
      Size64,
      Size128,

      Count,
    };
  }
  typedef IconSizes::IconSize IconSize;

  struct BitmapFileInfo
  {

    Nocturnal::File m_File;
    wxBitmap m_Bitmap;

    BitmapFileInfo()
    {
    }
  };

  /////////////////////////////////////////////////////////////////////////////
  //
  // 
  class UITOOLKIT_API ImageManager 
  {
  public:
    
    ImageManager( const std::string& defaultFolder = std::string( "" ), const std::string& themeFolder = std::string( "" ) );
    virtual ~ImageManager();

    //
    // wxBitmap shared storage
    //

    // will attempt to call GetBitmapLoadType if wxBITMAP_TYPE_ANY is passed in
    const wxBitmap& GetBitmap( const std::string& fileName, long type = wxBITMAP_TYPE_ANY );
    const wxBitmap& GetScaledBitmap( const std::string& fileName, int maxSize, long type = wxBITMAP_TYPE_ANY );

    // assuming that fileName points to an animated GIF, this function returns the corresponding animation
    const wxAnimation& GetAnimation( const std::string& fileName, wxAnimationType type = wxANIMATION_TYPE_GIF );

    //
    // wxImageList collection(s)
    //

    wxImageList* GetGuiImageList( const IconSize size = IconSizes::Size16 );

    bool LoadGuiArt( const IconSize size = IconSizes::Size16 );
    bool LoadImage( const std::string& fileName, const IconSize size = IconSizes::Size16 );
    int GetImageIndex( const std::string& fileName );

    const std::string& GetNameFromImageIndex( i32 index ) const;

    long GetBitmapLoadType( const std::string& fileName );
    void GetFullImagePath( const std::string& partialPath, std::string& fullPath );

  private:
    //
    // Members
    //    
    
    std::string       m_DefaultFolder;
    std::string       m_ThemeFolder;
    
    typedef std::map< std::string, long > M_ExtensionType;
    M_ExtensionType   m_ExtensionType;

    // wxBitmap shared storage
    typedef std::map< std::string, BitmapFileInfo > M_Bitmap;
    M_Bitmap          m_Bitmaps;

    // wxAnimation shared storage
    typedef std::map< std::string, wxAnimation > M_Animation;
    M_Animation       m_Animations;

    // wxImageList collection(s)
    typedef std::map< std::string, i32 > M_StrI32;
    M_StrI32          m_FileNameToIndex;
    
    typedef std::map< IconSize, wxImageList > M_IconSizeImageLists;
    M_IconSizeImageLists    m_GuiImageLists;
  };

} // namespace UIToolKit