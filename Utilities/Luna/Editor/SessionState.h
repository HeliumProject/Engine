#pragma once

#include "API.h"
#include "EditorState.h"
#include "Reflect/Serializers.h"

namespace Luna
{
  //
  // Session State
  //
  
  // key (std::string) is EditorTypes::EditorType enum values
  typedef std::map< std::string, EditorStatePtr > M_EditorState;

  class LUNA_EDITOR_API SessionState : public Reflect::Element
  {
  public:
    // maps an editor type to its state information
    M_EditorState m_EditorStates;

    REFLECT_DECLARE_CLASS( SessionState, Reflect::Element );

    static void EnumerateClass( Reflect::Compositor<SessionState>& comp );
  };

  typedef Nocturnal::SmartPtr<SessionState> SessionStatePtr;
}