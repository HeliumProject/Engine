//----------------------------------------------------------------------------------------------------------------------
// ObjectTypeRegistration.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Example
{
    /// @copydoc Helium::GameObjectTypeRegistration::Register()
    template< typename ObjectTypeRegistrationBase >
    void ObjectTypeRegistration< ObjectTypeRegistrationBase >::Register()
    {
        ObjectTypeRegistrationBase::Register();

        RegisterExampleGameTypes();
    }

    /// @copydoc Helium::GameObjectTypeRegistration::Unregister()
    template< typename ObjectTypeRegistrationBase >
    void ObjectTypeRegistration< ObjectTypeRegistrationBase >::Unregister()
    {
        UnregisterExampleGameTypes();

        ObjectTypeRegistrationBase::Unregister();
    }
}
