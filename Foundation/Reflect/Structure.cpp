#include "Structure.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

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

const void* Structure::GetDefaultInstance() const
{
    return m_Default;
}