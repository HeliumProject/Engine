#pragma once

#include "ExampleGame/ExampleGame.h"
#include "Engine/GameObject.h"

namespace Example
{
    /// Example object derived type.
    class EXAMPLE_EXAMPLE_GAME_API ExampleObject : public Helium::GameObject
    {
        HELIUM_DECLARE_OBJECT( ExampleObject, Helium::GameObject );

    public:
        /// @name Construction/Destruction
        //@{
        ExampleObject();
        virtual ~ExampleObject();
        //@}
    };
}
