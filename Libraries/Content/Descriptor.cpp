#include "Descriptor.h"

namespace Content 
{
  REFLECT_DEFINE_CLASS(Descriptor);

void Descriptor::EnumerateClass( Reflect::Compositor<Descriptor>& comp )
{
  Reflect::EnumerationField* enumExportType = comp.AddEnumerationField( &Descriptor::m_ExportType, "m_ExportType" );
  Reflect::EnumerationField* enumBangleGeometrySimulation = comp.AddEnumerationField( &Descriptor::m_BangleGeometrySimulation, "m_BangleGeometrySimulation" );
  Reflect::Field* fieldContentNum = comp.AddField( &Descriptor::m_ContentNum, "m_ContentNum" );
}


  // this must match the GeometrySimulations enum
  const char* GeometrySimulationsStrings[Content::GeometrySimulations::Count] = 
  { 
    "Off",
    "Brick",
    "Flex",
    "Glass",
    "Metal",
    "Plaster",
  };
}