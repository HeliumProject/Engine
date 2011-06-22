//----------------------------------------------------------------------------------------------------------------------
// ExampleObject.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef EXAMPLE_EXAMPLE_GAME_EXAMPLE_OBJECT_H
#define EXAMPLE_EXAMPLE_GAME_EXAMPLE_OBJECT_H

#include "ExampleGame/ExampleGame.h"
#include "Engine/GameObject.h"

namespace Example
{
    /// Example object derived type.
    class EXAMPLE_EXAMPLE_GAME_API ExampleObject : public Helium::GameObject
    {
        L_DECLARE_OBJECT( ExampleObject, Helium::GameObject );

    public:
        /// @name Construction/Destruction
        //@{
        ExampleObject();
        virtual ~ExampleObject();
        //@}
    };
}

#endif  // EXAMPLE_EXAMPLE_GAME_EXAMPLE_OBJECT_H
