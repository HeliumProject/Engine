#pragma once

namespace Application
{
  bool InitializeExceptionDB();
  void CleanupExceptionDB();

  class ExceptionReport;
  void UpdateExceptionDB( const ExceptionReport& report );
}