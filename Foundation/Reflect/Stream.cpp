#include "Stream.h" 

#include "Foundation/Profile.h" 
#include "Exceptions.h"

using namespace Reflect;

Profile::Accumulator Reflect::g_StreamWrite( TXT( "Reflect Stream Write" ) );
Profile::Accumulator Reflect::g_StreamRead( TXT( "Reflect Stream Read" ) ); 
