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

    virtual i32 GetImageIndex() const;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;

    std::string GetPath() const;
    tuid GetFileID() const;

    const Math::Color3& GetColor() const;

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;
    static void CreatePanel( CreatePanelArgs& args );

  protected:
    virtual void PackageChanged( const Reflect::ElementChangeArgs& args ) NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr< Zone > ZonePtr;
  typedef std::vector< Zone* > V_ZoneDumbPtr;
  typedef std::set< Zone* > S_ZoneDumbPtr;
}
