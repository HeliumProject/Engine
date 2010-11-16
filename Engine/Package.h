//----------------------------------------------------------------------------------------------------------------------
// Package.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_PACKAGE_H
#define LUNAR_ENGINE_PACKAGE_H

#include "Engine/Object.h"

namespace Lunar
{
    class PackageLoader;

    /// Object package.
    class LUNAR_ENGINE_API Package : public Object
    {
        L_DECLARE_OBJECT( Package, Object );

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

    private:
        /// Package loader.
        PackageLoader* m_pLoader;
    };
}

#include "Engine/Package.inl"

#endif  // LUNAR_ENGINE_PACKAGE_H
