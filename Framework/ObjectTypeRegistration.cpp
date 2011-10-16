//----------------------------------------------------------------------------------------------------------------------
// ObjectTypeRegistration.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/ObjectTypeRegistration.h"
#include "Graphics/GraphicsCustomTypeRegistration.h"

// Type registration functions.
extern void RegisterEngineTypes();
extern void RegisterGraphicsTypes();
extern void RegisterFrameworkTypes();

extern void UnregisterEngineTypes();
extern void UnregisterGraphicsTypes();
extern void UnregisterFrameworkTypes();

using namespace Helium;

/// Destructor.
ObjectTypeRegistration::~ObjectTypeRegistration()
{
}

/// Register type information for modules that will be used by the application.
///
/// @see Unregister()
void ObjectTypeRegistration::Register()
{
    RegisterEngineTypes();
    PreRegisterGraphicsTypes();
    RegisterGraphicsTypes();
    RegisterFrameworkTypes();
}

/// Unregister type information previously registered using Register().
///
/// @see Unregister()
void ObjectTypeRegistration::Unregister()
{
    UnregisterFrameworkTypes();
    UnregisterGraphicsTypes();
    PostRegisterGraphicsTypes();
    UnregisterEngineTypes();
}
