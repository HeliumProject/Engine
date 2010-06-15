#pragma once
#include "API.h"

// this object is in Asset because AssetExporter links against maya
// I'd like access to this object from AssetExporter and from Luna
// but I don't want to pollute the Luna solution with Maya, hence
// this lives here.  Sorry if you are the one who has to clean this
// up someday. -andy

#include "Foundation/TUID.h"

#include "Reflect/Element.h"
#include "Reflect/Serializers.h"

namespace Asset
{
  class ASSET_API ExporterJob : public Reflect::Element
  {
  public:
    S_tuid m_AssetIds;
    bool   m_Recursive;
    bool   m_SetupLighting;
    std::string m_StateTrackerFile;
    

    ExporterJob()
      : m_Recursive( false )
      , m_SetupLighting( false )
    {
    }

    REFLECT_DECLARE_CLASS( ExporterJob, Reflect::Element );

    static void EnumerateClass( Reflect::Compositor<ExporterJob>& comp );
  };

  typedef Nocturnal::SmartPtr< ExporterJob > ExporterJobPtr;
}