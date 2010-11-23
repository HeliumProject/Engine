#include "Structure.h"
#include "SerializerDeduction.h"

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