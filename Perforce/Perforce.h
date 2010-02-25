#pragma once

#include "API.h"
#include "Provider.h"
#include "Exceptions.h"
#include "Common/Automation/Event.h"
#include "RCS/RCS.h"

namespace Perforce
{
  PERFORCE_API extern u32 g_InitThread;

  PERFORCE_API void Initialize();
  PERFORCE_API void Cleanup();

  typedef Nocturnal::Signature<bool, WaitInterface*> WaitSignature;
  PERFORCE_API extern WaitSignature::Delegate g_ShowWaitDialog;

  struct MessageArgs
  {
    std::string m_Message;
    std::string m_Title;

    MessageArgs( const std::string& message, const std::string& title )
      : m_Message( message )
      , m_Title( title )
    {

    }
  };
  typedef Nocturnal::Signature<void, const MessageArgs&> MessageSignature;
  PERFORCE_API extern MessageSignature::Delegate g_ShowWarningDialog;
}
