#include "Structure.h"
#include "Serializers.h"

using namespace Helium::Reflect;

Structure::Structure ()
{

}

Structure::~Structure ()
{

}

Structure* Structure::Create()
{
    return new Structure();
}