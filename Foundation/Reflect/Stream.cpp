#include "Stream.h" 

#include "Foundation/Profile.h" 
#include "Exceptions.h"

using namespace Helium;
using namespace Helium::Reflect;

Profile::Accumulator Reflect::g_StreamWrite( "Reflect Stream Write" );
Profile::Accumulator Reflect::g_StreamRead( "Reflect Stream Read" ); 
