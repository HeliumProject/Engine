#pragma once

#include "Platform/Windows/Windows.h"

namespace Helium
{
    typedef HANDLE Handle;
    const Handle InvalidHandleValue = (Handle)INVALID_HANDLE_VALUE;
}