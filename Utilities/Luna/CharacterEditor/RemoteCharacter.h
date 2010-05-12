#pragma once

#include "API.h"

#include "Live/RemoteEditor.h"

namespace Luna
{
  class CharacterEditor;
  struct LunaViewTool;

  class RemoteCharacter : public RemoteEditor
  {
  public:
    RemoteCharacter( CharacterEditor* editor );
    virtual ~RemoteCharacter();

    virtual void SendScene() NOC_OVERRIDE;
    virtual void Enable( bool enable ) NOC_OVERRIDE;

    CharacterEditor* GetCharacterEditor() { return m_CharacterEditor; }

  protected:
    CharacterEditor*     m_CharacterEditor;
#if 0
    LunaViewTool*         m_LunaViewTool;
#endif
  };
}