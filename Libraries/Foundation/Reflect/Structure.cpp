#include "stdafx.h"
#include "Structure.h"
#include "Serializers.h"

using namespace Reflect;

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