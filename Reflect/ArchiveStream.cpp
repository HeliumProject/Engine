#include "ReflectPch.h"
#include "ArchiveStream.h" 

#include "Foundation/Profile.h" 
#include "Reflect/Exceptions.h"

using namespace Helium;
using namespace Helium::Reflect;

Profile::Accumulator Reflect::g_StreamWrite( "Reflect Stream Write" );
Profile::Accumulator Reflect::g_StreamRead( "Reflect Stream Read" ); 

#ifdef HELIUM_WCHAR_T
// We need to prevent the C runtime from attempting to delete our custom facet by giving it a non-zero
// initial reference count and keeping it around outside the scope of any stream object instances, as
// the C runtime does not use a memory allocator that is compatible with our custom allocator.
null_codecvt null_codecvt::sm_StaticInstance( 1 );
#endif
