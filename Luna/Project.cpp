#include "Precompile.h"
#include "Project.h"

#include "Foundation/Reflect/Serializers.h"

using namespace Luna;

REFLECT_DEFINE_CLASS( Project )

void Project::EnumerateClass( Reflect::Compositor< Project >& comp )
{
    Reflect::Field* files = comp.AddField( &Project::m_Files, "m_Files" );
}
