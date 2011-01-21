//----------------------------------------------------------------------------------------------------------------------
// ObjectTypeRegistration.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef EXAMPLE_OBJECT_TYPE_REGISTRATION_H
#define EXAMPLE_OBJECT_TYPE_REGISTRATION_H

/// Type registration functions.
extern void RegisterExampleGameTypes();
extern void UnregisterExampleGameTypes();

namespace Example
{
    template< typename ObjectTypeRegistrationBase >
    class ObjectTypeRegistration : public ObjectTypeRegistrationBase
    {
    public:
        /// @name Type Registration
        //@{
        virtual void Register();
        virtual void Unregister();
        //@}
    };
}

#include "ExampleMain/ObjectTypeRegistration.inl"

#endif  // EXAMPLE_OBJECT_TYPE_REGISTRATION_H
