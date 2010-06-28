#include "ExporterJob.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( ExporterJob );

void ExporterJob::EnumerateClass( Reflect::Compositor<ExporterJob>& comp )
{
  Reflect::Field* fieldAssetIds = comp.AddField( &ExporterJob::m_AssetIds, "m_AssetIds" );
  Reflect::Field* fieldRecursive = comp.AddField( &ExporterJob::m_Recursive, "m_Recursive" );

  Reflect::Field* fieldStateTrackerFile = comp.AddField( &ExporterJob::m_StateTrackerFile, "m_StateTrackerFile" );
  fieldStateTrackerFile->m_UIName = TXT( "State Tracker File" );

  Reflect::Field* fieldSetupLighting = comp.AddField( &ExporterJob::m_SetupLighting, "m_SetupLighting" );
}