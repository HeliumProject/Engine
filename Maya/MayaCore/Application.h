#include "API.h"

#include "Platform/Types.h"
#include "Platform/Compiler.h"
#include "Platform/Windows/Windows.h"
#include <wx/wx.h>

namespace MayaCore
{
  class Application : public wxApp
  {

  };

  DECLARE_APP( Application );
}