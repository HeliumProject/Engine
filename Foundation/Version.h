#pragma once

#include "Platform/String.h"

#define NOCTURNAL_VERSION_PROJECT     14
#define NOCTURNAL_VERSION_COMPATIBLE  1
#define NOCTURNAL_VERSION_FEATURE     6
#define NOCTURNAL_VERSION_PATCH       0

#define NOCTURNAL_VERSION_STRINGIFY(project, compatible, feature, patch) TXT( #project ) TXT( "." ) TXT( #compatible ) TXT( "." ) TXT( #feature ) TXT( "." ) TXT( #patch )
#define NOCTURNAL_VERSION_TOSTRING(project, compatible, feature, patch) NOCTURNAL_VERSION_STRINGIFY(project,compatible,feature,patch)
#define NOCTURNAL_VERSION_STRING NOCTURNAL_VERSION_TOSTRING( NOCTURNAL_VERSION_PROJECT, NOCTURNAL_VERSION_COMPATIBLE, NOCTURNAL_VERSION_FEATURE, NOCTURNAL_VERSION_PATCH)
