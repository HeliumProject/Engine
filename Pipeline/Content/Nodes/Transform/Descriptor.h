#pragma once

#include "Pipeline/API.h"

#include "Pipeline/Content/Nodes/Transform/PivotTransform.h"
#include "Pipeline/Content/ContentTypes.h"

namespace Content
{

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // GeometrySimulations - For bangles
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  namespace GeometrySimulations
  {
    enum GeometrySimulation
    {
      // Off - is the default and always the first in the enum
      Off      = 0,

      Brick,
      Flex,
      Glass,
      Metal,
      Plaster,

      // Count - is always the last enum in the geometry simulations
      Count
    };
    static void GeometrySimulationEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Off, TXT( "Off" ) );
      info->AddElement(Glass, TXT( "Glass" ) );
      info->AddElement(Plaster, TXT( "Plaster" ) );
      info->AddElement(Brick, TXT( "Brick" ) );
      info->AddElement(Metal, TXT( "Metal" ) );
      info->AddElement(Flex, TXT( "Flex" ) );
    }
  }
  typedef GeometrySimulations::GeometrySimulation GeometrySimulation;

  extern PIPELINE_API const char* GeometrySimulationsStrings[Content::GeometrySimulations::Count];

  class PIPELINE_API Descriptor : public PivotTransform
  {
  public:
    Content::ContentType m_ExportType;
    Content::GeometrySimulation m_BangleGeometrySimulation; // only used for Descriptors with ContentTypes::Bangle
    u16 m_ContentNum;

    Descriptor () 
      : m_ExportType( Content::ContentTypes::Default )
      , m_BangleGeometrySimulation( GeometrySimulations::Off )
      , m_ContentNum( 0 )
      
    { }

    Descriptor (const Nocturnal::TUID& id)
      : PivotTransform (id) 
      , m_ExportType( Content::ContentTypes::Default )
      , m_BangleGeometrySimulation( GeometrySimulations::Off )
      , m_ContentNum( 0 ) 
    {
   
    }

    REFLECT_DECLARE_CLASS(Descriptor, PivotTransform);

    static void EnumerateClass( Reflect::Compositor<Descriptor>& comp );
  };

  typedef Nocturnal::SmartPtr<Descriptor> DescriptorPtr;
  typedef std::vector<DescriptorPtr> V_Descriptor;
}