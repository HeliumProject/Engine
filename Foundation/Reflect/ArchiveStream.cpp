#include "ArchiveStream.h" 

#include "Foundation/Profile.h" 
#include "Foundation/Reflect/Exceptions.h"

using namespace Helium;
using namespace Helium::Reflect;

Profile::Accumulator Reflect::g_StreamWrite( "Reflect Stream Write" );
Profile::Accumulator Reflect::g_StreamRead( "Reflect Stream Read" ); 
