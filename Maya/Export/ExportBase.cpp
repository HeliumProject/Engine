#include "Precompile.h"
#include "ExportBase.h"
#include "MayaContentCmd.h"

using namespace MayaContent;

void ExportBase::GatherMayaData( V_ExportBase &newExportObjects )
{
  EXPORT_SCOPE_TIMER( ("") );

  //
  // At this point, we have all of the concrete (aggregate) data on the reflect object
  //  we now call into our general dynamic data export function to get all of the
  //  dynamic attributes for this object that have been configured by our maya plugins
  //

  MayaContentCmd::GetDynamicAttributes( m_MayaObject, m_ContentObject );
}