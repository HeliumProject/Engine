//----------------------------------------------------------------------------------------------------------------------
// Package.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_ENGINE_PACKAGE_H
#define HELIUM_ENGINE_PACKAGE_H

#include "Engine/GameObject.h"

namespace Helium
{
    class PackageLoader;

    /// GameObject package.
    class HELIUM_ENGINE_API Package : public GameObject
    {
        HELIUM_DECLARE_OBJECT( Package, GameObject );

    public:
        /// @name Construction/Destruction
        //@{
        Package();
        virtual ~Package();
        //@}

        /// @name Loader Information
        //@{
        inline PackageLoader* GetLoader() const;
        void SetLoader( PackageLoader* pLoader );
        //@}

        /// @name Package Serialization
        //@{
        void SavePackage();
        //@}

    private:
        /// Package loader.
        PackageLoader* m_pLoader;
    };
}

#include "Engine/Package.inl"

#endif  // HELIUM_ENGINE_PACKAGE_H
