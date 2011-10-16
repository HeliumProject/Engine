#include "FoundationPch.h"
#include "Structure.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium::Reflect;

Structure::Structure ()
{
}

Structure::~Structure ()
{
    delete m_Default;
}

Structure* Structure::Create()
{
    return new Structure();
}
