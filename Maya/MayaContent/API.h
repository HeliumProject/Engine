#pragma once

#ifdef MAYACONTENTDLL_EXPORTS
#  define MAYA_CONTENT_API __declspec (dllexport)
#else
#  define MAYA_CONTENT_API __declspec (dllimport)
#endif

// profiling for exporter
//#define EXPORT_PROFILE

#if defined( PROFILE_INSTRUMENT_ALL ) || defined ( EXPORT_PROFILE )
# define EXPORT_SCOPE_TIMER( __Str ) PROFILE_SCOPE_TIMER( __Str )
#else
# define EXPORT_SCOPE_TIMER(__Str)
#endif