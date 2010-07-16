#pragma once

#include "Luna/API.h"
#include "Platform/Types.h"
#include "Foundation/Reflect/Serializers.h"

namespace Finder { class FilterSpec; }

namespace Luna
{
  // Forwards
  class Editor;

  // Typedef for a create function for an Editor
  typedef Editor* (*CreateEditorFunc)();

  // Enumeration of all the different editors.  Add new ones here.
  namespace EditorTypes
  {
    enum EditorType
    {
      Invalid = -1,
      Scene,
      Asset,
      AnimationEvents, 
      Character,
      CinematicEvents
    };

    static void EditorTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Scene, TXT( "Scene" ) );
      info->AddElement(Asset, TXT( "Asset" ) );
      info->AddElement(AnimationEvents, TXT( "AnimationEvents" ) );
      info->AddElement(Character, TXT( "Character" ) ); 
      info->AddElement(CinematicEvents, TXT( "CinematicEvents" ) );
    }
  }
  typedef EditorTypes::EditorType EditorType;

  /////////////////////////////////////////////////////////////////////////////
  // Class managing all the information about an editor, including what function
  // to use to create the editor.
  // 
  class LUNA_EDITOR_API EditorInfo : public Nocturnal::RefCountBase< EditorInfo >
  {
  private:
    EditorType m_Type;
    CreateEditorFunc m_CreateFunc;
    tstring m_FileFilter;

  public:
      EditorInfo( EditorType type, CreateEditorFunc createFunc, const tstring& filter );
    virtual ~EditorInfo();

    Editor* Create() const;
    EditorType GetType() const;
    const tstring& GetFileFilter() const;
  };

  typedef Nocturnal::SmartPtr< EditorInfo > EditorInfoPtr;

  // Map of editor info keyed by type
  typedef std::map< EditorType, EditorInfoPtr > M_EditorInfo;
}
