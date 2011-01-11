//----------------------------------------------------------------------------------------------------------------------
// ResourceHandler.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_PC_SUPPORT_RESOURCE_HANDLER_H
#define LUNAR_PC_SUPPORT_RESOURCE_HANDLER_H

#include "PcSupport/PcSupport.h"
#include "Engine/GameObject.h"

#include "Engine/Resource.h"

namespace Helium
{
    class Stream;
}

namespace Lunar
{
    class ObjectPreprocessor;

    /// Interface for parsing resources and creating and caching objects based on them.
    class LUNAR_PC_SUPPORT_API ResourceHandler : public GameObject
    {
        L_DECLARE_OBJECT( ResourceHandler, GameObject );

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

#if L_EDITOR
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

#endif  // LUNAR_PC_SUPPORT_RESOURCE_HANDLER_H
