#include "DebugSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"

namespace FinderSpecs
{
  const FileSpec Debug::TRACE_FILE ( "Debug::TRACE_FILE", "trace",  Extension::TXT, "1" );
  const FileSpec Debug::WARNING_FILE ( "Debug::WARNING_FILE", "warning",  Extension::TXT, "1" );
  const FileSpec Debug::ERROR_FILE ( "Debug::ERROR_FILE", "error",  Extension::TXT, "1" );
}