#pragma once

#include "API.h"
#include "SceneNode.h"
#include "Content/Zone.h"

namespace Luna
{
  // Forwards
  class Enumerator;

  class Zone : public Luna::SceneNode
  {
    //
    // Members
    //

  protected:


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Zone, Luna::SceneNode );
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

  public:
    Zone( Luna::Scene* scene, Content::Zone* zone );
    virtual ~Zone();

    const Nocturnal::Path& GetPathObject() const
    {
        return m_Path;
    }

    std::string GetPath()
    {
        return m_Path.Get();
    }

    virtual i32 GetImageIndex() const;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;

    const Math::Color3& GetColor() const;

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;
    static void CreatePanel( CreatePanelArgs& args );

  protected:
    virtual void PackageChanged( const Reflect::ElementChangeArgs& args ) NOC_OVERRIDE;

    Nocturnal::Path m_Path;
  };

  typedef Nocturnal::SmartPtr< Zone > ZonePtr;
  typedef std::vector< Zone* > V_ZoneDumbPtr;
  typedef std::set< Zone* > S_ZoneDumbPtr;
}
