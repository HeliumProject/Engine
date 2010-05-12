#include "Precompile.h"
#include "SessionState.h"

using namespace Luna;

REFLECT_DEFINE_CLASS( SessionState )

void SessionState::EnumerateClass( Reflect::Compositor<SessionState>& comp )
{
  Reflect::Field* fieldEditorStates = comp.AddField( &SessionState::m_EditorStates, "m_EditorStates" );
}