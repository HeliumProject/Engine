#pragma once

#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Serializers.h"

#include "Luna/API.h"
#include "Task.h"

namespace Luna
{
  namespace Export
  {
    LUNA_TASK_API void Initialize();
    LUNA_TASK_API void Cleanup();
  }

  struct ExportParams
  {
  public:
    S_tuid m_AssetIds;
    bool   m_Recursive;
    bool   m_SetupLighting;
    tstring m_StateTrackerFile;

    ExportParams()
      : m_Recursive( false )
      , m_SetupLighting( false )
    {
    }
  };

  class ExportOptions : public Reflect::Element
  {
  public:
    bool m_Recursive;
    tstring m_StateTrackerFile;
    bool   m_SetupLighting;

    ExportOptions()
      : m_Recursive( false )
      , m_SetupLighting( false )
    {
    }

    REFLECT_DECLARE_CLASS( ExportOptions, Reflect::Element );

    static void EnumerateClass( Reflect::Compositor<ExportOptions>& comp );
  };
  typedef Nocturnal::SmartPtr< ExportOptions > ExportOptionsPtr;

  LUNA_TASK_API void ExportAsset( const tuid& assetId, wxWindow* parent, bool showOptions = false, bool blocking = false  );
  LUNA_TASK_API void ExportAssets( const S_tuid& assetIds, wxWindow* parent, bool showOptions = false, bool blocking = false, bool lightingSetup = false );
}

