#pragma once

#include "Luna/API.h"

#include "Core/Object.h"
#include "Primitive.h"

namespace Luna
{
  class SceneNode;
  class SceneNode;
  class TypeConfig;
  typedef Nocturnal::SmartPtr< TypeConfig > TypeConfigPtr;
  typedef std::vector< TypeConfigPtr > V_TypeConfigSmartPtr;

  class TypeConfig : public Object
  {
  public:
    //
    // Members
    //

    std::string m_Name;

    // Settings
    u32 m_Color; // ARGB - 8 bits each
    bool m_Solid;
    bool m_Transparent;
    std::string m_Icon;
    int m_IconIndex;

    // Criteria
    std::string m_ApplicationType;
    S_string m_ContainsComponents;
    S_string m_MissingComponents;
    std::string m_Location;


    //
    // RTTI
    //

    LUNA_DECLARE_TYPE(TypeConfig, Object);
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

    TypeConfig();

    int Validate(Luna::SceneNode* node);

    bool ContainsComponent(Luna::SceneNode* node, const std::string& name);

    static void LoadFromFile(V_TypeConfigSmartPtr& types);
  };
}
