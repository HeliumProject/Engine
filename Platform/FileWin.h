#pragma once

namespace Helium
{
    // these must mesh up with windows.h
    typedef void* Handle; // windows: HANDLE
    const Handle InvalidHandleValue = (Handle)-1; // windows: INVALID_HANDLE_VALUE
}