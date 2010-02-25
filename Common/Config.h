#pragma once

// build config support
#ifdef NOCTURNAL_BUILD_CONFIG
# define NOCTURNAL_BUILD_CONFIG_STRINGIFY(x) #x
# define NOCTURNAL_BUILD_CONFIG_TOSTRING(x) NOCTURNAL_BUILD_CONFIG_STRINGIFY(x)
# define NOCTURNAL_BUILD_CONFIG_STRING NOCTURNAL_BUILD_CONFIG_TOSTRING(NOCTURNAL_BUILD_CONFIG)
#endif

// this is used to prefix environment variables
#define NOCTURNAL_STUDIO_PREFIX         "NOC_"

// this is the name of the "project" (code name for game title)
#define NOCTURNAL_PROJECT_NAME          "nocturnal"
