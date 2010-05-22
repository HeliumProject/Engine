#include "Precompile.h"
#include "EditorState.h"

using namespace Luna;

REFLECT_DEFINE_CLASS( EditorState )

void EditorState::EnumerateClass( Reflect::Compositor<EditorState>& comp )
{
  Reflect::Field* fieldOpenFileRefs = comp.AddField( &EditorState::m_OpenFileRefs, "m_OpenFileRefs" );
}