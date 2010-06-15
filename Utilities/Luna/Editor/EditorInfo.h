#pragma once

#include "Luna/API.h"
#include "Platform/Types.h"
#include "Reflect/Serializers.h"

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
      info->AddElement(Scene, "Scene");
      info->AddElement(Asset, "Asset");
      info->AddElement(AnimationEvents, "AnimationEvents");
      info->AddElement(Character, "Character"); 
      info->AddElement(CinematicEvents, "CinematicEvents");
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
    const Finder::FilterSpec* m_FilterSpec;

  public:
    EditorInfo( EditorType type, CreateEditorFunc createFunc, const Finder::FilterSpec* filter );
    virtual ~EditorInfo();

    Editor* Create() const;
    EditorType GetType() const;
    const Finder::FilterSpec* GetFilterSpec() const;
  };

  typedef Nocturnal::SmartPtr< EditorInfo > EditorInfoPtr;

  // Map of editor info keyed by type
  typedef std::map< EditorType, EditorInfoPtr > M_EditorInfo;
}
