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

    tstring m_Name;

    // Settings
    u32 m_Color; // ARGB - 8 bits each
    bool m_Solid;
    bool m_Transparent;
    tstring m_Icon;
    int m_IconIndex;

    // Criteria
    tstring m_ApplicationType;
    std::set< tstring > m_ContainsComponents;
    std::set< tstring > m_MissingComponents;
    tstring m_Location;


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

    bool ContainsComponent(Luna::SceneNode* node, const tstring& name);

    static void LoadFromFile(V_TypeConfigSmartPtr& types);
  };
}
