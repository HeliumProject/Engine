//----------------------------------------------------------------------------------------------------------------------
// ExampleMainWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "ExampleMainPch.h"

#include "ExampleMain/ObjectTypeRegistration.h"

#include "Framework/Components.h"
#include "Framework/Facets.h"

#include "Foundation/Math/Vector2.h"

using namespace Lunar;

struct FLocated2 : public Helium::Facets::IFacet
{
  REFLECT_DECLARE_OBJECT(FLocated2, Helium::Facets::IFacet)

  Helium::Vector2 m_Location;
};

REFLECT_DEFINE_OBJECT(FLocated2);

class CTestComponent : public Helium::Components::Component
{
  OBJECT_DECLARE_COMPONENT(CTestComponent, Helium::Components::Component);
};

OBJECT_DEFINE_COMPONENT(CTestComponent);

class OTestObject : public Reflect::Object, public Components::Host
{
public:
  REFLECT_DECLARE_OBJECT(OTestObject, Reflect::Object);

  FLocated2 m_Located2;

  ENGINE_FACETS_BEGIN(OTestObject)
    REGISTER_FACET(FLocated2, m_Located2)
  ENGINE_FACETS_END()
};

REFLECT_DEFINE_OBJECT(OTestObject);

/// Windows application entry point.
///
/// @param[in] hInstance      Handle to the current instance of the application.
/// @param[in] hPrevInstance  Handle to the previous instance of the application (always null; ignored).
/// @param[in] lpCmdLine      Command line for the application, excluding the program name.
/// @param[in] nCmdShow       Flags specifying how the application window should be shown.
///
/// @return  Result code of the application.
int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int nCmdShow )
{
    HELIUM_TRACE_SET_LEVEL( TRACE_DEBUG );

    int32_t result = 0;

    {
        // Initialize a GameSystem instance.
        CommandLineInitializationWin commandLineInitialization;
        Example::ObjectTypeRegistration< ObjectTypeRegistrationWin > objectTypeRegistration;
        MemoryHeapPreInitialization memoryHeapPreInitialization;
        ObjectLoaderInitializationWin objectLoaderInitialization;
        ConfigInitializationWin configInitialization;
        WindowManagerInitializationWin windowManagerInitialization( hInstance, nCmdShow );
        RendererInitializationWin rendererInitialization;
        //NullRendererInitialization rendererInitialization;

        

        GameSystem* pGameSystem = GameSystem::CreateStaticInstance();
        HELIUM_ASSERT( pGameSystem );
        bool bSystemInitSuccess = pGameSystem->Initialize(
            commandLineInitialization,
            objectTypeRegistration,
            memoryHeapPreInitialization,
            objectLoaderInitialization,
            configInitialization,
            windowManagerInitialization,
            rendererInitialization,
            NULL );



        Components::Initialize();
        Facets::Initialize();

        Helium::Reflect::RegisterClassType<FLocated2>(TXT("FLocated2"));
        Helium::Reflect::RegisterClassType<CTestComponent>(TXT("CTestComponent"));
        CTestComponent::RegisterComponentType(16); // Up to 16 availabled
        Helium::Reflect::RegisterClassType<OTestObject>(TXT("OTestObject"));
        OTestObject::InitializeFacetOffsets();

        OTestObject to;
        CTestComponent *component1 = to.AttachComponent<CTestComponent>();
        FLocated2 *facet1 = to.GetFacet<FLocated2>();


        Components::Host host;


        if( bSystemInitSuccess )
        {
            // Run the application.
            result = pGameSystem->Run();
        }

        // Shut down and destroy the system.
        pGameSystem->Shutdown();
        System::DestroyStaticInstance();
    }

    // Perform final cleanup.
    ThreadLocalStackAllocator::ReleaseMemoryHeap();

#if HELIUM_ENABLE_MEMORY_TRACKING
    DynamicMemoryHeap::LogMemoryStats();
    ThreadLocalStackAllocator::ReleaseMemoryHeap();
#endif

    return result;
}
