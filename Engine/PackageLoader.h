//----------------------------------------------------------------------------------------------------------------------
// PackageLoader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_ENGINE_PACKAGE_LOADER_H
#define HELIUM_ENGINE_PACKAGE_LOADER_H

#include "Engine/GameObjectLoader.h"

namespace Helium
{
    /// Package loader interface.
    class HELIUM_ENGINE_API PackageLoader : NonCopyable
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~PackageLoader() = 0;
        //@}

        /// @name Loading
        //@{
        virtual bool TryFinishPreload() = 0;

        virtual size_t BeginLoadObject( GameObjectPath path ) = 0;
        virtual bool TryFinishLoadObject(
            size_t requestId, GameObjectPtr& rspObject, DynArray< GameObjectLoader::LinkEntry >& rLinkTable ) = 0;

        virtual void Tick() = 0;
        //@}

        /// @name Data Access
        //@{
        virtual size_t GetObjectCount() const = 0;
        virtual GameObjectPath GetObjectPath( size_t index ) const = 0;
        //@}

        /// @name Package File Information
        //@{
        virtual bool IsSourcePackageFile() const = 0;
        virtual int64_t GetFileTimestamp() const = 0;
        //@}
    };
}

#endif  // HELIUM_ENGINE_PACKAGE_LOADER_H
