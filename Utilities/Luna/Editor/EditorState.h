#pragma once

#include "API.h"
#include "TUID/TUID.h"
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
    V_tuid m_OpenFiles;

    REFLECT_DECLARE_CLASS( EditorState, Reflect::Element );

    static void EnumerateClass( Reflect::Compositor<EditorState>& comp );
  };

  typedef Nocturnal::SmartPtr<EditorState> EditorStatePtr;
}