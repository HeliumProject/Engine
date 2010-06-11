#pragma once

#include "API.h"
#include "Foundation/File/Path.h"
#include "Reflect/Serializers.h"

namespace Luna
{
    //
    // State of an editor
    //

    class LUNA_EDITOR_API EditorState : public Reflect::Element
    {
    public:
        // each file that was open in the last state
        Nocturnal::S_Path m_OpenFilePaths;

        REFLECT_DECLARE_CLASS( EditorState, Reflect::Element );

        static void EnumerateClass( Reflect::Compositor<EditorState>& comp );
    };

    typedef Nocturnal::SmartPtr<EditorState> EditorStatePtr;
}