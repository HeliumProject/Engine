//----------------------------------------------------------------------------------------------------------------------
// ObjectTypeRegistration.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/ObjectTypeRegistration.h"

// Type registration functions.
extern void RegisterGraphicsTypes();

extern void UnregisterGraphicsTypes();

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
    RegisterGraphicsTypes();
}

/// Unregister type information previously registered using Register().
///
/// @see Unregister()
void ObjectTypeRegistration::Unregister()
{
    UnregisterGraphicsTypes();
}
