#pragma once

#include "API.h"
#include "File/File.h"
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
        File::S_Reference m_OpenFileRefs;

        REFLECT_DECLARE_CLASS( EditorState, Reflect::Element );

        static void EnumerateClass( Reflect::Compositor<EditorState>& comp );
    };

    typedef Nocturnal::SmartPtr<EditorState> EditorStatePtr;
}