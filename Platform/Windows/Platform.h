#pragma once

// Temporary workaround for bug in Visual C++ 2008 with including intrin.h and math.h simultaneously
// (see http://connect.microsoft.com/VisualStudio/feedback/details/381422/warning-of-attributes-not-present-on-previous-declaration-on-ceil-using-both-math-h-and-intrin-h).
#pragma warning( push )
#pragma warning( disable : 4985 )  // 'symbol name': attributes not present on previous declaration

// Microsoft compiler instrinsics from windows sdk
#include <intrin.h>

#pragma warning( pop )
