#pragma once

namespace AppUtils
{
  bool InitializeExceptionDB();
  void CleanupExceptionDB();

  class ExceptionReport;
  void UpdateExceptionDB( const ExceptionReport& report );
}