#pragma once

#include "Application/API.h"
#include "ReflectClipboardData.h"
#include "Foundation/Reflect/Serializers.h"

namespace Inspect
{
  /////////////////////////////////////////////////////////////////////////////
  // Custom clipboard format to manage a list of files.
  // 
  class APPLICATION_API ClipboardFileList : public ReflectClipboardData
  {
  private:
    S_string m_Files;

    // These members are not serialized
    bool m_IsDirty;

  public:
    // Runtime Type Info
    REFLECT_DECLARE_CLASS( ClipboardFileList, ReflectClipboardData );
    static void InitializeType();
    static void CleanupType();

  public:
    ClipboardFileList();
    virtual ~ClipboardFileList();

    bool AddFilePath( const std::string& file );
    const S_string& GetFilePaths() const;

    virtual bool Merge( const ReflectClipboardData* source ) NOC_OVERRIDE;

  public:
    static void EnumerateClass( Reflect::Compositor<ClipboardFileList>& comp );
  };

  typedef Nocturnal::SmartPtr< ClipboardFileList > ClipboardFileListPtr;
}