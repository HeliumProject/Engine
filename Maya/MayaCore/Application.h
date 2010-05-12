#include "API.h"

#include "Common/Types.h"
#include "Common/Compiler.h"
#include "Windows/Windows.h"
#include <wx/wx.h>

namespace MayaCore
{
  class Application : public wxApp
  {

  };

  DECLARE_APP( Application );
}