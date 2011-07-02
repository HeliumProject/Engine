//----------------------------------------------------------------------------------------------------------------------
// ResourceHandler.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_PC_SUPPORT_RESOURCE_HANDLER_H
#define HELIUM_PC_SUPPORT_RESOURCE_HANDLER_H

#include "PcSupport/PcSupport.h"
#include "Engine/GameObject.h"

#include "Engine/Resource.h"

namespace Helium
{
    class Stream;
}

namespace Helium
{
    class ObjectPreprocessor;

    /// Interface for parsing resources and creating and caching objects based on them.
    class HELIUM_PC_SUPPORT_API ResourceHandler : public GameObject
    {
        HELIUM_DECLARE_OBJECT( ResourceHandler, GameObject );

    public:
        /// @name Construction/Destruction
        //@{
        ResourceHandler();
        virtual ~ResourceHandler();
        //@}

        /// @name Resource Handling Support
        //@{
        virtual const GameObjectType* GetResourceType() const;
        virtual void GetSourceExtensions( const tchar_t* const*& rppExtensions, size_t& rExtensionCount ) const;

#if HELIUM_EDITOR
        virtual bool CacheResource(
            ObjectPreprocessor* pObjectPreprocessor, Resource* pResource, const String& rSourceFilePath );
#endif
        //@}

        /// @name Static Resource Handling Support
        //@{
        static void GetAllResourceHandlers( DynArray< ResourceHandler* >& rResourceHandlers );
        static ResourceHandler* FindResourceHandlerForType( const GameObjectType* pType );
        //@}
    };
}

#endif  // HELIUM_PC_SUPPORT_RESOURCE_HANDLER_H
