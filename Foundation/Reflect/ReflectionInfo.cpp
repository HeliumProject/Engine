#include "FoundationPch.h"
#include "Foundation/Reflect/ReflectionInfo.h"

using namespace Helium;
using namespace Helium::Reflect;

const tchar_t* ReflectionTypes::Strings[ ReflectionTypes::Count ] =
{
    TXT("Type"),
    TXT("Enumeration"),
    TXT("Composite"),
    TXT("Structure"),
    TXT("Class"),
    TXT("GameObjectType"),
};

ReflectionInfo::ReflectionInfo()
{

}

ReflectionInfo::~ReflectionInfo()
{

}